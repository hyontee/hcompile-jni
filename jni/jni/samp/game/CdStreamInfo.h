#pragma once


#include "../game/common.h"

enum class eCdStreamStatus : int32 {
    READING_SUCCESS = 0,
    WAITING_TO_READ = 250,
    READING_FAILURE = 254,
    READING = 255,
};

int32 CdStreamOpen(const char* lpFileName);