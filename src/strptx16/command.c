#include <stdlib.h>  // NULL

#include "command.h"

struct CommandType command[] =
{
   {  // a file.ext - append strip sequence with file.ext
      0,
      "a",
      NULL,
      cmdln_completed,
      appendFilesToSequence
   },
   {  // i2 file.ext - insert file.ext into 2nd position in strip sequence
      0,
      "i#",
      NULL,
      cmdln_completed,
      insertFilesIntoSequence
   },
   {  // d3,4 - delete single entry or range from the strip sequence
      0,
      "d#",
      NULL,
      cmdln_completed,
      removeFilesFromSequence
   },
   {  // / - sort the strip sequence by path/file.ext
      0,
      "/",
      NULL,
      cmdln_completed,
      sortFilesByPathFilename
   },

   {  // l - list the strip sequence content
      0,
      "l",
      NULL,
      cmdln_completed,
      listStripSequenceContent
   },

   {  // $ STRPID - set six character strip ID to 'STRPID' for sequence identification
      2,
      "$",
      "id",
      cmdln_completed,
      setStripID
   },

   {  // o output.ps - set the output filename
      3,
      "o",
      "out",
      cmdln_completed,
      setOutputFilename
   },

   {  // t blah blah .. - set a verbose text sequence description
      4,
      "t",
      "text",
      cmdln_completed,
      setSequenceText
   },

   {  // j5 baz info - set adjunct information for #5 file
      50000U,
      "j#",
      "adj#",
      cmdln_completed,
      setStripAdjunct
   },

   {  // r true - set recursive directory search wildcard expansion
      6,
      "r",
      "recur",
      cmdln_completed,
      recursivelySearchSubdirectories
   },

   {  // w - write the strip sequence into postscript output file
      0,
      "w",
      NULL,
      cmdln_completed,
      writeStripSequenceContent
   },

   {  // x 650240 - set maximum byte limit for an encoding sequence
      7,
      "x",
      "maxb",
      cmdln_completed,
      setEncodingSequenceByteLimit
   },

   {  // z 8.75 - set aperture size limit of optical reader in inches
      8,
      "z",
      "asiz",
      cmdln_completed,
      setOpticalReaderApertureSize
   },

   {  // y - non-interactive generation of strip sequence
      0,
      "y",
      NULL,
      cmdln_render,
      setInteractiveSession
   },

   {  // q - quit program and exit back to command line
      0,
      "q",
      NULL,
      cmdln_halt,
      quitProgram
   },

   {  // @ - display the name and version of the program
      1,
      "@",
      "version",
      cmdln_completed,
      displayProgramVersion
   },

   {  // ? - display some helpful hints
      36,
      "?",
      "help",
      cmdln_halt,
      displayCommandLineHelp
   },

   {  // ? - display some helpful hints
      36,
      "?",
      "help",
      cmdln_completed,
      displayEditorHelp
   },

   {  // h/h1 0x08 - set hSync (nibbles/scan line) of #1 rendered strip
      10,
      "h#",
      "hsync#",
      cmdln_completed,
      setHSyncValue
   },

   {  // v/v2 0x60 - set vSync (uSteps/scan line) of #2 rendered strip
      12,
      "v#",
      "vsync#",
      cmdln_completed,
      setVSyncValue
   },

   {  // s/s3 0x08,0x60 - set the hSync and vSync of #3 rendered strip
      14,
      "s#",
      "sync#",
      cmdln_completed,
      setSyncValue
   },

   {  // c/c4 true - set generate CRC on #4 rendered strip
      16,
      "c#",
      "crc#",
      cmdln_completed,
      updateCRCGeneration
   },

   {  // f direct - set render format to { direct, reduce }
      17,
      "f",
      "format",
      cmdln_completed,
      updateRenderMode
   },

   {  // % - display nearest direct mode device-dependent hSync/vSync values
      0,
      "%",
      NULL,
      cmdln_completed,
      displayDirectModeSyncValues
   },

   {  // # - show bytes per strip with given hSync x vSync value table
      0,
      "#",
      NULL,
      cmdln_completed,
      displayByteSyncTable
   },

   {  // ! - display strip sequence metrics
      0,
      "!",
      NULL,
      cmdln_completed,
      displaySequenceMetrics
   },

   {  // m - set/display published media settings
      18,
      "m",
      "pub",
      cmdln_completed,
      updatePublishedMediaParameters
   },

   {  // m.height 11.0 - set publish media height
      19,
      "m.height",
      "pub.height",
      cmdln_completed,
      updatePublishedMediaParameters
   },
   {  // m.width 8.5 - set publish media width
      20,
      "m.width",
      "pub.width",
      cmdln_completed,
      updatePublishedMediaParameters
   },
   {  // m.tmar 0.5 - set publish media top margin
      21,
      "m.tmar",
      "pub.tmar",
      cmdln_completed,
      updatePublishedMediaParameters
   },
   {  // m.bmar 0.4 - set publish media bottom margin
      22,
      "m.bmar",
      "pub.bmar",
      cmdln_completed,
      updatePublishedMediaParameters
   },

   {  // m.lmar 0.7 - set publish media left margin
      23,
      "m.lmar",
      "pub.lmar",
      cmdln_completed,
      updatePublishedMediaParameters
   },

   {  // m.rmar 0.5 - set publish media right margin
      24,
      "m.rmar",
      "pub.rmar",
      cmdln_completed,
      updatePublishedMediaParameters
   },

   {  // p letter - set/display print media settings
      25,
      "p",
      "prt",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.height 11.0 - set print media height
      26,
      "p.height",
      "prt.height",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.width 8.5 - set print media width
      27,
      "p.width",
      "prt.width",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.tmar 0.5 - set print media top margin
      28,
      "p.tmar",
      "prt.tmar",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.bmar 0.4 - set print media bottom margin
      29,
      "p.bmar",
      "prt.bmar",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.lmar 0.7 - set print media left margin
      30,
      "p.lmar",
      "prt.lmar",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.rmar 0.5 - set print media right margin
      31,
      "p.rmar",
      "prt.rmar",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.feed sheet - set printer feed format to { sheet, continuous }
      32,
      "p.feed",
      "prt.feed",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // p.dots 300,300 - set printer horizontal and vertical resolution (dots/area)
      33,
      "p.dots",
      "prt.dots",
      cmdln_completed,
      updatePrintMediaParameters
   },

   {  // k 1.000 - set the ink spread index, measured in thousands of an inch
      34,
      "k",
      "ink",
      cmdln_completed,
      updateInkSpreadIndex
   },

   {  // n 1.0 - inverse scaling factor (photo reduction factor 6.0 to 12.0)
      35,
      "n",
      "scale",
      cmdln_completed,
      updateReductionFactor
   },
};

const uint16_t commandCount = sizeof( command ) / sizeof( command[ 0 ] );
