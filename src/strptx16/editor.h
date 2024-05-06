#ifndef STRPTX16_H
#define STRPTX16_H

#include <ctype.h> // isspace()
#include <math.h> // floor()

#include "command.h"
#include "layout.h" // struct PublishedPageType, struct PrintedPageType, pageName, pageCount
#include "platform.h"
#include "settings.h"  // struct SystemSettings
#include "stdtypes.h"
#include "txreduce.h"  // struct DataDensityEntry, densityCount

void displaySequenceInformation();
void displayStripSequenceContent();

void displayMediaSettings( char mediaType );

bool displayDirectModeSyncValues( bool isCmdLine, char* inputStr );
bool displayByteSyncTable( bool isCmdLine, char* inputStr );
bool displaySequenceMetrics( bool isCmdLine, char* inputStr );

void displayEditorCommands();

bool displayEditorHelp( bool isCmdLine, char* inputStr );
bool appendFilesToSequence( bool isCmdLine, char* inputStr );
bool insertFilesIntoSequence( bool isCmdLine, char* inputStr );
bool removeFilesFromSequence( bool isCmdLine, char* inputStr );
bool sortFilesByPathFilename( bool isCmdLine, char* inputStr );
bool listStripSequenceContent( bool isCmdLine, char* inputStr );
bool writeStripSequenceContent( bool isCmdLine, char* inputStr );
bool quitProgram( bool isCmdLine, char* inputStr );

enum CMDLineResultType stripSequenceEditor();

#endif
