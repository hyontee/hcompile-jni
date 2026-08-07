#include "plugin.h"
#include "xorstr.h"
#include <atomic>
#include <mutex>
#include <string>
#include <android/log.h>

// Логирование для отладки
#define LOG_TAG "NativePlugin"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

uintptr_t CGameAPI::m_address = 0;
std::once_flag m_address_once;
std::mutex m_address_mutex;

// Более надежная проверка адресов
bool IsValidAddress(uintptr_t address) {
    if (address < 0x1000) {
        LOGW("Address too low: 0x%lx", address);
        return false;
    }
    
#ifdef __LP64__
    // Для 64-битных систем
    if (address > 0x7FFFFFFFFFFF) {
        LOGW("Address too high (64-bit): 0x%lx", address);
        return false;
    }
#else
    // Для 32-битных систем
    if (address > 0x7FFFFFFF) {
        LOGW("Address too high (32-bit): 0x%lx", address);
        return false;
    }
#endif
    
    return true;
}

// Улучшенный callback
int callback(struct dl_phdr_info *info, size_t size, void *data) {
    if (!info || !info->dlpi_name) {
        LOGW("Invalid dl_phdr_info in callback");
        return 0;
    }
    
    const char* name = info->dlpi_name;
    LOGD("Checking library: %s", name);
    
    // Ищем полное имя или базовое имя
    const char* basename = strrchr(name, '/');
    if (basename) {
        basename++; // Пропускаем '/'
    } else {
        basename = name;
    }
    
    // Более гибкое сравнение
    if (strstr(name, xorstr("libblackrussia-client.so")) || 
        (basename && strcmp(basename, xorstr("libblackrussia-client.so")) == 0)) {
        
        // Проверяем, что библиотека имеет сегменты
        if (info->dlpi_phnum > 0 && info->dlpi_phdr) {
            std::lock_guard<std::mutex> lock(m_address_mutex);
            CGameAPI::m_address = info->dlpi_addr;
            LOGD("Found libblackrussia-client.so at address: 0x%lx", CGameAPI::m_address);
            return 1;
        } else {
            LOGW("Library found but has no segments: %s", name);
        }
    }
    return 0;
}

uintptr_t CGameAPI::GetBase(const char* offsetName)
{
    LOGD("GetBase called with offsetName: %s", offsetName ? offsetName : "NULL");
    
    // Инициализируем адрес один раз потокобезопасно
    std::call_once(m_address_once, []() {
        LOGD("Initializing base address...");
        dl_iterate_phdr(callback, nullptr);
    });
    
    if (CGameAPI::m_address) {
        if (!IsValidAddress(CGameAPI::m_address)) {
            LOGE("Invalid base address: 0x%lx", CGameAPI::m_address);
            return 0;
        }
        
        if (offsetName == NULL) {
            LOGD("Returning base address: 0x%lx", CGameAPI::m_address);
            return CGameAPI::m_address;
        } else {
            uintptr_t offset = COffset::Get(offsetName);
            LOGD("Offset for %s: 0x%lx", offsetName, offset);
            
            if (offset == 0) {
                LOGW("Offset not found for: %s", offsetName);
                return 0;
            }
            
            uintptr_t result = CGameAPI::m_address + offset;
            
            if (!IsValidAddress(result)) {
                LOGE("Invalid result address: 0x%lx (base: 0x%lx + offset: 0x%lx)", 
                     result, CGameAPI::m_address, offset);
                return 0;
            }
            
            LOGD("Returning address: 0x%lx", result);
            return result;
        }
    } else {
        LOGE("Failed to find base address");
        return 0;
    }
}

// Безопасная версия с использованием std::string (без утечек памяти)
std::string jbyteArrayToString(JNIEnv* env, jbyteArray byteArray) {
    if (byteArray == nullptr) {
        LOGW("jbyteArrayToString received null byteArray");
        return "";
    }
    
    // Проверяем исключения
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGW("JNI exception pending in jbyteArrayToString");
        return "";
    }
    
    jsize length = env->GetArrayLength(byteArray);
    if (length == 0) {
        return "";
    }
    
    jbyte* byteElements = env->GetByteArrayElements(byteArray, nullptr);
    if (byteElements == nullptr) {
        LOGE("Failed to get byte array elements");
        return "";
    }
    
    std::string result(reinterpret_cast<char*>(byteElements), length);
    
    env->ReleaseByteArrayElements(byteArray, byteElements, JNI_ABORT);
    
    return result;
}

// Старая версия для совместимости (если нужен указатель)
const char* jbyteArrayToCharArray(JNIEnv* env, jbyteArray byteArray) {
    static thread_local std::string buffer;
    buffer = jbyteArrayToString(env, byteArray);
    return buffer.c_str();
}

// Функция для освобождения памяти (если используется старая версия)
void FreeJByteArrayChars(const char* str) {
    // Пустая функция для совместимости - память управляется std::string
}

