//
// Created by @obfuscati0n
//

#include "CFileMgr.h"
#include "../util/patch.h"
#include "../game/scripting.h"

/**   Пример использования:
*     CFileMgr::SetDir("DATA"); // папка data
*     int file = CFileMgr::OpenFile("@obfuscati0n.DAT", "rb"); // открытие dat файла
*     CFileMgr::SetDir(""); // возвращаем в исходное
*     CFileMgr::CloseFile(file); // когда взяли из файла что нужно, закрываем его
*/

void CFileMgr::Initialise()
{

}

void CFileMgr::SetDir(char const* directory)
{
    ((void (*) (char const*))(g_libGTASA + 0x00395FD0 + 1))(directory);
}

int CFileMgr::LoadFile(char const* filename, unsigned char* buffer, int bufferSize, char const* mode)
{
    Log("Loading File: s%...", filename);
    return CHook::CallFunction<int>(g_libGTASA + 0x0039608C + 1, filename, buffer, bufferSize, mode);
}

int CFileMgr::OpenFile(char const* name, char const* mode)
{
    Log("Open File: s%...", name);
    return CHook::CallFunction<int>(g_libGTASA + 0x003960DC + 1, name, mode);
}

bool CFileMgr::CloseFile(int file)
{
    ((void (*) (int))(g_libGTASA + 0x003962A4 + 1))(file);
}