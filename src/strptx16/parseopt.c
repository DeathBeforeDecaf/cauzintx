#include "parseopt.h"

void printUsage()
{
   printf( LNFEED "  Options:" LNFEED );

   printf( "    /id=\"%-6s\"         : set the six character strip ID sequence identifier" LNFEED,
           settings.sequence.stripID );

   printf( "    /out=\"%s\"     : set the output filename for the rendered sequence" LNFEED, settings.sequence.outputFilename );

   printf( "    /text=\"blah blah ..\" : set verbose text description for the strip sequence" LNFEED );

   printf( "    /adj#=\"more info\"    : set adjunct information associated w/specific file" LNFEED );

   printf( "    /recur=%-5s         : set recursive directory search wildcard expansion" LNFEED,
           ( 0 == settings.searchRecursively ) ? "false" : "true" );

   printf( "    /maxb=%" FSTR_UINT32_T "         : set maximum byte limit for an encoding sequence" LNFEED, settings.sequence.limitMaxSequenceBytes );

   printf( "    /asiz=%1.2f           : set aperture size limit of optical reader in inches" LNFEED, settings.pageLayout.limitApertureSizeInches );

   printf( "    /y                   : non-interactive generation of strip sequence" LNFEED );

   printf( "    /version             : display the name and version of the program" LNFEED );

   printf( "    /help                : display some helpful hints" LNFEED LNFEED );


   printf( "    /hsync=0x%02X          : set default hSync (nibbles/scan line) of all strips" LNFEED, settings.sequence.encoding[ 0 ].hSync );

   printf( "    /hsync#=0x%02X         : set hSync (nibbles/scan line) of strip# in [1,%d]" LNFEED, settings.sequence.encoding[ 0 ].hSync, SEQUENCE_LIMIT );

   printf( "    /vsync=0x%02X          : set default vSync (uSteps/scan line) of all strips" LNFEED, settings.sequence.encoding[ 0 ].vSync );

   printf( "    /vsync#=0x%02X         : set vSync (uSteps/scan line) of strip# in [1,%d]" LNFEED, settings.sequence.encoding[ 0 ].vSync, SEQUENCE_LIMIT );

   printf( "    /sync=0x%02X,0x%02X      : set default Sync values of all rendered strips" LNFEED, settings.sequence.encoding[ 0 ].hSync, settings.sequence.encoding[ 0 ].vSync );

   printf( "    /sync#=0x%02X,0x%02X     : set hSync and vSync values of strip# in [1,%d]" LNFEED, settings.sequence.encoding[ 0 ].hSync, settings.sequence.encoding[ 0 ].vSync, SEQUENCE_LIMIT );

   printf( "    /crc=%-5s           : generate CRC on each strip by default" LNFEED,
           ( 0 == ( strip_crc & settings.sequence.encoding[ 0 ].override ) ) ? "false" : "true" );

   printf( "    /crc#=%-5s          : generate CRC on a specific strip, # in [1,%u]" LNFEED,
           ( 0 == ( strip_crc & settings.sequence.encoding[ 0 ].override ) ) ? "false" : "true",
           SEQUENCE_LIMIT );

   printf( "    /format=%-5s       : set output mode rendered format { direct, reduce }" LNFEED,
           ( ( direct_render == settings.outputMode ) ? "direct" : "reduce" ) );

   prompt( "[Press any key to continue] ", " ", 1, " " );

   printf( LNFEED "    /pub=%-7s         : set the published media { letter, legal, tabloid }" LNFEED,
           pageName[ settings.media.lastPublishSelection ] );

   printf( "    /pub.height=%1.2f    : set the published media height" LNFEED, settings.media.publish.height );
   printf( "    /pub.width=%1.2f      : set the published media width" LNFEED, settings.media.publish.width );
   printf( "    /pub.tmar=%1.2f       : set the published media top margin" LNFEED, settings.media.publish.margin.top );
   printf( "    /pub.bmar=%1.2f       : set the published media bottom margin" LNFEED, settings.media.publish.margin.bottom );

   printf( "    /pub.lmar=%1.2f       : set the published media left margin" LNFEED, settings.media.publish.margin.left );
   printf( "    /pub.rmar=%1.2f       : set the published media right margin" LNFEED LNFEED, settings.media.publish.margin.right );


   printf( "    /prt=%-7s         : set the printer media { letter, legal, tabloid }" LNFEED,
           pageName[ settings.media.lastPrintSelection ] );

   printf( "    /prt.height=%1.2f    : set the printer media height" LNFEED, settings.media.print.height );
   printf( "    /prt.width=%1.2f      : set the printer media width" LNFEED, settings.media.print.width );
   printf( "    /prt.tmar=%1.2f       : set the printer media top margin" LNFEED, settings.media.print.margin.top );
   printf( "    /prt.bmar=%1.2f       : set the printer media bottom margin" LNFEED, settings.media.print.margin.bottom );

   printf( "    /prt.lmar=%1.2f       : set the printer media left margin" LNFEED, settings.media.print.margin.left );
   printf( "    /prt.rmar=%1.2f       : set the printer media right margin" LNFEED, settings.media.print.margin.right );

   printf( "    /prt.feed=%s      : set printer feed format to { sheet, continuous }" LNFEED,
           ( sheet == settings.media.print.feed ) ? "sheet" : "continuous" );

   printf( "    /prt.dots=%" FSTR_UINT16_T ",%" FSTR_UINT16_T "    : set printer horizontal/vertical resolution(dots/area)" LNFEED,
           settings.media.print.dots.horizontal, settings.media.print.dots.vertical );

   if ( direct_render != settings.outputMode )
   {
      printf( "    /ink=%1.3f           : ink spread index, measured in thousands of an inch" LNFEED, settings.pageLayout.inkSpreadIndex );
      printf( "    /scale=%2.2f         : inverse scaling factor (photo reduction factor 6.0 to 12.0)" LNFEED, settings.pageLayout.reductionFactor );
   }

   fputs( LNFEED, stdout );
}


bool setStripID( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   if ( '$' == *index )
   {
      ++index;
   }
   else
   {
      index += 2;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 == length )
   {
      strcpy( settings.sequence.stripID, "STRPID" );

      if ( false == isCmdLine )
      {
         printf( "   reset stripID to default value 'STRPID'" LNFEED );
      }
   }
   else if ( length < 6 )
   {
      strncpy( settings.sequence.stripID, index, length );

      while ( length < 6 )
      {
         settings.sequence.stripID[ length ] = ' ';

         ++length;
      }

      if ( false == isCmdLine )
      {
         printf( "   updated stripID to value '%s'", settings.sequence.stripID );
      }
   }
   else
   {
      strncpy( settings.sequence.stripID, index, 6 );

      if ( false == isCmdLine )
      {
         printf( "   updated stripID to value '%s'", settings.sequence.stripID );
      }
   }

   return true;
}


