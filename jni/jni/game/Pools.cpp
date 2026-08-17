#include "Pools.h"
#include "util/patch.h"

PoolAllocator::Pool*      CPools::ms_pBuildingPool;
PoolAllocator::Pool*      CPools::ms_pDummyPool;
PoolAllocator::Pool*      CPools::ms_pColModelPool;
PoolAllocator::Pool*      CPools::ms_pEventPool;
PoolAllocator::Pool*      CPools::ms_pPointRoutePool;
PoolAllocator::Pool*      CPools::ms_pPatrolRoutePool;
PoolAllocator::Pool*      CPools::ms_pNodeRoutePool;
PoolAllocator::Pool*      CPools::ms_pTaskAllocatorPool;
PoolAllocator::Pool*      CPools::ms_pPedIntelligencePool;
PoolAllocator::Pool*      CPools::ms_pPedAttractorPool;

void CPools::Initialise()
{
	CPools::ms_pEntryInfoNodePool       = new CPool<CEntryInfoNode>(120000, "EntryInfoNodePool");
    CPools::ms_pPtrNodeSingleLinkPool   = new CPool<CPtrNodeSingleLink>(300000, "PtrNode Single");
    CPools::ms_pPtrNodeDoubleLinkPool   = new CPool<CPtrNodeDoubleLink>(150000, "PtrNode Double");
  //  CPools::ms_pPedPool                 = new CPool<CPedGta>(240, "Peds");
   // CPools::ms_pVehiclePool             = new CPool<CVehicle>(1000, "Vehicles");
  /*  CPools::ms_pObjectPool              = new CPool<CObjectGta>(3000, "Objects");
    CPools::ms_pTaskPool                = new CPool<CTask>(5000, "Task");
   
*/
    CPools::ms_pBuildingPool = PoolAllocator::Allocate(80000, 60);	// 14000

    CPools::ms_pDummyPool = PoolAllocator::Allocate(80000, 60);	// 3500
    // 487200 / 10150 = 48
    CPools::ms_pColModelPool = PoolAllocator::Allocate(100000, 48);	// 10150
    // 13600 / 200 = 68
    CPools::ms_pEventPool = PoolAllocator::Allocate(10000, 68);	// 200
    // 6400 / 64 = 100
    CPools::ms_pPointRoutePool = PoolAllocator::Allocate(2000, 100);	// 64
    // 13440 / 32 = 420
    CPools::ms_pPatrolRoutePool = PoolAllocator::Allocate(2000, 420);	// 32
    // 2304 / 64 = 36
    CPools::ms_pNodeRoutePool = PoolAllocator::Allocate(2000, 36);	// 64
    // 512 / 16 = 32
    CPools::ms_pTaskAllocatorPool = PoolAllocator::Allocate(30000, 32);	// 16
    // 92960 / 140 = 664
    CPools::ms_pPedIntelligencePool = PoolAllocator::Allocate(2400, 664);	// 140
    // 15104 / 64 = 236
    CPools::ms_pPedAttractorPool = PoolAllocator::Allocate(2000, 236);	// 64

    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93CC) = CPools::ms_pBuildingPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C4) = CPools::ms_pDummyPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C0) = CPools::ms_pColModelPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B8) = CPools::ms_pEventPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B4) = CPools::ms_pPointRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B0) = CPools::ms_pPatrolRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93AC) = CPools::ms_pNodeRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A8) = CPools::ms_pTaskAllocatorPool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A4) = CPools::ms_pPedIntelligencePool;
    *(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A0) = CPools::ms_pPedAttractorPool;
}

void CPools::InjectHooks() {
    //CHook::Redirect(g_libGTASA, 0x3AF1A0, &CPools::Initialise);

    CHook::Write(g_libGTASA + 0x005D1BF4, &CPools::ms_pEntryInfoNodePool);
    CHook::Write(g_libGTASA + 0x005CFD38, &CPools::ms_pPtrNodeSingleLinkPool);
    CHook::Write(g_libGTASA + 0x005D01B4, &CPools::ms_pPtrNodeDoubleLinkPool);
 //   CHook::Write(g_libGTASA + 0x005CE9E4, &CPools::ms_pPedPool);
  //  CHook::Write(g_libGTASA + 0x005D039C, &CPools::ms_pVehiclePool);
  //  CHook::Write(g_libGTASA + 0x005CE900, &CPools::ms_pObjectPool);
  //  CHook::Write(g_libGTASA + 0x005CDE64, &CPools::ms_pTaskPool);
}

