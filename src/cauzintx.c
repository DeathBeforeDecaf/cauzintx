#include <stdio.h>  // printf(), snprintf()
#include <stdlib.h> // NULL
#include <time.h>   // localtime()

#include "strptx16/command.h"   // CMDLineResultType
#include "strptx16/editor.h"    // displayEditorCommands(), displayStripSequenceContent(), stripSequenceEditor()
#include "strptx16/filelist.h"  // CZFList_initialize(), CZFList_relinquish(), CZFList_itemCount(), CZFList_applyInOrder()
#include "strptx16/globals.h"   // ISO8601_STR
#include "strptx16/parseopt.h"  // sortCommandLineArguments(), processCommandLineArguments()
#include "strptx16/platform.h"  // platformInitialize(), platformRelinquish
#include "strptx16/settings.h"  // StripSequenceType
#include "strptx16/support.h"   // generateIdentity()
#include "strptx16/txdirect.h"  // initializeDeviceDependentPixelTable()
#include "strptx16/txstrip.h"   // renderStripSequence()

void platformRelinquish();

// 1. Collect the layout parameters for the published and printed media
// 2. Calculate (and/or select) published strip height/width
// 3. Given a published strip height, calculate data density table VSync x HSync
// 4. Populate MetaStrip w/input file data
// 5. Select HSync & VSync values and encode MetaStrip with valid data density
//    (all MetaStrip data must fit in the first strip)
// 6. Given a data density, paginate remaining data into DataStrips until zero
//    bytes remaining (HSync & VSync values may vary per strip)
// 7. Convert MetaStrip and DataStrip data into bitfields
// 8. Render bitfields to postscript file, based on printed media parameters
// 9. Post-process printed media to published media scale if necessary

void printFileData( CZFListDataType* item )
{
   static char iso8601Str[ 32 ];

   struct InputFileType* file = *item;

   struct tm* datetime = localtime( &( file->lastModified ) );

   snprintf( iso8601Str,
             sizeof( iso8601Str ),
             ISO8601_STR,
             ( datetime->tm_year + 1900 ),
             ( datetime->tm_mon + 1 ),
             datetime->tm_mday,
             datetime->tm_hour,
             datetime->tm_min,
             datetime->tm_sec );

   printf( "File name     : %s" LNFEED, file->name );
   printf( "Full path     : %s" LNFEED, file->path );
   printf( "File size     : %" FSTR_UINT32_T LNFEED, file->sizeBytes );
   printf( "Time modified : %s" LNFEED LNFEED, iso8601Str );
}


void relinquishFLData( CZFListDataType* item )
{
   if ( NULL != ( *item )->entry.adjunct )
   {
      free( ( *item )->entry.adjunct );

      ( *item )->entry.adjunct = NULL;
   }

   free( *item );
}

int main( int argC, char* argV[], char* envP[] )
{
   enum CMDLineResultType cmdline;

   struct StripSequenceType strip = { 0 };

   platformInitialize();

   initializeDeviceDependentPixelTable();

   CZFList_initialize( 12, 25 );

   generateIdentity( argV[ 0 ] );

   argV = sortCommandLineArguments( argC, argV );

   // enum CMDLineResultType
   cmdline = processCommandLineArguments( argC, argV, &strip );

   if ( cmdln_completed == cmdline )
   {
      if ( 0 == CZFList_itemCount() )
      {
         printf( LNFEED "> ?" );

         displayEditorCommands();
      }
      else
      {
         printf( LNFEED "> l" LNFEED );

         CZFList_sort( CZFListDataType_pathLessThan );

         displayStripSequenceContent();
      }
   }

   // run the strip sequence editor loop (unless immediate mode)
   while ( cmdln_completed == cmdline )
   {
      cmdline = stripSequenceEditor();
   }

   // write the strip content and exit?
   if ( cmdln_render == cmdline )
   {
      if ( CZFList_itemCount() > 0 )
      {
         renderStripSequence();
      }
      else
      {
         printf( LNFEED "ERROR: No input files specified (nothing to encode)" LNFEED );
      }
   }

   // relinquish file list data, relinquish file list data type
   CZFList_applyInOrder( relinquishFLData );

   CZFList_relinquish();

   platformRelinquish();

   return EXIT_SUCCESS;
}
