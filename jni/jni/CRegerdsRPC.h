//
// Created by Regerds on 01.11.2024.
//

#ifndef BLACK_RUSSIA_CREGERDSRPC_H
#define BLACK_RUSSIA_CREGERDSRPC_H


#include "NetworkTypes.h"

class CRegerdsRPC {
public:
    static void Packet_SyncCustomRPC(Packet* packet);
    static void SendRadialOperation(int i);
    static void SendPlayerMenuRequestToServer(int i);
    static void SendNotificationResponse(int subid, int type);
    static void SendFuelFillRequest(int subid, int type);
};


#endif //BLACK_RUSSIA_CREGERDSRPC_H
