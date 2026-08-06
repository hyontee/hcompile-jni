#include "CSkyBox.h"

// ****************
// vk.com/we1kton
// ****************

#include "../game/game.h"
#include "../game/CVector.h"

#include "../net/netgame.h"
#include "../gui/gui.h"

#include "..//settings.h"

// -- EXTERN`S
extern CGame* pGame;
extern CNetGame* pNetGame;
extern CGUI* pGUI;
extern CSettings* pSettings;

// -- VARIABLE`S
CObject* CSkyBox::m_pSkyObject = nullptr;
RwTexture* pSkyTexture = nullptr;

const char* CSkyBox::m_TextureName = nullptr;

bool CSkyBox::m_bNeedRender = true;
float CSkyBox::m_fRotSpeed = 0.01f;

uint8_t pChangeTime;
// -- METHOD`S
MATRIX4X4* RwMatrixMultiplyByVector(VECTOR* out, MATRIX4X4* a2, VECTOR* in);

void CSkyBox::Initialise()
{
	auto* dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
	if (!dwModelArray[18659])
		return;

	m_pSkyObject = CreateObjectScaled(18659, 0.23f);


	//	SetTexture("BR_Nebo_Day");

		//*(uintptr_t *)(dwModelArray + 0x28) &= 0xFFFEu;


}


void RwMatrixScale(MATRIX4X4* matrix, CVector* vecScale)
{
	VECTOR vector = vecScale->Get();

	RwMatrixScale(matrix, &vector);
}
extern char* PLAYERS_REALLOC;
int sub_5212C(int a1)
{
	int v2; // r1
	int v3; // r1

	*(uintptr_t*)(a1 + 8) = 1;
	*(uintptr_t*)a1 = 0;
	*(uintptr_t*)(a1 + 0xC) = (uintptr_t)PLAYERS_REALLOC;
	*(uintptr_t*)(a1 + 4) = (uintptr_t)PLAYERS_REALLOC;
	v2 = *(uintptr_t*)(a1 + 0xC);
	*(uintptr_t*)(a1 + 0x10) = 0;
	// sub_57F64(0, v2);
	// sub_55814((uint16_t *)"~\bii", *(uintptr_t *)(a1 + 8), 1);
	/// sub_55814(word_1D4EC2, *(uintptr_t *)(a1 + 8), 0);
	// if ( *(uintptr_t *)(a1 + 0xC) && sub_57DF0(*(uintptr_t *)(a1 + 8), v3) )
	 //  sub_55814(word_1D50BA, *(uintptr_t *)(a1 + 8), 1);
	*(uintptr_t*)(a1 + 0x11) = 0;
	*(uintptr_t*)(a1 + 0x1D) = 1;
	*(uintptr_t*)(a1 + 0x15) = 0;
	*(uintptr_t*)(a1 + 0x19) = 0;
	*(uintptr_t*)(a1 + 0x1F) = 0x3C03126F;
	memset((void*)(a1 + 0x50), 0, 0x23Au);
	((void(__fastcall*)(uintptr_t))(g_libGTASA + 0x43E201))(*(uintptr_t*)(a1 + 0xC));
	((void(__fastcall*)(uintptr_t))(g_libGTASA + 0x43E229))(*(uintptr_t*)(a1 + 0xC));
	return a1;
}
float oldscale;
void CSkyBox::Process()
{
	if (!pSettings->GetReadOnly().iSkyBox)
		return;

	if (!m_pSkyObject)
		Initialise();

	if (m_pSkyObject)
	{
		CAMERA_AIM* aim = GameGetInternalAim();
		MATRIX4X4 matrix;
		/*VECTOR v77;
		CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
		if(pPed){
		  pPed->GetBonePosition(4, &v77);

		float v[3] = {
		(v77.X),
		(v77.Y),
		(v77.Z)};
		((void ( *)(uintptr_t, float*, int))(0x1AED7D + g_libGTASA))((uintptr_t)m_pSkyObject, v, 2);
		((void (*)(void))(g_libGTASA + 0x559EF9))();
		((void (*)(void))(g_libGTASA + 0x559FC9))();}*/
		m_pSkyObject->GetMatrix(&matrix);

		matrix.pos.X = aim->pos1x;
		matrix.pos.Y = aim->pos1y;
		matrix.pos.Z = aim->pos1z;
		auto* vecScale1 = new CVector(1.0f / oldscale);
		RwMatrixScale(&matrix, vecScale1);
		float scale = (float)((float)((float)*(unsigned int*)(0x63E3C4 + g_libGTASA) + 150.0) / 150.0) * 1.55;
		//*(uint32_t*)(0x61572C + g_libGTASA) |= *(uint32_t*)(0x61572C + g_libGTASA) & 0x8000000 | 0x800000;
		auto* vecScale = new CVector(scale);
		RwMatrixScale(&matrix, vecScale);
		//Log("sky scale %f", scale);
		oldscale = scale;



		m_bNeedRender = true;

		ReTexture();

		m_pSkyObject->SetMatrix(matrix);
		m_pSkyObject->UpdateMatrix(matrix);
		m_pSkyObject->Render();

		m_bNeedRender = false;
	}
}

