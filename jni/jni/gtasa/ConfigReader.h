#ifndef LIT_MOBILE_CONFIGREADER_H
#define LIT_MOBILE_CONFIGREADER_H


class СonfigReader {
public:
    int ReadIni(const char* filePath, const char* table, const char* valueName);
};


#endif //LIT_MOBILE_CONFIGREADER_H
