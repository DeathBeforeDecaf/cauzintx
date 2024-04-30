#ifndef PFORMDOS_H
#define PFORMDOS_H

#include <conio.h>  // kbhit()
#include <direct.h> // getcwd(), _mkdir()
#include <dos.h>    // dos_findfirst(), dos_findnext()
#include <errno.h>
#include <malloc.h>  // size_t, malloc()
#include <stdio.h> // FILENAME_MAX
#include <stdlib.h>  // atof()
#include <sys/types.h> // struct stat
#include <sys/stat.h>  // stat()
#include <stddef.h> // NULL

#if defined(__BORLANDC__)
#include <stdarg.h>
#endif

#include "filelist.h" // InputFile_initialize(), CZFList_insertAtTail(), CZFList_insertBefore()
#include "stdtypes.h"

#define LNFEED "\n"

#define PATH_SEPARATOR_STR "\\"
#define PATH_SEPARATOR_CHAR '\\'

#ifndef __BORLANDC__
#define stat _stat
#define S_IFREG _S_IFREG
#define S_IFDIR _S_IFDIR
#define S_IEXEC _S_IEXEC
#define snprintf _snprintf
#else
#define _find_t find_t
#define dos_findfirst _dos_findfirst
#define dos_findnext _dos_findnext
int snprintf( char* buffer, size_t bufferSize, const char* format, ... );
#endif



float strtof( char* inputStr, char** mark );

int makeDirectory( const char* pathname );

bool appendFilesInDirectory(char* path, char* filenameWildcard, uint16_t* insertOffset, bool searchSubdirectories);

bool appendInputFiles(char* inputFilename, uint16_t* insertOffset, bool searchSubdirectories);

uint32_t getVersionID( const char* filename );

char prompt( const char* promptStr, const char option[], unsigned char optionCount, const char* optionDefault );

void platformInitialize();

extern void platformRelinquish();

#endif
