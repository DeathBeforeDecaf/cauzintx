#include "editor.h"

void displaySequenceInformation()
{
   const char* stripDensityStr[] =
   {
      LNFEED "  Very Low",
      LNFEED "  Low",
      LNFEED "  Medium",
      LNFEED "  High",
      LNFEED "  Very High",
   };

   uint8_t hsync[] =
   {
      0x04,
      0x06,
      0x08,
      0x0A,
      0x0C
   };

   uint8_t vsync[] =
   {
      0xA0,
      0x80,
      0x60,
      0x40,
      0x40
   };

   float stripHeight;

   uint8_t stripCount;

   uint32_t value;
   uint16_t index;
   uint16_t i;

   struct InputFileType* item;

   calculateStripHeight( &settings.stripLayout, settings.media );

   populateDataDensityTable( settings.stripLayout );

   printf( LNFEED );
   printf( "Published Media" LNFEED );

   fputs( "  Overall Media Height( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.media.publish.height, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  - Margin top( ", stdout );

   printf( "%1.3f %s )", settings.media.publish.margin.top, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  Margin bottom( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.media.publish.margin.bottom, linearUnitsName[ settings.media.publish.units ] );


   fputs( "  - Align Marks top( ", stdout );

   printf( "%1.3f %s )", settings.stripLayout.align.top, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  Align Marks bottom( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.stripLayout.align.bottom, linearUnitsName[ settings.media.publish.units ] );


   fputs( "  - Strip hsync height( ", stdout );

   printf( "%1.3f %s )", settings.stripLayout.header.hsync_h, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  Strip vsync height( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.stripLayout.header.vsync_h, linearUnitsName[ settings.media.publish.units ] );


   fputs( "  - Strip Header top margin( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.stripLayout.header.top_m, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  - Footer top margin( ", stdout );

   printf( "%1.3f %s )", settings.stripLayout.footer.top_m, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  Footer bottom margin( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.stripLayout.footer.bottom_m, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  - Footer watermark height( ", stdout );

   printf( "%1.3f %s )" LNFEED, settings.stripLayout.footer.wmark_h, linearUnitsName[ settings.media.publish.units ] );


   // height = publish - margin
   stripHeight =
      settings.media.publish.height - settings.media.publish.margin.top - settings.media.publish.margin.bottom;

   // height -= alignment_marks (top and bottom adjustments)
   stripHeight = stripHeight - settings.stripLayout.align.top - settings.stripLayout.align.bottom;

   // height -= header_height ( ~1/4 inch )
   stripHeight =
      stripHeight - settings.stripLayout.header.top_m - settings.stripLayout.header.hsync_h
      - settings.stripLayout.header.vsync_h;

   // height - footer_height ( watermark? )
   stripHeight =
      stripHeight - settings.stripLayout.footer.top_m - settings.stripLayout.footer.bottom_m
      - settings.stripLayout.footer.wmark_h;

   if ( stripHeight < settings.pageLayout.limitApertureSizeInches )
   {
      printf( LNFEED "Limit to max aperture size: %1.3f inches", settings.pageLayout.limitApertureSizeInches );

      printf( LNFEED "Strip Height on Published Media = " );

      printf( "%1.4f %s" LNFEED, stripHeight, linearUnitsName[ settings.media.publish.units ] );
   }
   else
   {
      printf( LNFEED "Strip Height on Published Media = " );

      printf( "%1.4f %s", stripHeight, linearUnitsName[ settings.media.publish.units ] );

      printf( LNFEED "Limit to max aperture size: %1.3f inches" LNFEED, settings.pageLayout.limitApertureSizeInches );
   }

   printf( LNFEED "Data Density" LNFEED );

   fputs( "  Apparent Width( ", stdout );

   printf( "%1.4f %s )", settings.stripLayout.apparentWidth, linearUnitsName[ settings.media.publish.units ] );

   fputs( "  Apparent Height( ", stdout );

   printf( "%1.4f %s )" LNFEED, settings.stripLayout.apparentHeight, linearUnitsName[ settings.media.publish.units ] );

   prompt( LNFEED "[Press any key to continue] ", " ", 1, " " );

   for ( i = 0; i < sizeof( stripDensityStr ) / sizeof( stripDensityStr[ 0 ] ); i++ )
   {
      index = 9 * ( 0xFF - vsync[ i ] ) + hsync[ i ] - 4;

      fputs( LNFEED, stdout );
      fputs( stripDensityStr[ i ], stdout );

      printf( "  hSync = 0x%02X  vSync = 0x%02X", density[ index ].hsync, density[ index ].vsync );

      fputs( "  Bits per Scanline( ", stdout );

      printf( "%" FSTR_UINT16_T " )" LNFEED, ( ( ( uint16_t )density[ index ].hsync ) << 3 ) + 14 );


      fputs( "    Bit Width( ", stdout );

      printf( "%1.4f %s )", density[ index ].bitWidth, linearUnitsName[ settings.media.publish.units ] );

      fputs( "  Bit Height( ", stdout );

      printf( "%1.4f %s )" LNFEED, density[ index ].bitHeight, linearUnitsName[ settings.media.publish.units ] );


      fputs( "    Lines per Strip( ", stdout );

      printf( "%" FSTR_UINT16_T " )", density[ index ].lineCountPerStrip );

      fputs( "  Bytes per Strip( ", stdout );

      printf( "%" FSTR_UINT16_T " )" LNFEED, density[ index ].byteCountPerStrip );

      fputs( "    Lines per Watermarked Strip( ", stdout );

      printf( "%" FSTR_UINT16_T " )", density[ index ].lineCountPerWatermarkedStrip );

      fputs( "  Bytes per Watermarked Strip( ", stdout );

      printf( "%" FSTR_UINT16_T " )", density[ index ].byteCountPerWatermarkedStrip );
   }

   prompt( LNFEED "[Press any key to continue] ", " ", 1, " " );

   printf( LNFEED );
   printf( "Strip Sequence defaults" LNFEED );


   printf( LNFEED "  Output filename: %s", settings.sequence.outputFilename );

   printf( LNFEED "  Strip ID: %s", settings.sequence.stripID );

   printf( LNFEED "  Default Horizontal Sync: 0x%02X", settings.sequence.encoding[ 0 ].hSync );

   printf( "  Default Vertical Sync: 0x%02X", settings.sequence.encoding[ 0 ].vSync );


   index = 9 * ( 0xFF - settings.sequence.encoding[ 0 ].vSync ) + settings.sequence.encoding[ 0 ].hSync - 4;

   fputs( LNFEED "  Strip hsync line height( ", stdout );

   i = ( uint16_t )floor( settings.stripLayout.header.hsync_h / density[ index ].bitHeight + 0.51 );

   printf( "%" FSTR_UINT16_T " lines )", i );

   fputs( "  Strip vsync line height( ", stdout );

   i = ( uint16_t )floor( settings.stripLayout.header.vsync_h / density[ index ].bitHeight + 0.51 );

   printf( "%" FSTR_UINT16_T " lines )", i );


   fputs( LNFEED "  Bits per Scanline( ", stdout );

   printf( "%" FSTR_UINT16_T " )", ( ( ( uint16_t )settings.sequence.encoding[ 0 ].hSync ) << 3 ) + 14  );


   fputs( "  Scanlines per strip( ", stdout );

   printf( "%" FSTR_UINT16_T " )", density[ index ].lineCountPerStrip );

   value = density[ index ].lineCountPerStrip;

   value *= ( ( ( uint16_t )settings.sequence.encoding[ 0 ].hSync ) << 3 ) + 14; // bitsPerLine


   fputs( LNFEED "  Total Bits per Strip( ", stdout );

   printf( "%" FSTR_UINT32_T " )", value );

   fputs( "  Bytes of storage per strip( ", stdout );

   printf( "%" FSTR_UINT16_T " )" LNFEED, density[ index ].byteCountPerStrip );


   // calculate limits
   //    iterate over file count
   //    verify total byte size < maximum bytes
   //    verify total file count < maximum file count

   printf( LNFEED "Calculate limits" LNFEED );

   value = 0;

   for ( i = 0; i < CZFList_itemCount(); i++ )
   {
      if ( CZFList_get( i, &item ) )
      {
         value += item->sizeBytes;
      }
   }

   fputs( LNFEED "  Total bytes to encode( ", stdout );

   printf( "%" FSTR_UINT32_T " )", value );

   if ( value < settings.sequence.limitMaxSequenceBytes )
   {
      fputs( ": OK", stdout );
   }
   else
   {
      fputs( LNFEED "ERROR: Total encode bytes for strip sequence exceeded!" LNFEED, stdout );
   }

   if ( 256 > CZFList_itemCount() )
   {
      if ( 0 < CZFList_itemCount() )
      {
         calculateStripSequenceExtents( &stripCount, &stripHeight );

         if ( ( 0 < stripCount ) && ( stripCount < 128 ) )
         {
            printf( LNFEED "  Content will be encoded in %" FSTR_UINT8_T " strips with default settings." LNFEED, stripCount );

            printf( LNFEED "  The last strip will be %1.2f %s tall." LNFEED,
                    stripHeight, linearUnitsName[ settings.stripLayout.units ] );
         }
         else
         {
            fputs( LNFEED "ERROR: Strip count exceeded 128 strips, reduce the quantity and/or size of files to encode." LNFEED,
                   stderr );
         }
      }
      else
      {
         printf( LNFEED "  No source files selected - nothing to encode." LNFEED );
      }
   }
   else
   {
      fputs( LNFEED "ERROR: Sequence file count exceeded 255, reduce the quantity of files to encode." LNFEED, stderr );
   }
}


void displayStripSequenceContent()
{
   size_t length;

   uint16_t i, j;

   char* index;
   char* mark;

   struct InputFileType* item;

   for ( i = 0; i < CZFList_itemCount(); i++ )
   {
      if ( CZFList_get( i, &item ) )
      {
         if ( 0 != i )
         {
            fputs( LNFEED, stdout );
         }

         printf( "% 2" FSTR_UINT16_T ": ", ( i + 1 ) );

         index = item->name;

         while ( *index && ( '.' != *index ) )
         {
            ++index;
         }

         if ( *index )
         {
            mark = index;

            while ( *index )
            {
               ++index;
            }

            length = mark - item->name;

            length = ( length < 9 ) ? ( 8 - length ) : 0;

            for ( j = 0; j < length; j++ )
            {
               fputc( ' ', stdout );
            }

            fputs( item->name, stdout );
         }
         else
         {
            length = strlen( item->name );

            length = ( length < 9 ) ? ( 8 - length ) : 0;

            for ( j = 0; j < length; j++ )
            {
               fputc( ' ', stdout );
            }

            fputs( item->name, stdout );

            fputs( "  ", stdout );
         }

         if ( 100000UL > item->sizeBytes )
         {
            fputc( ' ', stdout );
         }

         if ( 10000UL > item->sizeBytes )
         {
            fputc( ' ', stdout );
         }

         if ( 1000UL > item->sizeBytes )
         {
            fputc( ' ', stdout );
         }

         if ( 100UL > item->sizeBytes )
         {
            fputc( ' ', stdout );
         }

         if ( 10UL > item->sizeBytes )
         {
            fputc( ' ', stdout );
         }

         printf( " %" FSTR_UINT32_T " ", item->sizeBytes );

         if ( item->name != item->path )
         {
            fputs( item->path, stdout );
         }

         if ( NULL != item->entry.adjunct )
         {
            fputs( LNFEED "       ", stdout );
            fputs( ( char* )item->entry.adjunct, stdout );
         }
      }
   }
}


void displayMediaSettings( char mediaType )
{
   char* unitStr = NULL;

   float width = 0.0f, height = 0.0f;
   float lmar = 0.0f, rmar = 0.0f, tmar = 0.0f, bmar = 0.0f;

   printf( LNFEED );

   if ( 'm' == mediaType )
   {
      printf( "Published Media" LNFEED );

      unitStr = ( char* )linearUnitsName[ settings.media.publish.units ];

      width = settings.media.publish.width;
      height = settings.media.publish.height;

      lmar = settings.media.publish.margin.left;
      rmar = settings.media.publish.margin.right;
      tmar = settings.media.publish.margin.top;
      bmar = settings.media.publish.margin.bottom;
   }
   else if ( 'p' == mediaType )
   {
      printf( "Printed Media" LNFEED );

      unitStr = ( char* )linearUnitsName[ settings.media.print.units ];

      width = settings.media.print.width;
      height = settings.media.print.height;

      lmar = settings.media.print.margin.left;
      rmar = settings.media.print.margin.right;
      tmar = settings.media.print.margin.top;
      bmar = settings.media.print.margin.bottom;
   }

   fputs( LNFEED "  Overall Media Height( ", stdout );

   printf( "%1.3f %s )" LNFEED, height, unitStr );

   fputs( "  - Margin top( ", stdout );

   printf( "%1.3f %s )", tmar, unitStr );

   fputs( "  Margin bottom( ", stdout );

   printf( "%1.3f %s )" LNFEED, bmar, unitStr );

   fputs( "  = Available Media Height( ", stdout );

   printf( "%1.3f %s )" LNFEED, height - tmar - bmar, unitStr );


   fputs( LNFEED "  Overall Media Width( ", stdout );

   printf( "%1.3f %s )" LNFEED, width, unitStr );

   fputs( "  - Margin left( ", stdout );

   printf( "%1.3f %s )", lmar, unitStr );

   fputs( "  Margin right( ", stdout );

   printf( "%1.3f %s )" LNFEED, rmar, unitStr );

   fputs( "  = Available Media Width( ", stdout );

   printf( "%1.3f %s )" LNFEED, width - lmar - rmar, unitStr );

   if ( 'p' == mediaType )
   {
      // wait, it continues...

      fputs( LNFEED "  Print Media Feed Format: ", stdout );

      switch ( settings.media.print.feed )
      {
         case feed_unknown: fputs( "unknown" LNFEED, stdout );
            break;
         case sheet: fputs( "single sheet" LNFEED, stdout );
            break;
         case continuous: fputs( "continuous roll or fanfold" LNFEED, stdout );
            break;
      }

      if ( photo_reduce == settings.outputMode )
      {
         fputs( "  Ink Spread Index ( ", stdout );

         printf( "%1.4f ) bleed in thousandths of an inch" LNFEED, settings.pageLayout.inkSpreadIndex );

         fputs( "  Reduction Factor to scale to Published form ( ", stdout );

         printf( "%1.3f )" LNFEED, settings.pageLayout.reductionFactor );
      }

      fputs( LNFEED "  Print Resolution ( ", stdout );

      printf( "%" FSTR_UINT16_T ", %" FSTR_UINT16_T " )",
              settings.media.print.dots.horizontal, settings.media.print.dots.vertical );

      switch ( settings.media.print.units )
      {
         case unit_inch: fputs( " dots per inch", stdout );
            break;
         case unit_mm:  fputs( " dots per centimeter", stdout );
            break;
      }
   }
}


bool displayDirectModeSyncValues( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      nearestDeviceDependentResolution( settings.media.print.dots.horizontal,
                                        settings.media.print.dots.vertical );

      return true;
   }

   return false;
}


bool displayByteSyncTable( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      displayDataDensityTable(); //  (density, densityCount);

      return true;
   }

   return false;
}


bool displaySequenceMetrics( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      displaySequenceInformation();

      return true;
   }

   return false;
}


void displayEditorCommands()
{
   const char* OutputModeTypeStr[] =
   {
      "unknown",
      "Direct Render",
      "Photo Reduction"
   };

   printf( LNFEED "  Commands:" LNFEED );

   printf( "    a  file.ext    - append strip sequence with file.ext" LNFEED );
   printf( "    i2 file.ext    - insert file.ext into 2nd position in strip sequence" LNFEED );
   printf( "    d3             - delete a specific file entry from the strip sequence" LNFEED );
   printf( "    d4,5           - delete the 4th through 5th entries from the strip sequence" LNFEED );
   printf( "    /              - sort the strip sequence entries by path/file.ext" LNFEED );
   printf( "    l              - list the current content of the strip sequence" LNFEED );
   printf( "    $ %-6s       - set six character strip ID for sequence identification" LNFEED,
           settings.sequence.stripID );
   printf( "    o %-12s - set the output filename" LNFEED, settings.sequence.outputFilename );
   printf( "    t blah blah .. - set a verbose text description for the strip sequence" LNFEED );
   printf( "    j5 more info   - set adjunct information associated with specific file" LNFEED );
   printf( "    r %-5s        - set recursive directory search wildcard expansion" LNFEED,
   ( 0 == settings.searchRecursively ) ? "false" : "true" );
   printf( "    w              - write strip sequence into postscript output file" LNFEED );

   printf( "    q              - quit program and exit back to command line" LNFEED );
   printf( "    ?              - display some helpful hints" LNFEED LNFEED );


   printf( "    h  0x%02X        - set default hSync (nibbles/scan line) of rendered strips" LNFEED, settings.sequence.encoding[ 0 ].hSync );
   printf( "    h1 0x%02X        - set hSync (nibbles/scan line) of #1 rendered strip" LNFEED, settings.sequence.encoding[ 0 ].hSync );

   printf( "    v  0x%02X        - set default vSync (uSteps/scan line) of rendered strips" LNFEED, settings.sequence.encoding[ 0 ].vSync );
   printf( "    v2 0x%02X        - set vSync (uSteps/scan line) of #2 rendered strip" LNFEED, settings.sequence.encoding[ 0 ].vSync );

   printf( "    s  0x%02X,0x%02X   - set the default hSync and vSync values" LNFEED, settings.sequence.encoding[ 0 ].hSync, settings.sequence.encoding[ 0 ].vSync );
   printf( "    s3 0x%02X,0x%02X   - set the hSync and vSync of #3 rendered strip" LNFEED, settings.sequence.encoding[ 0 ].hSync, settings.sequence.encoding[ 0 ].vSync );

   printf( "    c  %-5s       - set generate CRC on all strips by default" LNFEED,
           ( 0 == ( strip_crc & settings.sequence.encoding[ 0 ].override ) ) ? "false" : "true" );
   printf( "    c4 %-5s       - set generate CRC on #4 rendered strip" LNFEED,
           ( 0 == ( strip_crc & settings.sequence.encoding[ 0 ].override ) ) ? "false" : "true" );

   prompt( "[Press any key to continue] ", " ", 1, " " );

   printf( LNFEED "    f %s       - set output mode rendered format to { direct, reduce }" LNFEED,
           ( ( direct_render == settings.outputMode ) ? "direct" : "reduce" ) );

   printf( "    %%              - display nearest direct mode device-dependent Sync values" LNFEED );
   printf( "    #              - show bytes per strip with given hSync x vSync value table" LNFEED );

   printf( "    !              - display strip sequence metrics" LNFEED );

   printf( "    @              - display the name and version of the program" LNFEED );

   if ( direct_render != settings.outputMode )
   {
      printf( "    k  %1.3f       - ink spread index, measured in thousands of an inch" LNFEED, settings.pageLayout.inkSpreadIndex );
      printf( "    n  %1.2f        - inverse scaling factor (photo reduction factor 6.0 to 12.0)" LNFEED, settings.pageLayout.reductionFactor );
   }

   printf( "    x %-10" FSTR_UINT32_T "   - set maximum byte limit for an encoding sequence" LNFEED,
           settings.sequence.limitMaxSequenceBytes );

   printf( "    z %1.2f         - set aperture size limit of optical reader in inches" LNFEED LNFEED,
           settings.pageLayout.limitApertureSizeInches );


   printf( "    m              - display all the published media settings" LNFEED );

   printf( "    m %-7s      - set published media type to { letter, legal, tabloid }" LNFEED, pageName[ settings.media.lastPublishSelection ]  );

   printf( "    m.height %1.2f - set published media height" LNFEED, settings.media.publish.height );
   printf( "    m.width   %1.2f - set published media width" LNFEED, settings.media.publish.width );
   printf( "    m.tmar    %1.2f - set published media top margin" LNFEED, settings.media.publish.margin.top );
   printf( "    m.bmar    %1.2f - set published media bottom margin" LNFEED, settings.media.publish.margin.bottom );

   printf( "    m.lmar    %1.2f - set published media left margin" LNFEED, settings.media.publish.margin.left );
   printf( "    m.rmar    %1.2f - set published media right margin" LNFEED, settings.media.publish.margin.right );

   prompt( "[Press any key to continue] ", " ", 1, " " );

   printf( LNFEED "    p              - display all the print media settings" LNFEED );

   printf( "    p %-7s      - set print media to { letter, legal, tabloid }" LNFEED, pageName[ settings.media.lastPrintSelection ]  );

   printf( "    p.height %1.2f - set print media height" LNFEED, settings.media.print.height );
   printf( "    p.width   %1.2f - set print media width" LNFEED, settings.media.print.width );
   printf( "    p.tmar    %1.2f - set print media top margin" LNFEED, settings.media.print.margin.top );
   printf( "    p.bmar    %1.2f - set print media bottom margin" LNFEED, settings.media.print.margin.bottom );

   printf( "    p.lmar    %1.2f - set print media left margin" LNFEED, settings.media.print.margin.left );
   printf( "    p.rmar    %1.2f - set print media right margin" LNFEED, settings.media.print.margin.right );

   printf( "    p.feed %s - set printer feed format to { sheet, continuous }" LNFEED,
           ( ( sheet == settings.media.print.feed ) ? "sheet  " : "continuous" ) );

   printf( "    p.dots %" FSTR_UINT16_T ",%" FSTR_UINT16_T
           " - set printer horizontal and vertical resolution (dots/area)" LNFEED,
           settings.media.print.dots.horizontal, settings.media.print.dots.vertical );

   printf( LNFEED "Output Mode: %s" LNFEED, OutputModeTypeStr[ settings.outputMode ] );

   printf( "Working Directory = %s", settings.workingDirectory );
}


bool displayEditorHelp( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      displayEditorCommands();

      return true;
   }

   return false;
}


bool appendFilesToSequence( bool isCmdLine, char* inputStr )
{
   char* index = ltrim( inputStr + 1, hzWSpace, hzWSpaceCount );

   uint16_t originalCount = CZFList_itemCount();

   if ( false == isCmdLine )
   {
      if ( false == appendInputFiles( index, NULL, settings.searchRecursively ) )
      {
         fprintf( stderr, "   couldn't find anything matching: '%s'", index );
      }
      else if ( 1 < ( CZFList_itemCount() - originalCount ) )
      {
         printf( "   appended %" FSTR_UINT16_T " files to encode sequence" LNFEED, CZFList_itemCount() - originalCount );

         displayStripSequenceContent();
      }
      else if ( 1 == ( CZFList_itemCount() - originalCount ) )
      {
         printf( "   appended file to encode sequence" LNFEED );

         displayStripSequenceContent();
      }
      else
      {
         printf( "   couldn't find any matching files to append" LNFEED );
      }

      return true;
   }

   return false;
}


bool insertFilesIntoSequence( bool isCmdLine, char* inputStr )
{
   uint16_t originalCount = CZFList_itemCount();

   uint16_t offset;

   char* index = inputStr + 1;
   char* mark;

   uint16_t* insertOffset = NULL;

   if ( false == isCmdLine )
   {
      offset = ( uint16_t )strtoul( index, &mark, 10 );

      if ( mark != index )
      {
         index = ltrim( mark, hzWSpace, hzWSpaceCount );

         if ( offset > 0 )
         {
            // if offset in correct range insertBefore, else append
            if ( ( false == CZFList_isEmpty() ) && ( offset <= CZFList_itemCount() ) )
            {
               --offset;

               insertOffset = &offset;
            }

            if ( false == appendInputFiles( index, insertOffset, settings.searchRecursively ) )
            {
               fprintf( stderr, "   couldn't find anything matching: '%s'", index );
            }
            else
            {
               if ( 1 < ( CZFList_itemCount() - originalCount ) )
               {
                  printf( "   inserted %" FSTR_UINT16_T " files to encode sequence." LNFEED, CZFList_itemCount() - originalCount );
               }
               else
               {
                  printf( "   inserted file to encode sequence." LNFEED );
               }

               displayStripSequenceContent();
            }
         }
         else
         {
            // out of range error
         }
      }
      else
      {
         // offset not a number
      }

      return true;
   }

   return false;
}


bool removeFilesFromSequence( bool isCmdLine, char* inputStr )
{
   uint16_t offset;

   uint16_t start;
   uint16_t end;
   uint16_t swap;

   uint16_t i;

   struct InputFileType* item;

   char* index = inputStr + 1;
   char* mark;

   if ( false == isCmdLine )
   {
      if ( CZFList_itemCount() > 0 )
      {
         offset = ( uint16_t )strtoul( index, &mark, 10 );

         if ( ( offset > 0 ) && ( mark != index ) )
         {
            start = offset;
            --start;

            if ( '\0' == *mark )
            {
               if ( CZFList_removeFrom( start, &item ) )
               {
                  printf( "   removed item %" FSTR_UINT16_T " with name: %s", offset, item->name );

                  if ( NULL != item->entry.adjunct )
                  {
                     free( item->entry.adjunct );
                  }

                  free( item );
               }
               else
               {
                  printf( "   couldn't find file# %" FSTR_UINT16_T " to delete from sequence.", offset );
               }
            }
            else if ( ',' == *mark )
            {
               index = mark + 1;

               mark = index;

               offset = ( uint16_t )strtoul( index, &mark, 10 );

               if ( ( offset > 0 ) && ( index != mark ) )
               {
                  end = offset;
                  --end;

                  if ( ( start < CZFList_itemCount() ) && ( end < CZFList_itemCount() ) )
                  {
                     if ( end < start )
                     {
                        swap = start;
                        start = end;
                        end = swap;
                     }

                     for ( i = 0; i < end - start + 1; i++ )
                     {
                        if ( CZFList_removeFrom( start, &item ) )
                        {
                           if ( 0 != i )
                           {
                              fputs( LNFEED, stdout );
                           }

                           printf( "   removed item %" FSTR_UINT16_T " with name: %s", ( start + i + 1 ), item->name );

                           if ( NULL != item->entry.adjunct )
                           {
                              free( item->entry.adjunct );
                           }

                           free( item );
                        }
                        else
                        {
                           printf( "   couldn't delete file# %" FSTR_UINT16_T " from sequence.", ( start + i + 1 ) );
                        }
                     }
                  }
                  else
                  {
                     printf( "   file number must be in valid 1 - %" FSTR_UINT16_T " range to delete.", CZFList_itemCount() );
                  }
               }
               else
               {
                  printf(  "   couldn't delete file# '%s' from sequence.", index );
               }
            }
            else
            {
               printf(  "   didn't understand command: %s", inputStr );
            }
         }
         else
         {
            printf( "   couldn't delete file# '%s' from sequence.", index );
         }
      }
      else
      {
         printf( "   there are no files in sequence to delete." );
      }

      return true;
   }

   return false;
}


bool sortFilesByPathFilename( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      CZFList_sort( CZFListDataType_pathLessThan );

      fputs( "   sorted files by path/filename.ext" LNFEED, stdout );

      return true;
   }

   return false;
}


bool listStripSequenceContent( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      if ( false == CZFList_isEmpty() )
      {
         displayStripSequenceContent();
      }
      else
      {
         fputs( "   strip sequence is empty, add some files to get started", stdout );
      }

      return true;
   }

   return false;
}


