//
// Created by x1t2z on 28.12.2023.
//
#include "../main.h"
#include "Placeable.h"
#include "MatrixLinkList.h"
#include "util/armhook.h"

/*CPlaceable::CPlaceable() : m_placement() {
    m_matrix = nullptr;
}

CPlaceable::~CPlaceable() {
    if (m_matrix) {
        CPlaceable::RemoveMatrix();
        --numMatrices;
    }

    m_matrix = reinterpret_cast<CMatrixLink*>(&gDummyMatrix);
}*/

void CPlaceable::InitMatrixArray() {
    gMatrixList.Init(CPlaceable::NUM_MATRICES_TO_CREATE);
}

void CPlaceable::ShutdownMatrixArray() {
    gMatrixList.Shutdown();
}

CVector CPlaceable::GetRightVector() {
    if (m_matrix)
        return m_matrix->GetRight();
    return { std::cos(m_placement.m_fHeading), std::sin(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetForwardVector() {
    if (m_matrix)
        return m_matrix->GetForward();
    return { -std::sin(m_placement.m_fHeading), std::cos(m_placement.m_fHeading), 0.0f };
}

CVector CPlaceable::GetUpVector() {
    if (m_matrix)
        return m_matrix->GetUp();
    return {0.0f, 0.0f, 1.0f};
}

void CPlaceable::SetPosn(float x, float y, float z) {
    auto& pos = GetPosition();
    pos.Set(x, y, z);
    //(( void (*)(CPlaceable*, float, float, float, bool))(*(void**)(this->vtable+0x3C)))(this, x, y, z, 0);
}

void CPlaceable::SetPosn(const CVector& posn) {
    auto& pos = GetPosition();
    pos = posn;
    //(( void (*)(CPlaceable*, float, float, float, bool))(*(void**)(this->vtable+0x3C)))(this, posn.x, posn.y, posn.z, 0);
}

void CPlaceable::SetOrientation(float x, float y, float z) {
    if (!m_matrix) {
        m_placement.m_fHeading = z;
        return;
    }

    CVector vecPos = m_matrix->GetPosition();
    m_matrix->SetRotate(x, y, z);
    m_matrix->GetPosition() += vecPos;
}

void CPlaceable::SetHeading(float heading) {
    if (m_matrix)
        m_matrix->SetRotateZOnly(heading);
    else
        m_placement.m_fHeading = heading;
}

float CPlaceable::GetHeading() {
    if (!m_matrix)
        return m_placement.m_fHeading;

    const auto& vecForward = m_matrix->GetForward();
    return std::atan2(-vecForward.x, vecForward.y);
}

bool CPlaceable::IsWithinArea(float x1, float y1, float x2, float y2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    return vecPos.x >= x1 && vecPos.x <= x2 && vecPos.y >= y1 && vecPos.y <= y2;
}


bool CPlaceable::IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const {
    const auto& vecPos = GetPosition();
    if (x1 > x2)
        std::swap(x1, x2);

    if (y1 > y2)
        std::swap(y1, y2);

    if (z1 > z2)
        std::swap(z1, z2);

    return vecPos.x >= x1
           && vecPos.x <= x2
           && vecPos.y >= y1
           && vecPos.y <= y2
           && vecPos.z >= z1
           && vecPos.z <= z2;
}

void CPlaceable::RemoveMatrix() {
    const auto& vecForward = m_matrix->GetForward();
    auto fHeading = std::atan2(-vecForward.x, vecForward.y);

    m_placement.m_vPosn = m_matrix->GetPosition();
    m_placement.m_fHeading = fHeading;

    m_matrix->m_pOwner = nullptr;
    gMatrixList.MoveToFreeList(m_matrix);
    m_matrix = nullptr;
}

void CPlaceable::AllocateStaticMatrix() {
    if (m_matrix)
        return gMatrixList.MoveToList2(m_matrix);

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList2();
    m_matrix->m_pOwner = this;
}

void CPlaceable::AllocateMatrix() {
    if (m_matrix)
        return;

    if (gMatrixList.IsFull())
        gMatrixList.GetOldestLink()->m_pOwner->RemoveMatrix();

    m_matrix = gMatrixList.AddToList1();
    m_matrix->m_pOwner = this;
}

void CPlaceable::SetMatrix(CMatrix& matrix) {
    if (!m_matrix) {
        if (matrix.GetUp().z == 1.0F) {
            auto& vecForward = matrix.GetForward();
            auto fHeading = std::atan2(-vecForward.x, vecForward.y);

            m_placement.m_vPosn = matrix.GetPosition();
            m_placement.m_fHeading = fHeading;
            return;
        }
        CPlaceable::AllocateMatrix();
    }

    *static_cast<CMatrix*>(m_matrix) = matrix;
}

// NOTSA
bool CPlaceable::IsPointInRange(const CVector& point, float range) {
    return DistanceBetweenPointsSquared(point, GetPosition()) <= sq(range);
}

CMatrix& CPlaceable::GetMatrix() {
    if (!m_matrix) {
        CPlaceable::AllocateMatrix();
        m_placement.UpdateMatrix(m_matrix);
    }

    return *m_matrix;
}

void CPlaceable::GetOrientation(float& x, float& y, float& z) {
    if (!m_matrix) {
        z = m_placement.m_fHeading;
        return;
    }

    x = asinf(GetForward().z);

    float cosx = std::cosf(x);
    float cosy = GetUp().z / cosx;
    y = std::acosf(cosy);

    float cosz = GetForward().y / cosx;
    z = std::acosf(cosz);
}

void CPlaceable::FreeStaticMatrix() {
    gMatrixList.MoveToList1(m_matrix);
}

// ----------------------------- hooks

void CPlaceable_FreeStaticMatrix(CPlaceable *thiz) {
    thiz->FreeStaticMatrix();
}

void CPlaceable_AllocateMatrix(CPlaceable *thiz) {
    thiz->AllocateMatrix();
}

void CPlaceable_AllocateStaticMatrix(CPlaceable *thiz) {
    thiz->AllocateStaticMatrix();
}

bool CPlaceable_IsWithinArea4(CPlaceable *thiz, float x1, float y1, float x2, float y2) {
    return thiz->IsWithinArea(x1, y1, x2, y2);
}

bool CPlaceable_IsWithinArea6(CPlaceable *thiz, float x1, float y1, float z1, float x2, float y2, float z2) {
    return thiz->IsWithinArea(x1, y1, z1, x2, y2, z2);
}

void CPlaceable_RemoveMatrix(CPlaceable *thiz) {
    thiz->RemoveMatrix();
}

void CPlaceable_SetMatrix(CPlaceable *thiz, CMatrix& matrix) {
    thiz->SetMatrix(matrix);
}

void CPlaceable::InjectHooks() {
    Redirect(g_libGTASA, 0x3ABB08, &CPlaceable::InitMatrixArray);
    Redirect(g_libGTASA, 0x3ABB24, &CPlaceable::ShutdownMatrixArray);

    Redirect(g_libGTASA, 0x003ABB3C, &CPlaceable_FreeStaticMatrix);
    Redirect(g_libGTASA, 0x003ABC2C, &CPlaceable_AllocateMatrix);
    Redirect(g_libGTASA, 0x003ABBE8, &CPlaceable_AllocateStaticMatrix);
    Redirect(g_libGTASA, 0x003AB988, &CPlaceable_IsWithinArea4);
    Redirect(g_libGTASA, 0x003ABA24, &CPlaceable_IsWithinArea6);
    Redirect(g_libGTASA, 0x003ABB58, &CPlaceable_RemoveMatrix);
    Redirect(g_libGTASA, 0x003ABC64, &CPlaceable_SetMatrix);
}