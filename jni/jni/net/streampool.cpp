#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CGame* pGame;

extern int loaded_libBass;
CStreamPool::CStreamPool() // ready
{
	m_bIsDeactivated = false;
	for (int i = 0; i < MAX_STREAMS; i++)
	{
		m_pStreams[i] = nullptr;
		m_bSlotState[i] = false;
		m_bIsCreated[i] = false;
	}
	m_hIndividualStream = NULL;
	bShutdownThread = false;
	m_bWasPaused = false;
	m_szIndividualLastLink[0] = 0;
	m_StreamList.clear();
	/*pThread = new std::thread([this]
		{
			while (!bShutdownThread)
			{
				Process();
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
			}
		}
	);*/
}

CStreamPool::~CStreamPool() // ready
{
	for (int i = 0; i < MAX_STREAMS; i++)
	{
		DeleteStreamByID(i);
	}
	m_StreamList.clear();
	StopIndividualStream();
	bShutdownThread = true;
	pThread->join();
	delete pThread;
}

void CStreamPool::Deactivate()
{
	if (!loaded_libBass) return;
	BUFFERED_COMMAND_STREAMPOOL* bc;
	bc = bufferedCommands.WriteLock();

	bc->iID = 1;

	m_bIsDeactivated = true;

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ACTIVATE;

	bufferedCommands.WriteUnlock();
}

void CStreamPool::Activate()
{
	if (!loaded_libBass) return;
	BUFFERED_COMMAND_STREAMPOOL* bc;
	bc = bufferedCommands.WriteLock();

	bc->iID = 2;

	m_bIsDeactivated = false;

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ACTIVATE;

	bufferedCommands.WriteUnlock();
}

CStream* CStreamPool::AddStream(int iID, int iType, int iPlayerID, VECTOR* pPos, int iVirtualWorld, int iInterior, float fDistance, const char* szUrl) // ready
{
	if (!loaded_libBass) return nullptr;
	if (iID < 0 || iID >= MAX_STREAMS) return nullptr;
	
	CStream* pStream = new CStream(pPos, iVirtualWorld, iInterior, fDistance, szUrl);

	if (m_bIsDeactivated)
	{
		pStream->SetIsDeactivated(true);
	}

	BUFFERED_COMMAND_STREAMPOOL* bc;
	bc = bufferedCommands.WriteLock();

	bc->iID = iID;
	bc->iType = iType;
	bc->iPlayerID = iPlayerID;
	bc->pData = (const void*)pStream;
	SetStreamVolume(iID, pSettings->GetWrite().iVolumeStream);
	m_bIsCreated[iID] = true;

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ADDSTREAM;

	bufferedCommands.WriteUnlock();
	
	AttachedStream(iID, iType, iPlayerID);
	
	m_StreamList.push_back(iID);
	return nullptr;
}

void CStreamPool::AttachedStream(int iID, int iType, int iPlayerID)
{
	if (!loaded_libBass) return;
	BUFFERED_COMMAND_STREAMPOOL* bc;
	bc = bufferedCommands.WriteLock();

	bc->iID = iID;
	bc->iType = iType;
	bc->iPlayerID = iPlayerID;

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_ATTACH;

	bufferedCommands.WriteUnlock();
}

void CStreamPool::DeleteStreamByID(int iID) // ready
{
	if (!loaded_libBass) return;
	if (iID < 0 || iID >= MAX_STREAMS) return;
	if (m_bIsCreated[iID] == false) return;

	BUFFERED_COMMAND_STREAMPOOL* bc;
	bc = bufferedCommands.WriteLock();

	bc->iID = iID;
	bc->pData = nullptr;
	m_bIsCreated[iID] = false;
	
	m_StreamList.remove(iID);

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_DELETSTREAM;

	bufferedCommands.WriteUnlock();
}

