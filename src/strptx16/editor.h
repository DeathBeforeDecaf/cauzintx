#ifndef STRPTX16_H
#define STRPTX16_H

#include <ctype.h> // isspace()
#include <math.h> // floor()

#include "command.h"
#include "stdtypes.h"

void displayMediaSettings( char mediaType );

void displayEditorCommands();

void displayStripSequenceContent();

enum CMDLineResultType stripSequenceEditor();

bool displayEditorHelp( bool isCmdLine, char* inputStr );
bool appendFilesToSequence( bool isCmdLine, char* inputStr );
bool insertFilesIntoSequence( bool isCmdLine, char* inputStr );
bool removeFilesFromSequence( bool isCmdLine, char* inputStr );
bool sortFilesByPathFilename( bool isCmdLine, char* inputStr );
bool listStripSequenceContent( bool isCmdLine, char* inputStr );
bool writeStripSequenceContent( bool isCmdLine, char* inputStr );
bool quitProgram( bool isCmdLine, char* inputStr );
bool displayDirectModeSyncValues( bool isCmdLine, char* inputStr );
bool displayByteSyncTable( bool isCmdLine, char* inputStr );
bool displaySequenceMetrics( bool isCmdLine, char* inputStr );



#endif
