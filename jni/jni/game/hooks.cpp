#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

namespace
{
struct OutgoingPacketInfo
{
    bool hasTimestamp = false;
    uint32_t timestamp = 0;
    uint8_t packetId = 0;
};

constexpr int kMaxForwardPayloadBytes = 512;

void WriteOutgoingPacketPrefix(RakNet::BitStream& out, const OutgoingPacketInfo& info, uint8_t packetId);

bool ForwardRemappedPacket(const OutgoingPacketInfo& info,
                           RakNet::BitStream& payload,
                           uint8_t newPacketId,
                           PacketPriority priority,
                           BRPacketReliability reliability,
                           char orderingChannel)
{
    RakNet::BitStream bsSend;
    WriteOutgoingPacketPrefix(bsSend, info, newPacketId);

    const int unreadBits = payload.GetNumberOfUnreadBits();
    if (unreadBits > 0) {
        const int unreadBytes = (unreadBits + 7) / 8;
        if (unreadBytes > kMaxForwardPayloadBytes) {
            return false;
        }

        unsigned char rawPayload[kMaxForwardPayloadBytes] = {0};
        if (!payload.ReadBits(rawPayload, unreadBits)) {
            return false;
        }
        bsSend.WriteBits(rawPayload, unreadBits);
    }

    return pRakClient->Send(&bsSend, priority, ConvertBRToSampReliability(reliability), orderingChannel);
}


bool ReadOutgoingPacketInfo(RakNet::BitStream* source, RakNet::BitStream& copy, OutgoingPacketInfo& info)
{
    if (!source) {
        return false;
    }

    copy = RakNet::BitStream(source->GetData(), source->GetNumberOfBytesUsed() + 1, false);
    if (!copy.Read(info.packetId)) {
        return false;
    }

    if (info.packetId == BR_ID_TIMESTAMP) {
        info.hasTimestamp = true;
        if (!copy.Read(info.timestamp) || !copy.Read(info.packetId)) {
            return false;
        }
    }

    return true;
}

void WriteOutgoingPacketPrefix(RakNet::BitStream& out, const OutgoingPacketInfo& info, uint8_t packetId)
{
    if (info.hasTimestamp) {
        const uint8_t timestampId = ID_TIMESTAMP;
        out.Write(timestampId);
        out.Write(info.timestamp);
    }

    out.Write(packetId);
}

int MapBRSyncPacketToSampPacket(uint8_t packetId)
{
    switch (packetId) {
        case BR_ID_AIM_SYNC: return ID_AIM_SYNC;
        case BR_ID_WEAPONS_UPDATE: return ID_WEAPONS_UPDATE;
        case BR_ID_STATS_UPDATE: return ID_STATS_UPDATE;
        case BR_ID_PASSENGER_SYNC: return ID_PASSENGER_SYNC;
        case BR_ID_TRAILER_SYNC: return ID_TRAILER_SYNC;
        case BR_ID_PLAYER_SYNC: return ID_PLAYER_SYNC;
        case BR_ID_UNOCCUPIED_SYNC: return ID_UNOCCUPIED_SYNC;
        case BR_ID_SPECTATOR_SYNC: return ID_SPECTATOR_SYNC;
        case BR_ID_BULLET_SYNC: return ID_BULLET_SYNC;
        case BR_ID_VEHICLE_SYNC: return ID_VEHICLE_SYNC;
        default: return -1;
    }
}

}

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("188.127.241.8"), 1311, 0, 0, 5);
}


