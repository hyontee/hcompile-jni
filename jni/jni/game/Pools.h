//
// Created by plaka on 17.04.2023.
//

#include <cstddef>
#include <cstdint>
//#include "vehicle.h"
#include <algorithm>
#include "game/GTASA/Pool.h"
#include "game/GTASA/PtrNodeSingleLink.h"
#include "game/GTASA/PtrNodeDoubleLink.h"
#include "game/GTASA/EntryInfoNode.h"
class CPools {
public:
    static inline CPool<CPtrNodeSingleLink>*        ms_pPtrNodeSingleLinkPool;
    static inline CPool<CPtrNodeDoubleLink>*        ms_pPtrNodeDoubleLinkPool;
	static inline CPool<CEntryInfoNode>*            ms_pEntryInfoNodePool;
//	static inline CPool<CVehicle>*               ms_pVehiclePool;
	static PoolAllocator::Pool*         ms_pBuildingPool;
    static PoolAllocator::Pool*         ms_pDummyPool;
    static PoolAllocator::Pool*         ms_pColModelPool;
    static PoolAllocator::Pool*         ms_pEventPool;
    static PoolAllocator::Pool*         ms_pPointRoutePool;
    static PoolAllocator::Pool*         ms_pPatrolRoutePool;
    static PoolAllocator::Pool*         ms_pNodeRoutePool;
    static PoolAllocator::Pool*         ms_pTaskAllocatorPool;
    static PoolAllocator::Pool*         ms_pPedIntelligencePool;
    static PoolAllocator::Pool*         ms_pPedAttractorPool;
	

public:
    static void InjectHooks();

    static void Initialise();
};

static auto& GetPtrNodeSingleLinkPool()          { return CPools::ms_pPtrNodeSingleLinkPool; }
static auto& GetPtrNodeDoubleLinkPool()          { return CPools::ms_pPtrNodeDoubleLinkPool; }