bool setOutputFilename( bool isCmdLine, const char* inputStr )
{
   size_t length;

   char* index = ( char* )inputStr;

   if ( 0 == strnicmp( index, "out", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      strcpy( settings.sequence.outputFilename, index );

      if ( false == isCmdLine )
      {
         printf( "   updated output filename to value '%s'", settings.sequence.outputFilename );
      }
   }
   else
   {
      strcpy( settings.sequence.outputFilename, "output.ps" );

      if ( false == isCmdLine )
      {
         printf( "   reset output filename to default value '%s'", settings.sequence.outputFilename );
      }
   }

   return true;
}


bool setSequenceText( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* linesOut;

   char* index = inputStr;

   if ( 0 == strnicmp( index, "text", 4 ) )
   {
      index += 4;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( NULL != settings.sequence.verboseDescription )
   {
      free( settings.sequence.verboseDescription );
   }

   // force newline at 110 chars, truncate after 6 lines

   if ( ( 0 < length ) && softBreak( index, 110, 5, &linesOut ) )
   {
      settings.sequence.verboseDescription = linesOut;
   }
   else
   {
      settings.sequence.verboseDescription = NULL;
   }

   if ( 0 != length )
   {
      if ( false == isCmdLine )
      {
         printf( "   updated verbose text description for strip sequence" );
      }
   }
   else
   {
      if ( false == isCmdLine )
      {
         printf( "   removed verbose text description for strip sequence" );
      }
   }

   return true;
}


bool setStripAdjunct( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   char* nextSegment = NULL;

   bool result = false;

   uint16_t offset = 0;

   struct InputFileType* item;

   if ( 0 == strnicmp( index, "adj", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   offset = ( uint16_t )strtoul( index, &nextSegment, 10 );

   if ( nextSegment != index )
   {
      if (  ( 0 < offset ) && ( offset <= CZFList_itemCount() ) )
      {
         index = ltrim( nextSegment, hzWSpace, hzWSpaceCount );

         if ( isCmdLine && ( '=' == *index ) )
         {
            ++index;
         }

         index = ltrim( index, hzWSpace, hzWSpaceCount );

         length = strlen( index );

         if ( CZFList_get( ( uint16_t )( offset - 1 ), &item ) )
         {
            if ( 0 != length )
            {
               if ( length < 256 )
               {
                  if ( NULL != item->entry.adjunct )
                  {
                     free( item->entry.adjunct );
                  }

                  item->entry.adjunct = ( uint8_t* )strdup( index );
                  item->entry.adjunct_size = ( uint8_t )length;

                  result = true;

                  if ( false == isCmdLine )
                  {
                     printf( "   updated adjunct information for file #%" FSTR_UINT16_T , offset );
                  }
               }
               else  // length >= 256
               {
                  if ( false == isCmdLine )
                  {
                     fprintf( stderr, LNFEED "   ERROR: Maximum adjunct length exceeded for file #%" FSTR_UINT16_T " (255 Bytes max)", offset );
                  }
               }
            }
            else // 0 == length
            {
               if ( NULL != item->entry.adjunct )
               {
                  free( item->entry.adjunct );

                  item->entry.adjunct = NULL;

                  if ( false == isCmdLine )
                  {
                     printf( "   cleared adjunct information for file #%" FSTR_UINT16_T, offset );
                  }
               }

               result = true;
            }
         }
         else if ( false == isCmdLine ) // problem retrieving file# offset
         {
            printf( "   ERROR: Problem retrieving file #%" FSTR_UINT16_T ", update adjunct failed", offset );
         }
      }
      else if ( false == isCmdLine ) // offset out of range
      {
         printf( "   sorry, adjunct offset out of range for file #%" FSTR_UINT16_T , offset );
      }
   }
   else if ( false == isCmdLine ) // offset NaN
   {
      printf( "   sorry, adjunct offset# for file is not a number (%s)", index );
   }

   return result;
}


bool recursivelySearchSubdirectories( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   char* mark;

   bool searchRecursively = false;

   if ( 0 == strnicmp( index, "recur", 5 ) )
   {
      index += 5;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   if ( *index )
   {
      searchRecursively = strtobool( index, &mark );

      if ( index == mark )
      {
         // unknown value
         searchRecursively = true;
      }

      if ( false == isCmdLine )
      {
         printf( "   search recursively updated to %s", ( searchRecursively ? "true" : "false" ) );
      }
   }
   else if ( false == isCmdLine )
   {
      printf( "   search recursively reset to false (default)" );
   }

   settings.searchRecursively = searchRecursively;

   return true;
}


bool setEncodingSequenceByteLimit( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   uint32_t byteLimit = DEFAULT_SEQUENCE_LIMIT;

   if ( 0 == strnicmp( index, "maxb", 4 ) )
   {
      index += 4;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      byteLimit = strtoul( index, &mark, 10 );

      if ( mark != index )
      {
         if ( byteLimit >= MIN_SEQUENCE_SIZE )
         {
            if ( byteLimit < MAX_SEQUENCE_SIZE ) // over 10MB hard limit?
            {
               settings.sequence.limitMaxSequenceBytes = byteLimit;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   updated encoding sequence byte limit to %" FSTR_UINT32_T " bytes.", byteLimit );
               }
            }
            else if ( false == isCmdLine )
            {
               printf( "   encoding sequence byte limit must be less than %" FSTR_UINT32_T " bytes: %" FSTR_UINT32_T " requested", MAX_SEQUENCE_SIZE, byteLimit );
            }
         }
         else if ( false == isCmdLine )
         {
            printf( "   encoding sequence byte limit must be more than %" FSTR_UINT32_T " bytes: %" FSTR_UINT32_T " requested", MIN_SEQUENCE_SIZE, byteLimit );
         }
      }
      else if ( false == isCmdLine )
      {
         printf( "   encoding sequence limited to between 16KB and 10MB: %" FSTR_UINT32_T " bytes", byteLimit );
      }
   }
   else
   {
      settings.sequence.limitMaxSequenceBytes = byteLimit;

      result = true;

      if ( false == isCmdLine )
      {
         printf( "   reset encoding sequence byte limit to default value %" FSTR_UINT32_T " bytes", byteLimit );
      }
   }

   return result;
}


bool setOpticalReaderApertureSize( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   float apertureSize = 9.0f;

   if ( 0 == strnicmp( index, "asiz", 4 ) )
   {
      index += 4;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      apertureSize = strtof( index, &mark );

      if ( mark != index )
      {
         if ( apertureSize >= READER_MIN_APERTURE_SIZE_INCHES )
         {
            if ( apertureSize <= READER_MAX_APERTURE_SIZE_INCHES )
            {
               settings.pageLayout.limitApertureSizeInches = apertureSize;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   updated aperture size limit to %1.2f inches", apertureSize );
               }
            }
            else if ( false == isCmdLine )
            {
               printf( "   aperture size must be less than %1.2f inches", READER_MAX_APERTURE_SIZE_INCHES );
            }
         }
         else if ( false == isCmdLine )
         {
            printf( "   aperture size must be greater than %1.2f inches", READER_MIN_APERTURE_SIZE_INCHES );
         }
      }
      else if ( false == isCmdLine )
      {
         printf( "   expected aperture size between %1.2f to %1.2f inches: %1.2f inches",
                 READER_MIN_APERTURE_SIZE_INCHES, READER_MAX_APERTURE_SIZE_INCHES, apertureSize );
      }
   }
   else
   {
      settings.pageLayout.limitApertureSizeInches = apertureSize;

      if ( false == isCmdLine )
      {
         printf( "   reset aperture size to default value %1.2f inches", apertureSize );
      }
   }

   return result;
}


bool setInteractiveSession( bool isCmdLine, char* inputStr )
{
   bool result = false;

   if ( isCmdLine )
   {
      settings.isInteractive = false;

      result = true;
   }

   return result;
}


bool displayProgramVersion( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      printIdentity();

      return true;
   }

   return false;
}


bool displayCommandLineHelp( bool isCmdLine, char* inputStr )
{
   if ( true == isCmdLine )
   {
      printUsage();

      return true;
   }

   return false;
}


bool setHSyncValue( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   uint16_t offset = 0;

   uint16_t hSync = 0;

   if ( 0 == strnicmp( index, "hsync", 5 ) )
   {
      index += 5;
   }
   else
   {
      index += 1;
   }

   if ( ( '0' <= *index ) && ( '9' >= *index ) )
   {
      offset = ( uint16_t )strtoul( index, &mark, 10 );
   }
   else
   {
      mark = index;
   }

   index = ltrim( mark, hzWSpace, hzWSpaceCount );

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( offset <= SEQUENCE_LIMIT )
   {
      hSync = ( uint16_t )strtoul( index, &mark, 16 );

      if ( index != mark )
      {
         if ( ( 4 <= hSync ) && ( 12 >= hSync ) )
         {
            settings.sequence.encoding[ offset ].override |= strip_hsync;
            settings.sequence.encoding[ offset ].hSync = ( uint8_t )hSync;

            result = true;

            if ( false == isCmdLine )
            {
               if ( 0 == offset )
               {
                  printf( "   updated default hSync to 0x%02X nibbles per scanline", hSync );
               }
               else
               {
                  printf( "   updated hSync for strip #%" FSTR_UINT16_T " to 0x%02X nibbles per scanline", offset, hSync );
               }
            }
         }
         else if ( false == isCmdLine )
         {
            printf( "   hSync must be a hex value between 0x04 and 0x0C (12)" );
         }
      }
      else if ( false == isCmdLine ) // not a (base16) number
      {
         printf( "   hSync must be a hex value between 0x04 and 0x0C (12)");
      }
   }
   else if ( false == isCmdLine ) // offset range out of bounds
   {
      printf( "   strip #%" FSTR_UINT16_T " is beyond the limit of a valid sequence (%d)", offset, SEQUENCE_LIMIT );
   }

   return result;
}

bool setVSyncValue( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   uint16_t offset = 0;

   uint16_t vSync = 0;

   if ( 0 == strnicmp( index, "vsync", 5 ) )
   {
      index += 5;
   }
   else
   {
      index += 1;
   }

   if ( ( '0' <= *index ) && ( '9' >= *index ) )
   {
      offset = ( uint16_t )strtoul( index, &mark, 10 );
   }
   else
   {
      mark = index;
   }

   index = ltrim( mark, hzWSpace, hzWSpaceCount );

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( offset <= SEQUENCE_LIMIT )
   {
      vSync = ( uint16_t )strtoul( index, &mark, 16 );

      if ( index != mark )
      {
         if ( ( 32 <= vSync ) && ( 255 >= vSync ) )
         {
            settings.sequence.encoding[ offset ].override |= strip_vsync;
            settings.sequence.encoding[ offset ].vSync = ( uint8_t )vSync;

            result = true;

            if ( false == isCmdLine )
            {
               if ( 0 == offset )
               {
                  printf( "   updated default vSync to 0x%02X microsteps per scanline", vSync );
               }
               else
               {
                  printf( "   updated vSync for strip #%" FSTR_UINT16_T " to 0x%02X microsteps per scanline", offset, vSync );
               }
            }
         }
         else if ( false == isCmdLine ) // vsync value out of range
         {
            printf( "   vSync must be a hex value between 0x20 (32) and 0xFF (255)" );
         }
      }
      else if ( false == isCmdLine ) // not a (base16) number
      {
         printf( "   vSync must be a hex value between 0x20 (32) and 0xFF (255)" );
      }
   }
   else if ( false == isCmdLine ) // offset range out of bounds
   {
      printf( "   strip #%" FSTR_UINT16_T " is beyond the limit of a valid sequence (%d)", offset, SEQUENCE_LIMIT );
   }

   return result;
}


bool setSyncValue( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   uint16_t offset = 0;

   uint16_t hSync = 0;
   uint16_t vSync = 0;

   if ( 0 == strnicmp( index, "sync", 4 ) )
   {
      index += 4;
   }
   else
   {
      index += 1;
   }

   if ( ( '0' <= *index ) && ( '9' >= *index ) )
   {
      offset = ( uint16_t )strtoul( index, &mark, 10 );
   }
   else
   {
      mark = index;
   }

   index = ltrim( mark, hzWSpace, hzWSpaceCount );

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( offset <= SEQUENCE_LIMIT )
   {
      hSync = ( uint16_t )strtoul( index, &mark, 16 );

      if ( index != mark )
      {
         if ( ( 4 <= hSync ) && ( 12 >= hSync ) )
         {
            result = true;
         }
         else if ( false == isCmdLine )
         {
            printf( "   hSync must be a hex value between 0x04 and 0x0C (12)" );
         }
      }
      else if ( false == isCmdLine ) // hsync not a (base16) number
      {
         printf( "   hSync must be a hex value between 0x04 and 0x0C (12)" );
      }

      if ( true == result )
      {
         index = ltrim( mark, hzWSpace, hzWSpaceCount );

         if ( ',' == *index )
         {
            ++index;

            index = ltrim( index, hzWSpace, hzWSpaceCount );

            vSync = ( uint16_t )strtoul( index, &mark, 16 );

            if ( index != mark )
            {
               if ( ( 32 <= vSync ) && ( 255 >= vSync ) )
               {
                  settings.sequence.encoding[ offset ].override |= strip_hsync | strip_vsync;
                  settings.sequence.encoding[ offset ].hSync = ( uint8_t )hSync;
                  settings.sequence.encoding[ offset ].vSync = ( uint8_t )vSync;

                  if ( false == isCmdLine )
                  {
                     if ( 0 == offset )
                     {
                        printf( "   updated default hSync to 0x%02X nibbles per scanline" LNFEED, hSync );
                        printf( "   updated default vSync to 0x%02X microsteps per scanline", vSync );
                     }
                     else
                     {
                        printf( "   updated hSync for strip #%u to 0x%02X nibbles per scanline" LNFEED, offset, hSync );
                        printf( "   updated vSync for strip #%u to 0x%02X microsteps per scanline", offset, vSync );
                     }
                  }
               }
               else
               {
                  result = false;

                  if ( false == isCmdLine )
                  {
                     printf( "   vSync must be a hex value between 0x20 (32) and 0xFF (255)" );
                  }
               }
            }
            else // vsync not a (base16) number
            {
               result = false;

               if ( false == isCmdLine )
               {
                  printf( "   vSync must be a hex value between 0x20 (32) and 0xFF (255)" );
               }
            }
         }
         else // missing vSync from hSync,vSync pair
         {
            result = false;

            if ( false == isCmdLine )
            {
               printf( "   vSync missing from hex pair hSync,vSync" );
            }
         }
      }
   }
   else if ( false == isCmdLine ) // offset range out of bounds
   {
      printf( "   strip #%" FSTR_UINT16_T " is beyond the limit of a valid sequence (%d)", offset, SEQUENCE_LIMIT );
   }

   return result;
}


bool updateCRCGeneration( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   char* mark;

   uint16_t offset = 0;

   bool generateCRC = false;

   if ( 0 == strnicmp( index, "crc", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   if ( ( '0' <= *index ) && ( '9' >= *index ) )
   {
      offset = ( uint16_t )strtoul( index, &mark, 10 );
   }
   else
   {
      mark = index;
   }

   index = ltrim( mark, hzWSpace, hzWSpaceCount );

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( *index )
   {
      generateCRC = strtobool( index, &mark );

      if ( generateCRC )
      {
         settings.sequence.encoding[ offset ].override |= strip_crc;
      }
      else
      {
         settings.sequence.encoding[ offset ].override &= ~strip_crc;
      }

      if ( false == isCmdLine )
      {
         if ( 0 == offset )
         {
            printf( "   updated generate CRC for all strips to %s", ( generateCRC ? "true" : "false" ) );
         }
         else
         {
            printf( "   updated generate CRC for strip #%" FSTR_UINT16_T " to %s",
                    offset, ( generateCRC ? "true" : "false" ) );
         }
      }
   }
   else if ( false == isCmdLine )
   {
      if ( 0 == offset )
      {
         printf( "   reset generate CRC for all strips to false (default)" );
      }
      else
      {
         printf( "   reset generate CRC for strip #%" FSTR_UINT16_T " to false (default)", offset );
      }
   }

   return true;
}


bool updateRenderMode( bool isCmdLine, char* inputStr )
{
   char* index = inputStr;

   size_t length;

   bool result = false;

   if ( 0 == strnicmp( index, "format", 6 ) )
   {
      index += 6;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      if ( 6 == length )
      {
         if ( 0 == stricmp( index, "direct" ) )
         {
            if ( settings.outputMode != direct_render )
            {
               settings.outputMode = direct_render;
               settings.pageLayout.reductionFactor = 1.0f;
               settings.pageLayout.inkSpreadIndex = 0.0f;

               if ( false == isCmdLine )
               {
                  printf( "   updated render mode format to direct render" );
               }
            }
            else if ( false == isCmdLine )
            {
               printf( "   render mode format was already set to direct render" );
            }

            result = true;
         }
         else if ( 0 == stricmp( index, "reduce" ) )
         {
            if ( settings.outputMode != photo_reduce )
            {
               settings.outputMode = photo_reduce;
               settings.pageLayout.reductionFactor = 10.0f;
               settings.pageLayout.inkSpreadIndex = 1.0f;

               if ( false == isCmdLine )
               {
                  printf( "   updated render mode format to photo reduce" );
               }
            }
            else if ( false == isCmdLine )
            {
               printf( "   render mode format was already set to photo reduce" );
            }

            result = true;
         }
         else if ( false == isCmdLine ) // nonconforming value ignored
         {
            printf( "   a render format of '%s' was not recognized", index );
         }
      }
      else if ( ( 5 == length ) && ( 0 == stricmp( index, "photo" ) ) )
      {
         if ( settings.outputMode != photo_reduce )
         {
            settings.outputMode = photo_reduce;
            settings.pageLayout.reductionFactor = 10.0f;
            settings.pageLayout.inkSpreadIndex = 1.0f;

            if ( false == isCmdLine )
            {
               printf( "   updated render mode format to photo reduce" );
            }
         }
         else if ( false == isCmdLine )
         {
            printf( "   render mode format was already set to photo reduce" );
         }

         result = true;
      }
      else if ( false == isCmdLine ) // nonconforming value ignored
      {
         printf( "   a render format of '%s' was not recognized", index );
      }
   }
   else
   {
      settings.outputMode = direct_render;
      settings.pageLayout.reductionFactor = 1.0f;
      settings.pageLayout.inkSpreadIndex = 0.0f;

      result = true;

      if ( false == isCmdLine )
      {
         printf("   a render format of '%s' was not recognized, reset render mode format to direct (default)", index );
      }
   }

   return result;
}


bool updatePublishedMediaParameters( bool isCmdLine, char* inputStr )
{
   bool result = false;

   char* index = inputStr;

   char* mark;

   char* specifier = inputStr;

   size_t length = 0;

   uint16_t i;

   float fValue;

   if ( 0 == strnicmp( index, "pub", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( *index )
   {
      if ( '.' == *index )
      {
         specifier = index + 1;
      }

      switch ( *specifier )
      {
         case 'H':
         case 'h':
            index = specifier + 6;
            break;

         case 'W':
         case 'w':
            index = specifier + 5;
            break;

         case 'T':
         case 't':
         case 'B':
         case 'b':
         case 'L':
         case 'l':
         case 'R':
         case 'r':
            index = specifier + 4;
            break;
      }

      if ( isCmdLine && ( '=' == *index ) )
      {
         ++index;
      }

      index = ltrim( index, hzWSpace, hzWSpaceCount );

      length = strlen( index );

      if ( 0 < length )
      {
         // update value
         switch ( *specifier )
         {
            case 'M':
            case 'm':
            case 'P':
            case 'p':
            {
               // /pub=foo
               for ( i = 0; i < pageCount; i++ )
               {
                  if ( 0 == stricmp( pageName[ i ], index ) )
                  {
                     if ( settings.media.lastPublishSelection != i )
                     {
                        settings.media.lastPublishSelection = ( uint8_t )i;
                        settings.media.publish = publishStandard[ i ];

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media settings to %s", pageName[ i ]);
                        }
                     }

                     result = true;
                  }
               }

               if ( ( false == result ) && ( false == isCmdLine ) )
               {
                  printf( "   published media setting of '%s' not valid", index );
               }
            }
               break;

            case 'H':
            case 'h':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= PUBLISHED_HEIGHT_MIN_SIZE_INCHES )
                  {
                     if ( fValue <= PUBLISHED_HEIGHT_MAX_SIZE_INCHES )
                     {
                        settings.media.publish.height = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media height to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum 120.0f inches
                     {
                        printf( "   published media height input exceeds %1.2f inches",
                                PUBLISHED_HEIGHT_MAX_SIZE_INCHES );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 2.0f inches
                  {
                     printf( "   published media height input less than minimum %1.2f inches",
                             PUBLISHED_HEIGHT_MIN_SIZE_INCHES );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   published media height must be between %1.2f and %1.2f inches",
                          PUBLISHED_HEIGHT_MIN_SIZE_INCHES, PUBLISHED_HEIGHT_MAX_SIZE_INCHES );
               }
            }
               break;

            case 'W':
            case 'w':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= PUBLISHED_WIDTH_MIN_SIZE_INCHES )
                  {
                     if ( fValue <= PUBLISHED_WIDTH_MAX_SIZE_INCHES )
                     {
                        settings.media.publish.width = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media width to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum 120.0f inches
                     {
                        printf( "   published media width input exceeds %1.2f inches", PUBLISHED_WIDTH_MAX_SIZE_INCHES );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 2.0f inches
                  {
                     printf( "   published media width input less than minimum %1.2f inches", PUBLISHED_WIDTH_MIN_SIZE_INCHES );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   published media width must be between %1.2f and %1.2f inches",
                          PUBLISHED_WIDTH_MIN_SIZE_INCHES, PUBLISHED_WIDTH_MAX_SIZE_INCHES );
               }
            }
               break;

            case 'T':
            case 't':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.publish.height - settings.media.publish.margin.bottom - 1.5f ) )
                     {
                        settings.media.publish.margin.top = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media top margin to %1.2f", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum height inches
                     {
                        printf( "   top margin value exceeds maximum for published media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   top margin must be greater than or equal to 0.0" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   top margin input value not a valid number" );
               }
            }
               break;

            case 'B':
            case 'b':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.publish.height - settings.media.publish.margin.top - 1.5f ) )
                     {
                        settings.media.publish.margin.bottom = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media bottom margin to %1.2f", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum height inches
                     {
                        printf( "   bottom margin value exceeds maximum for published media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   bottom margin must be greater than or equal to 0.0" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   bottom margin input value not a valid number" );
               }
            }

               break;

            case 'L':
            case 'l':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.publish.width - settings.media.publish.margin.right - 1.5f ) )
                     {
                        settings.media.publish.margin.left = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media left margin to %1.2f", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum width inches
                     {
                        printf( "   left margin value exceeds maximum for published media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   left margin must be greater than or equal to 0.0" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   left margin input value not a valid number" );
               }
            }

               break;

            case 'R':
            case 'r':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.publish.width - settings.media.publish.margin.left - 1.5f ) )
                     {
                        settings.media.publish.margin.right = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated published media right margin to %1.2f", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum width inches
                     {
                        printf( "   right margin value exceeds maximum for published media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   right margin must be greater than or equal to 0.0" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   right margin input value not a valid number" );
               }
            }
               break;
         }
      }
      else
      {
         // view/reset value
         switch ( *specifier )
         {
            case 'M':
            case 'm':
            case 'P':
            case 'p':
            {
               // /pub= reset Published media settings

               settings.media.lastPublishSelection = 0;
               settings.media.publish = publishStandard[ 0 ];

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset published media settings to %s (default)", pageName[ 0 ] );
               }
            }
               break;

            case 'H':
            case 'h':
            case 'T':
            case 't':
            case 'B':
            case 'b':
            {
               // reset Published media settings height
               settings.media.publish.height = 11.0f;
               settings.media.publish.margin.top = 0.5f;
               settings.media.publish.margin.bottom = 0.75f;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset published media height, top and bottom margin values" );
               }
            }
               break;

            case 'W':
            case 'w':
            case 'L':
            case 'l':
            case 'R':
            case 'r':
            {
               // reset Published media settings width
               settings.media.publish.width = 8.5f;
               settings.media.publish.margin.left = 0.75f;
               settings.media.publish.margin.right = 0.50f;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset published media width, left and right margin values" );
               }
            }
               break;
         }
      }
   }
   else
   {
      // /pub
      // view Published media settings
      displayMediaSettings( 'm' );

      result = true;
   }

   return result;
}


