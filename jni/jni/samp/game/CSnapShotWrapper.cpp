//
// Created by Error on 18.10.2024.
//

#include "CSnapShotWrapper.h"
#include "CSnapShots.h"
#include "CCustomPlateManager.h"

extern CJavaWrapper* g_pJavaWrapper;
jclass CSnapShotWrapper::clazz = nullptr;

bool CSnapShotWrapper::isProcessing = false;


uint32_t CSnapShotWrapper::PlateItem::dwType = 0;
char CSnapShotWrapper::PlateItem::szRegion[10];
char CSnapShotWrapper::PlateItem::szNumber[30];

std::queue<CSnapShotWrapper::QueueItem*> CSnapShotWrapper::list;

std::mutex CSnapShotWrapper::queueMutex;


CSnapShotWrapper::QueueItem *CSnapShotWrapper::queueItem;



jbyteArray CSnapShotWrapper::ConvertTextureToByte(RwTexture * texture, bool isTexture) {
    Log(__FUNCTION__ );
    JNIEnv* pEnv = g_pJavaWrapper->GetEnv();
    uint32_t dest = 512 * 512 * 4;
    jbyte* bytes = new jbyte[dest];
    uintptr_t textureBuffer = (uintptr_t)texture->raster;

    (( void (*)(uintptr_t, int))(g_libGTASA + 0x001D0958 + 1))(*(uintptr_t *)(textureBuffer + *(uintptr_t *)(g_libGTASA + 0x6B31FC) + 0x18), isTexture);

    (( void (*)(int, int, int, int, uintptr_t, uintptr_t, jbyte*))(g_libGTASA+0x1B4C20+1))(0, 0, 512, 512, 0x1908, 0x1401, bytes);

    jbyteArray jByteArray = pEnv->NewByteArray(dest);
    pEnv->SetByteArrayRegion(jByteArray, 0, dest, bytes);
    delete bytes;

    return jByteArray;
}

void CSnapShotWrapper::SetToImageView(uint32_t id, jbyteArray jByteArray, uint32_t width, uint32_t height,
                                      jobject imageview) {
    Log("SetToImageView");
    JNIEnv* env = g_pJavaWrapper->GetEnv();

    jmethodID method = env->GetStaticMethodID(CSnapShotWrapper::clazz, "setToImageView", "(Landroid/widget/ImageView;[BII)V");
    env->CallStaticVoidMethod(CSnapShotWrapper::clazz, method, imageview, jByteArray, width, height);//поебота крашит наху
    CSnapShotWrapper::isProcessing = false;
    Log("end set image");
}

void CSnapShotWrapper::Process() {
    if (!CSnapShotWrapper::list.empty()) {

        CSnapShotWrapper::queueMutex.lock();
        CSnapShotWrapper::QueueItem *temp = CSnapShotWrapper::list.front();

        bool isTexture;

        RwTexture *snapshot = nullptr;
        jbyteArray bytes;
        switch (temp->type) {

            case SNAP_PED: {
                snapshot = CSnapShots::CreatePedSnapShot(temp->id,
                                                         000000,
                                                         &temp->vecRot,
                                                         temp->zoom);
                isTexture = true;
                break;
            }
            case SNAP_CAR: {
                snapshot = CSnapShots::CreateVehicleSnapShot(
                        temp->id,
                        000000,
                        &temp->vecRot,
                        temp->zoom,
                        temp->color1,
                        temp->color2);
                isTexture = true;
                break;
            }
            case SNAP_OBJ: {
                snapshot = CSnapShots::CreateObjectSnapShot(
                        temp->id,
                        000000,
                        &temp->vecRot,
                        &temp->vecOffset);
                isTexture = true;
                break;
            }
            case SNAP_TEXTURE: {
                char textName[12];

                sprintf(textName, "snap_%d", temp->id);
                Log(textName);
                snapshot = CSnapShots::CreateTextureSnapShot(textName);
                isTexture = true;
                break;
            }
            case SNAP_PLATE: {
                /*snapshot = CCustomPlateManager::GetSnapPlate(
                        CSnapShotWrapper::PlateItem::dwType,
                        CSnapShotWrapper::PlateItem::szNumber,
                        CSnapShotWrapper::PlateItem::szRegion);*/
                isTexture = true;
                break;
            }
            case SNAP_PLAYER: {
                snapshot = CSnapShots::CreatePlayerSnapShot(&temp->vecRot,
                                                         temp->zoom);
                isTexture = true;
                break;
            }
            default: {
                snapshot = nullptr;
                isTexture = false;
                CSnapShotWrapper::isProcessing = false;
                break;
            }
        }


        if (snapshot) {
            bytes = CSnapShotWrapper::ConvertTextureToByte(snapshot, isTexture);

            CSnapShotWrapper::SetToImageView(temp->jid, bytes,
                                             snapshot->raster->width,
                                             snapshot->raster->height,
                                             temp->ImageView);
        } else {
            Log("No generated tex for snap. snapshot = %d type = %d, model = %d",
                snapshot, temp->type, temp->id);
            CSnapShotWrapper::isProcessing = false;
            CSnapShotWrapper::list.pop();
            return;
        }
        CSnapShotWrapper::list.pop();
        snapshot->refCount = 0;
        RwTextureDestroy(snapshot);

        CSnapShotWrapper::queueMutex.unlock();

    }
}


CSnapShotWrapper::QueueItem::QueueItem() {

}

