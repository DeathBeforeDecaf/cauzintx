#include "editor.h"
#include "parseopt.h"
#include "stdtypes.h"
// #include "platform.h"

#ifndef COMMAND_H
#define COMMAND_H

enum CMDLineResultType
{
   cmdln_unknown = 0,
   cmdln_halt,
   cmdln_render,
   cmdln_completed,
};

struct CommandType
{
   uint16_t                  precedence;  // evaluation order of operation
   char*                     name;        // interactive verb
   char*                     alias;       // more verbose command line alias for same command
   enum CMDLineResultType    result;
   bool                   ( *cmdHandler )( bool isCmdLine, char* inputStr );
};

/*
   {  // j5 baz info - set adjunct information for #5 file
      50000,
      "j#",
      "adj#",
      cmdln_completed,
      setStripAdjunct
   },

*/

extern struct CommandType command[];
extern const uint16_t commandCount;

#endif
