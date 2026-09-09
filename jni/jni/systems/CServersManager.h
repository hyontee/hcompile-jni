#ifndef BLACK_RUSSIA_CSERVERSMANAGER_H
#define BLACK_RUSSIA_CSERVERSMANAGER_H

#include <jni.h>
#include <vector>
#include <string>

class CServersManager {
public:
    static std::string* mServersIp;
    static int32_t* mServersPort;
    static int16_t mCurServer;
};


#endif //BLACK_RUSSIA_CSERVERSMANAGER_H
