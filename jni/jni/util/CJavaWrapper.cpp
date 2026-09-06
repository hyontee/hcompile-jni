#include "CJavaWrapper.h"
#include "../main.h"
#include "vendor/json/json.hpp"

extern "C" JavaVM* javaVM;



extern int g_iServer;

char* setupJsonData(const char *fmt, ...)
{
    char buffer[10000];

    memset(buffer, 0, sizeof(buffer));

    va_list arg;
    va_start(arg, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    return buffer;
}

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

	if (getEnvStat == JNI_EDETACHED)
	{
		LOG(OBFUSCATE("GetEnv: not attached"));
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			LOG(OBFUSCATE("Failed to attach"));
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		LOG(OBFUSCATE("GetEnv: version not supported"));
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		LOG(OBFUSCATE("GetEnv: JNI_ERR"));
		return nullptr;
	}

	return env;
}

std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return std::string("");
	}

	jbyteArray retn = (jbyteArray)env->CallObjectMethod(activity, s_GetClipboardText);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	
	return str;
}

void CJavaWrapper::CallLauncherActivity(int type)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_CallLauncherActivity, type);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_ShowInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_HideInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText)
{
    JNIEnv* env = GetEnv();
    if (!env)
    {
        LOG(OBFUSCATE("No env"));
        return;
    }
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["o"] = 1;
    data["di"] = dialogId;
    data["i"] = dialogTypeId;
    data["c"] = (const char*)content;
    data["l"] = leftBtnText;
    data["r"] = rightBtnText;
    data["s"] = caption;
    data["array_w"] = 0;
    if((dialogTypeId == 2) ||  (dialogTypeId == 4) ||  (dialogTypeId == 5)){
        data["array_w"] = 1;
    }else{
        data["array_w"] = 0;
    }
    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, BrDialogWindow , bytes);

    EXCEPTION_CHECK(env);
}


void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_SetUseFullScreen, b);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetPauseState(bool a1)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendJsonData1(JNIEnv* pEnv, jobject thiz, jint guiid, jint i3, jint i, jint i2, jbyteArray str)
    {

        switch (guiid) {
            case BrDialogWindow:{
                jboolean isCopy = true;

                jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
                jsize length = pEnv->GetArrayLength(str);

                std::string szStr((char*)pMsg, length);

                if(CNetGame::Instance()) {
                    LOG(OBFUSCATE("dialog response | id : %i, btnId : %i, listBoxItem : %i, input : %s"), i3, i, i2, (char*)szStr.c_str());
                    CNetGame::Instance()->SendDialogResponse(i3, i, i2, (char*)szStr.c_str());
                    //pGame->FindPlayerPed()->TogglePlayerControllable(true);
                }

                pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
                break;
            }
        }
    }
}

void CJavaWrapper::ShowHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //g_pJavaWrapper->ShowNotification(4, "Худ успешно подключен", 5, "", ">>")); 
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["s"] = 1;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, HudManager , bytes);
}

void CJavaWrapper::HideHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //g_pJavaWrapper->ShowNotification(4, "Худ успешно скрыт", 5, "", ">>")); 
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["s"] = 0;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, HudManager , bytes);
}

void CJavaWrapper::ShowGPS()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
   // g_pJavaWrapper->ShowNotification(4, "Метка установлена", 5, "", ">>")); 
    //env->CallVoidMethod(this->activity, this->s_showGps);
}

void CJavaWrapper::HideGPS()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideGps);
}

void CJavaWrapper::ShowZona()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_showZona);
}

void CJavaWrapper::HideZona()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hideZona);
}

void CJavaWrapper::ShowX2()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //g_pJavaWrapper->ShowNotification(4, "x2", 5, "", ">>")); 
    //env->CallVoidMethod(this->activity, this->s_showx2);
}

void CJavaWrapper::HideX2()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
    //env->CallVoidMethod(this->activity, this->s_hidex2);
}

void CJavaWrapper::CrashLog(const char* error)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    std::string dataBytes = std::string(error);
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

    env->CallVoidMethod(this->activity, this->s_crashLog, jtext);
}

void CJavaWrapper::UpdateHudInfo(int health, int armour, int hunger, int weaponidughur, int ammo, int ammoinclip, int money, int wanted)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["b"] = health;
    data["u"] = armour;
    data["r"] = hunger;
    data["g"] = weaponidughur;
    data["e"] = ammo;
    data["v"] = ammoinclip;
    data["a"] = money;
    data["m"] = wanted;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, HudManager , bytes);
}

