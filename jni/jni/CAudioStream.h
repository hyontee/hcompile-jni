#pragma once

#include <atomic>
#define PLAYER 2
#define VEHICLE 1
#define COORDS 0
class CAudioStream
{
private:
	std::string					m_strAudioStreamURL;
	VECTOR						m_vecAudioStreamPosition;
	float						m_fAudioStreamRadius;
    float                       m_fVolume;

	bool						m_bUsePosition;
	HSTREAM						m_bassStreamSample;

	bool						m_bPlaying;
	bool						m_bPaused;
    bool						m_bPausedPlay;
    bool                        m_bOffMusic;
    uint16_t vehicleid;
    PLAYERID playerid;

public:
	CAudioStream();
	~CAudioStream();
    uint8_t 					StateMusic;
	bool PlayByURL(uint16_t index, uint8_t state, uint16_t vehid, const char* url, float fX, float fY, float fZ, float fRadius, bool bUsePosition, float volume);
	bool Stop();
	void Process();

	void AsyncAudioStreamSampleProcessing();

    void Pause();

    void SetVolume(float volume);

    void Resume();
};