bool writeStripSequenceContent( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      renderStripSequence();

      return true;
   }

   return false;
}


bool quitProgram( bool isCmdLine, char* inputStr )
{
   if ( false == isCmdLine )
   {
      return true;
   }

   return false;
}


enum CMDLineResultType stripSequenceEditor()
{
   enum CMDLineResultType result = cmdln_completed;

   char buffer[ 1024 ] = { 0 };

   int bufferSize = sizeof( buffer ) / sizeof( buffer[ 0 ] ) - 1;

   char* input;

   size_t nameLen;
   size_t aliasLen;

   uint16_t k;

   printf( LNFEED "> " );

   if ( NULL != fgets( buffer, bufferSize, stdin ) )
   {
      input = trim( buffer, whitespace, whitespaceCount );

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

         if ( ( 0 != aliasLen ) && ( 0 == strnicmp( command[ k ].alias, input, aliasLen ) ) )
         {
            if ( isspace( input[ aliasLen ] )
                 || ( '\0' == input[ aliasLen ] ) )
            {
               if ( command[ k ].cmdHandler( false, input )
                  && ( command[ k ].result < result ) )
               {
                  result = command[ k ].result;

                  break;
               }
            }
            else if ( ( '#' == command[ k ].alias[ aliasLen ] )
                      && ( '0' <= input[ aliasLen ] )
                      && ( '9' >= input[ aliasLen ] ) )
            {
               if ( command[ k ].cmdHandler( false, input )
                    && ( command[ k ].result < result ) )
               {
                  result = command[ k ].result;

                  break;
               }
            }
         }
         else if ( 0 == strnicmp( command[ k ].name, input, nameLen ) )
         {
            if ( isspace( input[ nameLen ] )
                 || ( '\0' == input[ nameLen ] ) )
            {
               if ( command[ k ].cmdHandler( false, input )
                    && ( command[ k ].result < result ) )
               {
                  result = command[ k ].result;

                  break;
               }
            }
            else if ( ( '#' == command[ k ].name[ nameLen ] )
                      && ( '0' <= input[ nameLen ] )
                      && ( '9' >= input[ nameLen ] ) )
            {
               if ( command[ k ].cmdHandler( false, input )
                    && ( command[ k ].result < result ) )
               {
                  result = command[ k ].result;

                  break;
               }
            }
         }
      }
   }

   return result;
}
