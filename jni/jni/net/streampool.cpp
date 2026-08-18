#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame* pGame;
DataStructures::SingleProducerConsumer<BUFFERED_COMMAND_STREAMPOOL> CStreamPool::bufferedCommands;
bool CStreamPool::m_bIsDeactivated;
bool CStreamPool::bShutdownThread = false;

CStreamPool::CStreamPool()
{
    CStreamPool::m_bIsDeactivated = false;
    for (int i = 0; i < MAX_STREAMS; i++)
    {
        m_pStreams[i] = nullptr;
        m_bSlotState[i] = false;
    }
    m_hIndividualStream = NULL;
    CStreamPool::bShutdownThread = false;
    m_bWasPaused = false;
    m_bIndividualNeedReplay = 0;
    m_szIndividualLastLink[0] = 0;

// --- ДОБАВЛЕНО: статический аудио-стрим вокзала ---
    addStaticStream(2747.9321f, -2451.045654f, 21.689149f,
            /*interior*/ 0, /*dist*/ 100.0f, // радиус слышимости увеличен
                    "http://wh27264.web2.maze-tech.ru/vokzal.mp3");
// --- КОНЕЦ ДОБАВЛЕНОГО ---
    SetStreamVolume(0, 0.8f); // тише (0.3 = 30% громкости)

    pThread = new std::thread([this]
                              {
                                  while (!CStreamPool::bShutdownThread)
                                  {
                                      CStreamPool::Process();
                                      std::this_thread::sleep_for(std::chrono::milliseconds(100));
                                  }
                              });
}


CStreamPool::~CStreamPool()
{
    for (int i = 0; i < MAX_STREAMS; i++)
    {
        DeleteStreamByID(i);
    }
    StopIndividualStream();
    CStreamPool::bShutdownThread = true;
    pThread->join();
    delete pThread;
}

