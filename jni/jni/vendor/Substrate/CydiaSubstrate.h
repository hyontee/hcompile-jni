#ifndef CYDIA_SUBSTRATE_H
#define CYDIA_SUBSTRATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef const void *MSImageRef;

MSImageRef MSGetImageByName(const char *file);
void *MSFindSymbol(MSImageRef image, const char *name);

void MSHookMemory(void *target, const void *data, size_t size);

#ifdef __arm__
void MSHookFunction(void *symbol, void *replace, void **result);
#elif defined(__aarch64__)
void MSHookFunction(void *symbol, void *replace, void **result);
#elif defined(__i386__)
void MSHookFunction(void *symbol, void *replace, void **result);
#elif defined(__x86_64__)
void MSHookFunction(void *symbol, void *replace, void **result);
#else
void MSHookFunction(void *symbol, void *replace, void **result);
#endif

#ifdef __OBJC__
#include <objc/runtime.h>
void MSHookMessageEx(Class _class, SEL message, IMP hook, IMP *old);
#endif

#ifdef __cplusplus
}
#endif

#endif // CYDIA_SUBSTRATE_H
