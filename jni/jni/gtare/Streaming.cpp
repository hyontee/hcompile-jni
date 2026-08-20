//
// Created by @obfuscati0n
//

#include "Streaming.h"
#include "../util/patch.h"
#include "../game/scripting.h"

int CStreaming::AddImageToList(const char* fileName, bool bNotPlayerImg)
{
    Log("Loading: %s...", fileName);
    return CHook::CallFunction<int>(g_libGTASA + 0x0028E7B0 + 1, fileName, bNotPlayerImg);
}

void CStreaming::RemoveAllUnusedModels()
{

}
	
void CStreaming::RequestModel(int32_t index, int32_t flags)
{
    Log("Request Model: %d(id)...", index);
    ((void (*) (int32_t, int32_t))(g_libGTASA + 0x0028EB10 + 1))(index, flags);
}

void CStreaming::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    ((void (*) (bool))(g_libGTASA + 0x00294CB4 + 1))(bPriorityRequestsOnly);
}

void CStreaming::RemoveModel(int32_t index)
{
    ((void (*) (int32_t))(g_libGTASA + 0x00290C4C + 1))(index);
}