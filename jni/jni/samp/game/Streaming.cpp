//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////


#include "Streaming.h"
#include "scripting.h"
#include "game/Models/ModelInfo.h"
#include "chatwindow.h"
#include "CameraGta.h"
#include "game/Enums/ModelID.h"
#include "patch.h"


int CStreaming::AddImageToList(const char* fileName, bool bNotPlayerImg)
{

    std::int32_t fileIndex = 0;
    for (; fileIndex < TOTAL_IMG_ARCHIVES; fileIndex++) {
        if (!ms_files[fileIndex].m_szName[0])
            break;
    }
    if (fileIndex == TOTAL_IMG_ARCHIVES)
        return 0;
    // free slot found, load the IMG file
    Log("Loading: %s...", fileName);
    strcpy(ms_files[fileIndex].m_szName, fileName);
    ms_files[fileIndex].m_StreamHandle = CdStreamOpen(fileName);
    ms_files[fileIndex].m_bNotPlayerImg = bNotPlayerImg;

    return fileIndex;
    //return CHook::CallFunction<int>(g_libGTASA + 0x0028E7B0 + 1, fileName, bNotPlayerImg);
}
void CStreaming::RemoveAllUnusedModels()
{
    // ((void (*) ())(g_libGTASA + 0x002CFEF0))();
}

void CStreaming::RequestModel(int32_t index, int32_t flags)
{
    if(CModelInfo::GetModelInfo(index) == nullptr) {
        Log("WARNING! NO OBJ MODEL = %d", index);
        return;
    }

//    ScriptCommand(&request_model, iModel);
    Log("RequestModel %d", index);
//    ScriptCommand(&request_model, index);

    //((void (*) (int32_t, int32_t))(g_libGTASA + 0x002D299C + 1))(index, flags);
    CHook::CallFunction<void>("_ZN10CStreaming12RequestModelEii", index, flags);
}

void CStreaming::LoadAllRequestedModels(bool bPriorityRequestsOnly)
{
    CHook::CallFunction<void>("_ZN10CStreaming22LoadAllRequestedModelsEb", bPriorityRequestsOnly);
}


void CStreaming::RemoveModel(int32_t index)
{
    CHook::CallFunction<void>("_ZN10CStreaming11RemoveModelEi", index);
}
void CStreaming::InjectHooks() {

    //CHook::Redirect(g_libGTASA, 0x002CF7D0, &CStreaming::AddImageToList);
    CHook::HookRedirect(OBFUSCATE("_ZN10CStreaming14AddImageToListEPKcb"), CStreaming::AddImageToList);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00676AB8 : 0x84B5D0), &CStreaming::ms_files);

}

void CStreaming::RemoveModelIfNoRefs(int32 modelId) {
    CStreamingInfo& streamingInfo = GetInfo(modelId);
    if(streamingInfo.IsLoaded() && !CModelInfo::GetModelInfo(modelId)->m_nRefCount) {
        RemoveModel(modelId);

        streamingInfo.ClearAllFlags();
    }
}


bool CStreaming::TryLoadModel(int modelId) {
    if(!CStreaming::GetInfo(modelId).IsLoaded()) {
        CStreaming::RequestModel(modelId, STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY);
        CStreaming::LoadAllRequestedModels(false);
        uint32 count = 0;
        while (!CStreaming::GetInfo(modelId).IsLoaded()) {
            count++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (count > 30) {
                CChatWindow::AddDebugMessage("Warning: Model %u wouldn't load in time!", modelId);
                return false;
            }
        }
    }
    return true;
}
//////////////////////////////////////
/// Created by Cross on 12.09.2025////
//////////////////////////////////////
