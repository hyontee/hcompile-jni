#include "hooks.h"
#include "xorstr.h"
#include "plugin/netrpc.h"

extern bool g_bInitGameProcess;

RakClientInterface* pRakClient = RakNetworkFactory::GetRakClientInterface();

void (*orig_CNetGame__ProcessNetwork)();
void hook_CNetGame__ProcessNetwork()
{
    CNetGame::ProcessNetwork();
}

bool (*orig_RakClient__Connect)(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer);
bool hook_RakClient__Connect(uintptr_t thiz, const char* host, uint16_t serverPort, uint16_t clientPort, unsigned int depreciated, int threadSleepTimer)
{
    return pRakClient->Connect(xorstr("80.242.59.112"), 3561, 0, 0, 5);
}

bool (*orig_RakClient__RPC)( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget );
bool hook_RakClient__RPC( uintptr_t thiz, BRRpcIds uniqueID, RakNet::BitStream *bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel, bool shiftTimestamp, NetworkID networkID, RakNet::BitStream *replyFromTarget )
{
	int sampRpcId = ConvertBRIDToSampID(uniqueID);
	
	if(uniqueID == 394) {
        sampRpcId = 97; 
    }
    
    if(sampRpcId == 97) {
        int currentOffset = bitStream->GetReadOffset();
        
        int actionType = -1;
        unsigned char actionSubtype;  
        
        if(bitStream->GetNumberOfUnreadBits() >= 32) {
            bitStream->ReadBits((unsigned char*)&actionType, 32, true);
        }
        if(bitStream->GetNumberOfUnreadBits() >= 8) {
            bitStream->ReadBits(&actionSubtype, 8, true); 
        }
		
        bitStream->SetReadOffset(currentOffset);
    }
	
    if(sampRpcId != -1) {
        if(sampRpcId == RPC_RequestClass && g_bInitGameProcess) {
            return false;
        }
        
        return pRakClient->RPC(&sampRpcId, bitStream, priority, ConvertBRToSampReliability(reliability), orderingChannel, shiftTimestamp, networkID, replyFromTarget);
    }
	else {
	}

    return false;
}

