#pragma once

#include "../common.h"

class CEntryInfoNode
{
public:
    class CPtrListDoubleLink* m_pDoubleLinkList;
    class CPtrNodeDoubleLink* m_pDoubleLink;
    class CRepeatSector* m_pRepeatSector;
    CEntryInfoNode* m_pPrevious;
    CEntryInfoNode* m_pNext;

public:
    static void InjectHooks();

    static void* operator new(unsigned int size);
    static void operator delete(void* ptr, size_t sz);
    void AddToList(CEntryInfoNode* pNext); //Fully inlined in final game
};

