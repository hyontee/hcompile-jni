#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame *pGame;

void CObjectPool::Free()
{
    for(uint16_t i = 0; i < MAX_OBJECTS; i++)
    {
        Delete(i);
    }

    m_mapColoredObjectPool.clear();
}

bool CObjectPool::Delete(uint16_t objectId)
{
    if(!GetAt(objectId))
        return false;

    delete list[objectId];
    list.erase(objectId);

    return true;
}

bool CObjectPool::New(uint16_t objectId, int iModel, CVector vecPos, CVector vecRot, float fDrawDistance)
{
    if(GetAt(objectId))
        Delete(objectId);

    list[objectId] = pGame->NewObject(iModel, vecPos.x, vecPos.y, vecPos.z, vecRot, fDrawDistance);
    return true;
}

CObject *CObjectPool::GetObjectFromGtaPtr(ENTITY_TYPE *pGtaObject)
{
    for(auto &pair : list) {
        auto pObject = pair.second;

        if(pObject->m_pEntity == pGtaObject)
        {
            return pObject;
        }
    }
    return nullptr;
}

uint16_t CObjectPool::FindIDFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
    for(auto &pair : list) {
        auto pObject = pair.second;

        if(pObject->m_pEntity == pGtaObject)
        {
            return pair.first;
        }
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

    for(auto &pair : list) {
        auto pObject = pair.second;

        if(pObject) pObject->Process(fElapsedTime);
    }

    s_ulongLastCall = ulongTick;
}

void CObjectPool::AddColoredObject(CObject* pObject)
{

    if (!pObject) return;
    if (m_mapColoredObjectPool.find(pObject->m_pEntity) == m_mapColoredObjectPool.end())
    {
        m_mapColoredObjectPool.insert(std::make_pair(pObject->m_pEntity, pObject));
    }
}

void CObjectPool::ResetColoredObject(){
    m_mapColoredObjectPool.clear();
}

void CObjectPool::RemoveColoredObject(CObject* pObject)
{

    if (!pObject) return;
    if (m_mapColoredObjectPool.find(pObject->m_pEntity) != m_mapColoredObjectPool.end())
    {
        m_mapColoredObjectPool.erase(m_mapColoredObjectPool.find(pObject->m_pEntity));
    }
}

CObject* CObjectPool::GetColoredObjectFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
    if (!pGtaObject) return nullptr;
    if (m_mapColoredObjectPool.find(pGtaObject) != m_mapColoredObjectPool.end())
    {
        return m_mapColoredObjectPool.find(pGtaObject)->second;
    }
    return nullptr;
}

bool CObjectPool::ObjectCollisionProcess(int CColSphere1, int CColSphere2)
{
    for(auto &pair : list)
    {
        auto pObject = pair.second;
        if(pObject)
        {
            if(pObject->IsAdded() &&
               !pObject->m_bDontCollideWithCamera &&
               pObject->GetDistanceFromCamera() <= 200.0f &&
               !pObject->IsEntityIgnored() &&
               pObject->TestSphereCastVsEntity(&CColSphere1, &CColSphere2, true))
            {
                return true;
            }
        }
    }

    return false;
}