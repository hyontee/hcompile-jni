// -- -- -- -- -- --
// Created by x1y2z
// -- -- -- -

// -- INCLUDE`S
#include "CSkyBox.h"

#include "../game/game.h"
#include "../game_sa/game_sa.h"

#include "../net/netgame.h"
#include "../gui/gui.h"

#include "../CSettings.h"
#include "../game/RW/RwHelper.h"
#include "util/armhook.h"

// -- EXTERN`S
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

#define MAGIC_FLOAT       (float)(50.0 / 30.0)
#define WEATHER_FOR_STARS (eWeatherType::WEATHER_UNDERWATER)

RpAtomic* pSkyAtomic = NULL;
RwFrame* pSkyFrame = NULL;
Skybox *aSkyboxes[eWeatherType::WEATHER_UNDERWATER + 1];
CVector oldSkyboxScale, newSkyboxScale, starsSkyboxScale, cloudsRotationVector, starsRotationVector;
bool changeWeather = true, usingInterp = false, processedFirst = false;
float testInterp = 0.0f, inCityFactor = 0.0f;
CVector ZAxis(0.0f, 0.0f, 1.0f);
CVector XYAxis(1.0f, 1.0f, 0.0f);
RwRGBAReal vecSkyColor = {1.0f, 1.0f, 1.0f, 1.0f};
float increaseRot = 0.0f, windStrengthRot = 0.0f, windRotScale = 0.006f;
bool sunReflectionChanged = false, skyboxDrawAfter = true, windStrengthAffectsRotation = true;
float lastFarClip = 0.0f, minFarPlane = 1100.0f, gameDefaultFogDensity = 1.0f, fogDensityDefault = 0.0012f, fogDensity = fogDensityDefault;
int skyboxFogType = 2;
float cloudsRotationSpeed = 0.002f, starsRotationSpeed = 0.0002f, skyboxSizeXY = 0.4f, skyboxSizeZ = 0.4f, cloudsMultBrightness = 0.4f,
        cloudsNightDarkLimit = 0.8f, cloudsMinBrightness = 0.3f, cloudsCityOrange = 1.0f, starsCityAlphaRemove = 0.8f, cloudsMultSunrise = 2.5f;

float *ms_fTimeScale, *ms_fTimeStep, *UnderWaterness, *InterpolationValue, *Wind;
uint32_t *m_snTimeInMilliseconds;
uint16_t *NewWeatherType, *OldWeatherType, *ForcedWeatherType;
uint8_t *ms_nGameClockMonths, *ms_nGameClockHours;
int *m_bExtraColourOn, *m_CurrentStoredValue;
eWeatherRegion *WeatherRegion;
CColourSet *m_CurrentColours;
CVector *m_VectorToSun;
bool *m_aCheatsActive;

float           (*GetDayNightBalance)();
void            (*DeActivateDirectional)();
void            (*SetAmbientColours)(RwRGBAReal*);
void            (*SetFullAmbient)();
void            (*RenderAtomicWithAlpha)(RpAtomic*, int alphaVal);
void            (*RwFrameScale)(RwFrame*, CVector*, RwOpCombineType);

