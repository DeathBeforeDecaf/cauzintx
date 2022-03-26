#ifndef PLATFORM_H
#define PLATFORM_H

#include <stddef.h>  // size_t

#ifdef _DOS

#define LNFEED "\n"

#define PATH_SEPARATOR_STR "\\"
#define PATH_SEPARATOR_CHAR '\\'

#define snprintf _snprintf

float strtof( char* inputStr, char** mark );

#else

#include <sys/syslimits.h>

#define _MAX_PATH ( NAME_MAX + PATH_MAX )

#define LNFEED "\r\n"

#define PATH_SEPARATOR_STR "/"
#define PATH_SEPARATOR_CHAR '/'

void errorExit( const char* message, unsigned line, const char* filename );

int ttySetRaw( int fildes );

int stricmp( const char* lhs, const char* rhs );

int strnicmp( const char* lhs, const char* rhs, size_t length );

int kbhit();

#endif

void platformInitialize( void );

void platformRelinquish( void );

#endif
