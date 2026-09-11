#pragma once

#define INVALID_OBJECT_ID	0xFFF9
#define MAX_OBJECTS			1000

class CObjectPool
{
public:
    static inline std::unordered_map<uint16_t, CObject*> list;
    static inline std::unordered_map<ENTITY_TYPE*, CObject*> m_mapColoredObjectPool;
    //static inline CObject *m_pObjects[MAX_OBJECTS];

public:
    static void Free();

    static bool New(uint16_t ObjectID, int iModel, CVector vecPos, CVector vecRot, float fDrawDistance = 0);
    static bool Delete(uint16_t ObjectID);

    static CObject* GetAt(uint16_t objectId)
    {
        auto it = list.find(objectId);
        if(it != list.end())
            return list[objectId];

        return nullptr;
    }

    static uint16_t FindIDFromGtaPtr(ENTITY_TYPE *pGtaObject);

    static CObject *GetObjectFromGtaPtr(ENTITY_TYPE *pGtaObject);

    static void Process();

    static void AddColoredObject(CObject* pObject);

    static void RemoveColoredObject(CObject* pObject);

    static CObject* GetColoredObjectFromGtaPtr(ENTITY_TYPE* pGtaObject);

    static void ResetColoredObject();

    static bool ObjectCollisionProcess(int CColSphere1, int CColSphere2);
};