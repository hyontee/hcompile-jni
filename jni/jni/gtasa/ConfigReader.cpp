#include "../main.h"
#include "ConfigReader.h"

int СonfigReader::ReadIni(const char* filePath, const char* table, const char* valueName) {
    FILE* file = fopen(filePath, "r");
    if (file == nullptr) {
        Log("Не удалось открыть файл: %s", filePath);
        return-1;
    }

    char line[256];
    bool inTableSection = false;
    int value = -1;

    while (fgets(line, sizeof(line), file)) {
        char* trimmedLine = line;
        while (isspace(*trimmedLine)) {
            trimmedLine++;
        }
        char* end = trimmedLine + strlen(trimmedLine) - 1;
        while (end > trimmedLine && isspace(*end)) {
            end--;
        }
        *(end + 1) = '\0';

        if (*trimmedLine == '\0' || *trimmedLine == ';') {
            continue;
        }

        if (*trimmedLine == '[') {
            char* sectionEnd = strchr(trimmedLine, ']');
            if (sectionEnd != nullptr) {
                *sectionEnd = '\0';
                char section[256];
                strncpy(section, trimmedLine + 1, sizeof(section) - 1);
                section[sizeof(section) - 1] = '\0';

                if (strcmp(section, table) == 0) {
                    inTableSection = true;
                } else if(inTableSection) {
                    break;
                }
            }
        } else if(inTableSection) {
            char name[256];
            int num;
            if (sscanf(trimmedLine, "%[^=]=%d", name, &num) == 2) {
                char* nameEnd = name + strlen(name) - 1;
                while (nameEnd > name && isspace(*nameEnd)) {
                    nameEnd--;
                }
                *(nameEnd + 1) = '\0';

                if (strcmp(name, valueName) == 0) {
                    value = num;
                    break;
                }
            }
        }
    }
    fclose(file);
    return value;
}