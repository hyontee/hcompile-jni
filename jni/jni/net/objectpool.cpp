#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame *pGame;

static bool IsValidObjectModel(int modelId)
{
	if (modelId <= 0 || modelId >= 20000)
		return false;

	uintptr_t* modelInfo = (uintptr_t*)(g_libGTASA + 0x87BF48);
	return modelInfo[modelId] != 0;
}

CObjectPool::CObjectPool()
{
	for(uint16_t ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		m_bObjectSlotState[ObjectID] = false;
		m_pObjects[ObjectID] = 0;
	}
}

CObjectPool::~CObjectPool()
{
	for(uint16_t i = 0; i < MAX_OBJECTS; i++)
	{
		Delete(i);
	}
}

bool CObjectPool::Delete(uint16_t ObjectID)
{
	if (ObjectID < 0 || ObjectID >= MAX_OBJECTS)
	{
		return false;
	}
	if(!GetSlotState(ObjectID) || !m_pObjects[ObjectID])
		return false;

	m_bObjectSlotState[ObjectID] = false;
	delete m_pObjects[ObjectID];
	m_pObjects[ObjectID] = 0;

	return true;
}

bool CObjectPool::New(uint16_t ObjectID, int iModel, VECTOR vecPos, VECTOR vecRot, float fDrawDistance)
{
	if (ObjectID >= MAX_OBJECTS)
	{
		Log("[AntiCrash] Bad object slot %u for model %d. Object spawn skipped.", ObjectID, iModel);
		return false;
	}

	if (!IsValidObjectModel(iModel))
	{
		Log("[AntiCrash] Bad object model id %d at slot %u. Spawn skipped: model not registered in CModelInfo.", iModel, ObjectID);
		return false;
	}

	if(m_pObjects[ObjectID] != 0)
		Delete(ObjectID);

	m_pObjects[ObjectID] = pGame->NewObject(iModel, vecPos.X, vecPos.Y, vecPos.Z, vecRot, fDrawDistance);

	if(m_pObjects[ObjectID])
	{
		m_bObjectSlotState[ObjectID] = true;

		return true;
	}

	return false;
}

CObject *CObjectPool::GetObjectFromGtaPtr(ENTITY_TYPE *pGtaObject)
{
	uint16_t x=1;

	while(x!=MAX_OBJECTS)
	{
		if(m_pObjects[x])
			if(pGtaObject == m_pObjects[x]->m_pEntity) return m_pObjects[x];
		
		x++;
	}

	return 0;
}

uint16_t CObjectPool::FindIDFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
	uint16_t x=1;

	while(x!=MAX_OBJECTS)
	{
		if(m_pObjects[x])
			if(pGtaObject == m_pObjects[x]->m_pEntity) return x;

		x++;
	}

	return INVALID_OBJECT_ID;
}

void CObjectPool::Process()
{
	static unsigned long s_ulongLastCall = 0;
	if (!s_ulongLastCall) s_ulongLastCall = GetTickCount();
	unsigned long ulongTick = GetTickCount();
	float fElapsedTime = ((float)(ulongTick - s_ulongLastCall)) / 1000.0f;
	// Get elapsed time in seconds
	for (int i = 0; i < MAX_OBJECTS; i++)
	{
		if (m_bObjectSlotState[i]) m_pObjects[i]->Process(fElapsedTime);
	}
	s_ulongLastCall = ulongTick;
}
