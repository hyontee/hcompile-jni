#include "voicechat.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include <pthread.h>
#include <math.h>
#include <vector>
#include <cstdint>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ---------------------------------------------------------------------------
// Разбор ENet-пакета. Раскладка разная для ARM/ARM64 (из декомпиле):
//   ARM64 (sub_643E34):  event->packet @ event+24; data @ packet+16; len @ packet+24
//   ARM   (sub_43F964):  event->packet @ event+16; data @ packet+8;  len @ packet+12
// Тип пакета — первый байт data (CRawData read offset стартует с 0).
//
// Защита от случайных крашей: проверяем каждый указатель перед разыменованием
// и санитарно ограничиваем длину. Если что-то не так — отдаём пакет оригиналу
// (штатное поведение), а не падаем.
// ---------------------------------------------------------------------------
static inline bool LooksLikePtr(uintptr_t p)
{
    // отсекаем null и явно мусорные низкие адреса
    return p >= 0x1000;
}

static uint8_t* GetVoicePacketData(void* event, uint32_t* outLen)
{
    uintptr_t ev = (uintptr_t)event;
    if (!LooksLikePtr(ev)) return nullptr;

#if defined(__aarch64__)
    const uintptr_t kPacketOff = 24, kDataOff = 16, kLenOff = 24;
#elif defined(__arm__)
    const uintptr_t kPacketOff = 16, kDataOff = 8,  kLenOff = 12;
#else
    const uintptr_t kPacketOff = 24, kDataOff = 16, kLenOff = 24;
#endif

    uintptr_t packet = *(volatile uintptr_t*)(ev + kPacketOff);
    if (!LooksLikePtr(packet)) return nullptr;

    uintptr_t data = *(volatile uintptr_t*)(packet + kDataOff);
    if (!LooksLikePtr(data)) return nullptr;

    uint32_t len = *(volatile uint32_t*)(packet + kLenOff);
    if (len == 0 || len > (1u << 20)) return nullptr; // санитарный потолок ~1 МБ

    if (outLen) *outLen = len;
    return (uint8_t*)data;
}

// ---------------------------------------------------------------------------
// Самодостаточный звук-уведомление через OpenSL ES (без внешних ассетов).
// Тон 880 Гц, 16 кГц, mono, int16, с fade in/out. Ленивая инициализация под мьютексом.
// При любой ошибке инициализации тихо выключаемся (g_slFailed) — без краша.
// ---------------------------------------------------------------------------
namespace {

pthread_mutex_t g_slMutex = PTHREAD_MUTEX_INITIALIZER;
bool g_slReady  = false;
bool g_slFailed = false;

SLObjectItf g_engineObj = nullptr;
SLEngineItf g_engine    = nullptr;
SLObjectItf g_mixObj    = nullptr;
SLObjectItf g_playerObj = nullptr;
SLPlayItf   g_play      = nullptr;
SLAndroidSimpleBufferQueueItf g_bq = nullptr;

const int kSampleRate = 16000;
const int kDurationMs = 150;
std::vector<int16_t> g_tone;

void BuildTone()
{
    const int n = kSampleRate * kDurationMs / 1000;
    g_tone.resize(n);
    const double freq = 880.0;
    const double amp  = 0.30 * 32767.0;
    const int fade = kSampleRate / 100; // ~10 мс затухание, чтобы не было щелчка
    for (int i = 0; i < n; ++i) {
        double env = 1.0;
        if (i < fade)          env = (double)i / fade;
        else if (i > n - fade) env = (double)(n - i) / fade;
        g_tone[i] = (int16_t)(amp * env * sin(2.0 * M_PI * freq * (double)i / kSampleRate));
    }
}

bool InitEngineLocked()
{
    if (g_slReady)  return true;
    if (g_slFailed) return false;

    if (slCreateEngine(&g_engineObj, 0, nullptr, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) { g_slFailed = true; return false; }
    if ((*g_engineObj)->Realize(g_engineObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)         { g_slFailed = true; return false; }
    if ((*g_engineObj)->GetInterface(g_engineObj, SL_IID_ENGINE, &g_engine) != SL_RESULT_SUCCESS) { g_slFailed = true; return false; }

    if ((*g_engine)->CreateOutputMix(g_engine, &g_mixObj, 0, nullptr, nullptr) != SL_RESULT_SUCCESS) { g_slFailed = true; return false; }
    if ((*g_mixObj)->Realize(g_mixObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)                       { g_slFailed = true; return false; }

    SLDataLocator_AndroidSimpleBufferQueue locBq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 1 };
    SLDataFormat_PCM fmt;
    fmt.formatType    = SL_DATAFORMAT_PCM;
    fmt.numChannels   = 1;
    fmt.samplesPerSec = SL_SAMPLINGRATE_16;
    fmt.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
    fmt.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    fmt.channelMask   = SL_SPEAKER_FRONT_CENTER;
    fmt.endianness    = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource src = { &locBq, &fmt };

    SLDataLocator_OutputMix locMix = { SL_DATALOCATOR_OUTPUTMIX, g_mixObj };
    SLDataSink sink = { &locMix, nullptr };

    const SLInterfaceID ids[1] = { SL_IID_BUFFERQUEUE };
    const SLboolean     req[1] = { SL_BOOLEAN_TRUE };
    if ((*g_engine)->CreateAudioPlayer(g_engine, &g_playerObj, &src, &sink, 1, ids, req) != SL_RESULT_SUCCESS) { g_slFailed = true; return false; }
    if ((*g_playerObj)->Realize(g_playerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)                            { g_slFailed = true; return false; }
    if ((*g_playerObj)->GetInterface(g_playerObj, SL_IID_PLAY, &g_play) != SL_RESULT_SUCCESS)                   { g_slFailed = true; return false; }
    if ((*g_playerObj)->GetInterface(g_playerObj, SL_IID_BUFFERQUEUE, &g_bq) != SL_RESULT_SUCCESS)              { g_slFailed = true; return false; }

    (*g_play)->SetPlayState(g_play, SL_PLAYSTATE_PLAYING);

    BuildTone();
    g_slReady = true;
    return true;
}

} // namespace

void CVoiceNotify_PlaySound()
{
    pthread_mutex_lock(&g_slMutex);
    if (InitEngineLocked() && !g_tone.empty() && g_bq) {
        (*g_bq)->Clear(g_bq);
        (*g_bq)->Enqueue(g_bq, g_tone.data(), (SLuint32)(g_tone.size() * sizeof(int16_t)));
    }
    pthread_mutex_unlock(&g_slMutex);
}

// ---------------------------------------------------------------------------
// Сам хук. Возвращает int и прокидывает результат оригинала (единый ABI ARM/ARM64).
// ---------------------------------------------------------------------------
int (*orig_CVoiceChatClient__OnPacketIncoming)(void* event) = nullptr;

int hook_CVoiceChatClient__OnPacketIncoming(void* event)
{
    uint32_t len = 0;
    uint8_t* data = GetVoicePacketData(event, &len);
    if (data && len >= 1) {
        const uint8_t type = data[0];
        if (type == 246) {
            // ИГНОР: микрофон не глушим (оригинал не зовём → нет CSoundInput::Stop
            // и m_nRecording = 0). Просто проигрываем звук и выходим.
            CVoiceNotify_PlaySound();
            return 0;
        }
    }

    // 244 (голос), 245 (громкость) и всё прочее / невалидное — штатно через оригинал.
    if (orig_CVoiceChatClient__OnPacketIncoming) {
        return orig_CVoiceChatClient__OnPacketIncoming(event);
    }
    return 0;
}
