#pragma once

// Minimal Android/POSIX compatibility layer for the legacy RakNet
// _findfirst/_findclose API used by FileOperations.cpp.
//
// The original RakNet code only relies on these functions for checking
// whether a directory can be opened.  Android does not ship the Windows
// CRT _findfirst API, so map the small subset used by this project to
// POSIX dirent/stat calls.

#ifndef _WIN32

#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

struct _finddata_t
{
    unsigned attrib;
    time_t time_create;
    time_t time_access;
    time_t time_write;
    uint64_t size;
    char name[260];
};

namespace raknet_findfirst_detail
{
struct Handle
{
    DIR* dir;
};

inline const char* base_name(const char* pattern)
{
    const char* slash = strrchr(pattern, '/');
    return slash ? slash + 1 : pattern;
}

inline void fill_entry(_finddata_t* out, const char* full_path, const char* name)
{
    memset(out, 0, sizeof(*out));
    snprintf(out->name, sizeof(out->name), "%s", name);

    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        out->size = static_cast<uint64_t>(st.st_size);
        out->time_create = st.st_ctime;
        out->time_access = st.st_atime;
        out->time_write = st.st_mtime;
        if (S_ISDIR(st.st_mode))
            out->attrib |= 0x10; // _A_SUBDIR
    }
}
} // namespace raknet_findfirst_detail

inline intptr_t _findfirst(const char* pattern, struct _finddata_t* out)
{
    if (pattern == nullptr || out == nullptr)
        return -1;

    char dir_path[1024];
    const char* slash = strrchr(pattern, '/');
    if (slash)
    {
        size_t len = static_cast<size_t>(slash - pattern);
        if (len == 0)
            len = 1; // root '/'
        if (len >= sizeof(dir_path))
            return -1;
        memcpy(dir_path, pattern, len);
        dir_path[len] = '\0';
    }
    else
    {
        snprintf(dir_path, sizeof(dir_path), ".");
    }

    DIR* dir = opendir(dir_path);
    if (dir == nullptr)
        return -1;

    auto* handle = new raknet_findfirst_detail::Handle{dir};
    struct dirent* ent = nullptr;
    while ((ent = readdir(dir)) != nullptr)
    {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char full_path[1200];
        if (slash)
            snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, ent->d_name);
        else
            snprintf(full_path, sizeof(full_path), "%s", ent->d_name);

        raknet_findfirst_detail::fill_entry(out, full_path, ent->d_name);
        return reinterpret_cast<intptr_t>(handle);
    }

    closedir(dir);
    delete handle;
    errno = ENOENT;
    return -1;
}

inline int _findclose(intptr_t handle)
{
    if (handle == -1)
        return -1;

    auto* h = reinterpret_cast<raknet_findfirst_detail::Handle*>(handle);
    if (h == nullptr)
        return -1;

    int result = closedir(h->dir);
    delete h;
    return result;
}

#endif // !_WIN32
