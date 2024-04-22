#ifndef SUPPORT_H
#define SUPPORT_H

#include <stdio.h>  // fputc(), fputs()
#include <string.h>  // stricmp()

// #include "filelist.h"
#include "globals.h"
#include "platform.h"
#include "stdtypes.h"
#include "txdirect.h"

void generateIdentity( const char* filename );
void printIdentity();

// char prompt( const char* promptStr, const char option[], unsigned char optionCount, const char* optionDefault );

char* trim( char* input, const char cutChar[], unsigned char cutCharSize );
char* ltrim( char* input, const char cutChar[], unsigned char cutCharSize );
char* rtrim( char* input, const char cutChar[], unsigned char cutCharSize );

bool CZFListDataType_pathLessThan( struct InputFileType* lhs, struct InputFileType* rhs );

bool strtobool( const char* input, char** mark );

bool softBreak( const char* lineIn, uint16_t colLimit, uint16_t rowLimit, char** linesOut );

#endif
