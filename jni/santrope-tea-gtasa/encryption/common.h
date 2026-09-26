#pragma once
#include <stdint.h>

// Compatibility implementation for the missing upstream encryption headers.
// The original project references these macros from several translation units.
// Keep them deterministic and type-safe for the Android build.
#ifndef OBFUSCATE_DATA
#define OBFUSCATE_DATA(x) (x)
#endif
#ifndef UNOBFUSCATE_DATA
#define UNOBFUSCATE_DATA(x) (x)
#endif
#ifndef PROTECT_CODE_SERVER_CHECK2
#define PROTECT_CODE_SERVER_CHECK2 do { } while (0)
#endif
