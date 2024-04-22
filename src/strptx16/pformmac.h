#ifndef PFORMMAC_H
#define PFORMMAC_H

#include <dirent.h>  // DIR
#include <errno.h>
#include <regex.h>  // regcomp(), regexec(), regfree()
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // strerror()
#include <sys/ioctl.h>
#include <sys/stat.h>  // stat(), S_IFREG
#include <termios.h> // TCSAFLUSH
#include <unistd.h> // getcwd(), STDIN_FILENO

#include "filelist.h"
#include "stdtypes.h"
#include "txstrip.h"


#define LNFEED "\r\n"

#define PATH_SEPARATOR_STR "/"
#define PATH_SEPARATOR_CHAR '/'

extern struct termios* sysTermSet;

void errorExit( const char* message, unsigned line, const char* filename );

int ttySetRaw( int fildes );

int stricmp( const char* lhs, const char* rhs );

int strnicmp( const char* lhs, const char* rhs, size_t length );

int kbhit();

int makeDirectory( const char *pathname );

bool appendFilesInDirectory( char* path, char* wildcard, uint16_t* insertOffset, bool searchSubdirectories );

bool appendInputFiles( char* inputSource, uint16_t* insertOffset, bool searchSubdirectories );

uint32_t getVersionID( const char* filename );

char prompt( const char* promptStr, const char option[], unsigned char optionCount, const char* optionDefault );

void platformInitialize( void );

void platformRelinquish( void );

#endif
