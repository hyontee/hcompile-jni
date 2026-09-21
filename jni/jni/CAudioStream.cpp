#include "main.h"
#include "game/game.h"

#include <pthread.h>
#include <mutex>
#include <chrono>
#include <thread>

#include "vendor/bass/bass.h"
#include "CAudioStream.h"
#include "net/netgame.h"
#include "/util/CJavaWrapper.h"
#include "chatWindow.h"

extern CGame* pGame;
extern CNetGame* pNetGame;
extern CChatWindow* pChatWindow;

CAudioStream::CAudioStream()
{
	m_strAudioStreamURL.clear();

	m_fAudioStreamRadius = -1;
	m_bassStreamSample = NULL;

	m_bUsePosition = false;
	m_bPlaying = false;
	m_bPaused = false;
    m_bOffMusic = false;
    m_fVolume = 1.0f;
    vehicleid;

	memset(&m_vecAudioStreamPosition, 0, sizeof(m_vecAudioStreamPosition));
}

CAudioStream::~CAudioStream()
{
	if (m_bPlaying) Stop();
	m_strAudioStreamURL.clear();
}

bool CAudioStream::PlayByURL(uint16_t index, uint8_t state, uint16_t vehid, const char* url, float fX, float fY, float fZ, float fRadius, bool bUsePosition, float volume)
{
    if(m_bOffMusic) return false;
	if (m_bPlaying && m_bassStreamSample)
	{
		if (!Stop())
			return false;
	}
    vehicleid = vehid;
	m_strAudioStreamURL = url;
    m_fVolume = volume;

	m_vecAudioStreamPosition.X = fX;
	m_vecAudioStreamPosition.Y = fY;
	m_vecAudioStreamPosition.Z = fZ;

	m_fAudioStreamRadius = fRadius;
	m_bUsePosition = bUsePosition;

	m_bPaused = false;
    m_bPausedPlay = false;
    StateMusic = state;
    playerid = index;

	m_bassStreamSample = BASS_StreamCreateURL(m_strAudioStreamURL.c_str(), 0, 9699328, nullptr, nullptr);
	if (m_bassStreamSample)
	{
		BASS_ChannelPlay(m_bassStreamSample, true);
		m_bPlaying = true;
	}

	return true;
}

bool CAudioStream::Stop()
{
	if (m_bPlaying && m_bassStreamSample)
	{
		BASS_StreamFree(m_bassStreamSample);
		m_bassStreamSample = NULL;
		m_bPlaying = false;
	}
	return true;
}

void CAudioStream::Pause()
{

    m_bPausedPlay = true;

}
void CAudioStream::Resume(){

    m_bPausedPlay = false;

}
void CAudioStream::SetVolume(float volume)
{
    if(volume > 1.0f)
        volume = 1.0f;

    m_fVolume = volume;
}
inline float DistanceBetweenPoints3D(const VECTOR& vecPosition1, const VECTOR& vecPosition2)
{
   

    return 0.0f;
}
void CAudioStream::Process()
{
	

}