#include "main.h"
#include "patch.h"

#include <sys/mman.h>

#include <unistd.h> // system api
#include <sys/mman.h>
#include <assert.h> // assert()
#include <dlfcn.h> // dlopen



void* CHook::Lib = nullptr;
void CHook::installPLTHook(uintptr_t addr, uintptr_t func, uintptr_t *orig)
{
#if VER_x32
    if ( mprotect((void *)((addr) & 0xFFFFF000), 0x1000u, 7) )
        mprotect((void *)(addr & 0xFFFFF000), 0x1000u, 3);
#else
    if ( mprotect((void *)((addr) & 0xFFFFFFFFFFFFF000), 0x1000u, 7) )
        mprotect((void *)(addr & 0xFFFFFFFFFFFFF000), 0x1000u, 3);
#endif
    if(orig) {
        *orig = *(uintptr_t*)(addr);
    }

    *(uintptr_t*)(addr) = func;
}


void CHook::UnFuck(uintptr_t ptr, size_t len)
{
#if VER_x32
    if(mprotect((void*)(ptr & 0xFFFFF000), len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
            return;

        mprotect((void*)(ptr & 0xFFFFF000), len, PROT_READ | PROT_WRITE);
#else
    if(mprotect((void*)(ptr & 0xFFFFFFFFFFFFF000), len, PROT_READ | PROT_WRITE | PROT_EXEC) == 0)
        return;

    mprotect((void*)(ptr & 0xFFFFFFFFFFFFF000), len, PROT_READ | PROT_WRITE);
#endif
}


