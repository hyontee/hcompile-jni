#pragma once

class CFileMgr
{
public:
                  static bool CloseFile(int file);
	static void Initialise();
	static int LoadFile(char const* filename, unsigned char* buffer, int bufferSize, char const* mode);
	static int OpenFile(char const* filename, char const* mode);
	static void SetDir(char const* directory);
private:

};