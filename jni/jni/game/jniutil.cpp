#include "jniutil.h"
#include "chat.h"


extern "C" JavaVM* javaVM;

JNIEnv* CJavaWrapper::GetEnv()
{
    JNIEnv* env = nullptr;
    int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);

    if (getEnvStat == JNI_EDETACHED)
    {
        if (javaVM->AttachCurrentThread(&env, NULL) != 0)
        {
            return nullptr;
        }
    }
    if (getEnvStat == JNI_EVERSION)
    {
        return nullptr;
    }

    if (getEnvStat == JNI_ERR)
    {
        return nullptr;
    }

    return env;
}
#include "xorstr.h"

void CJavaWrapper::ShowInventory(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 33 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowFamily(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 45 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowCases(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 73 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowBpRewards(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 74 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowRadialMenu(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 27 /* экран ID */, jsonObject);  

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowGifts(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 69 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowElectric(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 56 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowCalendar(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); 

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 71 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowAdminTools(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 66 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowMarketplace(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 77 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowCraft(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 49 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowFuel(const std::string &jsonString) 
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 2 /* экран ID */, jsonObject); 

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowDonate(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 22 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowFraction(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 46 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowTuning(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 28 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowReg(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "onPacketIncoming", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 38 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowReg2(const std::string &jsonString)
{
    JNIEnv* env = nullptr;
    javaVM->GetEnv((void**)& env, JNI_VERSION_1_6);
    if (!env) {
        return;
    }
    if (guiManagerClass != NULL) {
        jmethodID getInstanceMethod = env->GetStaticMethodID(guiManagerClass, "getInstance", "()Lcom/blackhub/bronline/game/GUIManager;");
        if (getInstanceMethod != NULL) {
            jobject guiManagerInstance = env->CallStaticObjectMethod(guiManagerClass, getInstanceMethod);

            jstring jsonJString = env->NewStringUTF(jsonString.c_str()); // Преобразуем строку JSON в JNI тип

            jmethodID showCurrentGUIMethod = env->GetMethodID(guiManagerClass, "handleFragmentScreen", "(ILorg/json/JSONObject;)V");
            if (showCurrentGUIMethod != NULL) {
                jclass jsonObjectClass = env->FindClass("org/json/JSONObject");
                if (jsonObjectClass != NULL) {
                    jmethodID constructor = env->GetMethodID(jsonObjectClass, "<init>", "(Ljava/lang/String;)V");
                    if (constructor != NULL) {
                        jobject jsonObject = env->NewObject(jsonObjectClass, constructor, jsonJString);

                        // Теперь вызываем метод Java
                        env->CallVoidMethod(guiManagerInstance, showCurrentGUIMethod, 38 /* экран ID */, jsonObject);

                        env->DeleteLocalRef(jsonObject);
                    }
                }
            }

            env->DeleteLocalRef(jsonJString);
            env->DeleteLocalRef(guiManagerInstance);
        }
    }

    env->DeleteLocalRef(guiManagerClass);

    EXCEPTION_CHECK(env);
}

CJavaWrapper::CJavaWrapper(JNIEnv* env)
{
	guiManagerClass = env->FindClass("com/blackhub/bronline/game/GUIManager");
	
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