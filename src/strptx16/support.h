#ifndef SUPPORT_H
#define SUPPORT_H

#ifdef _DOS
#include <conio.h>  // kbhit()
#include <direct.h> // getcwd()
#include <dos.h>    // dos_findfirst(), dos_findnext()
#else
#include <unistd.h> // getcwd()
#endif
#include <stdio.h>  // fputc(), fputs()
#include <stdlib.h> // _MAX_PATH
#include <string.h> // strcmp()

#include "stdtypes.h"
#include "filelist.h"
#include "parseopt.h"
#include "settings.h"
#include "txdirect.h"

struct StripSequenceType;

void generateIdentity( char* filename );
void printIdentity();

void printVersion( char* filename );

char prompt( char* promptStr, char option[], unsigned char optionCount, char* optionDefault );

char* trim( char* input, const char cutChar[], unsigned char cutCharSize );
char* ltrim( char* input, const char cutChar[], unsigned char cutCharSize );
void rtrim( char* input, const char cutChar[], unsigned char cutCharSize );

bool appendInputFiles( char* inputFilename, uint16_t* insertOffset, bool searchSubdirectories );

bool appendFilesInDirectory( char* path, char* filenameWildcard, uint16_t* insertOffset, bool searchSubdirectories );

#ifndef _DOS
int stricmp( const char* lhs, const char* rhs );
int strnicmp( const char* lhs, const char* rhs, size_t length );
int kbhit();
#endif

bool CZFListDataType_pathLessThan( CZFListDataType lhs, CZFListDataType rhs );

bool strtobool( char* input, char** mark );

#endif