// 17476
void CSkyBox::Initialise()
{
    const char* szStartPath = "realskybox/tex/";

    FILE *file;
    char line[256], textureName[32];
    bool bStartLoading = false;
    int weatherId = 0;

    // Loading ONLY DXT because it's unpixelated and looks way more better!
    uintptr_t tdb = (( uintptr_t (*)(const char*))(g_libGTASA + 0x1BF530 + 1))("samp");
    if(!tdb) return;

    TextureDatabaseRuntime::Register((TextureDatabase *)tdb);

    snprintf(line, sizeof(line), "%s/SAMP/sampn.dat", g_pszStorage);
    if ((file = fopen(line, "r")) == NULL) return;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if(line[0] == 0 || line[0] == '#' || line[0] == ';' || line[0] == '/') continue;
        if(!bStartLoading)
        {
            if(!strncmp(line, "skytexs", 7)) bStartLoading = true;
            continue;
        }

        if(!strncmp(line, "end", 3)) break;

        if((sscanf(line, "%d, %s", &weatherId, (char*)&textureName) == 2 ||
            sscanf(line, "%d %s", &weatherId, (char*)&textureName) == 2) && weatherId <= WEATHER_FOR_STARS)
        {
            //aSkyboxes[weatherId]->tex = GetTexIfLoaded(textureName);
            //if (!aSkyboxes[weatherId]->tex)
            {
                aSkyboxes[weatherId]->tex = TextureDatabaseRuntime::GetTexture(textureName);
                if (aSkyboxes[weatherId]->tex)
                {
                    Log("texture loaded %s", textureName);
                    aSkyboxes[weatherId]->tex->filterAddressing = 2;
                }
                else
                {
                    Log("texture not loaded %s", textureName);
                }
            }
        }
    }

    TextureDatabaseRuntime::UnRegister((TextureDatabase *)tdb);
    fclose(file);

    RwStream *stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "SAMP/sampn.dff");
    if (stream)
    {
        if (RwStreamFindChunk(stream, 16, 0, 0))
        {
            RpClump *clump = RpClumpStreamRead(stream);
            if (clump)
            {
                RpAtomic *atomic = GetFirstAtomic(clump);
                atomic->renderCallBack = AtomicDefaultRenderCallBack;

                if (atomic)
                {
                    atomic->geometry->flags |= rpGEOMETRYMODULATEMATERIALCOLOR;
                    pSkyAtomic = atomic;
                    pSkyFrame = RwFrameCreate();
                    RpAtomicSetFrame(pSkyAtomic, pSkyFrame);
                    RwFrameUpdateObjects(pSkyFrame);
                }
            }
        }
    }
    RwStreamClose(stream, 0);
}

