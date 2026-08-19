#ifndef SAALOADER_H
#define SAALOADER_H

#include <vector>

struct SAAHeader
{
    char magic[4];
    unsigned short version;
    unsigned int fileSize;
    unsigned int fileCount;
    unsigned char padding[8];
};

typedef unsigned char byte;

class SAALoader {
public:
    SAALoader();
    ~SAALoader();
    const char */*void*/ getFileFromSAA(const char *filename, const char *saaFilename);
};

#endif // SAALOADER_H