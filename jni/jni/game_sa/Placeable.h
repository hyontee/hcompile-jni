//
// Created by x1y2z on 30.12.2023.
//

#pragma once

#include "SimpleTransform.h"
#include "MatrixLink.h"
#include "../main.h"
#include "game/RW/common.h"

#pragma pack(push, 1)
class CPlaceable
{
public:
    uint32_t            vtable;
    CSimpleTransform    m_placement;
    CMatrixLink         *m_matrix;

public:
    static void InjectHooks();

    CMatrix& GetMatrix();
    CVector GetRightVector();
    CVector GetForwardVector();
    CVector GetUpVector();

    static void ShutdownMatrixArray();
    static void InitMatrixArray();

    void FreeStaticMatrix();
    void SetPosn(float x, float y, float z);
    void SetPosn(const CVector& posn);
    void SetOrientation(float x, float y, float z);
    void SetOrientation(CVector radians) { SetOrientation(radians.x, radians.y, radians.z); } // TODO: Replace method above with this
    void GetOrientation(float& x, float& y, float& z);
    void SetHeading(float heading);
    float GetHeading();
    bool IsWithinArea(float x1, float y1, float x2, float y2) const;
    bool IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const;
    void RemoveMatrix();
    void AllocateStaticMatrix();
    void AllocateMatrix();
    void SetMatrix(CMatrix& matrix);

    // NOTSA
    bool IsPointInRange(const CVector& point, float range);
    bool IsEntityInRange(const CPlaceable* entity, float range) { return IsPointInRange(entity->GetPosition(), range); }

public:
    static constexpr uint32 NUM_MATRICES_TO_CREATE = 4000;

    inline CVector& GetRight() const { return m_matrix->GetRight(); }
    inline CVector& GetForward() const { return m_matrix->GetForward(); }
    inline CVector& GetUp() const { return m_matrix->GetUp(); }
    inline const CVector& GetPosition() const { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    inline CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    // inline CVector2D GetPosition2D() { return { GetPosition() }; }
};
#pragma pack(pop)

VALIDATE_SIZE(CPlaceable, 0x18);