CObject* CSkyBox::CreateObjectScaled(int iModel, float fScale)
{
	auto* vecRot = new CVector();
	oldscale = fScale;
	auto* vecScale = new CVector(fScale);

	if (!pNetGame)
		return nullptr;

	if (!pNetGame->GetObjectPool())
		return nullptr;

	auto* object = pGame->NewObject(iModel, 0.0f, 0.0f, 0.0f, vecRot->Get(), 0.0f);

	*(uint32_t*)((uintptr_t)object->m_pEntity + 28) &= 0xFFFFFFFE;
	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;

	object->RemovePhysical();

	MATRIX4X4 matrix;
	object->GetMatrix(&matrix);

	RwMatrixScale(&matrix, vecScale);

	object->SetMatrix(matrix);
	object->UpdateRwMatrixAndFrame();

	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
	object->AddPhysical();

	return object;
}

void CSkyBox::ReTexture()
{
	int iHours = pNetGame->m_byteWorldTime;

	// -- TODO - Accidentally from several
	if (pChangeTime != iHours)
	{
		pChangeTime = iHours;
		if (iHours >= 10 && iHours <= 17)
			SetTexture("BR_Nebo_Day");

		if (iHours >= 18 && iHours <= 21)
			SetTexture("BR_Nebo_Zakat");
		if (iHours >= 22 && iHours <= 24)
			SetTexture("BR_Nebo_Nght");

		if (iHours >= 0 && iHours <= 4)
			SetTexture("BR_Nebo_Nght");

		if (iHours >= 5 && iHours <= 9)
			SetTexture("BR_Nebo_Rasv");

	}
	// ---

	uintptr_t pAtomic = m_pSkyObject->m_pEntity->m_RwObject;
	if (!pAtomic)
		return;

	if (!*(uintptr_t*)(pAtomic + 4))
		return;

	((void(*)())(SA_ADDR(0x559EF8 + 1)))(); // -- DeActivateDirectional
	((void* (*)())(SA_ADDR(0x559FC8 + 1)))(); // -- SetFullAmbient
	((void(*)())(SA_ADDR(0x559FEC + 1)))(); // -- SetAmbientColours
	*(uintptr_t*)(g_libGTASA + 0x61572C) |= 0x1000000;
	*(uintptr_t*)(g_libGTASA + 0x61572C) |= 0x08;
	((uintptr_t(*)(uintptr_t, uintptr_t, CObject*))(SA_ADDR(0x1AEE2C + 1)))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCallback, m_pSkyObject); // RwFrameForAllObjects
	*(uintptr_t*)(g_libGTASA + 0x61572C) &= ~0x1000000;
	*(uintptr_t*)(g_libGTASA + 0x61572C) &= ~0x08;
}

uintptr_t CSkyBox::RwFrameForAllObjectsCallback(uintptr_t object, CObject* pObject)
{
	if (*(uint8_t*)object != 1)
		return object;

	uintptr_t pAtomic = object;
	RpGeometry* pGeom = *(RpGeometry**)(pAtomic + 24);
	if (!pGeom)
		return object;

	int numMats = pGeom->matList.numMaterials;
	if (numMats > 16)
		numMats = 16;

	for (int i = 0; i < numMats; i++)
	{
		RpMaterial* pMaterial = pGeom->matList.materials[i];
		if (!pMaterial)
			continue;

		if (pSkyTexture)
			pMaterial->texture = pSkyTexture;
	}

	return object;
}

void CSkyBox::SetTexture(const char* texName)
{
	if (texName == nullptr)
		return;


	if (m_TextureName != texName) {
		m_TextureName = texName;
		pSkyTexture = (RwTexture*)LoadTextureFromDB("mobile", texName);
	}


}

void CSkyBox::SetRotSpeed(float speed)
{
	m_fRotSpeed = speed;
}

bool CSkyBox::IsNeedRender()
{
	return m_bNeedRender;
}

CObject* CSkyBox::GetSkyObject()
{
	return m_pSkyObject;
}