void SetInUseForThisTexture(RwTexture *tex)
{
    for (int i = 0; i <= eWeatherType::WEATHER_UNDERWATER; ++i)
    {
        if (tex == aSkyboxes[i]->tex) aSkyboxes[i]->inUse = true;
    }
}
void SetRotationForThisTexture(RwTexture *tex, float rot)
{
    for (int i = 0; i <= eWeatherType::WEATHER_UNDERWATER; ++i)
    {
        if (tex == aSkyboxes[i]->tex) aSkyboxes[i]->rot = rot;
    }
}
bool NoSunriseWeather(eWeatherType id)
{
    return (id == eWeatherType::WEATHER_CLOUDY_COUNTRYSIDE || id == eWeatherType::WEATHER_CLOUDY_LA || id == eWeatherType::WEATHER_CLOUDY_SF || id == eWeatherType::WEATHER_CLOUDY_VEGAS ||
            id == eWeatherType::WEATHER_RAINY_COUNTRYSIDE || id == eWeatherType::WEATHER_RAINY_SF || id == eWeatherType::WEATHER_FOGGY_SF);
}
void CSkyBox::RenderSkybox()
{
    int oldWeatherType = *OldWeatherType;
    int newWeatherType = *NewWeatherType;
    if (oldWeatherType > eWeatherType::WEATHER_UNDERWATER) oldWeatherType = eWeatherType::WEATHER_SUNNY_LA;
    if (newWeatherType > eWeatherType::WEATHER_UNDERWATER) newWeatherType = eWeatherType::WEATHER_SUNNY_LA;

    if (increaseRot > 0.0f)
    {
        increaseRot -= pow(0.08f, 2) * *ms_fTimeStep * MAGIC_FLOAT;
        if (increaseRot < 0.0f) increaseRot = 0.0f;
    }

    // Tweak by distance
    float farPlane = TheCamera->m_pRwCamera->farPlane-300.0f;
    float scaleFactor = 0.95f * farPlane / pSkyAtomic->boundingSphere.radius / 0.4f;
    float goodDistanceFactor = (farPlane - 1000.0f) / 1000.0f; //  if farPlane is 2000.0, goodDistanceFactor is 2.0
    if (goodDistanceFactor < 0.01f) goodDistanceFactor = 0.01f;

    if (skyboxFogType <= 1) //linear
    {
        fogDensity = gameDefaultFogDensity;
    }
    else
    {
        fogDensity = fogDensityDefault / goodDistanceFactor;
        if (*UnderWaterness > 0.4f)
        {
            fogDensity *= 1.0f + ((*UnderWaterness - 0.4f) * 100.0f);
            fogDensity *= 0.1f;
        }
    }

    oldSkyboxScale.x = skyboxSizeXY * scaleFactor;//goodDistanceFactor;
    oldSkyboxScale.y = skyboxSizeXY * scaleFactor;//goodDistanceFactor;
    oldSkyboxScale.z = skyboxSizeZ  * scaleFactor;//goodDistanceFactor;

    newSkyboxScale.x = oldSkyboxScale.x * 1.05f;
    newSkyboxScale.y = oldSkyboxScale.y * 1.05f;
    newSkyboxScale.z = oldSkyboxScale.z * 1.05f;

    starsSkyboxScale.x = newSkyboxScale.x * 1.05f;
    starsSkyboxScale.y = newSkyboxScale.y * 1.05f;
    starsSkyboxScale.z = newSkyboxScale.z * 1.05f;

    float oldAlpha = (1.0f - *InterpolationValue) * 255.0f;
    float newAlpha = *InterpolationValue * 255.0f;
    float dayNightBalance = GetDayNightBalance();

    // Get position
    CVector camPos = TheCamera->GetPosition();

    for (int i = 0; i <= eWeatherType::WEATHER_UNDERWATER; ++i)
    {
        if (!aSkyboxes[i]->inUse)
        {
            if (i == WEATHER_FOR_STARS)
            {
                aSkyboxes[i]->rot = *ms_nGameClockMonths * 30.0f; // stars always starts with rotation based on month
            }
            else
            {
                srand(time(NULL));
                aSkyboxes[i]->rot = rand() * (360.0f / (float)RAND_MAX);
            }
        }
        aSkyboxes[i]->inUse = false; // reset flag
    }

    if (*WeatherRegion == eWeatherRegion::WEATHER_REGION_DEFAULT || *WeatherRegion == eWeatherRegion::WEATHER_REGION_DESERT)
    {
        inCityFactor -= 0.001f * *ms_fTimeStep * MAGIC_FLOAT;
        if (inCityFactor < 0.0f) inCityFactor = 0.0f;
    }
    else
    {
        inCityFactor += 0.001f * *ms_fTimeStep * MAGIC_FLOAT;
        if (inCityFactor > 1.0f) inCityFactor = 1.0f;
    }

    // Get stars alpha
    float starsAlpha = 0.0f;
    if (dayNightBalance > 0.0f)
    {
        float skyIllumination = (m_CurrentColours->m_nSkyBottomRed + m_CurrentColours->m_nSkyBottomGreen + m_CurrentColours->m_nSkyBottomBlue + m_CurrentColours->m_nSkyBottomRed + m_CurrentColours->m_nSkyBottomGreen + m_CurrentColours->m_nSkyBottomBlue) / 255.0f;
        if (skyIllumination > 1.0f) skyIllumination = 1.0f;
        starsAlpha = (1.0f - skyIllumination) * dayNightBalance;
        starsAlpha -= 1.0f * (inCityFactor * (starsCityAlphaRemove / 2.0f));
    }

    // Next weather texture is different from current?
    bool newTexIsDifferent = (aSkyboxes[oldWeatherType]->tex != aSkyboxes[newWeatherType]->tex);
    if (!newTexIsDifferent)
    {
        oldAlpha += newAlpha;
        if (oldAlpha > 255.0f) oldAlpha = 255.0f;
    }

    // Process rotation
    if (m_aCheatsActive[0x13]) // fast clock
    {
        aSkyboxes[oldWeatherType]->rot += 0.1f + increaseRot * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
        if (newTexIsDifferent) aSkyboxes[newWeatherType]->rot += (0.1f * 0.7f) + increaseRot * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
        aSkyboxes[WEATHER_FOR_STARS]->rot += 0.005f + increaseRot * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
    }
    else
    {
        if(windStrengthAffectsRotation) windStrengthRot = *Wind * windRotScale;
        else windStrengthRot = 0;

        aSkyboxes[oldWeatherType]->rot += (cloudsRotationSpeed * 0.5f) + (increaseRot + windStrengthRot) * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
        if (newTexIsDifferent) aSkyboxes[newWeatherType]->rot += (cloudsRotationSpeed * 0.5f * 0.7f) + (increaseRot + windStrengthRot) * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
        aSkyboxes[WEATHER_FOR_STARS]->rot += (starsRotationSpeed * 0.5f) + increaseRot * *ms_fTimeScale * (*ms_fTimeStep * MAGIC_FLOAT);
    }
    while (aSkyboxes[oldWeatherType]->rot > 360.0f) aSkyboxes[oldWeatherType]->rot -= 360.0f;
    while (aSkyboxes[newWeatherType]->rot > 360.0f) aSkyboxes[newWeatherType]->rot -= 360.0f;
    while (aSkyboxes[WEATHER_FOR_STARS]->rot > 360.0f) aSkyboxes[WEATHER_FOR_STARS]->rot -= 360.0f;
    SetRotationForThisTexture(aSkyboxes[oldWeatherType]->tex, aSkyboxes[oldWeatherType]->rot);
    if (newTexIsDifferent) SetRotationForThisTexture(aSkyboxes[newWeatherType]->tex, aSkyboxes[newWeatherType]->rot);

    // Get Ilumination
    float skyboxIllumination = ((m_CurrentColours->m_nSkyBottomRed + m_CurrentColours->m_nSkyBottomGreen + m_CurrentColours->m_nSkyBottomBlue) * cloudsMultBrightness) / 255.0f;
    if (skyboxIllumination > 1.0f) skyboxIllumination = 1.0f;
    if (dayNightBalance != 0.0f && inCityFactor != 0.0f) skyboxIllumination -= (dayNightBalance / 12.0f) * (1.0f - inCityFactor);
    float dayNightBalanceReverse = (1.0f - dayNightBalance);
    if (dayNightBalanceReverse < cloudsNightDarkLimit) dayNightBalanceReverse = cloudsNightDarkLimit;
    skyboxIllumination *= dayNightBalanceReverse;
    if (skyboxIllumination < cloudsMinBrightness) skyboxIllumination = cloudsMinBrightness;
    if (skyboxIllumination > 1.0f) skyboxIllumination = 1.0f;

    // Get color
    float sunriseFactor = 0.0f;
    float sunHorizonFactor = m_VectorToSun[*m_CurrentStoredValue].z;
    if (sunHorizonFactor > 0.0f)
    {
        if (sunHorizonFactor > 0.2f) sunHorizonFactor -= (sunHorizonFactor - 0.2f) * 2.0f; // 0.0 - 0.2 - 0.0
        sunriseFactor = sunHorizonFactor * 10.0f;
        if (sunriseFactor > 1.0f) sunriseFactor = 1.0f;
        if (NoSunriseWeather((eWeatherType)oldWeatherType)) sunriseFactor -= (oldAlpha / 255.0f);
        if (NoSunriseWeather((eWeatherType)newWeatherType)) sunriseFactor -= (newAlpha / 255.0f);
        if (sunriseFactor > 0.0f)
        {
            sunriseFactor *= cloudsMultSunrise;
            if (sunHorizonFactor > 0.0f) sunriseFactor += (abs(1.0f - sunHorizonFactor) * sunHorizonFactor);
        }
        else
        {
            sunriseFactor = 0.0f;
        }
    }
    sunriseFactor += ((cloudsCityOrange / 4.0f) * inCityFactor) * dayNightBalance;

    vecSkyColor =
            {
                    skyboxIllumination,
                    (skyboxIllumination - (sunriseFactor / 16.0f)),
                    (skyboxIllumination - (sunriseFactor / 10.0f)),
                    1.0f,
            };

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)true);
    RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void*)(intptr_t)skyboxFogType); // warning: cast to 'void *' from smaller integer type 'int'
    RwRenderStateSet(rwRENDERSTATEFOGDENSITY, &fogDensity);

    // Render skyboxes
    if (starsAlpha > 0.0f) // Stars
    {
        RwFrameTranslate(pSkyFrame, &camPos, rwCOMBINEREPLACE);
        RwFrameScale(pSkyFrame, &starsSkyboxScale, rwCOMBINEPRECONCAT);
        RwFrameRotate(pSkyFrame, &ZAxis, aSkyboxes[WEATHER_FOR_STARS]->rot, rwCOMBINEPRECONCAT);
        RwFrameUpdateObjects(pSkyFrame);

        pSkyAtomic->geometry->matList.materials[0]->texture = aSkyboxes[WEATHER_FOR_STARS]->tex;
        aSkyboxes[WEATHER_FOR_STARS]->inUse = true;

        int finalAlpha = (int)(starsAlpha * 255.0f);
        if (skyboxFogType <= 1 && *UnderWaterness > 0.4f) finalAlpha /= 1.0f + ((*UnderWaterness - 0.4f) * 100.0f);

#ifdef RENDER_MIRRORED
        CVector mirrorCamPos = camPos;
            mirrorCamPos.z -= 2.0f * scaleFactor * pSkyAtomic->boundingSphere.center.z;
            RwFrameTranslate(pSkyFrame, &mirrorCamPos, rwCOMBINEREPLACE);
            RwFrameRotate(pSkyFrame, &XYAxis, 180.0f, rwCOMBINEPRECONCAT);
            RwFrameScale(pSkyFrame, &starsSkyboxScale, rwCOMBINEPRECONCAT);
            RwFrameUpdateObjects(pSkyFrame);
            RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
#endif

        SetFullAmbient();
        DeActivateDirectional();
        pSkyAtomic->geometry->matList.materials[0]->color.alpha = finalAlpha;
        RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
    }

    if (aSkyboxes[oldWeatherType]->tex && oldAlpha > 0.0f) // Old (current)
    {
        RwFrameTranslate(pSkyFrame, &camPos, rwCOMBINEREPLACE);
        RwFrameScale(pSkyFrame, &oldSkyboxScale, rwCOMBINEPRECONCAT);
        RwFrameRotate(pSkyFrame, &ZAxis, aSkyboxes[oldWeatherType]->rot, rwCOMBINEPRECONCAT);
        RwFrameUpdateObjects(pSkyFrame);

        pSkyAtomic->geometry->matList.materials[0]->texture = aSkyboxes[oldWeatherType]->tex;
        SetInUseForThisTexture(aSkyboxes[oldWeatherType]->tex);

        int finalAlpha = (int)oldAlpha;
        if (skyboxFogType <= 1 && *UnderWaterness > 0.4f) finalAlpha /= 1.0f + ((*UnderWaterness - 0.4f) * 100.0f);

#ifdef RENDER_MIRRORED
        CVector mirrorCamPos = camPos;
            mirrorCamPos.z -= 2.0f * scaleFactor * pSkyAtomic->boundingSphere.center.z;
            RwFrameTranslate(pSkyFrame, &mirrorCamPos, rwCOMBINEREPLACE);
            RwFrameRotate(pSkyFrame, &XYAxis, 180.0f, rwCOMBINEPRECONCAT);
            RwFrameScale(pSkyFrame, &oldSkyboxScale, rwCOMBINEPRECONCAT);
            RwFrameUpdateObjects(pSkyFrame);
            RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
#endif

        SetAmbientColours(&vecSkyColor);
        DeActivateDirectional();
        pSkyAtomic->geometry->matList.materials[0]->color.alpha = finalAlpha;
        RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
    }

    if (newTexIsDifferent && aSkyboxes[newWeatherType]->tex && newAlpha > 0.0f) // New (next)
    {
        RwFrameTranslate(pSkyFrame, &camPos, rwCOMBINEREPLACE);
        RwFrameScale(pSkyFrame, &newSkyboxScale, rwCOMBINEPRECONCAT);
        RwFrameRotate(pSkyFrame, &ZAxis, aSkyboxes[newWeatherType]->rot, rwCOMBINEPRECONCAT);
        RwFrameUpdateObjects(pSkyFrame);

        pSkyAtomic->geometry->matList.materials[0]->texture = aSkyboxes[newWeatherType]->tex;
        SetInUseForThisTexture(aSkyboxes[newWeatherType]->tex);

        int finalAlpha = (int)newAlpha;
        if (skyboxFogType <= 1 && *UnderWaterness > 0.4f) finalAlpha /= 1.0f + ((*UnderWaterness - 0.4f) * 100.0f);

#ifdef RENDER_MIRRORED
        CVector mirrorCamPos = camPos;
            mirrorCamPos.z -= 2.0f * scaleFactor * pSkyAtomic->boundingSphere.center.z;
            RwFrameTranslate(pSkyFrame, &mirrorCamPos, rwCOMBINEREPLACE);
            RwFrameRotate(pSkyFrame, &XYAxis, 180.0f, rwCOMBINEPRECONCAT);
            RwFrameScale(pSkyFrame, &newSkyboxScale, rwCOMBINEPRECONCAT);
            RwFrameUpdateObjects(pSkyFrame);
            RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
#endif

        SetAmbientColours(&vecSkyColor);
        DeActivateDirectional();
        pSkyAtomic->geometry->matList.materials[0]->color.alpha = finalAlpha;
        RenderAtomicWithAlpha(pSkyAtomic, finalAlpha);
    }

    RwRenderStateSet(rwRENDERSTATEFOGDENSITY, &gameDefaultFogDensity);
    RwRenderStateSet(rwRENDERSTATEFOGTYPE, (void*)true);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);
}


