#include <jni.h>

#include "../main.h"
#include "snapshothelper.h"
#include "../interface/hud.h"
#include "../game/RW/RenderWare.h"
extern CSnapShotHelper* pSnapShotHelper;

extern "C" {
JNIEXPORT jbyteArray Java_com_nvidia_devtech_NvEventQueueActivity_requestSnapShotVehicle(JNIEnv *env, jobject obj, jint iModel, jint dwColor, jint dwColor1, jint dwColor2) {

    VECTOR rotation = {20.0f, 180.0f, 45.0f};

    uintptr_t result = pSnapShotHelper->CreateVehicleSnapShot(iModel, dwColor, &rotation, 0.78f, dwColor1, dwColor2);

    int size = sizeof(result);
    Log("[CGameRender] Size = %d", size);

    /*uintptr_t value = result;
    jbyteArray byteArray = env->NewByteArray(size);

    jbyte* buffer = env->GetByteArrayElements(byteArray, nullptr);

    memcpy(buffer, &value, size);

    return byteArray;*/

    // Получите указатель на данные RwTexture
    //uintptr_t rwTextureData = result;

    // Определите размер буфера для jbyteArray
    //size_t bufferSize = 512 * 512 * 4;

    // Создайте новый jbyteArray с определенным размером буфера
    //jbyteArray pixelDataArray = env->NewByteArray(bufferSize);

    // Получите указатель на буфер jbyteArray
    //jbyte* pixelData = env->GetByteArrayElements(pixelDataArray, NULL);

    // Скопируйте данные пикселей RwTexture в буфер jbyteArray
    //memcpy(pixelData, (void*)rwTextureData, bufferSize);

    // Освободите указатель на буфер jbyteArray
    //env->ReleaseByteArrayElements(pixelDataArray, pixelData, 0);

    // Верните jbyteArray из JNI в Java
    //return pixelDataArray;
    // Получаем доступ к объекту RwTexture

    size_t bufferSize = 512 * 512 * 4;
    jbyteArray byteArray = env->NewByteArray(bufferSize);

    return byteArray;

    }
    JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_createSnapShotVehicle(JNIEnv *env, jobject obj, jint iModel, jint dwColor, jint dwColor1, jint dwColor2) {

    VECTOR rotation = {20.0f, 180.0f, 45.0f};

    uintptr_t result = pSnapShotHelper->CreateVehicleSnapShot(iModel, dwColor, &rotation, 0.78f, dwColor1, dwColor2);

    Log("[CGameRender] by EDGAR 3.0");
    int size = sizeof(result);
    Log("[CGameRender] Size = %d", size);
    Hud::SetTexture((RwTexture*) result);

    }
    JNIEXPORT jintArray JNICALL Java_ru_edgar_space_GameRender_createVehicleSnapshotTex
    (JNIEnv *env, jobject obj, jint iModel, jint dwColor, jfloatArray vecRot, jfloat fZoom, jint dwColor1, jint dwColor2) {
    // Extract vecRot values from jfloatArray
    jfloat *elements = env->GetFloatArrayElements(vecRot, 0);
    VECTOR rotation = { elements[0], elements[1], elements[2] };

    // Call the existing snapshot function
    uintptr_t result = pSnapShotHelper->CreateVehicleSnapShot(iModel, dwColor, &rotation, fZoom, dwColor1, dwColor2);

    // Convert the 'result' to byte array for Java
    jintArray buffer = env->NewIntArray(sizeof(result));
    env->SetIntArrayRegion(buffer, 0, sizeof(result), (jint*)&result);

    // Release the float array
    env->ReleaseFloatArrayElements(vecRot, elements, 0);

    return buffer;
    }
}

//Log("[CGameRender] Queue %d %d %d", a3, a4, a5);
/* native byte[] createVehicleSnapshotTex(int iModel, int dwColor, float[] vecRot, float fZoom, int dwColor1, int dwColor2); */
