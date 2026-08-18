#pragma once

#include "main.h"
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <shadowhook.h>
#ifdef __arm__
#define __32BIT
#define DETHUMB(_a) (((uintptr_t)_a) & ~0x1)
#define RETHUMB(_a) (((uintptr_t)_a) | 0x1)
#define THUMBMODE(_a) ((((uintptr_t)_a) & 0x1)||(((uintptr_t)_a) & 0x2)||(CHook::GetAddrBaseXDL((uintptr_t)_a) & 0x1))
extern "C" bool MSHookFunction(void* symbol, void* replace, void** result);
#elif defined __aarch64__
#define __64BIT
    #define DETHUMB(_a)
    #define RETHUMB(_a)
    #define THUMBMODE(_a) (false)
    #define cacheflush(c, n, zeroarg) __builtin___clear_cache((char*)(c), (char*)(n))
#else
    #error This lib is supposed to work on ARM only!
#endif
#define GET_LR(dwRetAddr) \
    do { \
        uintptr_t lr = reinterpret_cast<uintptr_t>(__builtin_return_address(0)); \
        dwRetAddr = lr - g_libGTASA; \
    } while (0)
class CHook {
public:

    static void UnFuck(uintptr_t ptr, size_t len = 100);

    static uintptr_t GetAddrBaseXDL(uintptr_t addr)
    {
#ifdef __XDL
        xdl_info_t info;
        void* cache = NULL;
        if(!xdl_addr((void*)addr, &info, &cache)) return 0;
        xdl_addr_clean(&cache);
        return (uintptr_t)info.dli_saddr;
#endif

        return 0;
    }
    template<typename Addr>
    static void NOP(Addr adr, size_t count)
    {
        // fully check
        auto addr = (uintptr)(adr);
#if VER_x32
        int bytesCount = 2 * count;
        uintptr_t endAddr = addr + bytesCount;
        UnFuck(addr, bytesCount);
        for (uintptr_t p = addr; p != endAddr; p += 2)
        {
            (*(char*)(p + 0)) = 0x00;
            (*(char*)(p + 1)) = 0xBF;
        }
        cacheflush(addr, endAddr, 0);

#else
        if(count > 1)
            count /= 2;

        int bytesCount = 4 * count;
        uintptr_t endAddr = addr + bytesCount;
        UnFuck(addr, bytesCount);
        for (uintptr_t p = addr; p != endAddr; p += 4)
        {
            (*(char*)(p + 0)) = 0x1F;
            (*(char*)(p + 1)) = 0x20;
            (*(char*)(p + 2)) = 0x03;
            (*(char*)(p + 3)) = 0xD5;
        }
        cacheflush(addr, endAddr, 0);
#endif
    }
    template <typename Func, typename Orig>
    static void HookFunc(const char * sym, Func func, Orig orig){
        auto hook = dlsym(CHook::Lib, sym);
        if ( !hook )
        {
            Log(OBFUSCATE("Error find %s"), sym);
            exit(0);
        }
        shadowhook_hook_func_addr(hook, (void*)func, (void**)orig);
    }
    template <typename Func, typename Orig>
    static void HookFunc(uintptr_t ptr, Func func, Orig orig){

        shadowhook_hook_func_addr((void*)ptr, (void*)func, (void**)orig);
    }
    template <typename Func>
    static void HookRedirect(const char *sym, Func func){
        auto hook = dlsym(CHook::Lib, sym);
        if ( !hook )
        {
            Log(OBFUSCATE("Error find %s"), sym);
            exit(0);
        }
        shadowhook_hook_func_addr(hook, (void*)func, nullptr);
    }


    static void RET(const char* sym)
    {
        auto addr = getSym(sym);
#if VER_x32
        if(THUMBMODE(addr))
        {
            WriteMemory2(DETHUMB(addr), (uintptr_t)"\x70\x47", 2);
        }
        else
        {
            WriteMemory2(addr, (uintptr_t)"\x1E\xFF\x2F\xE1", 4);
        }
#else
        WriteMemory2(addr, (uintptr_t)"\xC0\x03\x5F\xD6", 4);
#endif
    }