void CJavaWrapper::ShowSplash() {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["t"] = 1;
    data["s"] = 0;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, BrChooseServerDialog , bytes);
}

void CJavaWrapper::UpdateSplash(int percent) {
	
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    if(percent >= 99){
        data["t"] = 0;
        data["s"] = percent;
    }else{
        data["t"] = 1;
        data["s"] = percent;
    }

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, BrChooseServerDialog , bytes);
}

void CJavaWrapper::ShowSpeed()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
	//env->CallVoidMethod(this->activity, this->s_showSpeed);
}

void CJavaWrapper::HideSpeed()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}
	//env->CallVoidMethod(this->activity, this->s_hideSpeed);
}

void CJavaWrapper::UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    /*nlohmann::json data;
    data["t"] = type;
    data["i"] = (const char*)text;
    data["d"] = duration;
    data["s"] = 1;
    data["b"] = 1;
    data["k"] = (const char*)textBtn;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


    env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, BrNotification , jtext);*/
}

void CJavaWrapper::ShowNotification(int type, char* text, int duration, char* actionforBtn, char* textBtn) 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		LOG(OBFUSCATE("No env"));
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    nlohmann::json data;
    data["t"] = type;
    data["i"] = (const char*)text;
    data["d"] = duration;
    data["s"] = 1;
    data["b"] = 1;
    data["k"] = (const char*)textBtn;

    std::string dataBytes = data.dump();
    const char* dataBytesChr = dataBytes.c_str();

    jbyteArray bytes = env->NewByteArray(strlen(dataBytesChr));
    env->SetByteArrayRegion(bytes, 0, strlen(dataBytesChr), (jbyte*)dataBytesChr);
    jstring jtext = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);


	env->CallVoidMethod(this->activity, this->s_onJsonDataIncoming, BrNotification , bytes);
}

CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);
	if (!nvEventClass)
	{
		LOG(OBFUSCATE("nvEventClass null"));
		return;
	}

	//s_CallLauncherActivity = env->GetMethodID(nvEventClass, "callLauncherActivity", "(I)V"));
	s_GetClipboardText = env->GetMethodID(nvEventClass, "getClipboardText", "()[B");

	s_ShowInputLayout = env->GetMethodID(nvEventClass, "showInputLayout", "()V");
	s_HideInputLayout = env->GetMethodID(nvEventClass, "hideInputLayout", "()V");

	s_SetUseFullScreen = env->GetMethodID(nvEventClass, "setUseFullscreen", "(I)V");

	s_setPauseState = env->GetMethodID(nvEventClass, "setPauseState", "(Z)V");

	s_onJsonDataIncoming = env->GetMethodID(nvEventClass, "onJsonDataIncoming", "(I[B)V");
	s_crashLog = env->GetMethodID(nvEventClass, "crashLog", "(Ljava/lang/String;)V");

	/*s_MakeDialog = env->GetMethodID(nvEventClass, "showDialog", "(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"));

	s_updateHudInfo = env->GetMethodID(nvEventClass, "updateHudInfo", "(IIIIIIII)V"));
                  s_showHud = env->GetMethodID(nvEventClass, "showHud", "()V"));
                  s_hideHud = env->GetMethodID(nvEventClass, "hideHud", "()V"));

                  s_showGps = env->GetMethodID(nvEventClass, "showGps", "()V"));
                  s_hideGps = env->GetMethodID(nvEventClass, "hideGps", "()V"));

                  s_showZona = env->GetMethodID(nvEventClass, "showZona", "()V"));
                  s_hideZona = env->GetMethodID(nvEventClass, "hideZona", "()V"));

                  s_showx2 = env->GetMethodID(nvEventClass, "showx2", "()V"));
                  s_hidex2 = env->GetMethodID(nvEventClass, "hidex2", "()V"));

				  //s_updateSpeedInfo = env->GetMethodID(nvEventClass, "updateSpeedInfo", "(IIIIIIII)V"));
				  //s_showSpeed = env->GetMethodID(nvEventClass, "showSpeed", "()V"));
				  //s_hideSpeed = env->GetMethodID(nvEventClass, "hideSpeed", "()V"));

	s_showNotification = env->GetMethodID(nvEventClass, "showNotification","(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V"));

	s_updateSplash = env->GetMethodID(nvEventClass, "updateSplash", "(I)V"));
	s_showSplash = env->GetMethodID(nvEventClass, "showSplash", "()V"));*/





	env->DeleteLocalRef(nvEventClass);
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

CJavaWrapper* g_pJavaWrapper = nullptr;