/**
 * Minimal RakNet transport interface required by the legacy console/parser
 * sources included in this client build.
 *
 * The client does not instantiate a transport provider; these parser sources
 * only need the PlayerID-based Send() contract. Keeping the interface local
 * avoids pulling in server/console transport implementations that are not part
 * of the Android client.
 */
#ifndef __TRANSPORT_INTERFACE_H
#define __TRANSPORT_INTERFACE_H

#include "NetworkTypes.h"
#include "Export.h"

class RAK_DLL_EXPORT TransportInterface
{
public:
    virtual ~TransportInterface() {}

    virtual void Send(PlayerID playerId, const char *data, ...) = 0;
};

#endif // __TRANSPORT_INTERFACE_H
