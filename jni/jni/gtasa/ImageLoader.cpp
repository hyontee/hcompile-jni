#include "../util/patch.h"
#include "../game/scripting.h"
#include "ImageLoader.h"
#include "../obfuscate/str_obfuscate.hpp"
#include "../util/armhook.h"
#include <fstream>
#include <vector>
#include <jni.h>

const char* ImageLoader::key = OBFUSCATE("q6WcyAP4");

void decryptHeader(char* data, size_t size, const char* key, size_t key_len) {
    size_t headerSize = size < 8 ? size : 8;
    for (size_t i = 0; i < headerSize; ++i) {
        data[i] ^= key[i % key_len];
    }
}

void decryptAndSave(const char* inputPath) {
    FILE* file = fopen(inputPath, "rb+");
    if (!file) return;

    char header[8] = {0};
    if (fread(header, 1, 8, file) == 8) {
        size_t key_len = strlen(ImageLoader::key);
        char tempHeader[8];
        memcpy(tempHeader, header, 8);

        for (size_t i = 0; i < 8; ++i)
            tempHeader[i] ^= ImageLoader::key[i % key_len];

        if (tempHeader[0] == 'V' && tempHeader[1] == 'E' && tempHeader[2] == 'R') {
            fseek(file, 0, SEEK_SET);
            fwrite(tempHeader, 1, 8, file);
        } else {
            Log("file is decrypted");
        }
    }

    fclose(file);
}

void encryptAndSave(const char* inputPath) {
    FILE* file = fopen(inputPath, "rb+");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (size >= 8) {
        char header[8];
        if (fread(header, 1, 8, file) == 8) {
            size_t key_len = strlen(ImageLoader::key);
            char tempHeader[8];
            memcpy(tempHeader, header, 8);

            for (size_t i = 0; i < 8; ++i)
                tempHeader[i] ^= ImageLoader::key[i % key_len];

            if (tempHeader[0] == 'V' && tempHeader[1] == 'E' && tempHeader[2] == 'R') {
                Log("file already encrypted");
                fclose(file);
                return;
            }
            for (size_t i = 0; i < 8; ++i)
                header[i] ^= ImageLoader::key[i % key_len];

            fseek(file, 0, SEEK_SET);
            fwrite(header, 1, 8, file);
            Log("encrypt file");
        }
    }

    fclose(file);
}

int ImageLoader::AddImageToList(char const* pFileName, bool bNotPlayerImg) {
    // find a free slot
    Log("loading %s...", pFileName);
    std::int32_t fileIndex = 0;
    for (; fileIndex < 12; fileIndex++) {
        if (!ms_files[fileIndex].m_szName[0])
            break;
    }
    if (fileIndex == 12)
        return 0;
    // free slot found, load the IMG file
    Log("loaded %s, free slot found!", pFileName);
    strcpy(ms_files[fileIndex].m_szName, pFileName);
    ms_files[fileIndex].m_StreamHandle = CHook::CallFunction<int32_t>(g_libGTASA + 0x00289BA0 + 1, pFileName);
    ms_files[fileIndex].m_bNotPlayerImg = bNotPlayerImg;
    return fileIndex;
}

void ImageLoader::RemoveAllUnusedModels() {
    ((void (*) ())(g_libGTASA + 0x293325))();

    // Remove all possibly removable vehicles
//    for (int32 i = 0; i < MAX_VEHICLES_LOADED; i++) {
//        RemoveLoadedVehicle();
//    }

    // Remove majority of models with no refs

}

void ImageLoader::RequestModel(int32_t index, int32_t flags)
{
    Log("Request Model: %d(id)...", index);
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(index, flags);
}

void ImageLoader::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    ((void (*) (bool))(g_libGTASA + 0x00294CB4 + 1))(bPriorityRequestsOnly);
}

void ImageLoader::InitImageList() {
    for (auto &ms_file : ms_files) {
        ms_file.m_szName[0] = 0;
        ms_file.m_StreamHandle = 0;
    }

    decryptAndSave((std::string(g_pszStorage) + "res/main.mtk").c_str());
    //decryptAndSave((std::string(g_pszStorage) + "res/interiors.mtk").c_str());
    //decryptAndSave((std::string(g_pszStorage) + "res/player.mtk").c_str());
    //decryptAndSave((std::string(g_pszStorage) + "res/multiplayer.mtk").c_str());
    decryptAndSave((std::string(g_pszStorage) + "res/assets.mtk").c_str());

    ImageLoader::AddImageToList(OBFUSCATE("RES\\MAIN.MTK"), true);
    ImageLoader::AddImageToList(OBFUSCATE("RES\\INTERIORS.MTK"), true);

    WriteMemory(g_libGTASA + 0x5B6394, (uintptr_t)"RES\\PLAYER.MTK", 17);

    ImageLoader::AddImageToList(OBFUSCATE("RES\\MULTIPLAYER.MTK"), true);
    ImageLoader::AddImageToList(OBFUSCATE("RES\\ASSETS.MTK"), true);
}

void (*request)(int32_t index, int32_t flags);
void request_hook(int32_t index, int32_t flags){
    ImageLoader::RequestModel(index, flags);
}
void ImageLoader::InjectHooks() {
    CHook::Redirect(g_libGTASA, 0x28E83C, &ImageLoader::InitImageList);
    CHook::Write(g_libGTASA + 0x005CE80C, &ImageLoader::ms_files);
    //CHook::SetUpHook(0x28EB10 + g_libGTASA, (uintptr_t)request_hook, (uintptr_t*)&request);
}
void ImageLoader::RemoveModel(int32_t index)
{
    ((void (*) (int32_t))(g_libGTASA + 0x00290C4C + 1))(index);
}


void ImageLoader::RemoveBuildingsNotInArea(int areaCode) {
    ((void (*) (int))(g_libGTASA + 0x0028FBAC + 1))(areaCode);
}

void ImageLoader::EncryptImage() {
    // шифруем и расшифровываем тока нужные архивы, иначе оно ваще не успеет сработать из за нагрузки
    encryptAndSave((std::string(g_pszStorage) + "res/main.mtk").c_str());
    //encryptAndSave((std::string(g_pszStorage) + "res/interiors.mtk").c_str());
    //encryptAndSave((std::string(g_pszStorage) + "res/player.mtk").c_str());
    //encryptAndSave((std::string(g_pszStorage) + "res/multiplayer.mtk").c_str());
    encryptAndSave((std::string(g_pszStorage) + "res/assets.mtk").c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_rockstargames_gtasa_core_GTASA_encrypt(JNIEnv *env, jobject thiz) {
    ImageLoader::EncryptImage();
}