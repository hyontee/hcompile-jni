#include "ServerSetter.h"
#include "..//settings.h"
#include "vendor/raknet/SHA1.h"

extern CSettings* pSettings;
#include "crashlytics.h"

char CClientInfo::szIP[0xFF];
uint16_t CClientInfo::usPort = 0;

extern "C"
{
	/*JNIEXPORT void JNICALL Java_com_blackrussia_core_GTASA_setClientIp(JNIEnv* pEnv, jobject thiz, jbyteArray ip)
	{
		CClientInfo::SetClientIP(pEnv, thiz, ip);
	}

	JNIEXPORT void JNICALL Java_com_blackrussia_core_GTASA_setClientPort(JNIEnv* pEnv, jobject thiz, jint port)
	{
		CClientInfo::SetClientPort(pEnv, thiz, port);
	}*/
}

void CClientInfo::SetClientIP(JNIEnv* pEnv, jobject thiz, jbyteArray ip)
{
	if (ip == 0)
	{
		Log("IP: NULL");
		return;
	}

	jboolean isCopy = true;

	jbyte* pIP = pEnv->GetByteArrayElements(ip, &isCopy);
	jsize length = pEnv->GetArrayLength(ip);

	memset(szIP, 0, sizeof(szIP) - 1);
	memcpy(szIP, pIP, length);

	firebase::crashlytics::SetCustomKey("IP", szIP);

	pEnv->ReleaseByteArrayElements(ip, pIP, JNI_ABORT);

                  Log("CLYENT AXAXAXA BY WEIKTON");
}

void CClientInfo::SetClientPort(JNIEnv* pEnv, jobject thiz, jint port)
{
	usPort = (uint16_t)port;
}