bool (*orig_RakClient__Send)( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel );
bool hook_RakClient__Send( uintptr_t thiz, RakNet::BitStream* bitStream, PacketPriority priority, BRPacketReliability reliability, char orderingChannel )
{
    RakNet::BitStream bsCopy(bitStream->GetData(), bitStream->GetNumberOfBytesUsed() + 1, false);
    uint8_t pktId;
    bsCopy.Read(pktId);
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
					bool result = pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
				}
			}
			else {
				CNetGame::SendOnData(guiId, buffGUI, jsonLen);
			}
		}
	}
	if(pktId == BR_ID_AIM_SYNC)
{
    uint8_t pktAimSync = ID_AIM_SYNC;

    uint8_t buffer[31]{};
    for(int i = 0; i < 31; ++i)
        if(!bsCopy.Read(buffer[i])) return false;

    RakNet::BitStream bsSend;
    bsSend.Write(pktAimSync);

    for(int i = 0; i < 31; ++i)
        bsSend.Write(buffer[i]);

    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

if(pktId == BR_ID_BULLET_SYNC)
{
    uint8_t pktBltSync = ID_BULLET_SYNC;

    uint8_t buffer[40]{};
    for(int i = 0; i < 40; ++i)
        if(!bsCopy.Read(buffer[i])) return false;

    RakNet::BitStream bsSend;
    bsSend.Write(pktBltSync);

    for(int i = 0; i < 40; ++i)
        bsSend.Write(buffer[i]);

    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

if(pktId == BR_ID_PLAYER_SYNC)
{
    uint8_t pktPlayerSync = ID_PLAYER_SYNC;
    BROnFootSyncData data{};

    if(!bsCopy.Read(data.lrAnalogLeftStick)) return false;
    if(!bsCopy.Read(data.udAnalogLeftStick)) return false;
    if(!bsCopy.Read(data.wKeys)) return false;

    if(!bsCopy.Read(data.vecPos.x)) return false;
    if(!bsCopy.Read(data.vecPos.y)) return false;
    if(!bsCopy.Read(data.vecPos.z)) return false;

    if(!bsCopy.Read(data.quatw)) return false;
    if(!bsCopy.Read(data.quatx)) return false;
    if(!bsCopy.Read(data.quaty)) return false;
    if(!bsCopy.Read(data.quatz)) return false;

    if(!bsCopy.Read(data.health)) return false;
    if(!bsCopy.Read(data.armour)) return false;

    if(!bsCopy.Read(data.byteCurrentWeapon)) return false;
    if(!bsCopy.Read(data.byteSpecialAction)) return false;

    if(!bsCopy.Read(data.vecMoveSpeed.x)) return false;
    if(!bsCopy.Read(data.vecMoveSpeed.y)) return false;
    if(!bsCopy.Read(data.vecMoveSpeed.z)) return false;

    if(!bsCopy.Read(data.vecSurfOffsets.x)) return false;
    if(!bsCopy.Read(data.vecSurfOffsets.y)) return false;
    if(!bsCopy.Read(data.vecSurfOffsets.z)) return false;

    if(!bsCopy.Read(data.wSurfInfo)) return false;
    if(!bsCopy.Read(data.dwAnimation)) return false;

    RakNet::BitStream bsSend;
    bsSend.Write(pktPlayerSync);

    ConvertBROnFootSyncToSampSync(&bsSend, data);

    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

if(pktId == BR_ID_VEHICLE_SYNC)
{
    uint8_t pktVehicleSync = ID_VEHICLE_SYNC;
    BRInCarSyncData data{};

    if(!bsCopy.Read(data.VehicleID)) return false;

    if(!bsCopy.Read(data.lrAnalogLeftStick)) return false;
    if(!bsCopy.Read(data.udAnalogLeftStick)) return false;
    if(!bsCopy.Read(data.wKeys)) return false;

    if(!bsCopy.Read(data.quatw)) return false;
    if(!bsCopy.Read(data.quatx)) return false;
    if(!bsCopy.Read(data.quaty)) return false;
    if(!bsCopy.Read(data.quatz)) return false;

    if(!bsCopy.Read(data.vecPos.x)) return false;
    if(!bsCopy.Read(data.vecPos.y)) return false;
    if(!bsCopy.Read(data.vecPos.z)) return false;

    if(!bsCopy.Read(data.vecMoveSpeed.x)) return false;
    if(!bsCopy.Read(data.vecMoveSpeed.y)) return false;
    if(!bsCopy.Read(data.vecMoveSpeed.z)) return false;

    if(!bsCopy.Read(data.fCarHealth)) return false;
    if(!bsCopy.Read(data.playerHealth)) return false;
    if(!bsCopy.Read(data.playerArmour)) return false;

    if(!bsCopy.Read(data.byteCurrentWeapon)) return false;
    if(!bsCopy.Read(data.byteSirenOn)) return false;
    if(!bsCopy.Read(data.byteLandingGearState)) return false;

    if(!bsCopy.Read(data.TrailerID)) return false;

    RakNet::BitStream bsSend;
    bsSend.Write(pktVehicleSync);

    ConvertBRInCarSyncToSampSync(&bsSend, data);

    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

if(pktId == BR_ID_PASSENGER_SYNC)
{
    uint8_t pktPassengerSync = ID_PASSENGER_SYNC;
    BRPassengerSyncData data{};

    if(!bsCopy.Read(data.VehicleID)) return false;

    uint8_t seatFlags = 0;
    if(!bsCopy.ReadBits(&seatFlags, 7)) return false;
    data.byteSeatFlags = seatFlags;

    uint8_t driveBy = 0;
    if(!bsCopy.ReadBits(&driveBy, 1)) return false;
    data.byteDriveBy = driveBy;

    if(!bsCopy.Read(data.byteCurrentWeapon)) return false;
    if(!bsCopy.Read(data.playerHealth)) return false;
    if(!bsCopy.Read(data.playerArmour)) return false;

    if(!bsCopy.Read(data.lrAnalog)) return false;
    if(!bsCopy.Read(data.udAnalog)) return false;
    if(!bsCopy.Read(data.wKeys)) return false;

    if(!bsCopy.Read(data.vecPos.x)) return false;
    if(!bsCopy.Read(data.vecPos.y)) return false;
    if(!bsCopy.Read(data.vecPos.z)) return false;

    RakNet::BitStream bsSend;
    bsSend.Write(pktPassengerSync);

    ConvertBRPassengerSyncToSampSync(&bsSend, data);

    return pRakClient->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

return false;
}
