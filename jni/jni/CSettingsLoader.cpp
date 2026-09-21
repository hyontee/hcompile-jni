//add full work by @vlad_codin
//add full work by @vlad_codin
//add full work by @vlad_codin
#include "CSettingsLoader.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {

char* TrimLeft(char* s) {
    while (s && *s && isspace((unsigned char)*s)) {
        ++s;
    }
    return s;
}

void TrimRight(char* s) {
    if (!s) {
        return;
    }
    size_t len = strlen(s);
    while (len > 0) {
        unsigned char ch = (unsigned char)s[len - 1U];
        if (!isspace(ch)) {
            break;
        }
        s[--len] = '\0';
    }
}

char* Trim(char* s) {
    s = TrimLeft(s);
    TrimRight(s);
    return s;
}

void StripComment(char* s) {
    if (!s) {
        return;
    }
    for (char* p = s; *p; ++p) {
        if (*p == ';' || *p == '#') {
            *p = '\0';
            break;
        }
    }
    TrimRight(s);
}

}  // namespace

CSettingsLoader::CSettingsLoader()
    : water(0), carrefl(0), d(0), sky(0), aa(0), lowerdd(0), lowercars(0) {
    const char filePath[] = "/storage/emulated/0/Android/data/blackrussia.online/files/SAMP/settings.ini";

    FILE* fp = fopen(filePath, "r");
    if (!fp) {
        return;
    }

    bool inGraphics = false;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        char* s = Trim(line);
        if (!s || *s == '\0' || *s == ';' || *s == '#') {
            continue;
        }

        if (*s == '[') {
            inGraphics = (strcmp(s, "[graphics]") == 0);
            continue;
        }

        if (!inGraphics) {
            continue;
        }

        char* eq = strchr(s, '=');
        if (!eq) {
            continue;
        }

        *eq = '\0';
        char* key = Trim(s);
        char* value = Trim(eq + 1);
        StripComment(value);

        const int parsed = atoi(value);
        if (strcmp(key, "water") == 0 || strcmp(key, "w") == 0) {
            water = parsed;
        } else if (strcmp(key, "carrefl") == 0 || strcmp(key, "reflections") == 0) {
            carrefl = parsed;
        } else if (strcmp(key, "d") == 0 || strcmp(key, "distance") == 0 || strcmp(key, "draw_distance") == 0) {
            d = parsed;
        } else if (strcmp(key, "sky") == 0) {
            sky = parsed;
        } else if (strcmp(key, "s") == 0) {
            sky = parsed;
        } else if (strcmp(key, "aa") == 0) {
            aa = parsed;
        } else if (strcmp(key, "a") == 0) {
            aa = parsed;
        } else if (strcmp(key, "lowerdd") == 0) {
            lowerdd = parsed;
        } else if (strcmp(key, "l") == 0) {
            lowerdd = parsed;
        } else if (strcmp(key, "lowercars") == 0) {
            lowercars = parsed;
        } else if (strcmp(key, "z") == 0) {
            lowercars = parsed;
        }
    }

    fclose(fp);
}

//add full work by @vlad_codin
//add full work by @vlad_codin
//add full work by @vlad_codin

CSettingsLoader::~CSettingsLoader() {}
