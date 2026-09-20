#pragma once

#include "NetworkTypes.h"
#include "Export.h"

class RAK_DLL_EXPORT TransportInterface
{
public:
    TransportInterface() {}
    virtual ~TransportInterface() {}

    virtual void Send(PlayerID playerId, const char* data, const int length) = 0;
    virtual void CloseConnection(PlayerID playerId) = 0;
    virtual const char* Receive(PlayerID *playerId) = 0;
    virtual void DeallocatePacket(const char *data) = 0;
    virtual PlayerID GetMyID(void) = 0;
};