int (*CGame_Init3)(uintptr_t thiz);
int CGame_Init3_hook(uintptr_t thiz)
{
    Log("CGame_Init3_hook");

    for (int i = 0; i <= eWeatherType::WEATHER_UNDERWATER; ++i) { aSkyboxes[i] = new Skybox(); }

    //LoadSkyboxTextures();
    CSkyBox::Initialise();

    return CGame_Init3(thiz);
}

void (*GameLogicPassTime)(uintptr_t thiz, unsigned int time);
void GameLogicPassTime_hook(uintptr_t thiz, unsigned int time)
{
    if (TheCamera->m_fFadeAlpha > 160.0f)
    {
        int oldWeatherType = *OldWeatherType;
        int newWeatherType = *NewWeatherType;
        if (oldWeatherType > eWeatherType::WEATHER_UNDERWATER) oldWeatherType = eWeatherType::WEATHER_SUNNY_LA;
        if (newWeatherType > eWeatherType::WEATHER_UNDERWATER) newWeatherType = eWeatherType::WEATHER_SUNNY_LA;

        float fTime = log10((float)time) * 100.0f;
        aSkyboxes[oldWeatherType]->rot += (0.1f + fTime) * (*ms_fTimeStep * MAGIC_FLOAT);
        aSkyboxes[newWeatherType]->rot += ((0.1f * 0.7f) + fTime) * (*ms_fTimeStep * MAGIC_FLOAT);
        aSkyboxes[WEATHER_FOR_STARS]->rot += (0.005f + fTime) * (*ms_fTimeStep * MAGIC_FLOAT);
    }
    else
    {
        if (*m_bExtraColourOn == 0 && *OldWeatherType != eWeatherType::WEATHER_UNDERWATER && *NewWeatherType != eWeatherType::WEATHER_UNDERWATER && *ForcedWeatherType != eWeatherType::WEATHER_UNDERWATER)
        {
            float fTime = log10((float)time) * (*ms_fTimeStep * MAGIC_FLOAT);

            float increaseLimitMin = 0.1f * (*ms_fTimeStep * MAGIC_FLOAT);
            if (fTime < increaseLimitMin) fTime = increaseLimitMin;

            increaseRot += fTime;

            float increaseLimitMax = fTime * 0.5f * (*ms_fTimeStep * MAGIC_FLOAT);
            if (increaseRot > increaseLimitMax) increaseRot = increaseLimitMax;
        }
    }
    GameLogicPassTime(thiz, time);
}

