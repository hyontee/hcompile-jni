#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <jni.h>

#include "main.h"
#include "game/common.h"
#include "game/game.h"
#include "net/netgame.h"

class CClientInfo
{
public:
	static char szIP[0xFF];
	static uint16_t usPort;

	static void WriteClientInfoToBitStream(RakNet::BitStream& bs);

	static void SetClientIP(JNIEnv* pEnv, jobject thiz, jbyteArray ip);
	static void SetClientPort(JNIEnv* pEnv, jobject thiz, jint port);
};