int64_t (*orig_CChat__AddDebugMessage)(const char* format, ...);
int64_t hook_CChat__AddDebugMessage(const char* format, ...)
{
    return 0;
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, int* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, int* uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
    if (!uniqueID) {
        return false;
    }

    const int originalRpcId = *uniqueID;
    int sampRpcId = NormalizeOutgoingRpcId(originalRpcId);

    if(sampRpcId == 97) {
        int currentOffset = bitStream->GetReadOffset();
        
        int actionType = -1;
        unsigned char actionSubtype = 0;  
        
        if(bitStream->GetNumberOfUnreadBits() >= 32) {
            bitStream->ReadBits((unsigned char*)&actionType, 32, true);
        }
        if(bitStream->GetNumberOfUnreadBits() >= 8) {
            bitStream->ReadBits(&actionSubtype, 8, true); 
        }
        
        bitStream->SetReadOffset(currentOffset);
    }
    
    if(sampRpcId != -1) {
        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }
    
    return orig_RakClient__RPC(thiz, uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp, networkID, replyFromTarget);
}

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
	RakNet::BitStream bsCopy;
    OutgoingPacketInfo pktInfo;
    if (!ReadOutgoingPacketInfo(bitStream, bsCopy, pktInfo)) {
        return pRakClient->Send(bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel);
    }

	uint8_t pktId = pktInfo.packetId;
	if(pktId == BR_ID_USER_INTERFACE_SYNC) {
		uint16_t guiId;
		uint32_t jsonLen;
		bsCopy.Read(guiId);
		bsCopy.Read(jsonLen);
		if(jsonLen > 0) {
			char* buff = new char[4096];
			bsCopy.Read(buff, jsonLen);
			buff[jsonLen] = 0;
			static char* buffGUI;
			if(!buffGUI) { buffGUI = new char[4096]; }
			cp1251_to_utf8(buffGUI, buff);
			if(guiId == 10) {
				if(nlohmann::json::accept(buffGUI)) {
					nlohmann::json jsonObj = nlohmann::json::parse(buffGUI);
					uint8_t btn = jsonObj["r"];
					int16_t listInput = jsonObj["l"];
					std::string input = jsonObj["i"];
					uint8_t inputLen = input.length();
					RakNet::BitStream bsSend;
					bsSend.WriteBits((unsigned char *)&CNetGame::m_nLastSAMPDialogID, 16);
					bsSend.WriteBits((unsigned char *)&btn, 8);
					bsSend.WriteBits((unsigned char *)&listInput, 16);
					bsSend.WriteBits((unsigned char *)&inputLen, 8);
					bsSend.Write(input.c_str(), inputLen);
                    const bool result = pRakClient->RPC(&RPC_DialogResponse, &bsSend,
                                                        HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                                        false, UNASSIGNED_NETWORK_ID, NULL);
                    delete[] buff;
                    return result;
                }
            } else {
                CNetGame::SendOnData(guiId, buffGUI, static_cast<uint32_t>(strlen(buffGUI)));
                delete[] buff;
                return true;
            }

            delete[] buff;
            return true;
        }
	}
	if(pktId == BR_ID_AIM_SYNC) {
        return ForwardRemappedPacket(pktInfo, bsCopy, ID_AIM_SYNC, HIGH_PRIORITY, BR_RELIABILITY_UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_BULLET_SYNC) {
        return ForwardRemappedPacket(pktInfo, bsCopy, ID_BULLET_SYNC, HIGH_PRIORITY, BR_RELIABILITY_UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_PLAYER_SYNC) {
        return ForwardRemappedPacket(pktInfo, bsCopy, ID_PLAYER_SYNC, HIGH_PRIORITY, BR_RELIABILITY_UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_VEHICLE_SYNC) {
        return ForwardRemappedPacket(pktInfo, bsCopy, ID_VEHICLE_SYNC, HIGH_PRIORITY, BR_RELIABILITY_UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_PASSENGER_SYNC) {
        return ForwardRemappedPacket(pktInfo, bsCopy, ID_PASSENGER_SYNC, HIGH_PRIORITY, BR_RELIABILITY_UNRELIABLE_SEQUENCED, 0);
	}

    const int remappedSyncPacketId = MapBRSyncPacketToSampPacket(pktId);
    if (remappedSyncPacketId != -1) {
        return ForwardRemappedPacket(pktInfo, bsCopy, static_cast<uint8_t>(remappedSyncPacketId), priority, reliability, orderingChannel);
    }
	
	return pRakClient->Send(bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel);
}
