#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"
#include <cstdarg>
#include <cstdio>

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetGame__ProcessNetwork)();

void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

int64_t (*orig_CChat__AddDebugMessage)(const char* format, ...);
int64_t hook_CChat__AddDebugMessage(const char* format, ...)
{
    if(!orig_CChat__AddDebugMessage) {
        return 0;
    }
    if(!format) {
        return orig_CChat__AddDebugMessage("%s", "");
    }

    char formatted[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(formatted, sizeof(formatted), format, args);
    va_end(args);

    std::string text = formatted;
    const std::string from = "BLACK RUSSIA";
    const std::string to = "KAZAK RP";
    size_t pos = 0;
    while((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.length(), to);
        pos += to.length();
    }

    return orig_CChat__AddDebugMessage("%s", text.c_str());
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("188.127.241.74:"), 2569, 0, 0, 5);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );

bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
    int sampRpcId = ConvertBRIDToSampID(uniqueID);
    const bool isRpc313 = (uniqueID == static_cast<BRRpcIds>(313));

    if(isRpc313) {
        sampRpcId = 97;
    }
    
    if(sampRpcId != -1) {
        if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }
        
        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }
    
    return false;
}


bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId;
    if(!bsCopy.Read(pktId)) {
        return false;
    }
    if(pktId == BR_ID_USER_INTERFACE_SYNC) {
        uint16_t guiId;
        uint32_t jsonLen;
        if(!bsCopy.Read(guiId) || !bsCopy.Read(jsonLen)) {
            return false;
        }
        if(jsonLen > 0) {
            const uint32_t kMaxJsonSize = 1u << 20;
            if (jsonLen > kMaxJsonSize) {
                CNetGame::SendJsonData(guiId, "", 0);
                return false;
            }

            char* buff = new char[jsonLen + 1];
            if (!bsCopy.Read(buff, jsonLen)) {
                delete[] buff;
                return false;
            }
            buff[jsonLen] = '\0';


            size_t utfCap = static_cast<size_t>(jsonLen) * 3 + 1;
            char* buffGUI = new char[utfCap];
            cp1251_to_utf8(buffGUI, buff, jsonLen);
            if(guiId == 10) {
                if(nlohmann::json::accept(buffGUI)) {
                    nlohmann::json jsonObj = nlohmann::json::parse(buffGUI, nullptr, false);
                    if(!jsonObj.is_discarded()
                        && jsonObj.contains("r")
                        && jsonObj.contains("l")
                        && jsonObj.contains("i")
                        && jsonObj["r"].is_number_integer()
                        && jsonObj["l"].is_number_integer()
                        && jsonObj["i"].is_string()) {
                        uint8_t btn = jsonObj["r"].get<uint8_t>();
                        int16_t listInput = jsonObj["l"].get<int16_t>();
                        std::string input = jsonObj["i"].get<std::string>();
                        uint8_t inputLen = static_cast<uint8_t>(input.length() > 255 ? 255 : input.length());
                        RakNet::BitStream bsSend;
                        bsSend.WriteBits((unsigned char *)&CNetGame::m_nLastSAMPDialogID, 16);
                        bsSend.WriteBits((unsigned char *)&btn, 8);
                        bsSend.WriteBits((unsigned char *)&listInput, 16);
                        bsSend.WriteBits((unsigned char *)&inputLen, 8);
                        bsSend.Write(input.c_str(), inputLen);
                        pRakClient->RPC(&RPC_DialogResponse, &bsSend,
                                        HIGH_PRIORITY, RELIABLE_ORDERED, 0,
                                        false, UNASSIGNED_NETWORK_ID, NULL);
                    }
                }
            }
        else {
            uint32_t utfLen = static_cast<uint32_t>(strlen(buffGUI));
            CNetGame::SendJsonData(guiId, buffGUI, utfLen);
        }
            delete[] buffGUI;
            delete[] buff;
        }
    }
	if(pktId == BR_ID_AIM_SYNC) {
		uint8_t pktAimSync = ID_AIM_SYNC;
	    uint8_t aimSyncBuffer[31] = {0};
		if(!bsCopy.ReadBits(aimSyncBuffer, 31 * 8)) {
            return false;
        }
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktAimSync, 8);
	    bsSend.WriteBits(aimSyncBuffer, 31 * 8);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_BULLET_SYNC) {
		uint8_t pktBltSync = ID_BULLET_SYNC;
	    uint8_t bltSyncBuffer[40] = {0};
		if(!bsCopy.ReadBits(bltSyncBuffer, 40 * 8)) {
            return false;
        }
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktBltSync, 8);
	    bsSend.WriteBits(bltSyncBuffer, 40 * 8);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_PLAYER_SYNC) {
		uint8_t pktPlayerSync = ID_PLAYER_SYNC;
	    BROnFootSyncData data = {0};
	    if(!bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.wKeys, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecPos, 96)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatw, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatx, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quaty, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatz, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.health, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.armour, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.byteSpecialAction, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed, 96)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecSurfOffsets, 96)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.wSurfInfo, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.dwAnimation, 32)) { return false; }
	    RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktPlayerSync, 8);
	    ConvertBROnFootSyncToSampSync(&bsSend, data);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	if(pktId == BR_ID_VEHICLE_SYNC) {
		uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
	    BRInCarSyncData data = {0};
	    if(!bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.lrAnalogLeftStick, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.udAnalogLeftStick, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.wKeys, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatw, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatx, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quaty, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.quatz, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecPos.x, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecPos.y, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecPos.z, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.x, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.y, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecMoveSpeed.z, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.fCarHealth, 32)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.byteSirenOn, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.byteLandingGearState, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.TrailerID, 16)) { return false; }

    RakNet::BitStream bsSend;
    bsSend.WriteBits(&pktVehicleSync, 8);
    ConvertBRInCarSyncToSampSync(&bsSend, data);
    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}
	if(pktId == BR_ID_PASSENGER_SYNC) {
		uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
		BRPassengerSyncData data = {0};
		if(!bsCopy.ReadBits((unsigned char *)&data.VehicleID, 16)) { return false; }
		uint8_t tempByte = 0;
		if(!bsCopy.ReadBits((unsigned char *)&tempByte, 7)) { return false; }
		data.byteSeatFlags = tempByte;
		if(!bsCopy.ReadBits((unsigned char *)&tempByte, 1)) { return false; }
		data.byteDriveBy = tempByte;
		if(!bsCopy.ReadBits((unsigned char *)&data.byteCurrentWeapon, 8)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.playerHealth, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.playerArmour, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.lrAnalog, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.udAnalog, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.wKeys, 16)) { return false; }
		if(!bsCopy.ReadBits((unsigned char *)&data.vecPos, 96)) { return false; }
		RakNet::BitStream bsSend;
	    bsSend.WriteBits(&pktPassengerSync, 8);
	    ConvertBRPassengerSyncToSampSync(&bsSend, data);
	    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
	
	return false;
}
