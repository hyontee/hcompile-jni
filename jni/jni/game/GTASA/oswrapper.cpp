//
// Created by Weikton
//

#include "oswrapper.h"
#include "../../util/patch.h"

uint64_t GetOSWPerformanceTime()
{
    return (uint64_t)(OS_TimeAccurate() * 1000000.0);
}

// OS_TimeAccurate(void)	0x23861C	
double OS_TimeAccurate() {
    CHook::CallFunction<double>(g_libGTASA + 0x0023861C + 1);
}
