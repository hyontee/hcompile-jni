//
// Created by x1y2z on 28.12.2023.
//

#pragma once


#include "Vector.h"

class CSimpleTransform {
public:
    CSimpleTransform() : m_vPosn(), m_fHeading(0.0F) {}

public:
    CVector m_vPosn;
    float m_fHeading;

    void UpdateRwMatrix(RwMatrix* out);
    void Invert(const CSimpleTransform& base);
    void UpdateMatrix(class CMatrix* out);
};
static_assert(sizeof(CSimpleTransform) == 0x10, "Invalid size CSimpleTransform");