bool updatePrintMediaParameters( bool isCmdLine, char* inputStr )
{
   bool result = false;

   char* index = inputStr;

   char* mark;

   char* specifier = inputStr;

   size_t length = 0;

   uint16_t i;

   float fValue;

   struct PrintResolutionType dots = { 0 };

   uint32_t hRes;
   uint32_t vRes;

   if ( 0 == strnicmp( index, "prt", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   if ( *index )
   {
      if ( '.' == *index )
      {
         specifier = index + 1;
      }

      switch ( *specifier )
      {
         case 'H':
         case 'h':
            index = specifier + 6;
            break;

         case 'W':
         case 'w':
            index = specifier + 5;
            break;

         case 'T':
         case 't':
         case 'B':
         case 'b':
         case 'L':
         case 'l':
         case 'R':
         case 'r':
         case 'F':
         case 'f':
         case 'D':
         case 'd':
            index = specifier + 4;
            break;
      }

      if ( isCmdLine && ( '=' == *index ) )
      {
         ++index;
      }

      index = ltrim( index, hzWSpace, hzWSpaceCount );

      length = strlen( index );

      if ( 0 < length )
      {
         // update value
         switch ( *specifier )
         {
            case 'P':
            case 'p':
            {
               // /pub=foo
               for ( i = 0; i < pageCount; i++ )
               {
                  if ( 0 == stricmp( pageName[ i ], index ) )
                  {
                     if ( settings.media.lastPrintSelection != i )
                     {
                        settings.media.lastPrintSelection = ( uint8_t )i;
                        settings.media.print = printStandard[ i ];

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media settings to %s", pageName[ i ]);
                        }
                     }

                     result = true;
                  }
               }

               if ( ( false == result ) && ( false == isCmdLine ) )
               {
                  printf( "   print media setting of '%s' not valid", index );
               }
            }
               break;

            case 'H':
            case 'h':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= PRINT_HEIGHT_MIN_SIZE_INCHES )
                  {
                     if ( fValue <= PRINT_HEIGHT_MAX_SIZE_INCHES )
                     {
                        settings.media.print.height = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media height to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum 120.0f inches
                     {
                        printf( "   print media height input exceeds %1.2f inches",
                                PRINT_HEIGHT_MAX_SIZE_INCHES );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 2.0f inches
                  {
                     printf( "   print media height input less than minimum %1.2f inches",
                             PRINT_HEIGHT_MIN_SIZE_INCHES );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   print media height must be between %1.2f and %1.2f inches",
                          PRINT_HEIGHT_MIN_SIZE_INCHES, PRINT_HEIGHT_MAX_SIZE_INCHES );
               }
            }
               break;

            case 'W':
            case 'w':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= PRINT_WIDTH_MIN_SIZE_INCHES )
                  {
                     if ( fValue <= PRINT_WIDTH_MAX_SIZE_INCHES )
                     {
                        settings.media.print.width = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media width to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum 120.0f inches
                     {
                        printf( "   print media width input exceeds %1.2f inches",
                                PRINT_WIDTH_MAX_SIZE_INCHES );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 2.0f inches
                  {
                     printf( "   print media width input less than minimum %1.2f inches",
                             PRINT_WIDTH_MIN_SIZE_INCHES );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   print media width must be between %1.2f and %1.2f inches",
                          PRINT_WIDTH_MIN_SIZE_INCHES, PRINT_WIDTH_MAX_SIZE_INCHES );
               }
            }
               break;

            case 'T':
            case 't':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.print.height - settings.media.print.margin.bottom - 1.5f ) )
                     {
                        settings.media.print.margin.top = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media top margin to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum height inches
                     {
                        printf( "   top margin value exceeds maximum for print media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   top margin must be greater than or equal to 0.0 inches" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   top margin input value not a valid number" );
               }
            }
               break;

            case 'B':
            case 'b':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.print.height - settings.media.print.margin.top - 1.5f ) )
                     {
                        settings.media.print.margin.bottom = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media bottom margin to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum height inches
                     {
                        printf( "   bottom margin value exceeds maximum for print media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   bottom margin must be greater than or equal to 0.0 inches" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   bottom margin input value not a valid number" );
               }
            }

               break;

            case 'L':
            case 'l':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.print.width - settings.media.print.margin.right - 1.5f ) )
                     {
                        settings.media.print.margin.left = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media left margin to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum width inches
                     {
                        printf( "   left margin value exceeds maximum for print media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   left margin must be greater than or equal to 0.0 inches" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   left margin input value not a valid number" );
               }
            }

               break;

            case 'R':
            case 'r':
            {
               fValue = ( float )strtod( index, &mark );

               if ( index != mark )
               {
                  if ( fValue >= 0.0f )
                  {
                     if ( fValue <= ( settings.media.print.width - settings.media.print.margin.left - 1.5f ) )
                     {
                        settings.media.print.margin.right = fValue;

                        result = true;

                        if ( false == isCmdLine )
                        {
                           printf( "   updated print media right margin to %1.2f inches", fValue );
                        }
                     }
                     else if ( false == isCmdLine ) // exceeds maximum width inches
                     {
                        printf( "   right margin value exceeds maximum for print media" );
                     }
                  }
                  else if ( false == isCmdLine ) // less than minimum 0.0f inches
                  {
                     printf( "   sorry, right margin must be greater than or equal to 0.0 inches" );
                  }
               }
               else if ( false == isCmdLine ) // fValue not a number
               {
                  printf( "   right margin input value not a valid number" );
               }
            }
               break;

            case 'F':
            case 'f':
            {
               // handle print feed type
               if ( 0 == stricmp( "sheet", index ) )
               {
                  settings.media.print.feed = sheet;

                  if ( false == isCmdLine )
                  {
                     printf( "   updated print media attribute feed to new value %s", index );
                  }
               }
                else if ( 0 == stricmp( "continuous", index ) )
                {
                   settings.media.print.feed = continuous;

                   if ( false == isCmdLine )
                   {
                      printf( "   updated print media attribute feed to new value %s", index );
                   }
                }
                else if ( false == isCmdLine ) // not a valid selection
                {
                   printf( "   didn't recognize '%s' as valid input for print feed media.", index );
                }
            }
               break;

            case 'D':
            case 'd':
            {
               // handle dots per area
               hRes = strtoul( index, &mark, 10 );

               if ( mark != index )
               {
                  index = ltrim( index, hzWSpace, hzWSpaceCount );

                  if ( ',' == *mark )
                  {
                     index = trim( mark + 1, whitespace, whitespaceCount );

                     mark = index;

                     vRes = strtoul( index, &mark, 10 );

                     if ( mark != index )
                     {
                        if ( ( hRes > 31 ) && ( hRes < 24001 ) && ( vRes > 31 ) && ( vRes < 24001 ) )
                        {
                           dots.horizontal = ( uint16_t )hRes;
                           dots.vertical = ( uint16_t )vRes;

                           settings.media.print.dots = dots;

                           if ( false == isCmdLine )
                           {
                              printf( "   updated print media dot resolution to %" FSTR_UINT16_T ", %" FSTR_UINT16_T,
                                      dots.horizontal, dots.vertical );
                           }
                        }
                        else if ( false == isCmdLine ) // out of range
                        {
                           printf( "   print media dot resolution not in valid range (32 to 24000)" );
                        }
                     }
                     else if ( false == isCmdLine ) // vRes not a number
                     {
                        printf( "   sorry, vertical dot resolution not in valid range (32 to 24000)" );
                     }
                  }
                  else if ( false == isCmdLine ) // missing vRes
                  {
                     printf( "   sorry, vertical dot resolution missing from input value" );
                  }
               }
               else if ( false == isCmdLine ) // hRes not a number
               {
                   printf( "   sorry, horizontal dot resolution not in valid range (32 to 24000)" );
               }
            }
               break;
         }
      }
      else
      {
         // view/reset value
         switch ( *specifier )
         {
            case 'M':
            case 'm':
            case 'P':
            case 'p':
            {
               // /pub= reset Print media settings

               settings.media.lastPrintSelection = 0;
               settings.media.print = printStandard[ 0 ];

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset print media settings to %s (default)", pageName[ 0 ]);
               }
            }
               break;

            case 'H':
            case 'h':
            case 'T':
            case 't':
            case 'B':
            case 'b':
            {
               // reset Print media settings height
               settings.media.print.height = 11.0f;
               settings.media.print.margin.top = 0.5f;
               settings.media.print.margin.bottom = 0.75f;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset print media height, top and bottom margin values" );
               }
            }
               break;

            case 'W':
            case 'w':
            case 'L':
            case 'l':
            case 'R':
            case 'r':
            {
               // reset Print media settings width
               settings.media.print.width = 8.5f;
               settings.media.print.margin.left = 0.75f;
               settings.media.print.margin.right = 0.50f;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset print media width, left and right margin values" );
               }
            }
               break;

            case 'F':
            case 'f':
            {
               settings.media.print.feed = sheet;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset print media feed to sheet" );
               }
            }
               break;

            case 'D':
            case 'd':
            {
               settings.media.print.dots.horizontal = 600;
               settings.media.print.dots.vertical = 600;

               result = true;

               if ( false == isCmdLine )
               {
                  printf( "   reset print media horizontal/vertical resolution to 600,600" );
               }
            }
               break;
         }
      }
   }
   else
   {
      // /pub
      // view Print media settings
      displayMediaSettings( 'p' );

      result = true;
   }

   return result;
}

