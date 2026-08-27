#pragma once
#include <string>
#include <jni.h>
#include <cstdlib>

// ===================== МУСОРНЫЕ МАКРОСЫ =====================

// Генерация случайных имён (привязывается к строке/строке кода)
#define RANDOM_NAME(base) base##__LINE__

// Фейковые циклы (мусор)
#define FAKE_LOOP for(int RANDOM_NAME(i)=0;RANDOM_NAME(i)<5;RANDOM_NAME(i)++){volatile int z=RANDOM_NAME(i)*1337;z^=777;}

// Фейковые матем. операции
#define FAKE_MATH(x,y) { volatile int RANDOM_NAME(res) = ((x) ^ (y)) + 1337; volatile int zz=RANDOM_NAME(res)*3; zz^=0xBEEF; }

// Фейковые условия
#define FAKE_IF(x) if(((x) ^ 0x1337) & 1){volatile int RANDOM_NAME(tmp)=x*42;}

// ===================== ОБФУСКАТОР =====================

#define OBFUSCATE(str) Obfuscator::decode(Obfuscator::encode(str))

class Obfuscator {
public:
    // Шифрование строки
    static std::string encode(const std::string& input) {
        std::string out = input;
        for (size_t i = 0; i < out.size(); i++) {
            out[i] ^= 0x5A;                       // XOR
            out[i] = (char)((out[i] + 17) % 256); // Сдвиг
        }
        return out;
    }

    // Дешифрование строки
    static std::string decode(const std::string& input) {
        std::string out = input;
        for (size_t i = 0; i < out.size(); i++) {
            out[i] = (char)((out[i] - 17 + 256) % 256);
            out[i] ^= 0x5A;
        }
        return out;
    }

    // ===================== МУСОРНЫЕ ФУНКЦИИ =====================

    static void RANDOM_NAME(fakeTrash)() {
        FAKE_LOOP;
        FAKE_MATH(123, 456);
        FAKE_IF(1337);
    }

    static int RANDOM_NAME(fakeCalc)(int a, int b) {
        FAKE_LOOP;
        return ((a * 1337) ^ (b + 0xC0FFEE)) + (rand() % 999);
    }

    static std::string RANDOM_NAME(fakeStr)() {
        return OBFUSCATE("FAKE_STRING_USED_FOR_CONFUSION");
    }
};