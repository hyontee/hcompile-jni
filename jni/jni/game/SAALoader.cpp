#include "SAALoader.h"
#include "../main.h"
#include <fstream>

// OBFUSCATE
#include "obfuscate.h"

SAALoader::SAALoader() 
{
              /* code */
}

SAALoader::~SAALoader() 
{
              /* code */
}

/*void*/const char * SAALoader::getFileFromSAA(const char *fileName, const char *archiveName)
{
    std::ifstream archive(OBFUSCATE("/storage/emulated/0/Edgar/.archive.saa"), std::ios::binary);
    if (!archive)
    {
        Log(OBFUSCATE("San Andread Archive File | Error: Failed to open archive"));
        std::terminate();
    //    return;
    }
    SAAHeader header;
    archive.read(reinterpret_cast<char*>(&header), sizeof(SAAHeader));
    if (strncmp(header.magic, "SAA", 3) != 0)
    {
        Log(OBFUSCATE("San Andread Archive File | Warning: Invalid (Modifed) archive format"));
       // std::terminate();
       // return;
    }
    for (int i = 0; i < header.fileCount; i++)
    {
        int fileSize;
        archive.read(reinterpret_cast<char*>(&fileSize), sizeof(int));

        char buffer[1024];
        int bytesWritten = 0;
        std::fstream outFile(fileName, std::ios::binary | std::ios::out);
        while (bytesWritten < fileSize)
        {
            int readSize = std::min(static_cast<int>(sizeof(buffer)), fileSize - bytesWritten);
            archive.read(buffer, readSize);
            outFile.write(buffer, readSize);
            bytesWritten += readSize;
        }
        outFile.close();
    }
    archive.close();
}

/*void SAALoader::getFileFromSAA(const char *filename, const char *saaFilename) 
{
    //std::ifstream saaFile(saaFilename, std::ios::binary);
      std::ifstream saaFile(OBFUSCATE("/storage/emulated/0/FussRussia/.archive.saa"), std::ios::binary); 

    // Проверка на правильное открытиe SAA
    if (!saaFile.is_open()) 
    {
        return;
    }

    // SAA header
    byte header[14];
    saaFile.read((char *)header, 14);

    int numFiles = header[5] * 256 + header[4];

    // Поиск в SAA archive
    bool fileFound = false;
    for (int i = 0; i < numFiles; i++) 
    {
        byte fileNameLength = 0;
        saaFile.read((char *)&fileNameLength, 1);

        char *currentFileName = new char[fileNameLength + 1];
        saaFile.read(currentFileName, fileNameLength);
        currentFileName[fileNameLength] = '\0';

        int fileSize = header[7] * 256 + header[6];
        for (int j = 0; j < i; j++)
        {
            int fileOffset = (header[1] * 256 + header[0]) * 1024 + j * fileSize;
            saaFile.seekg(fileOffset, std::ios::beg);
            byte curFileNameLength;
            saaFile.read((char *)&curFileNameLength, 1);
            char *curFileName = new char[curFileNameLength + 1];
            saaFile.read(curFileName, curFileNameLength);
            curFileName[curFileNameLength] = '\0';
            delete[] curFileName;
        }
        int fileOffset = (header[1] * 256 + header[0]) * 1024 + i * fileSize;
        saaFile.seekg(fileOffset, std::ios::beg);

        if (strcmp(currentFileName, filename) == 0) 
        {
            fileFound = true;

            // Чтение файла
            const char * fileData(fileSize);
            saaFile.read((char *)&fileData[0], fileSize);
            delete[] currentFileName;
            saaFile.close();
            return const char * fileData;
        }

        delete[] currentFileName;
    }
    saaFile.close();

    // Если файл не найден
    Log(OBFUSCATE("San Andread Archive File not found."));
    return std::vector<byte>();
}*/