bool updateInkSpreadIndex( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   float inkSpreadIndex = ( direct_render == settings.outputMode ) ? 0.0f : 1.0f;

   if ( 0 == strnicmp( index, "ink", 3 ) )
   {
      index += 3;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      inkSpreadIndex = strtof( index, &mark );

      if ( mark != index )
      {
         if ( inkSpreadIndex >= 0.0f )
         {
            if ( inkSpreadIndex <= 50.01f )
            {
               settings.pageLayout.inkSpreadIndex = inkSpreadIndex;

               result = true;
            }
            else
            {
               // too large
            }
         }
         else
         {
            // too small
         }
      }
      else
      {
         // not a number
      }
   }
   else
   {
      settings.pageLayout.inkSpreadIndex = inkSpreadIndex;

      result = true;
   }

   return result;
}


bool updateReductionFactor( bool isCmdLine, char* inputStr )
{
   size_t length;

   char* index = inputStr;

   char* mark = NULL;

   bool result = false;

   float reductionFactor = ( direct_render == settings.outputMode ) ? 1.0f : 10.0f;

   if ( 0 == strnicmp( index, "scale", 5 ) )
   {
      index += 5;
   }
   else
   {
      index += 1;
   }

   if ( isCmdLine && ( '=' == *index ) )
   {
      ++index;
   }

   index = ltrim( index, hzWSpace, hzWSpaceCount );

   length = strlen( index );

   if ( 0 < length )
   {
      reductionFactor = strtof( index, &mark );

      if ( mark != index )
      {
         if ( reductionFactor >= 1.0f )
         {
            if ( reductionFactor <= 20.01f )
            {
               settings.pageLayout.reductionFactor = reductionFactor;

               result = true;
            }
            else
            {
               // too large
            }
         }
         else
         {
            // too small
         }
      }
      else
      {
         // not a number
      }
   }
   else
   {
      settings.pageLayout.reductionFactor = reductionFactor;

      result = true;
   }

   return result;
}