// Улучшенная функция конвертации с проверками
bool cp1251_to_utf8_safe(char *out, size_t out_size, const char *in, unsigned int len) {
    if (!out || !in || out_size == 0) {
        LOGE("Invalid parameters in cp1251_to_utf8_safe");
        return false;
    }
    
    static const int table[128] = 
    {                    
        // 80
        0x82D0,     0x83D0,     0x9A80E2,   0x93D1,     0x9E80E2,   0xA680E2,   0xA080E2,   0xA180E2,
        0xAC82E2,   0xB080E2,   0x89D0,     0xB980E2,   0x8AD0,     0x8CD0,     0x8BD0,     0x8FD0,
        // 90
        0x92D1,     0x9880E2,   0x9980E2,   0x9C80E2,   0x9D80E2,   0xA280E2,   0x9380E2,   0x9480E2,
        0,          0xA284E2,   0x99D1,     0xBA80E2,   0x9AD1,     0x9CD1,     0x9BD1,     0x9FD1,
        // A0
        0xA0C2,     0x8ED0,     0x9ED1,     0x88D0,     0xA4C2,     0x90D2,     0xA6C2,     0xA7C2,              
        0x81D0,     0xA9C2,     0x84D0,     0xABC2,     0xACC2,     0xADC2,     0xAEC2,     0x87D0,
        // B0
        0xB0C2,     0xB1C2,     0x86D0,     0x96D1,     0x91D2,     0xB5C2,     0xB6C2,     0xB7C2,              
        0x91D1,     0x9684E2,   0x94D1,     0xBBC2,     0x98D1,     0x85D0,     0x95D1,     0x97D1,
        // C0
        0x90D0,     0x91D0,     0x92D0,     0x93D0,     0x94D0,     0x95D0,     0x96D0,     0x97D0,
        0x98D0,     0x99D0,     0x9AD0,     0x9BD0,     0x9CD0,     0x9DD0,     0x9ED0,     0x9FD0,
        // D0
        0xA0D0,     0xA1D0,     0xA2D0,     0xA3D0,     0xA4D0,     0xA5D0,     0xA6D0,     0xA7D0,
        0xA8D0,     0xA9D0,     0xAAD0,     0xABD0,     0xACD0,     0xADD0,     0xAED0,     0xAFD0,
        // E0
        0xB0D0,     0xB1D0,     0xB2D0,     0xB3D0,     0xB4D0,     0xB5D0,     0xB6D0,     0xB7D0,
        0xB8D0,     0xB9D0,     0xBAD0,     0xBBD0,     0xBCD0,     0xBDD0,     0xBED0,     0xBFD0,
        // F0
        0x80D1,     0x81D1,     0x82D1,     0x83D1,     0x84D1,     0x85D1,     0x86D1,     0x87D1,
        0x88D1,     0x89D1,     0x8AD1,     0x8BD1,     0x8CD1,     0x8DD1,     0x8ED1,     0x8FD1
    };

    int count = 0;
    char *original_out = out;
    
    while (*in && count < (int)out_size - 4) // -4 для запаса под самый длинный UTF-8 символ
    {
        if(len && (count >= (int)len)) break;

        if (*in & 0x80)
        {
            int v = table[(int)(0x7f & *in++)];
            if (!v) {
                LOGW("Invalid CP1251 character at position %d", count);
                continue;
            }
            
            // Проверяем, достаточно ли места
            int bytes_needed = (v >> 16) ? 3 : 2;
            if (out - original_out + bytes_needed >= (int)out_size - 1) {
                LOGW("Output buffer too small in cp1251_to_utf8");
                break;
            }
            
            *out++ = (char)v;
            *out++ = (char)(v >> 8);
            if (v >>= 16)
                *out++ = (char)v;
        }
        else // ASCII
        {
            if (out - original_out >= (int)out_size - 1) {
                LOGW("Output buffer too small for ASCII in cp1251_to_utf8");
                break;
            }
            *out++ = *in++;
        }
        count++;
    }

    *out = '\0';
    return true;
}

// Оригинальная функция для совместимости
void cp1251_to_utf8(char *out, const char *in, unsigned int len) {
    // Используем буфер достаточного размера
    const size_t buffer_size = 4096;
    static thread_local char buffer[buffer_size];
    
    if (cp1251_to_utf8_safe(buffer, buffer_size, in, len)) {
        strcpy(out, buffer);
    } else {
        // В случае ошибки копируем оригинальную строку
        strncpy(out, in, buffer_size - 1);
        out[buffer_size - 1] = '\0';
    }
}

// Дополнительная функция для обработки JNI исключений
JNIEnv* SafeGetJNIEnv(JavaVM* vm) {
    if (!vm) {
        LOGE("JavaVM is null");
        return nullptr;
    }
    
    JNIEnv* env = nullptr;
    jint result = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    
    if (result == JNI_EDETACHED) {
        LOGD("Attaching thread to JVM");
        result = vm->AttachCurrentThread(&env, nullptr);
        if (result != JNI_OK || !env) {
            LOGE("Failed to attach thread to JVM: %d", result);
            return nullptr;
        }
    } else if (result != JNI_OK || !env) {
        LOGE("Failed to get JNIEnv: %d", result);
        return nullptr;
    }
    
    return env;
}

// Функция для проверки доступности библиотеки
bool IsLibraryLoaded() {
    uintptr_t base = CGameAPI::GetBase(nullptr);
    if (base && IsValidAddress(base)) {
        LOGD("Library is loaded at: 0x%lx", base);
        return true;
    }
    
    LOGE("Library is not loaded or invalid");
    return false;
}