#pragma once

#define SET_TO(__a1, __a2) *(void**)&(__a1) = (void*)(__a2)

#include <dlfcn.h>
#include "inlinehook.h"

extern "C"
{
void sub_naebal(uintptr_t dest, uintptr_t src, size_t size);
}

extern uintptr_t mmap_start;
extern uintptr_t mmap_end;
extern uintptr_t memlib_start;
extern uintptr_t memlib_end;
extern uintptr_t count_map;

void unProtect(uintptr_t ptr, size_t dwSize = 100);
void FuckCode(uintptr_t ptr, size_t dwSize = 100);

void WriteMemory(uintptr_t dest, uintptr_t src, size_t size);
void ReadMemory(uintptr_t dest, uintptr_t src, size_t size);

void makeNOP(uintptr_t addr, unsigned int count);
void makeJMP(uintptr_t func, uintptr_t addr);
void makeBLX(uintptr_t func, uintptr_t addr);

void installHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);
void InstallMethodHook(uintptr_t addr, uintptr_t func);
void installJMPHook(uintptr_t addr, uintptr_t func);
void installBLXHook(uintptr_t addr, uintptr_t func);

void CodeInject(uintptr_t addr, uintptr_t func, int reg);
uintptr_t zalupa(uintptr_t func, uintptr_t addr);

void RET(uintptr_t addr);

template <typename Ret, typename... Args>
static Ret CallFunction(unsigned int address, Args... args)
{
	return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
}

template <typename Src>
static void WriteMemory(uintptr_t dest, Src src, size_t size)
{
	dest += CRYPT_MASK;
	size += CRYPT_MASK;

	sub_naebal(dest, (uintptr_t)src, size);
}

template <typename Src>
static Src Write(uintptr_t dest, Src src, size_t size = 0)
{
	size = sizeof(Src);
	WriteMemory(dest, &src, size);
	return src;
}

void installPLTHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);

template <typename Addr, typename Func>
static void Redirect(uintptr_t lib, Addr addr, Func func)
{
	addr += CRYPT_MASK;
	registerInlineHook(lib + addr + 1, (uint32_t)func, (uint32_t**) nullptr);
	inlineHook(lib + addr + 1);
}

template <typename Addr, typename Func>
static void Redirect(Addr addr, Func func)
{
	addr += CRYPT_MASK;
	registerInlineHook(addr + 1, (uint32_t)func, (uint32_t**) nullptr);
	inlineHook(addr + 1);
}

static uintptr_t getSym(void* handle, const char* sym)
{
	return (uintptr_t)dlsym(handle, sym);
}

template <typename Addr, typename Func, typename Orig>
static void InlineHook(uintptr_t lib, Addr addr, Func func, Orig orig)
{
	*orig = NULL;
	addr += CRYPT_MASK;
	registerInlineHook(lib + addr + 1, (uint32_t)func, (uint32_t**)orig);
	inlineHook(lib + addr + 1);
}