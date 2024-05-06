#ifndef PLATFORM_H
#define PLATFORM_H

// #include <stddef.h>  // size_t

#if defined(_DOS)||defined(__MSDOS__)
#include "pformdos.h"
#elif _MSC_VER
#include "pformwin.h"
#elif defined(__APPLE__) && defined(__MACH__)
#include <sys/syslimits.h>
#define _MAX_PATH ( NAME_MAX + PATH_MAX )
#include "pformmac.h"
#endif

// void platformInitialize( void );

// void platformRelinquish( void );

#endif
