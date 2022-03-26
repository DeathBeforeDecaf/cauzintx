#include <stdio.h>  // printf()
#include <stdlib.h> //
#include <string.h> // strlen()
#include <time.h>   // localtime()

#include "strptx16/editor.h"   // displayEditorCommands(), displayStripSequenceContent(), stripSequenceEditor()
#include "strptx16/filelist.h" // CZFList
#include "strptx16/layout.h"   // initializeLayout()
#include "strptx16/platform.h"
#include "strptx16/settings.h"
#include "strptx16/stdtypes.h"
#include "strptx16/support.h"  // StripSequenceType, CMDLineResultType
#include "strptx16/txreduce.h" // struct DataDensityEntry
#include "strptx16/txstrip.h"  // renderStripSequence()

struct SystemSettings settings;

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
             "%04d-%02d-%02dT%02d:%02d:%02d",
             ( datetime->tm_year + 1900 ),
             ( datetime->tm_mon + 1 ),
             datetime->tm_mday,
             datetime->tm_hour,
             datetime->tm_min,
             datetime->tm_sec );

   printf( "File name     : %s" LNFEED, file->name );
   printf( "Full path     : %s" LNFEED, file->path );
   printf( "File size     : %lu" LNFEED, file->sizeBytes );
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
