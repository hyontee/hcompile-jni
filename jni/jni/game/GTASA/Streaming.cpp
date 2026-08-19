//
// Created by Weikton
//
// Связка с libGTASA.so (1.x)
// Теперь можно отключать gta3.img и другие 
// ВСЕ СТРОГО В hooks.cpp 

#include "Streaming.h"
#include "../../util/patch.h"
#include "../scripting.h"

// CStreaming::AddImageToList(char const*,bool)
int CStreaming::AddImageToList(const char* fileName, bool bNotPlayerImg)
{
    Log(fileName);
    return CHook::CallFunction<int>(g_libGTASA + 0x0028E7B0 + 1, fileName, bNotPlayerImg);
}

void CStreaming::RemoveAllUnusedModels()
{
   // unused
}

// CStreaming::RequestModel(int,int)	
void CStreaming::RequestModel(int32_t index, int32_t flags)
{
    Log("RequestModel %d", index);
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(index, flags);
}

// CStreaming::LoadAllRequestedModels(bool)	
void CStreaming::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    ((void (*) (bool))(g_libGTASA + 0x00294CB4 + 1))(bPriorityRequestsOnly);
}

// CStreaming::RemoveModel(int)	
void CStreaming::RemoveModel(int32_t index)
{
    ((void (*) (int32_t))(g_libGTASA + 0x00290C4C + 1))(index);
}