#include "netrpc.h"
#include "xorstr.h"

#include "plugin.h"
#include "common.h"

void DialogBoxRPC(RPCParameters* rpcParams)
{
	reinterpret_cast<void(*)(RPCParameters*)>(CGameAPI::GetBase(xorstr("RPC::DialogBox")))(rpcParams);
}

void RegisterRPCs(RakClientInterface* pInterface)
{
#ifdef __aarch64__
   pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2AC67C))); //
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACBE4))); //
   pInterface->RegisterAsRemoteProcedureCall(&RPC_InitGame, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2A3C94)));
  
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrDialogBox, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2B1868))); //
  
  pInterface->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ACDC4))); //
    pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x2ADFE8)));//
  
  // pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCustomizeVehicle, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x5026B0)));
//  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCommonStuff, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x502F30)));
//  pInterface->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ((void (*)(RPCParameters*))(CGameAPI::m_address + 0x4FE200)));
#endif
}