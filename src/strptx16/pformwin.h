#if defined(_MSC_VER)

#ifndef PFORMWIN_H
#define PFORMWIN_H

#include <conio.h> // _kbhit(), _getch()
#include <direct.h> // _getcwd()
#include <io.h> // _find_first, _find_next
#include <stdlib.h> // _MAX_PATH
#include <string.h> // _strdup(), _stricmp(), _strnicmp()
#include <sys/types.h> // struct _stat
#include <sys/stat.h>  // stat()

#include "filelist.h" // CZFList_insertAtTail(), CZFList_insertBefore()
#include "txstrip.h"  // InputFile_initialize()
#include "stdtypes.h"

#define LNFEED "\n"

#define PATH_SEPARATOR_STR  "\\"
#define PATH_SEPARATOR_CHAR '\\'


#define strdup    _strdup
#define stricmp   _stricmp
#define strnicmp  _strnicmp

#define kbhit   _kbhit
#define getch   _getch
#define getcwd  _getcwd
#define mkdir   _mkdir

int makeDirectory( const char* pathname );

bool appendFilesInDirectory( char* path, char* filenameWildcard, uint16_t* insertOffset, bool searchSubdirectories );

bool appendInputFiles( char* inputFilename, uint16_t* insertOffset, bool searchSubdirectories );

uint32_t getVersionID( const char* filename );

char prompt( const char* promptStr, const char option[], unsigned char optionCount, const char* optionDefault );

void platformInitialize();

extern void platformRelinquish();

#endif

#endif