void (*CClouds_Render)(uintptr_t thiz);
void CClouds_Render_hook(uintptr_t thiz)
{
    CSkyBox::RenderSkybox();
    //CClouds_Render(thiz);
}


void CSkyBox::InjectHooks()
{
    SET_TO(ms_fTimeScale,                   getSym(hGTASA, "_ZN6CTimer13ms_fTimeScaleE"));
    SET_TO(ms_fTimeStep,                    getSym(hGTASA, "_ZN6CTimer12ms_fTimeStepE"));
    SET_TO(UnderWaterness,                  getSym(hGTASA, "_ZN8CWeather14UnderWaternessE"));
    SET_TO(InterpolationValue,              getSym(hGTASA, "_ZN8CWeather18InterpolationValueE"));
    SET_TO(Wind,                            getSym(hGTASA, "_ZN8CWeather4WindE"));
    SET_TO(NewWeatherType,                  getSym(hGTASA, "_ZN8CWeather14NewWeatherTypeE"));
    SET_TO(OldWeatherType,                 getSym(hGTASA, "_ZN8CWeather14OldWeatherTypeE"));
    SET_TO(ForcedWeatherType,               getSym(hGTASA, "_ZN8CWeather17ForcedWeatherTypeE"));
    SET_TO(ms_nGameClockMonths,             getSym(hGTASA, "_ZN6CClock19ms_nGameClockMonthsE"));
    SET_TO(ms_nGameClockHours,              getSym(hGTASA, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(m_bExtraColourOn,                getSym(hGTASA, "_ZN10CTimeCycle16m_bExtraColourOnE"));
    SET_TO(m_CurrentStoredValue,            getSym(hGTASA, "_ZN10CTimeCycle20m_CurrentStoredValueE"));
    SET_TO(WeatherRegion,                  getSym(hGTASA, "_ZN8CWeather13WeatherRegionE"));
    SET_TO(m_CurrentColours,                getSym(hGTASA, "_ZN10CTimeCycle16m_CurrentColoursE"));
    SET_TO(m_VectorToSun,                   getSym(hGTASA, "_ZN10CTimeCycle13m_VectorToSunE"));
    SET_TO(m_aCheatsActive,                 getSym(hGTASA, "_ZN6CCheat15m_aCheatsActiveE"));

    SET_TO(GetDayNightBalance,             getSym(hGTASA, "_Z18GetDayNightBalancev"));
    SET_TO(DeActivateDirectional,           getSym(hGTASA, "_Z21DeActivateDirectionalv"));
    SET_TO(SetAmbientColours,               getSym(hGTASA, "_Z17SetAmbientColoursP10RwRGBAReal"));
    SET_TO(SetFullAmbient,                  getSym(hGTASA, "_Z14SetFullAmbientv"));
    SET_TO(RwFrameScale,                    getSym(hGTASA, "_Z12RwFrameScaleP7RwFramePK5RwV3d15RwOpCombineType"));
    SET_TO(RenderAtomicWithAlpha,           getSym(hGTASA, "_ZN18CVisibilityPlugins21RenderAtomicWithAlphaEP8RpAtomici"));

    InlineHook(g_libGTASA, 0x4088CC, &CGame_Init3_hook, &CGame_Init3);
    InlineHook(g_libGTASA, 0x2C3BB4, &GameLogicPassTime_hook, &GameLogicPassTime);
    InlineHook(g_libGTASA, 0x5292CC, &CClouds_Render_hook, &CClouds_Render);
}