    static void RET(uintptr sym)
    {
#if VER_x32
        if(THUMBMODE(sym))
        {
            WriteMemory2(DETHUMB(sym), (uintptr_t)"\x70\x47", 2);
        }
        else
        {
            WriteMemory2(sym, (uintptr_t)"\x1E\xFF\x2F\xE1", 4);
        }
#else
        WriteMemory2(addr, (uintptr_t)"\xC0\x03\x5F\xD6", 4);
#endif
    }
    template <typename Src>
    static void WriteMemory2(uintptr_t dest, Src src, size_t size)
    {
        UnFuck(dest, size);
        memcpy((void*)dest, (void*)src, size);

        cacheflush(dest, dest + size, 0);
    }

    static void WriteMemory(uintptr_t dest, uintptr_t src, size_t size)//краши краши краши
    {
#if VER_x32
        if ( mprotect((void *)((dest) & 0xFFFFF000), size, 7) )
            mprotect((void *)((dest) & 0xFFFFF000), size, 3);
#else
        if ( mprotect((void *)((dest) & 0xFFFFFFFFFFFFF000), size, 7) )
            mprotect((void *)((dest) & 0xFFFFFFFFFFFFF000), size, 3);
#endif
        if(size == 1){
            *(uint8_t *)(dest) = src;
        }
        else if(size <= 2){
            *(uint16_t *)(dest) = src;
        }
        else if(size <= 4){
            *(uintptr_t *)(dest) = src;

        }


        //*(char*)(dest) = src;

        cacheflush(dest, dest + size, 0);
    }





    template <typename Src>
    static Src Write(uintptr_t lib, Src src)
    {
#if VER_x32
        if ( mprotect((void *)((lib) & 0xFFFFF000), 4u, 7) )
            mprotect((void *)((lib) & 0xFFFFF000), 4u, 3);
#else
        if ( mprotect((void *)((lib) & 0xFFFFFFFFFFFFF000), 8u, 7) )
            mprotect((void *)((lib) & 0xFFFFFFFFFFFFF000), 8u, 3);
#endif
        size_t size = sizeof(Src);
        Log(OBFUSCATE("write size: %d"), size);
        *(uintptr_t*)(lib) = (uintptr_t)src;
        cacheflush(*(uintptr_t*)(lib), *(uintptr_t*)(lib) + size, 0);

	    return src;
    }

    template <typename Src>
    static Src Write2(uintptr_t dest, Src src, size_t size = 0)
    {
        size = sizeof(Src);
        CHook::WriteMemory(dest, reinterpret_cast<uintptr_t>(&src), size);
        return src;
    }




    template <typename Ret, typename... Args>
    static inline Ret CallFunction(uintptr address, Args... args)
    {
        return (( Ret(*)(Args...))(address) )(args...);
    }
    template <typename Ret, typename... Args>
    static Ret CallFunction(const char *sym, Args... args)
    {
        void *func = dlsym((void *)CHook::Lib, sym);
        if ( !func )
        {
            Log(OBFUSCATE("Error find \"%s\""), sym);
            exit(0);
        }
	    return reinterpret_cast<Ret(__cdecl *)(Args...)>(func)(args...);
    }

    static void installPLTHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);

    static void unprotect(uintptr_t address, size_t size = 100);

    static void *Lib;
    static uintptr_t getSym(const char* sym)
    {

        auto res = (uintptr_t)dlsym(Lib, sym);
        if(res == 0){
            Log("Error find %s", sym);
            exit(0);
            return 0;
        }
        // Log("getsym = %x", sym - g_libGTASA);
        return res;
    }
    template <typename Addr, typename Func, typename Orig>
    static void InlineHook(uintptr_t lib, Addr addr, Func func, Orig orig)
    {
        shadowhook_hook_func_addr(
                (void*)(lib + addr),
                (void *)func,
                (void **)orig);
    }
    template <typename Func>
    static void Redirect(const char* sym, Func func)
    {
        auto addr = getSym(sym);

#ifdef __32BIT
        uint32_t hook[2] = {0xE51FF004, reinterpret_cast<uintptr_t>(func)};
        if (THUMBMODE(addr)) {
            addr &= ~0x1;
            if (addr & 0x2) {
                NOP(RETHUMB(addr), 1);
                addr += 2;
            }
            hook[0] = 0xF000F8DF;
        }
        WriteMemory2(DETHUMB(addr), reinterpret_cast<uintptr_t>(hook), sizeof(hook));
#elif defined __64BIT
        UnFuck(addr, 16);
        uint64_t hook[2] = {0xD61F022058000051, reinterpret_cast<uintptr_t>(func)};
        WriteMemory2(addr, reinterpret_cast<uintptr_t>(hook), sizeof(hook));
#endif
    }
};