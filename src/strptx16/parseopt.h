#ifndef PARSEOPT_H
#define PARSEOPT_H

#include "stdtypes.h"
#include "command.h"
#include "layout.h"
#include "filelist.h"
#include "support.h"
#include "txstrip.h" // encoding[]
#include "txreduce.h" // struct DataDensityEntry

void printUsage();

bool setStripID( bool isCmdLine, char* inputStr );
bool setOutputFilename( bool isCmdLine, char* inputStr );
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
bool updatePublishedMediaParameters( bool isCmdLine, char* inputStr );
bool updatePrintMediaParameters( bool isCmdLine, char* inputStr );
bool updateInkSpreadIndex( bool isCmdLine, char* inputStr );
bool updateReductionFactor( bool isCmdLine, char* inputStr );


bool applyOption( char* optionStr );

char** sortCommandLineArguments( uint16_t argC, char* argV[] );

enum CMDLineResultType processCommandLineArguments( uint16_t argC, char* argV[], struct StripSequenceType* sequenceOut );

#endif
