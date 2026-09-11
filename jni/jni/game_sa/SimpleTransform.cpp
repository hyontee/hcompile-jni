//
// Created by x1y2z on 28.12.2023.
//

#include "SimpleTransform.h"
#include "main.h"
// CSimpleTransform::UpdateRwMatrix(RwMatrixTag *)	003AB878
void CSimpleTransform::UpdateRwMatrix(RwMatrix* out)
{
    ((void(__thiscall*)(CSimpleTransform*, RwMatrix*))(g_libGTASA + 0x3AB878 + 1))(this, out);
}
// CSimpleTransform::UpdateMatrix(CMatrix *)	003AB8D0
void CSimpleTransform::Invert(const CSimpleTransform& base)
{
    ((void(__thiscall*)(CSimpleTransform*, const CSimpleTransform&))(g_libGTASA + 0x3AB8D0 + 1))(this, base);
}
// CSimpleTransform::Invert(CSimpleTransform const&)	003AB8F4
void CSimpleTransform::UpdateMatrix(CMatrix* out)
{
    ((void(__thiscall*)(CSimpleTransform*, class CMatrix*))(g_libGTASA + 0x3AB8F4 + 1))(this, out);
}