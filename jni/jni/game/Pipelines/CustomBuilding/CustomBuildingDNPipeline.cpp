#include "CustomBuildingDNPipeline.h"
#include "../../../util/patch.h"

void CCustomBuildingDNPipeline::InjectHooks() {
    CHook::Write(g_libGTASA + 0x5CEBE4, &m_fDNBalanceParam);
}