// bubble sort command line arguments based on command precedence
char** sortCommandLineArguments( uint16_t argC, char* argV[] )
{
   char** result = argV;

   uint16_t i;
   uint16_t j;
   uint16_t k;
   uint16_t swapCounter;

   char* temp;

   struct CommandType* lhs;
   struct CommandType* rhs;

   size_t aliasLen;
   size_t nameLen;

   if ( argC > 2 )
   {
      for ( i = 1; i < argC - 1; i++ )
      {
         swapCounter = 0;

         for ( j = 1; j < argC - i; j++ )
         {
            lhs = NULL;
            rhs = NULL;

            // lhs == result[ j ]
            // rhs == result[ j + 1 ]
            for ( k = 0; k < commandCount; k++ )
            {
               nameLen = strlen( command[ k ].name );

               if ( ( nameLen > 1 ) && ( '#' == command[ k ].name[ nameLen - 1 ] ) )
               {
                  nameLen--;
               }

               aliasLen = ( NULL != command[ k ].alias ) ? strlen( command[ k ].alias ) : 0;

               if ( ( 0 < aliasLen ) && ( '#' == command[ k ].alias[ aliasLen - 1 ] ) )
               {
                  aliasLen--;
               }

               if ( NULL == lhs )
               {
                  if ( '/' == result[ j ][ 0 ] )
                  {
                     if ( ( 0 != aliasLen ) && ( 0 == strnicmp( command[ k ].alias, result[ j ] + 1, aliasLen ) ) )
                     {
                        if ( ( '=' == result[ j ][ 1 + aliasLen ] )
                             || ( '\0' == result[ j ][ 1 + aliasLen ] ) )
                        {
                           lhs = &command[ k ];
                        }
                        else if ( ( '#' == command[ k ].alias[ aliasLen ] )
                                  && ( '0' <= result[ j ][ 1 + aliasLen ] )
                                  && ( '9' >= result[ j ][ 1 + aliasLen ] ) )
                        {
                           lhs = &command[ k ];
                        }
                     }
                     else if ( 0 == strnicmp( command[ k ].name, result[ j ] + 1, nameLen ) )
                     {
                        if ( ( '=' == result[ j ][ 1 + nameLen ] )
                             || ( '\0' == result[ j ][ 1 + nameLen ] ) )
                        {
                           lhs = &command[ k ];
                        }
                        else if ( ( '#' == command[ k ].name[ nameLen ] )
                                  && ( '0' <= result[ j ][ 1 + nameLen ] )
                                  && ( '9' >= result[ j ][ 1 + nameLen ] ) )
                        {
                           lhs = &command[ k ];
                        }
                     }
                  }
               }

               if ( NULL == rhs )
               {
                  if ( '/' == result[ j + 1 ][ 0 ] )
                  {
                     if ( ( 0 != aliasLen ) && ( 0 == strnicmp( command[ k ].alias, result[ j + 1 ] + 1, aliasLen ) ) )
                     {
                        if ( ( '=' == result[ j + 1 ][ 1 + aliasLen ] )
                             || ( '\0' == result[ j + 1 ][ 1 + aliasLen ] ) )
                        {
                           rhs = &command[ k ];
                        }
                        else if ( ( '#' == command[ k ].alias[ aliasLen ] )
                                  && ( '0' <= result[ j + 1 ][ 1 + aliasLen ] )
                                  && ( '9' >= result[ j + 1 ][ 1 + aliasLen ] ) )
                        {
                           rhs = &command[ k ];
                        }
                     }
                     else if ( 0 == strnicmp( command[ k ].name, result[ j + 1 ] + 1, nameLen ) )
                     {
                        if ( ( '=' == result[ j + 1 ][ 1 + nameLen ] )
                             || ( '\0' == result[ j + 1 ][ 1 + nameLen ] ) )
                        {
                           rhs = &command[ k ];
                        }
                        else if ( ( '#' == command[ k ].name[ nameLen ] )
                                  && ( '0' <= result[ j + 1 ][ 1 + nameLen ] )
                                  && ( '9' >= result[ j + 1 ][ 1 + nameLen ] ) )
                        {
                           rhs = &command[ k ];
                        }
                     }
                  }
               }

               if ( ( NULL != lhs ) && ( NULL != rhs ) )
               {
                  break;
               }
            }

            // lhs > rhs?
            if ( ( NULL != lhs ) && ( NULL != rhs ) )
            {
               if ( lhs->precedence > rhs->precedence )
               {
                  temp = result[ j ];
                  result[ j ] = result[ j + 1 ];
                  result[ j + 1 ] = temp;

                  ++swapCounter;
               }
            }
            else if ( NULL != lhs )
            {
               // move unknown commands left of well-formed commands
               if ( ( '/' == result[ j + 1 ][ 0 ] )
                    || ( lhs->precedence > 32767 ) ) // move /adj|/j to right of files
               {
                  temp = result[ j ];
                  result[ j ] = result[ j + 1 ];
                  result[ j + 1 ] = temp;

                  ++swapCounter;
               }
            }
            else if ( NULL != rhs )
            {
               // move commands left of file entries
               if ( ( '/' != result[ j ][ 0 ] )
                    && ( rhs->precedence <= 32767 ) ) // unless /adj|/j rhs command
               {
                  temp = result[ j ];
                  result[ j ] = result[ j + 1 ];
                  result[ j + 1 ] = temp;

                  ++swapCounter;
               }
            }
            else // ( NULL == lhs ) && ( NULL == rhs )
            {
               // one unknown command, one file entry
               if ( ( '/' == result[ j ][ 0 ] ) ^ ( '/' == result[ j + 1 ][ 0 ] ) )
               {
                  // move unknown commands left of file entries
                  if ( '/' == result[ j + 1 ][ 0 ] )
                  {
                     temp = result[ j ];
                     result[ j ] = result[ j + 1 ];
                     result[ j + 1 ] = temp;

                     ++swapCounter;
                  }
               }
            }
         }

         if ( 0 == swapCounter )
         {
            break;
         }
      }
   }

   return result;
}