void CStreamPool::PlayIndividualStream(const char* szUrl) // ready
{
	if (!loaded_libBass) return;
	if (m_hIndividualStream)
	{
		StopIndividualStream();
	}
	m_hIndividualStream = BASS_StreamCreateURL(szUrl, 0, BASS_SAMPLE_LOOP, NULL, 0);

	strcpy(&m_szIndividualLastLink[0], szUrl);

	BASS_ChannelPlay(m_hIndividualStream, false);
}

void CStreamPool::StopIndividualStream() // ready
{
	if (m_hIndividualStream)
	{
		BASS_StreamFree(m_hIndividualStream);
	}
	m_szIndividualLastLink[0] = 0;
	m_hIndividualStream = NULL;
}

void CStreamPool::PostListenerMatrix(MATRIX4X4* pMat)
{
	memcpy(&m_matListener, pMat, sizeof(MATRIX4X4));
}

void CStreamPool::SetStreamVolume(int iID, float fVolume)
{
	if (!loaded_libBass) return;
	BUFFERED_COMMAND_STREAMPOOL* bc;

	bc = bufferedCommands.WriteLock();

	float* pVolume = new float;

	*pVolume = fVolume;

	bc->iID = iID;
	bc->pData = (const void*)pVolume;

	bc->command = BUFFERED_COMMAND_STREAMPOOL::BC_VOLUME;

	bufferedCommands.WriteUnlock();
}

void CStreamPool::SetStreamVolumeJava(float fVolume)
{
	if (!loaded_libBass) return;
	if (m_bIsDeactivated == true) return;
	for (int i: m_StreamList)
	{
		if (m_bIsCreated[i] == false) continue ;
		
		SetStreamVolume(i, fVolume);
	}
}

#include "netgame.h"
extern CNetGame* pNetGame;
extern uint32_t bProcessedRender2dstuff;
void CStreamPool::Process() // ready
{
	if (!loaded_libBass) return;
	
	int state1 = *(int*)(SA_ADDR(0x63E06C)); // gMobileMenu
	int state3 = 0;

	if (GetTickCount() - bProcessedRender2dstuff >= 500)
	{
		state3 = 1;
	}


	if (state1 || state3)
	{
		for (int i: m_StreamList)
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
			for (int i: m_StreamList)
			{
				if (m_pStreams[i] && m_bSlotState[i])
				{
					m_pStreams[i]->SetIsDeactivated(false);
				}
			}
			if (m_szIndividualLastLink[0])
			{
				PlayIndividualStream(&m_szIndividualLastLink[0]);
			}
			m_bWasPaused = false;
		}
	}

	BUFFERED_COMMAND_STREAMPOOL* bcs;
	if (!m_bWasPaused)
	{
		while ((bcs = bufferedCommands.ReadLock()) != 0)
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
						m_pStreams[bcs->iID]->AttachToVehicle(bcs->iPlayerID);
					}
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
			if (bcs->command == BUFFERED_COMMAND_STREAMPOOL::BC_ACTIVATE)
			{
				for (int i: m_StreamList)
				{
					if (m_pStreams[i] && m_bSlotState[i])
					{
						if (bcs->iID == 2)
						{
							m_pStreams[i]->SetIsDeactivated(false);
						}
						else if (bcs->iID == 1)
						{
							m_pStreams[i]->SetIsDeactivated(true);
						}
					}
				}
			}
			bufferedCommands.ReadUnlock();
		}
	}

	for (int i: m_StreamList)
	{
		if (m_bSlotState[i] && m_pStreams[i])
		{
			m_pStreams[i]->Process(&m_matListener);
		}
	}

	BASS_3DVECTOR pos, vel, front, top;

	pos.x = m_matListener.pos.X;
	pos.y = m_matListener.pos.Y;
	pos.z = m_matListener.pos.Z;
	vel.x = vel.y = vel.z = 0.0f;
	front.x = m_matListener.at.X;
	front.y = m_matListener.at.Y;
	front.z = m_matListener.at.Z;
	top.x = m_matListener.up.X;
	top.y = m_matListener.up.X;
	top.z = m_matListener.up.X;
	BASS_Set3DPosition(&pos, &vel, &front, &top);

	BASS_Apply3D();
}
