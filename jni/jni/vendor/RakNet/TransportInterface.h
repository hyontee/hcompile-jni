#pragma once

#include "NetworkTypes.h"
#include "Export.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

class RAK_DLL_EXPORT TransportInterface
{
public:
    TransportInterface() {}
    virtual ~TransportInterface() {}

    virtual void Send(PlayerID playerId, const char* format, ...) 
    {
        (void)playerId;
        (void)format;
    }
    virtual void CloseConnection(PlayerID playerId) { (void)playerId; }
    virtual const char* Receive(PlayerID *playerId) { (void)playerId; return 0; }
    virtual void DeallocatePacket(const char *data) { (void)data; }
    virtual PlayerID GetMyID(void) { PlayerID p; p.binaryAddress=0; p.port=0; return p; }
};
