#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "CSnow.h"
#include "../chatwindow.h"

extern CChatWindow *pChatWindow;

uint32_t CSnow::m_dwParticeHandle = 0;
uint32_t CSnow::m_dwLastTickCreated = 0;
int CSnow::m_iCurrentSnow = 0;

static const char* g_iSnows[] = { "SNOWFXII", "SNOWFXIII", "SNOWFXIV" };

void CSnow::Initialise()
{
    m_dwParticeHandle = 0;
    m_dwLastTickCreated = GetTickCount();
}

void CSnow::Process(CPlayerPed* pPed, int iInterior)
{
    if (!pPed)
    {
        return;
    }

    if (iInterior && m_dwParticeHandle)
    {
        ScriptCommand(&destroy_particle, m_dwParticeHandle);
        m_dwParticeHandle = 0;
        return;
    }

    if (!m_dwParticeHandle)
    {
        if (m_iCurrentSnow == 0)
        {
            return;
        }

        if (m_iCurrentSnow < 0 || m_iCurrentSnow >= 4)
        {
            m_iCurrentSnow = 1;
        }

        uint32_t dwActorhandle = pPed->m_dwGTAId;
        pChatWindow->AddDebugMessage("SNEGA MAMU EBAL: %d", m_iCurrentSnow);

        ScriptCommand(&attach_particle_to_actor, "SNOWFXII", dwActorhandle, 0.0f, 0.0f, 2.0f, 1, &m_dwParticeHandle);
        ScriptCommand(&make_particle_visible, m_dwParticeHandle);

        m_dwLastTickCreated = GetTickCount();
    }

    if (GetTickCount() - m_dwLastTickCreated >= 2000)
    {
        if (m_dwParticeHandle)
        {
            ScriptCommand(&destroy_particle, m_dwParticeHandle);
            m_dwParticeHandle = 0;
        }
    }
}

void CSnow::SetCurrentSnow(int iSnow)
{
    m_iCurrentSnow = iSnow;
}

int CSnow::GetCurrentSnow()
{
    return m_iCurrentSnow;
}