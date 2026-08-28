//
// Created by Error on 09.06.2024.
//
#include "main.h"
#include "CAudioStreamManager.h"
#include "CAudioStream.h"
#include "settings.h"

extern CSettings* pSettings;
CAudioStreamManager* CAudioStreamManager::instance = nullptr;

CAudioStreamManager::CAudioStreamManager()
{
    m_fVolume = pSettings->GetWrite().fjblVolume;
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        mAudioStream[i] = nullptr;
    }
}
CAudioStreamManager::~CAudioStreamManager()
{
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (mAudioStream != nullptr)
        {
            delete mAudioStream[i];
            mAudioStream[i] = nullptr;
        }
    }
}
CAudioStreamManager* CAudioStreamManager::getInstance()
{
    if (instance == nullptr)
        instance = new CAudioStreamManager();

    return instance;
}
CAudioStream* CAudioStreamManager::getAt(std::uint16_t index)
{
    if (index < 0 || index > MAX_SOUNDS - 1)
        return nullptr;

    return mAudioStream[index];
}

void CAudioStreamManager::Process()
{
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (mAudioStream[i] != nullptr)
        {
            mAudioStream[i]->Process();
        }
    }
}

void CAudioStreamManager::PauseAll()
{
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (mAudioStream[i] != nullptr)
        {
            mAudioStream[i]->Pause();
        }
    }
}
void CAudioStreamManager::Pause(uint16_t index){
    if (mAudioStream[index] != nullptr)
    mAudioStream[index]->Pause();
}
void CAudioStreamManager::Resume(uint16_t index){
    if (mAudioStream[index] != nullptr)
    mAudioStream[index]->Resume();
}
void CAudioStreamManager::Stop(uint16_t index){
    if (mAudioStream[index] != nullptr)
    delete mAudioStream[index];
}
void CAudioStreamManager::SetPut(uint16_t index, uint8_t state){
    if (mAudioStream[index] != nullptr)
    mAudioStream[index]->StateMusic = state;
}
void CAudioStreamManager::StopAll(){
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (mAudioStream[i] != nullptr)
        {
            delete mAudioStream[i];
        }
    }
}
void CAudioStreamManager::PlayURL(uint16_t index, uint8_t state, uint16_t vehid, const char* url, float fX, float fY, float fZ, float fRadius, bool bUsePosition){
    if (index < 0 || index > MAX_SOUNDS - 1)
        return;

    if (mAudioStream[index])
        delete mAudioStream[index];
    Log("AudioStream:PlayURL");
    mAudioStream[index] = new CAudioStream();
    mAudioStream[index]->PlayByURL(index, state, vehid, url, fX, fY, fZ, fRadius, bUsePosition, m_fVolume);
}
void CAudioStreamManager::SetVoulme(float volume) {

    m_fVolume = volume;
    for (int i = 0; i < MAX_SOUNDS; i++)
    {
        if (mAudioStream[i] != nullptr)
        {
            mAudioStream[i]->SetVolume(volume);
        }
    }
}
