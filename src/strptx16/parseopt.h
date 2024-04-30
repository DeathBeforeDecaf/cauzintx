#ifndef PARSEOPT_H
#define PARSEOPT_H

#include <stddef.h>  // NULL
#include <stdlib.h>  // atol()
#include <string.h>  // strlen()

#include "command.h"
#include "editor.h"
#include "globals.h"
#include "layout.h"  // pageName, pageCount
#include "filelist.h"
#include "platform.h"
#include "settings.h" // struct SystemSettings
#include "support.h"
#include "txstrip.h" // encoding[]
#include "txreduce.h" // struct DataDensityEntry
#include "stdtypes.h"

void printUsage();

bool setStripID( bool isCmdLine, char* inputStr );
bool setOutputFilename( bool isCmdLine, const char* inputStr );
bool setSequenceText( bool isCmdLine, char* inputStr );
bool setStripAdjunct( bool isCmdLine, char* inputStr );
bool recursivelySearchSubdirectories( bool isCmdLine, char* inputStr );
bool setEncodingSequenceByteLimit( bool isCmdLine, char* inputStr );
bool setOpticalReaderApertureSize( bool isCmdLine, char* inputStr );
bool setInteractiveSession( bool isCmdLine, char* inputStr );
bool displayProgramVersion( bool isCmdLine, char* inputStr );
bool displayCommandLineHelp( bool isCmdLine, char* inputStr );
bool setHSyncValue( bool isCmdLine, char* inputStr );
bool setVSyncValue( bool isCmdLine, char* inputStr );
bool setSyncValue( bool isCmdLine, char* inputStr );
bool updateCRCGeneration( bool isCmdLine, char* inputStr );
bool updateRenderMode( bool isCmdLine, char* inputStr );
bool updateWatermarkGeneration( bool isCmdLine, char* inputStr );
bool updatePublishedMediaParameters( bool isCmdLine, char* inputStr );
bool updatePrintMediaParameters( bool isCmdLine, char* inputStr );
bool updateInkSpreadIndex( bool isCmdLine, char* inputStr );
bool updateReductionFactor( bool isCmdLine, char* inputStr );

char** sortCommandLineArguments( uint16_t argC, char* argV[] );

enum CMDLineResultType processCommandLineArguments( uint16_t argC, char* argV[], struct StripSequenceType* sequenceOut );

#endif
