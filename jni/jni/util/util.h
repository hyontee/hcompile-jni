#pragma once
#include "gtare/OSDeviceForm.h"
uintptr_t FindLibrary(const char* library);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);
int convert_utf8_to_windows1251(const char* utf8, char* windows1251, size_t n);
void AND_OpenLink(const char* szLink);
char* ConvertColorToHtml(std::string format);
static class CUtil {
public:
    static OSDeviceForm OS_SystemForm();
};