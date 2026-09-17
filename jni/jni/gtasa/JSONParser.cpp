#include "JSONParser.h"
#include "../main.h"
#include <cstdio>
#include <cstring>
#include <sstream>
#include <map>
#include <vector>

JSONParser::JSONParser(const std::string& path) : filePath(path) {}

bool JSONParser::loadFile() {
    FILE* file = fopen(filePath.c_str(), "r");
    if (file == nullptr) {
        Log("failed to open file: %s", filePath.c_str());
        return false;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    fileContent.resize(fileSize);
    fread(&fileContent[0], 1, fileSize, file);
    fclose(file);

    return true;
}

std::string JSONParser::getValue(const std::string& key) {
    std::string searchKey = key;
    size_t pos = 0;
    std::stringstream ss(searchKey);
    std::string subKey;
    std::string result;

    while (std::getline(ss, subKey, '.')) {
        pos = fileContent.find("\"" + subKey + "\"", pos);
        if (pos != std::string::npos) {
            pos = fileContent.find(":", pos);
            if (pos != std::string::npos) {
                pos++;
                while (fileContent[pos] == ' ' || fileContent[pos] == '\"') pos++;

                size_t endPos = fileContent.find("\"", pos);
                if (endPos != std::string::npos) {
                    result = fileContent.substr(pos, endPos - pos);
                }
            }
        } else {
            Log("error: Key '%s' not found", key.c_str());
            return "";
        }
    }

    return result;
}

void JSONParser::addString(const std::string& key, const std::string& value) {
    std::string newEntry = "\"" + key + "\": \"" + value + "\"";
    jsonEntries.push_back(newEntry);
}

void JSONParser::addInt(const std::string& key, int value) {
    std::string newEntry = "\"" + key + "\": " + std::to_string(value);
    jsonEntries.push_back(newEntry);
}

void JSONParser::addFloat(const std::string& key, float value) {
    std::string newEntry = "\"" + key + "\": " + std::to_string(value);
    jsonEntries.push_back(newEntry);
}

void JSONParser::saveToFile() {
    FILE* file = fopen(filePath.c_str(), "w");
    if (file == nullptr) {
        Log("failed to open file for saving: %s", filePath.c_str());
        return;
    }

    fputs("{\n", file);

    for (size_t i = 0; i < jsonEntries.size(); ++i) {
        fputs("    ", file);
        fputs(jsonEntries[i].c_str(), file);
        if (i < jsonEntries.size() - 1) {
            fputs(",\n", file);
        }
    }

    fputs("\n}\n", file);

    fclose(file);
}

void JSONParser::createObject(const std::string& objectName) {
    std::string newEntry = "\"" + objectName + "\": { }";
    jsonEntries.push_back(newEntry);
}

int JSONParser::getValueAsInt(const std::string& key, int defaultValue) {
    std::string value = getValue(key);
    try {
        return std::stoi(value);
    } catch (...) {
        return defaultValue;
    }
}

float JSONParser::getValueAsFloat(const std::string& key, float defaultValue) {
    std::string value = getValue(key);
    try {
        return std::stof(value);
    } catch (...) {
        return defaultValue;
    }
}

std::string JSONParser::getValue(const std::string& key, const std::string& defaultValue) {
    std::string value = getValue(key);
    if (value.empty()) return defaultValue;
    return value;
}