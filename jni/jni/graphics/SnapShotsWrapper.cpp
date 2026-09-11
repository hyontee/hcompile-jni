//
// Created by admin on 02.10.2023.
//

#include "SnapShotsWrapper.h"
#include "game/CCustomPlateManager.h"
#include "main.h"
#include "util/CJavaWrapper.h"
#include "util/armhook.h"
#include "game/RW/RenderWare.h"
#include <jni.h>
#include <GLES2/gl2.h>
#include "../game/snapshothelper.h"
#include <jni.h>
#include <regex>

extern CSnapShotHelper* pSnapShotHelper;
extern CGame* pGame;

void SnapShotsWrapper::Process() {
    queueMutex.lock();

    if(!queue.empty()) {
        auto item = queue.front();
        auto id = item.id;
        auto vehNumber = item.vehNumber;
        RwTexture* tex = nullptr;

        CVector rot;
        rot.x = item.rotX;
        rot.y = item.rotY;
        rot.z = item.rotZ;

        if(item.type == 1) {
            tex = (RwTexture *) pSnapShotHelper->CreateVehicleSnapShot(
                    item.modelId,
                    0x00000000,
                    &rot,
                    item.zoom, 8, 0);
        }
        else if(item.type == 2) {
            tex = (RwTexture*)pSnapShotHelper->CreatePedSnapShot(
                    item.modelId,
                    0x00000000,
                    &rot,
                    item.zoom);
        }
        else if(item.type == 3) {
            tex = (RwTexture *) pSnapShotHelper->CreateObjectSnapShot(
                    item.modelId,
                    0x00000000,
                    &rot,
                    item.zoom);
        }
        else if(item.type == 4) {
            std::vector<std::string> patterns = {
                    "^[ABEKMHOPCTУX]{1}(?!0{3})[0-9]{3}[ABEKMHOPCTУX]{2}(\\[(?!0{2})[0-9]{2}\\]|\\[(?!0{1,3})[0-9]{2,3}\\])$", // RU
                    "^[ABCEHIKMOPTX]{2}(?!0{4})[0-9]{4}[ABCEHIKMOPTX]{2}$", // UA
                    "^(?!0{4})[0-9]{4}[ABEKMHOPCTYXI]{2}[-][1-7]{1}$", // BY
                    "^((?!0{3})[0-9]{3}|(?!0{2})[0-9]{2})[ABEKMHOPCTYXI]{2,3}\\[(?!0{2})[0-9]{2}\\]$", // KZ
                    "^[0-9]{2}[ABEKMHOPCTYXI]{2}[0-9]{3}$" // AM
            };

            Log("rendertype 4 %s", vehNumber.c_str());
            int8_t numberid;
            for (size_t i = 0; i < patterns.size(); ++i) {
                std::regex regex(patterns[i]);
                if (std::regex_match(vehNumber, regex)) {
                    numberid = i+1;
                    if(numberid == 5) numberid = 6;
                    Log("numberid %d", numberid);
                    if(numberid != -1) {
                        size_t openingBracketPos = vehNumber.find("[");
                        size_t closingBracketPos = vehNumber.find("]");

                        if (openingBracketPos != std::string::npos &&
                            closingBracketPos != std::string::npos) {
                            // Выделяем подстроку до открывающей скобки
                            std::string prefix = vehNumber.substr(0, openingBracketPos);

                            // Выделяем подстроку между скобками
                            std::string suffix = vehNumber.substr(openingBracketPos + 1,
                                                                  closingBracketPos -
                                                                  openingBracketPos - 1);

                            // Выводим результат
                            /* tex = (RwTexture *) CCustomPlateManager::createTexture(
                                     (ePlateType) numberid, (char *) prefix.c_str(),
                                     (char *) suffix.c_str());
                         }
                         else
                         {
                             tex = (RwTexture *) CCustomPlateManager::createTexture(
                                     (ePlateType) numberid,
                                     (char *) vehNumber.c_str(),
                                     "");
                         } */
                        }
                    }
                }
                else
                {
                    Log("regex dont match %d", i);
                }
            }
        }

        if(tex == nullptr)
        {
            Log("tex null");
            queue.pop();
            queueMutex.unlock();
            return;
        }

        jbyteArray array = ConvertTexToBitMapBytes(tex);

        if(array == nullptr)
        {
            Log("array null");
            queue.pop();
            tex->refCount = 0;
            RwTextureDestroy(tex);
            queueMutex.unlock();
            return;
        }

        g_pJavaWrapper->onNativeRendered(id, array, tex->raster->width , tex->raster->height);

        g_pJavaWrapper->GetEnv()->DeleteGlobalRef(array);
        queue.pop();
        tex->refCount = 0;
        RwTextureDestroy(tex);
    }

    queueMutex.unlock();
}

jbyteArray SnapShotsWrapper::ConvertTexToBitMapBytes(RwTexture* tex) {
    Log("ConvertTexToBuffer");

    auto env = g_pJavaWrapper->GetEnv();
    auto &raster = tex->raster;

    auto size = raster->width * raster->height * sizeof(RwRGBA);
    auto buffer = new uint8_t[size] ;

    uintptr_t oldTarget = *(uintptr_t*)(g_libGTASA + 0x0061B3D8);

    uintptr_t RasterExtOffset_local = *(uintptr_t*)(g_libGTASA + 0x00611844);
    uintptr_t renderTarget = *(uintptr_t*)((char*)(tex->raster) + RasterExtOffset_local + 24);

    ((void (*)(uintptr_t, bool))(g_libGTASA + 0x001A6F98 + 1))(renderTarget, true);

    CallFunction<void>(g_libGTASA + 0x0018D528 + 1, 0, 0, raster->width, raster->height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

//    auto len = sizeof(buffer);
    jbyteArray imageBytes = env->NewByteArray(size);
    env->SetByteArrayRegion(imageBytes, 0, size, reinterpret_cast<const jbyte *>(buffer));
    auto globalRef = (jbyteArray)env->NewGlobalRef(imageBytes);

    delete[] buffer;
    env->DeleteLocalRef(imageBytes);

    return globalRef;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_stage_core_ui_util_SnapShotHelper_requestSnapShot(JNIEnv *env, jobject thiz,
                                                          jint render_type, jint count,
                                                          jint model_id, jint color1, jint color2,
                                                          jfloat rot_x, jfloat rot_y, jfloat rot_z,
                                                          jfloat zoom, jbyteArray vehNumber) {
    std::lock_guard<std::mutex> lock(SnapShotsWrapper::queueMutex);

    jboolean isCopy = true;

    jbyte* pMsg = env->GetByteArrayElements(vehNumber, &isCopy);
    jsize length = env->GetArrayLength(vehNumber);

    std::string szStr((char*)pMsg, length);

    Log("requestSnapShot %s", szStr.c_str());

    SnapShotsWrapper::queue.push({
                                         count,
                                         model_id,
                                         render_type,
                                         rot_x, rot_y, rot_z,
                                         zoom, szStr
                                 });

    env->ReleaseByteArrayElements(vehNumber, pMsg, JNI_ABORT);
}