void CStreamPool::Deactivate()
{
    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = 1;

    CStreamPool::m_bIsDeactivated = true;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ACTIVATE;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::UpdateAllVolumes(){
    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_UPDATE;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::Activate()
{
    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = 2;

    CStreamPool::m_bIsDeactivated = false;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ACTIVATE;

    CStreamPool::bufferedCommands.WriteUnlock();
}

CStream* CStreamPool::GetStream(int iID)
{
    if (iID > 0 && iID <= MAX_STREAMS)
    {
        return m_pStreams[iID];
    }
    return nullptr;
}

void CStreamPool::addStaticStream(float x, float y, float z, int interior, float dist, const char* url) {
    CVector vec{x, y, z};
    m_pStaticStream.push_back( new CStream(&vec, interior, dist, url) );
}

CStream* CStreamPool::AddStream(int iID, CVector pPos, int iVirtualWorld, PLAYERID playerid, VEHICLEID vehicleid, int iInterior, float fDistance, const char* szUrl) // ready
{
    if (iID < 0 || iID >= MAX_STREAMS) return nullptr;

    CStream* pStream = new CStream(pPos, iInterior, fDistance, szUrl);

    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;
    bc->pData = (const void*)pStream;
    bc->iPlayerID = playerid;
    bc->iVehicleID = vehicleid;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ADDSTREAM;

    CStreamPool::bufferedCommands.WriteUnlock();
    return nullptr;
}

void CStreamPool::DeleteStreamByID(int iID)
{
    if (iID < 0 || iID >= MAX_STREAMS) return;

    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;
    bc->pData = nullptr;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_DELETSTREAM;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::PauseStream(int iID){
    if (iID < 0 || iID >= MAX_STREAMS) return;
    BUFFERED_COMMAND_STREAMPOOL *bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;


    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_PAUSE;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::PlayStream(int iID){
    if (iID < 0 || iID >= MAX_STREAMS) return;
    BUFFERED_COMMAND_STREAMPOOL *bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;


    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_PLAY;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::AttachToPlayer(int iID, PLAYERID id) {
    if (iID < 0 || iID >= MAX_STREAMS) return;
    if(id == -1) return;
    BUFFERED_COMMAND_STREAMPOOL *bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;
    bc->iPlayerID = id;
    bc->iType = 1;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ATTACH;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::AttachToVehicle(int iID, VEHICLEID id){
    if (iID < 0 || iID >= MAX_STREAMS) return;
    if(id == -1) return;
    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;
    bc->iVehicleID = id;
    bc->iType = 2;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ATTACH;

    CStreamPool::bufferedCommands.WriteUnlock();
}
void CStreamPool::PlayIndividualStream(const char* szUrl, int type)
{
    if (m_hIndividualStream)
    {
        StopIndividualStream();
    }
    m_hIndividualStream = BASS_StreamCreateURL(szUrl, 0, type, nullptr, nullptr);

    strcpy(&m_szIndividualLastLink[0], szUrl);
    m_bIndividualNeedReplay = type;

    BASS_ChannelPlay(m_hIndividualStream, false);
}

void CStreamPool::StopIndividualStream()
{
    if (m_hIndividualStream)
    {
        BASS_StreamFree(m_hIndividualStream);
    }
    m_szIndividualLastLink[0] = 0;
    m_bIndividualNeedReplay = 0;
    m_hIndividualStream = NULL;
}
void CStreamPool::DeAttachStreamByID(int iID){
    if (iID < 0 || iID >= MAX_STREAMS) return;

    BUFFERED_COMMAND_STREAMPOOL* bc;
    bc = CStreamPool::bufferedCommands.WriteLock();

    bc->iID = iID;
    bc->iVehicleID = -1;
    bc->iPlayerID = -1;
    bc->iType = 0;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_DEATTACH;

    CStreamPool::bufferedCommands.WriteUnlock();
}

void CStreamPool::SetStreamVolume(int iID, float fVolume)
{
    BUFFERED_COMMAND_STREAMPOOL* bc;

    bc = CStreamPool::bufferedCommands.WriteLock();

    float* pVolume = new float;

    *pVolume = fVolume;

    bc->iID = iID;
    bc->pData = (const void*)pVolume;

    bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_VOLUME;

    CStreamPool::bufferedCommands.WriteUnlock();
}

#include "netgame.h"

#include "game/Timer.h"

#include "util/CJavaWrapper.h"

#include <jni.h>

extern CNetGame* pNetGame;

void CStreamPool::Process()
{
    if (CTimer::m_UserPause)
    {
        for(auto pStream : m_pStaticStream) {
            pStream->SetIsDeactivated(true);
        }
        for (int i = 0; i < MAX_STREAMS; i++)
        {
            if (m_pStreams[i] && m_bSlotState[i])
            {
                m_pStreams[i]->SetIsDeactivated(true);
            }
        }
        char temp[256];
        bool bCopied = false;
        if (m_szIndividualLastLink[0])
        {
            strcpy(&temp[0], &m_szIndividualLastLink[0]);

            if(m_bIndividualNeedReplay != BASS_STREAM_AUTOFREE)
                bCopied = true;
        }
        StopIndividualStream();
        if (bCopied)
        {
            strcpy(&m_szIndividualLastLink[0], &temp[0]);
        }

        m_bWasPaused = true;
    }
    else
    {
        if (m_bWasPaused)
        {
            for(auto pStream : m_pStaticStream) {
                pStream->SetIsDeactivated(false);
            }
            for (int i = 0; i < MAX_STREAMS; i++)
            {
                if (m_pStreams[i] && m_bSlotState[i])
                {
                    m_pStreams[i]->SetIsDeactivated(false);
                }
            }
            if (m_szIndividualLastLink[0])
            {
                PlayIndividualStream(&m_szIndividualLastLink[0], m_bIndividualNeedReplay);
            }
            m_bWasPaused = false;
        }
    }

    BUFFERED_COMMAND_STREAMPOOL* bcs;
    if (!m_bWasPaused)
    {
        while ((bcs = CStreamPool::bufferedCommands.ReadLock()) != 0)
        {
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_ADDSTREAM)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    delete m_pStreams[bcs->iID];
                }
                m_pStreams[bcs->iID] = (CStream*)bcs->pData;

                m_bSlotState[bcs->iID] = true;
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_DELETSTREAM)
            {
                delete m_pStreams[bcs->iID];
                m_bSlotState[bcs->iID] = false;
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_ATTACH)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    if (bcs->iType == 1)
                    {
                        m_pStreams[bcs->iID]->AttachToPlayer(bcs->iPlayerID);
                    }
                    if (bcs->iType == 2)
                    {
                        m_pStreams[bcs->iID]->AttachToVehicle(bcs->iVehicleID);
                    }
                }
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_DEATTACH)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    m_pStreams[bcs->iID]->DeAttachStream();
                }
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_VOLUME)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    float* pVolume = (float*)bcs->pData;

                    m_pStreams[bcs->iID]->SetVolume(*pVolume);

                    delete pVolume;
                }
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_PAUSE)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    m_pStreams[bcs->iID]->Pause();
                }
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_PLAY)
            {
                if (m_pStreams[bcs->iID] && m_bSlotState[bcs->iID])
                {
                    m_pStreams[bcs->iID]->Play();
                }
            }
            if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_UPDATE)
            {
                for (int i = 0; i < MAX_STREAMS; i++)
                {
                    if (m_pStreams[i] && m_bSlotState[i])
                    {
                        m_pStreams[i]->SetVolume(1.0f);
                    }
                }
            }
            CStreamPool::bufferedCommands.ReadUnlock();
        }
    }
    
    int value = 10000;
    
    if(pNetGame) {
        CPlayerPool* playerPool = pNetGame->GetPlayerPool();
        if(!playerPool) return;
        
        CLocalPlayer* localPlayer = playerPool->GetLocalPlayer();
        if(!localPlayer) return;
        
        if (localPlayer->m_bDontSendSync || CTimer::m_UserPause) {
            value = 0;
        } else {
            value = 10000;
        }
    }
    
    BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, value);
    RwMatrix matLocal;
    pGame->FindPlayerPed()->GetMatrix(&matLocal);

    for (int i = 0; i < MAX_STREAMS; i++)
    {
        if (m_bSlotState[i] && m_pStreams[i])
        {
            m_pStreams[i]->Process(&matLocal);
        }
    }
    for(auto pStream : m_pStaticStream) {
        pStream->Process(&matLocal);
    }

    CCamera *pCamera = pGame->GetCamera();
    uintptr_t * TheCamera = pCamera->TheCamera;
    RwMatrix* CamMatrix = pCamera->m_matPos;
    CVector CamPos = CamMatrix->pos;
    if ( !CamMatrix )
        CamPos = g_libGTASA + 0x951FAC;

    BASS_3DVECTOR front(CamMatrix->at.x, CamMatrix->at.y, CamMatrix->at.z);
    BASS_3DVECTOR top(CamMatrix->up.x, CamMatrix->up.y, CamMatrix->up.z);
    BASS_3DVECTOR pos(CamPos.x, CamPos.y, CamPos.z);
    BASS_Set3DPosition(&pos, nullptr, &front, &top);

    BASS_Apply3D();
}