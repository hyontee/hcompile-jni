#pragma once

#include <stdint.h>

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef signed char    int8;
typedef signed short   int16;
typedef signed int     int32;

#ifndef NULL
#define NULL 0
#endif

// cat namespace types required by BigTypes.h
namespace cat
{
    typedef uint32_t u32;
    typedef uint16_t u16;
    typedef uint8_t  u8;
    typedef int32_t  s32;
    typedef int16_t  s16;
    typedef int8_t   s8;
    typedef uint64_t u64;
    typedef int64_t  s64;
}

#define INLINE inline
