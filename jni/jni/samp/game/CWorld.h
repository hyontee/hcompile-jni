//
// Created by plaka on 20.02.2023.
//

#ifndef LIVERUSSIA_CWORLD_H
#define LIVERUSSIA_CWORLD_H

#include "CPlayerInfoGta.h"

class CWorld {
public:
    static inline CPlayerInfoGta Players[1004];
    static inline int PlayerInFocus;
    static void Add(CEntityGta* entity);
};


#endif //LIVERUSSIA_CWORLD_H
