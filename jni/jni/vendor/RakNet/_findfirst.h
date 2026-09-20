#pragma once

#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

struct _finddata_t
{
    char name[260];
    int attrib;
    unsigned long size;
};

#define _A_NORMAL 0x00
#define _A_RDONLY 0x01
#define _A_HIDDEN 0x02
#define _A_SYSTEM 0x04
#define _A_SUBDIR 0x10
#define _A_ARCH   0x20

struct _find_handle
{
    DIR* dir;
    char pattern[260];
    char basepath[260];
};

inline intptr_t _findfirst(const char* pattern, _finddata_t* fileinfo)
{
    char basepath[260];
    strncpy(basepath, pattern, sizeof(basepath)-1);
    // Remove wildcard part (e.g. "*.*)
    char* slash = strrchr(basepath, '/');
    if (slash) *slash = '\0';
    else strncpy(basepath, ".", sizeof(basepath)-1);

    DIR* dir = opendir(basepath);
    if (!dir) return -1;

    _find_handle* h = new _find_handle();
    h->dir = dir;
    strncpy(h->basepath, basepath, sizeof(h->basepath)-1);
    strncpy(h->pattern, pattern, sizeof(h->pattern)-1);

    struct dirent* entry = readdir(dir);
    if (!entry) { closedir(dir); delete h; return -1; }

    strncpy(fileinfo->name, entry->d_name, sizeof(fileinfo->name)-1);
    fileinfo->attrib = (entry->d_type == DT_DIR) ? _A_SUBDIR : _A_NORMAL;
    fileinfo->size = 0;

    return (intptr_t)h;
}

inline int _findnext(intptr_t handle, _finddata_t* fileinfo)
{
    _find_handle* h = (_find_handle*)handle;
    struct dirent* entry = readdir(h->dir);
    if (!entry) return -1;

    strncpy(fileinfo->name, entry->d_name, sizeof(fileinfo->name)-1);
    fileinfo->attrib = (entry->d_type == DT_DIR) ? _A_SUBDIR : _A_NORMAL;
    fileinfo->size = 0;
    return 0;
}

inline int _findclose(intptr_t handle)
{
    _find_handle* h = (_find_handle*)handle;
    closedir(h->dir);
    delete h;
    return 0;
}
