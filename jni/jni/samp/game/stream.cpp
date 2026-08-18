/*

SA:MP Multiplayer Modification
Copyright 2004-2005 SA:MP Team

Version: $Id: textdraw.cpp,v 1.4 2008-04-16 08:54:17 kyecvs Exp $

*/

#include "../main.h"
#include "game.h"
#include "font.h"
#include "stream.h"

#include <vector>
#include <string>
#include <thread>
#include <sstream>

extern CGame* pGame;
#include "..//net/netgame.h"
#include "..//util/CJavaWrapper.h"

extern CNetGame* pNetGame;

void CStream::CreateStream() // ready
{

    m_hStream = BASS_StreamCreateURL(m_szUrl, 0, BASS_SAMPLE_3D | BASS_SAMPLE_MONO | BASS_SAMPLE_LOOP | 0x940000, nullptr, nullptr);

    BASS_3DVECTOR vec(m_vPos.x, m_vPos.y, m_vPos.z);

    BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);

    BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);

    BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);

    BASS_ChannelSet3DAttributes(m_hStream, BASS_3DMODE_NORMAL, (m_fDistance * 0.1f), m_fDistance, 360, 360, 0);

    BASS_Apply3D();

    BASS_ChannelPlay(m_hStream, false);
}

void CStream::DestroyStream() // ready
{
    if (m_hStream)
    {
        BASS_StreamFree(m_hStream);
    }
    m_hStream = NULL;
}

void CStream::ProcessAttached() // ready
{
    if (!pNetGame)
    {
        return;
    }
    if (!pNetGame->GetPlayerPool() || !pNetGame->GetVehiclePool())
    {
        return;
    }
    if(m_iAttachType == 0) return;
    if (m_iAttachType == 1) // vehicle
    {
        CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(m_iAttachedTo);
        if (!pVeh)
        {
            return;
        }
        m_vPos = pVeh->m_pVehicle->GetPosition();
        //CChatWindow::AddDebugMessage("processed for vehicle %d", m_iAttachedTo);
    }
    if (m_iAttachType == 2) // player
    {
        CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
        if(!pPlayerPool) return;

        CPlayerPed* pPed;

        if (m_iAttachedTo == pPlayerPool->GetLocalPlayerID()) {
            pPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
        }
        else
        {
            if(pPlayerPool->GetAt(m_iAttachedTo))
                pPed = pPlayerPool->GetAt(m_iAttachedTo)->GetPlayerPed();
        }

        if (!pPed) return;


        m_vPos = pPed->m_pPed->GetPosition();
        //CChatWindow::AddDebugMessage("processed for player %d", m_iAttachedTo);
    }

    BASS_3DVECTOR vec(m_vPos.x, m_vPos.y, m_vPos.z);
    BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);
    BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);

    BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);
}

CStream::CStream(CVector pPos, int iInterior, float fDistance, const char* szUrl) // ready
{
    m_bIsDeactivated = false;
    m_bIsAttached = false;
    m_iAttachType = 0;
    m_iAttachedTo = 0;

    m_hStream = NULL;

    strcpy(&m_szUrl[0], szUrl);

    m_vPos = pPos;

    m_iInterior = iInterior;
    m_fDistance = fDistance;
}

CStream::~CStream() // ready
{
    DestroyStream();
}

void CStream::AttachToVehicle(int iVehicleID) // ready
{
    m_bIsAttached = true;
    m_iAttachType = 1;
    m_iAttachedTo = iVehicleID;
}

void CStream::AttachToPlayer(int iPlayerID) // ready
{
    m_bIsAttached = true;
    m_iAttachType = 2;
    m_iAttachedTo = iPlayerID;
}
void CStream::DeAttachStream(){
    m_bIsAttached = false;
    m_iAttachType = 0;
    m_iAttachedTo = -1;
}

void CStream::SetVolume(float fValue)
{
    BASS_ChannelSetAttribute(m_hStream, BASS_ATTRIB_VOL, fValue);
}
void CStream::Pause(){
    BASS_ChannelPause(m_hStream);
}
void CStream::Play(){
    BASS_ChannelPlay(m_hStream, false);
}
void CStream::SetIsDeactivated(bool bIsDeactivated)
{
    m_bIsDeactivated = bIsDeactivated;
}

void CStream::Process(RwMatrix* pMatListener) // todo
{
    CVector pos;
    pos.x = pMatListener->pos.x;
    pos.y = pMatListener->pos.y;
    pos.z = pMatListener->pos.z;

    ProcessAttached();

    float fDistance = GetDistanceBetween3DPoints(&pos, &m_vPos);
    if (fDistance <= m_fDistance && !m_hStream)
    {
        //CChatWindow::AddDebugMessage("create stream");
        CreateStream();
    }

    float fDistDiff = m_fDistance - 5.0f;

    float fVolume;
    if (fDistance <= 5.0f)
        fVolume = 1.0f;
    else if (fDistance >= m_fDistance)
        fVolume = 0.0f;
    else
        fVolume = exp(-(fDistance - 5.0f) * (5.0f / fDistDiff));

    BASS_ChannelSetAttribute(m_hStream, BASS_ATTRIB_VOL, fVolume);
}

void CStream::SetPosition(CVector vvec)
{
    BASS_3DVECTOR vec(vvec.x, vvec.y, vvec.z);
    BASS_3DVECTOR orient(0.0f, 0.0f, 0.0f);
    BASS_3DVECTOR vel(0.0f, 0.0f, 0.0f);

    m_vPos = vvec;

    BASS_ChannelSet3DPosition(m_hStream, &vec, &orient, &vel);
    BASS_ChannelSet3DAttributes(m_hStream, BASS_3DMODE_OFF, 1.0f, m_fDistance, 360, 360, 1.0f);
}
void CStream::UpdateVolume(float volume)
{
    BASS_ChannelSetAttribute(m_hStream, BASS_ATTRIB_VOL, volume);
}