enum CMDLineResultType processCommandLineArguments( uint16_t argC, char* argV[], struct StripSequenceType* sequenceOut )
{
   enum CMDLineResultType result = cmdln_completed;

   uint16_t i;
   uint16_t k;

   size_t nameLen;
   size_t aliasLen;

   if ( NULL != getcwd(settings.workingDirectory, _MAX_PATH + 14) )
   {
      settings.workingDirectory[ strlen( settings.workingDirectory ) + 1 ] = PATH_SEPARATOR_CHAR;
   }

   // find input files specified on command line
   for ( i = 1; i < argC; i++ )
   {
      if ( '/' == argV[ i ][ 0 ] )
      {
         for ( k = 0; k < commandCount; k++ )
         {
            nameLen = strlen( command[ k ].name );

            if ( '#' == command[ k ].name[ nameLen - 1 ] )
            {
               nameLen--;
            }

            aliasLen = ( NULL != command[ k ].alias ) ? strlen( command[ k ].alias ) : 0;

            if ( ( 0 < aliasLen ) && ( '#' == command[ k ].alias[ aliasLen - 1 ] ) )
            {
               aliasLen--;
            }

            if ( ( 0 != aliasLen ) && ( 0 == strnicmp( command[ k ].alias, argV[ i ] + 1, aliasLen ) ) )
            {
               if ( ( '=' == argV[ i ][ 1 + aliasLen ] )
                    || ( '\0' == argV[ i ][ 1 + aliasLen ] ) )
               {
                  if ( command[ k ].cmdHandler( true, argV[ i ] + 1 )
                     && ( command[ k ].result < result ) )
                  {
                     result = command[ k ].result;
                  }

                  break;
               }
               else if ( ( '#' == command[ k ].alias[ aliasLen ] )
                         && ( '0' <= argV[ i ][ 1 + aliasLen ] )
                         && ( '9' >= argV[ i ][ 1 + aliasLen ] ) )
               {
                  if ( command[ k ].cmdHandler( true, argV[ i ] + 1 )
                       && ( command[ k ].result < result ) )
                  {
                     result = command[ k ].result;
                  }

                  break;
               }
            }
            else if ( 0 == strnicmp( command[ k ].name, argV[ i ] + 1, nameLen ) )
            {
               if ( ( '=' == argV[ i ][ 1 + nameLen ] )
                    || ( '\0' == argV[ i ][ 1 + nameLen ] ) )
               {
                  if ( command[ k ].cmdHandler( true, argV[ i ] + 1 )
                       && ( command[ k ].result < result ) )
                  {
                     result = command[ k ].result;
                  }

                  break;
               }
               else if ( ( '#' == command[ k ].name[ nameLen ] )
                         && ( '0' <= argV[ i ][ 1 + nameLen ] )
                         && ( '9' >= argV[ i ][ 1 + nameLen ] ) )
               {
                  if ( command[ k ].cmdHandler( true, argV[ i ] + 1 )
                       && ( command[ k ].result < result ) )
                  {
                     result = command[ k ].result;
                  }

                  break;
               }
            }
         }
      }
      else if ( !appendInputFiles( argV[ i ], NULL, settings.searchRecursively ) )
      {
         fprintf( stderr, "IGNORED: Input file (%s) - could not open for read." LNFEED, argV[ i ] );
      }
   }


   if ( ( result > cmdln_halt ) && ( false == settings.isInteractive ) )
   {
      // write the strip content and exit?
      if ( CZFList_itemCount() > 0 )
      {
         // calculate Data Density
         calculateStripHeight( &settings.stripLayout, settings.media );

         populateDataDensityTable( settings.stripLayout );
      }
      else
      {
         printf( LNFEED "ERROR: No input files specified (nothing to do)" LNFEED );
      }
   }

   return result;
}
