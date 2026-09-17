#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <string>
#include <vector>

class JSONParser {
private:
    std::string filePath;
    std::string fileContent;
    std::vector<std::string> jsonEntries;

public:
    JSONParser(const std::string& path);

    bool loadFile();

    std::string getValue(const std::string& key);

    int getValueAsInt(const std::string& key, int defaultValue);
    float getValueAsFloat(const std::string& key, float defaultValue);
    std::string getValue(const std::string& key, const std::string& defaultValue);

    void addString(const std::string& key, const std::string& value);
    void addInt(const std::string& key, int value);
    void addFloat(const std::string& key, float value);

    void createObject(const std::string& objectName = "");

    void saveToFile();
};

#endif // JSONPARSER_H
