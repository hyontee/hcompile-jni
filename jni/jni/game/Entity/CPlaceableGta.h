//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CPLACEABLEGTA_H
#define LIVERUSSIA_CPLACEABLEGTA_H

#include "../Core/Quaternion.h"
#include "../CSimpleTransform.h"
#include "../Core/MatrixLink.h"

#pragma pack(1)
struct CPlaceableGta
{
    uint32_t vtable;
    CSimpleTransform m_transform;
    CMatrixLink *mat;

public:
    inline const CVector& GetPosition() const { return mat ? mat->GetPosition() : m_transform.m_vPosn; }
    inline CVector& GetPosition() { return mat ? mat->GetPosition() : m_transform.m_vPosn; }
};

#endif //LIVERUSSIA_CPLACEABLEGTA_H
