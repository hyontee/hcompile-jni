//
// Created by Weikton
//
// Связка с libGTASA.so (1.x)
// Теперь можно загружать gta2.dat и другие 

#include "CFileMgr.h"
#include "../../util/patch.h"
#include "../scripting.h"

/**   Пример использования:
*     CFileMgr::SetDir("DATA"); // папка data
*     int file = CFileMgr::OpenFile("WEIKTON.DAT", "rb"); // открытие dat файла
*     CFileMgr::SetDir(""); // возвращаем в исходное
*     CFileMgr::CloseFile(file); // когда взяли из файла что нужно, закрываем его (чтобы не висел в памяти)
*/

/**
*     CFileMgr::SetDir("");
*     CFileMgr::OpenFile("gta_sa.set", "rb");
*     CFileMgr::SetDir("");
*     CFileMgr::CloseFile(file);
*/

void CFileMgr::Initialise()
{
   // unused
}

// CFileMgr::SetDir(char const*)	0x395FD0	
void CFileMgr::SetDir(char const* directory)
{
    ((void (*) (char const*))(g_libGTASA + 0x00395FD0 + 1))(directory);
}

// CFileMgr::LoadFile(char const*,uchar *,int,char const*)	0x39608C	
int CFileMgr::LoadFile(char const* filename, unsigned char* buffer, int bufferSize, char const* mode)
{
    Log(filename);
    return CHook::CallFunction<int>(g_libGTASA + 0x0039608C + 1, filename, buffer, bufferSize, mode);
}

// CFileMgr::OpenFile(char const*,char const*)	0x3960DC	
int CFileMgr::OpenFile(char const* name, char const* mode)
{
    Log(name);
    return CHook::CallFunction<int>(g_libGTASA + 0x003960DC + 1, name, mode);
}

// CFileMgr::CloseFile(int)	0x3962A4	
bool CFileMgr::CloseFile(int file)
{
    ((void (*) (int))(g_libGTASA + 0x003962A4 + 1))(file);
}