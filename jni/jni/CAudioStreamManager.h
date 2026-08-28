//
// Created by Error on 09.06.2024.
//

#ifndef BR_CAUDIOSTREAMMANAGER_H
#define BR_CAUDIOSTREAMMANAGER_H
#include "vendor/bass/bass.h"
#include "/util/CJavaWrapper.h"
#define MAX_SOUNDS (1024)
class CAudioStream;
class CAudioStreamManager {
private:
    CAudioStream* mAudioStream[MAX_SOUNDS];
    static CAudioStreamManager* instance;

public:
    CAudioStreamManager();
    ~CAudioStreamManager();
    float m_fVolume;
    void Process();
    void PauseAll();
    void UnpauseAll();

    CAudioStream* getAt(uint16_t index);

    static CAudioStreamManager* getInstance();

    void PlayURL(uint16_t index, uint8_t state, uint16_t vehid, const char *url, float fX, float fY, float fZ, float fRadius, bool bUsePosition);

    void Pause(uint16_t index);

    void Stop(uint16_t index);

    void StopAll();

    void Resume(uint16_t index);
    void SetVoulme(float voulme);

    void SetPut(uint16_t index, uint8_t state);
};


#endif //BR_CAUDIOSTREAMMANAGER_H
