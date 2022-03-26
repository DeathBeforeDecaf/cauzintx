#include "platform.h"
#include "settings.h"
#include "txdirect.h"
#include "txreduce.h"

/// TXDirect.c : Render bitfields directly in integral, device-dependent pixel
///              resolution printing postscript bitmaps on individual sheets
///////////////////////////////////////////////////////////////////////////////

extern struct SystemSettings settings;

extern struct PublishedPageType publishStandard[];
extern struct PrintedPageType printStandard[];

struct DevicePixelationRow pixelTable[ 6 ] = { 0 }; // 300, 600, 1200, 2400, 4800, 9600

uint8_t pixelRowCount = sizeof( pixelTable ) / sizeof( pixelTable[ 0 ] );


// populate Device-Dependent Pixel Table
void initializeDeviceDependentPixelTable()
{
   uint8_t dpi = 0;
   uint8_t hSync = 0;
   uint8_t vSync = 0;

   uint16_t nearestMax;
   uint16_t nearestMin;

   float minWidthDotRatio;
   float maxWidthDotRatio;

   float heightDotRatio;

   uint32_t dotHeight;

   settings.media.publish = publishStandard[ 0 ];
   settings.media.print = printStandard[ 0 ];

   settings.sequence.encoding[ 0 ].hSync = 0x06;
   settings.sequence.encoding[ 0 ].vSync = 0x60;

   // populate imperialHSyncTable
   for ( dpi = 0; dpi < sizeof( pixelTable ) / sizeof( pixelTable[ 0 ] ); dpi++ ) // 300 to 9600 dpi
   {
      pixelTable[ dpi ].units = dots_per_inch;

      pixelTable[ dpi ].resolution = 300U << dpi;

      for ( hSync = 0; hSync < 9; hSync++ )  // 4 to 12 nibbles per scan line
      {
         minWidthDotRatio =
            settings.stripLayout.MIN_PUBLISHED_WIDTH * ( float )( pixelTable[ dpi ].resolution )
            / ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine

         maxWidthDotRatio =
            settings.stripLayout.MAX_PUBLISHED_WIDTH * ( float )( pixelTable[ dpi ].resolution )
            / ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine

         nearestMax = ( uint16_t )floor( ( double )maxWidthDotRatio );

         if ( ( nearestMax > 1 ) && ( minWidthDotRatio <= ( float )nearestMax ) )
         {
            pixelTable[ dpi ].hPixel[ hSync ].maxDotsPerHPixel = nearestMax;
         }

         nearestMin = ( uint16_t )ceil( ( double )minWidthDotRatio );

         if ( ( 1 < nearestMin ) && ( ( float )nearestMin <= maxWidthDotRatio ) )
         {
            pixelTable[ dpi ].hPixel[ hSync ].minDotsPerHPixel = nearestMin;
         }

         if ( 1 < nearestMin )
         {
            if ( nearestMin < nearestMax )
            {
               if ( 1 < ( nearestMax - nearestMin ) )
               {
                  pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel = ( nearestMax + nearestMin ) / 2;

                  pixelTable[ dpi ].hPixel[ hSync ].bitWidth =
                     pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel / ( float )( pixelTable[ dpi ].resolution );

                  pixelTable[ dpi ].hPixel[ hSync ].stripWidth =
                     pixelTable[ dpi ].hPixel[ hSync ].bitWidth
                     * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine
               }
               else // check difference between calculated strip width and min/max limits
               {
                  if ( ( settings.stripLayout.MAX_PUBLISHED_WIDTH -
                         ( float )nearestMax
                           / ( float )( pixelTable[ dpi ].resolution )
                           * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ) ) // bitsPerLine
                       > ( ( float )nearestMin
                             / ( float )( pixelTable[ dpi ].resolution )
                             * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ) // bitsPerLine
                           - settings.stripLayout.MIN_PUBLISHED_WIDTH ) )
                  {
                     pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel = nearestMax;

                     pixelTable[ dpi ].hPixel[ hSync ].bitWidth =
                        nearestMax / ( float )( pixelTable[ dpi ].resolution );

                     pixelTable[ dpi ].hPixel[ hSync ].stripWidth =
                        pixelTable[ dpi ].hPixel[ hSync ].bitWidth
                        * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine
                  }
                  else
                  {
                     pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel = nearestMin;

                     pixelTable[ dpi ].hPixel[ hSync ].bitWidth =
                        nearestMin / ( float )( pixelTable[ dpi ].resolution );

                     pixelTable[ dpi ].hPixel[ hSync ].stripWidth =
                        pixelTable[ dpi ].hPixel[ hSync ].bitWidth
                        * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine
                  }
               }
            }
            else if ( 0 != pixelTable[ dpi ].hPixel[ hSync ].maxDotsPerHPixel )
            {
               pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel = nearestMax;

               pixelTable[ dpi ].hPixel[ hSync ].bitWidth =
                  nearestMax / ( float )( pixelTable[ dpi ].resolution );

               pixelTable[ dpi ].hPixel[ hSync ].stripWidth =
                  pixelTable[ dpi ].hPixel[ hSync ].bitWidth
                  * ( float )( ( ( hSync + 4 ) << 3 ) + 14 ); // bitsPerLine
            }
         }
      }

      for ( vSync = 0; vSync < 8; vSync++ )
      {
         // for an ideal device 0x30 = 0.0075" = 3/400, 0x40 : 0.0100" = 4/400, etc
         dotHeight = ( ( 0x30 + vSync * 0x10 ) / 0x10 );
         dotHeight *= pixelTable[ dpi ].resolution;

         // span vertical pixels with whole dots only (device-dependent)
         heightDotRatio = ( float )ceil( ( double )( dotHeight ) / 400.0 );

         // ignore if resolution is low and device dot blurs into the next larger size
         if ( ( heightDotRatio / ( float )pixelTable[ dpi ].resolution ) <
              ( ( float )( ( 0x30 + ( vSync + 1 ) * 0x10 ) / 0x10 ) / 400.0f ) )
         {
            pixelTable[ dpi ].vPixel[ vSync ].dotsPerVPixel = ( uint16_t )heightDotRatio;

            pixelTable[ dpi ].vPixel[ vSync ].bitHeight =
               heightDotRatio / ( float )pixelTable[ dpi ].resolution;

            pixelTable[ dpi ].vPixel[ vSync ].vSync =
               ( uint8_t )floor( heightDotRatio * 32000.0f / ( 5.0f * ( float )pixelTable[ dpi ].resolution ) + 0.5f );

            // prevent rounding up to the next higher vSync if on boundary (paranoia:)
            if ( ( ( 0x30 + vSync * 0x10 ) / 0x10 ) < ( pixelTable[ dpi ].vPixel[ vSync ].vSync / 0x10 ) )
            {
               pixelTable[ dpi ].vPixel[ vSync ].vSync =
                  ( uint8_t )floor( heightDotRatio * 32000.0f / ( 5.0f * ( float )pixelTable[ dpi ].resolution ) );
            }
         }
      }
   }
}


void nearestDeviceDependentResolution( uint16_t xDPI, uint16_t yDPI )
{
   uint8_t dpi = 0;
   uint8_t hSync = 0;
   uint8_t vSync;

   printf( "Nearest Device-Dependent Resolution (Native %ux%u Render)" LNFEED, xDPI, yDPI );

   if ( ( xDPI > 0 ) && ( 0 == ( xDPI % 300U ) ) )
   {
      switch ( xDPI / 300U )
      {
         case  1: dpi = 0;
            break;
         case  2: dpi = 1;
            break;
         case  4: dpi = 2;
            break;
         case  8: dpi = 3;
            break;
         case 16: dpi = 4;
            break;
         case 32: dpi = 5;
            break;

         default:
            break;
      };

      printf( LNFEED "dotsPerUnitWidth = %u" LNFEED, pixelTable[ dpi ].resolution );

      hSync = 9;

      do
      {
         --hSync;

         if ( 0 != pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel )
         {
            if ( pixelTable[ dpi ].hPixel[ hSync ].minDotsPerHPixel == pixelTable[ dpi ].hPixel[ hSync ].maxDotsPerHPixel )
            {
               printf( "   hSync = 0x%02X, stripWidth = %f @ %u dPP (%3.6f)" LNFEED,
                       hSync + 4,
                       pixelTable[ dpi ].hPixel[ hSync ].stripWidth,
                       pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel,
                       pixelTable[ dpi ].hPixel[ hSync ].bitWidth );
            }
            else
            {
               printf( "   hSync = 0x%02X, stripWidth = %f @ %u dPP (%3.6f): min(%u), max(%u)" LNFEED,
                       hSync + 4,
                       pixelTable[ dpi ].hPixel[ hSync ].stripWidth,
                       pixelTable[ dpi ].hPixel[ hSync ].dotsPerHPixel,
                       pixelTable[ dpi ].hPixel[ hSync ].bitWidth,
                       pixelTable[ dpi ].hPixel[ hSync ].minDotsPerHPixel,
                       pixelTable[ dpi ].hPixel[ hSync ].maxDotsPerHPixel );
            }
         }
      }
      while ( hSync > 0 );
   }

   if ( ( yDPI > 0 ) && ( 0 == ( yDPI % 300U ) ) )
   {
      switch ( yDPI / 300U )
      {
         case  1: dpi = 0;
            break;
         case  2: dpi = 1;
            break;
         case  4: dpi = 2;
            break;
         case  8: dpi = 3;
            break;
         case 16: dpi = 4;
            break;
         case 32: dpi = 5;
            break;

         default:
            break;
      };

      printf( LNFEED "dotsPerUnitHeight = %u" LNFEED, yDPI );

      for ( vSync = 0; vSync < 8; vSync++ )
      {
         if ( 0 != pixelTable[ dpi ].vPixel[ vSync ].dotsPerVPixel )
         {
               printf( "   vSync = 0x%02X @ %u dPP (%3.6f)" LNFEED,
                       pixelTable[ dpi ].vPixel[ vSync ].vSync,
                       pixelTable[ dpi ].vPixel[ vSync ].dotsPerVPixel,
                       pixelTable[ dpi ].vPixel[ vSync ].bitHeight );
         }
      }
   }
}


bool generateDirectPostscriptOutput( struct MetaStripType* lStrip, uint8_t stripNumber, uint8_t stripCount )
{
   bool result = true;

   uint16_t i;
   uint16_t j;

   uint8_t pageNumber;
   uint8_t pageCount;

   uint8_t imgByte;
   uint8_t bitOut;

   uint8_t vSync;
   uint8_t hSync;

   struct DataDensityEntry density = { 0 };

   uint16_t hShift = 0;
   uint16_t dots_per_inch =
      ( settings.media.print.dots.horizontal <= settings.media.print.dots.vertical )
      ? settings.media.print.dots.horizontal : settings.media.print.dots.vertical;

   // bounding box for view
   uint16_t left = ( uint16_t )( 72U * settings.media.publish.margin.left );
   uint16_t right =
      ( uint16_t )( 72U * settings.media.publish.width ) - ( uint16_t )( 72U * settings.media.publish.margin.right );
   uint16_t top =
      ( uint16_t )( 72U * settings.media.publish.height ) - ( uint16_t )( 72U * settings.media.publish.margin.top );
   uint16_t bottom = ( uint16_t )( 72U * settings.media.publish.margin.bottom );

   struct DataStripType* dStrip;

   struct BitField32Type* field = NULL;

   FILE* output = NULL;

   uint16_t strips_per_page =
      1U + ( right - left - settings.pageLayout.FIRST_STRIP_WIDTH_POINTS )
           / settings.pageLayout.NEXT_STRIP_WIDTH_POINTS;

   uint32_t rawBytes;

   time_t epochDeltaT;
   struct tm* now;

   if ( 1 < stripNumber )
   {
      dStrip = lStrip->nextStrip;

      if ( NULL != dStrip )
      {
         for ( i = 2; i < stripNumber; i++ )
         {
            if ( NULL != dStrip->nextStrip )
            {
               dStrip = dStrip->nextStrip;
            }
            else
            {
               result = false;

               return result;
            }
         }

         field = dStrip->field;
         vSync = dStrip->vSync;
         hSync = dStrip->hSync;
      }
      else
      {
         result = false;

         return result;
      }
   }
   else
   {
      field = lStrip->field;
      vSync = lStrip->vSync;
      hSync = lStrip->hSync;
   }

   density = getDataDensity( hSync, vSync );

   BitField32_not( field );

   if ( 1 != stripNumber )
   {
      // open the existing output file for concatenation
      if ( NULL == ( output = fopen( settings.sequence.outputFilename, "ab" ) ) )
      {
         result = false;
      }
   }
   else
   {
      // create the output file (truncate if exists)

      if ( NULL != ( output = fopen( settings.sequence.outputFilename, "wb" ) ) )
      {
         pageCount = stripCount / ( uint8_t )strips_per_page;

         if ( 0 != ( stripCount % strips_per_page ) )
         {
            pageCount++;
         }

         // prepend the postscript header
         fputs( "%!PS-Adobe-3.0\n", output );

         fprintf( output, "%%%%Pages: %u\n", pageCount );

         fputs( "%%DocumentData: Clean7Bit\n", output );

         fprintf( output, "%%%%Creator: The New Laser Archivizt v0.0.1\n" );
         fprintf( output, "%%%%Title: (%s)\n", settings.sequence.outputFilename );

         time( &epochDeltaT );

         if ( NULL != ( now = localtime( &epochDeltaT ) ) )
         {
            fprintf( output, "%%%%CreationDate: (%04d-%02d-%02dT%02d:%02d:%02d)\n",
                     1900 + now->tm_year, 1 + now->tm_mon, now->tm_mday,
                     now->tm_hour, now->tm_min, now->tm_sec );
         }

         fputs( "%%EndComments\n", output );
         fputs( "%%BeginDefaults\n", output );
         fputs( "%%PageResources: font Courier\n", output );
         fputs( "%%PageMedia: Letter\n", output );
         fputs( "%%PageOrientation: Portrait\n", output );
         fputs( "%%EndDefaults\n", output );
         fputs( "%%BeginProlog\n", output );
         fputs( "%%EndProlog\n", output );
         fputs( "%%BeginSetup\n", output );

         fputs( "/nearest\n", output );
         fputs( "{\n   transform\n", output );
         fputs( "   round .25 add exch\n", output );
         fputs( "   round .25 add exch\n", output );
         fputs( "   itransform\n", output );
         fputs( "} bind def\n", output );

         fputs( "/isColor          " \
                "% from Adobe Color Separation Conventions for PostScript Language Programs, EX4\n", output );
         fputs( "{\n", output );
         fputs( "   statusdict /processcolors known\n", output );
         fputs( "   {\n", output );
         fputs( "      statusdict /processcolors get exec\n", output );
         fputs( "   }\n", output );
         fputs( "   {\n", output );
         fputs( "      /deviceinfo where\n", output );
         fputs( "      {\n", output );
         fputs( "         pop deviceinfo /Colors known\n", output );
         fputs( "         {\n", output );
         fputs( "            deviceinfo /Colors get\n", output );
         fputs( "            statusdict /processcolors\n", output );
         fputs( "            {\n", output );
         fputs( "               deviceinfo /Colors known\n", output );
         fputs( "               {\n", output );
         fputs( "                  deviceinfo /Colors get\n", output );
         fputs( "               }\n", output );
         fputs( "               { 1 }\n", output );
         fputs( "               ifelse\n", output );
         fputs( "            }\n", output );
         fputs( "            put\n", output );
         fputs( "         }\n", output );
         fputs( "         { 1 }\n", output );
         fputs( "         ifelse\n", output );
         fputs( "      }\n", output );
         fputs( "      { 1 }\n", output );
         fputs( "      ifelse\n", output );
         fputs( "   }\n", output );
         fputs( "   ifelse\n", output );
         fputs( "   1 gt\n", output );
         fputs( "} bind def\n", output );

         fputs( "%%EndSetup\n", output );
      }
      else
      {
         result = false;
      }
   }

   pageNumber = 1 + ( stripNumber - 1 ) / ( uint8_t )strips_per_page;

   if ( NULL != output )
   {
      if ( 1 == ( stripNumber % strips_per_page ) )
      {
         fprintf( output, "\n%%%%Page: %u %u\n", pageNumber, pageNumber );

         fputs( "%%BeginPageSetup\n", output );
         fputs( "/pgsave save def\n", output );
         fprintf( output, "72 %u div setlinewidth\n\n", dots_per_inch );

         // emit output image w/o halftone @ DPI freq 0 angle
         fputs( "isColor\n", output );
         fputs( "{\n", output );
         fputs( "   currentcolorscreen\n", output );
         fputs( "   exch pop exch pop\n", output );
         fprintf( output, "   %u exch 0 exch\n", dots_per_inch );
         fputs( "   setcolorscreen\n", output );
         fputs( "}\n", output );
         fputs( "{\n", output );
         fputs( "   currentscreen\n", output );
         fputs( "   exch pop exch pop\n", output );
         fprintf( output, "   %u exch 0 exch\n", dots_per_inch );
         fputs( "   setscreen\n", output );
         fputs( "}\n", output );
         fputs( "ifelse\n", output );

         fputs( "%%EndPageSetup\n\n", output );


         fprintf( output, "/Courier findfont\n" );
         fprintf( output, "10 scalefont\n" );
         fprintf( output, "setfont\n" );
      }
      else
      {
         hShift = settings.pageLayout.NEXT_STRIP_WIDTH_POINTS * ( ( stripNumber - 1 ) % strips_per_page );
      }

      if ( 1 != stripNumber )
      {
         fprintf( output, "\n%% strip #%u\n", stripNumber );

         fprintf( output, "gsave\n" );
         fprintf( output, "   90 rotate\n" );
         fprintf( output, "   %u -%u moveto\n", bottom, 9 + left + hShift );

         if ( stripNumber < 10 )
         {
            fprintf( output, "   ( %u.) show\n", stripNumber );
         }
         else if ( stripNumber < 100 )
         {
            fprintf( output, "   (%u.) show\n", stripNumber );
         }
         else // 100+
         {
            fprintf( output, "   (%u) show\n", stripNumber );
         }
      }
      else
      {
         fprintf( output, "\n%% strip #%u and description\n", stripNumber );

         fprintf( output, "gsave\n" );
         fprintf( output, "   90 rotate\n" );
         fprintf( output, "   %u -%u moveto\n", bottom, 9 + left );

         if ( NULL != settings.sequence.verboseDescription )
         {
            fprintf( output, "   ( %u. %s) show\n", stripNumber, settings.sequence.verboseDescription );
         }
         else
         {
            fprintf( output, "   ( %u. ) show\n", stripNumber );
         }
      }

      fprintf( output, "grestore\n\n" );

      fputs( "% lower fiducial (tick mark)\n", output );

      fprintf( output, "72 300 div setlinewidth\n" );

      fprintf( output, "%2.1f %u nearest moveto\n", 11.0f + left + hShift, bottom );

      fprintf( output, "0 72 600 div rmoveto\n\n" );

      fprintf( output, "gsave\n" );
      fprintf( output, "   0 18 72 300 div sub rlineto\n" );
      fprintf( output, "   3 72 600 div sub 0 rlineto\n" );
      fprintf( output, "   0 18 72 300 div sub neg rlineto\n" );
      fprintf( output, "   closepath stroke\n" );
      fprintf( output, "grestore\n" );

      fprintf( output, "gsave\n" );
      fprintf( output, "   72 150 div 0 rmoveto\n" );
      fprintf( output, "   0 15 72 600 div sub rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   144 150 div 0 rmoveto\n" );
      fprintf( output, "   0 11 72 600 div sub rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   216 150 div 0 rmoveto\n" );
      fprintf( output, "   0 7 72 600 div sub rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   288 150 div 0 rmoveto\n" );
      fprintf( output, "   0 3 72 600 div sub rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );

      fprintf( output, "gsave\n" );
      fprintf( output, "   144 150 div 18 72 600 div sub rmoveto\n" );
      fprintf( output, "   0 3 72 600 div sub neg rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   216 150 div 18 72 600 div sub rmoveto\n" );
      fprintf( output, "   0 7 72 600 div sub neg rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   288 150 div 18 72 600 div sub rmoveto\n" );
      fprintf( output, "   0 11 72 600 div sub neg rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );
      fprintf( output, "gsave\n" );
      fprintf( output, "   360 150 div 18 72 600 div sub rmoveto\n" );
      fprintf( output, "   0 15 72 600 div sub neg rlineto\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "grestore\n" );

      fprintf( output, "72 %u div setlinewidth\n\n", dots_per_inch );

      fprintf( output, "%% upper fiducial (circular registration mark) strip #%u\n", stripNumber );

      fprintf( output, "gsave\n" );
      fprintf( output, "   /Courier findfont\n" );
      fprintf( output, "   5.5 scalefont\n" );
      fprintf( output, "   setfont\n\n" );

      fprintf( output, "   %2.6f %2.4f\n", 13.125f + left + hShift, ( ( float )top ) - 1.15f );

      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.48 22.5 67.5 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.48 112.5 157.5 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.48 202.5 247.5 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.48 292.5 337.5 arc\n" );
      fprintf( output, "   stroke\n" );

      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.96 60 120 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.96 150 210 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.96 240 300 arc\n" );
      fprintf( output, "   stroke\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.96 330 30 arc\n" );
      fprintf( output, "   stroke\n\n" );

      fprintf( output, "   newpath\n" );
      fprintf( output, "   2 copy 5.0 0 360 arc\n" );
      fprintf( output, "   fill\n" );
      fprintf( output, "   1 setgray\n" );
      fprintf( output, "   newpath\n" );
      fprintf( output, "   4.0 0 360 arc\n" );
      fprintf( output, "   fill\n" );
      fprintf( output, "   0 setgray\n" );
      fprintf( output, "   %2.4f %2.4f moveto\n", 9.875f + left + hShift, ( ( float )top ) - 2.9f );

      if ( stripNumber < 10 )
      {
         fprintf( output, "   ( %u) show\n", stripNumber );
      }
      else if ( stripNumber < 100 )
      {
         fprintf( output, "   (%u) show\n", stripNumber );
      }
      else
      {
         fprintf( output, "   (%u) show\n", stripNumber % 100 );
      }

      fprintf( output, "grestore\n\n" );

//    fputs( "% debug strip alignment w/keepout\n", output );
//
//    fprintf( output, "[3] 0 setdash\n" );
//    fprintf( output, "%u %u moveto\n", 107 + left + hShift, bottom );
//    fprintf( output, "%u %2.4f lineto\n", 107 + left + hShift,
//             ( ( float )top ) - 36.0f - field->rowSize * 72.0f * density.bitHeight );
//    fprintf( output, "stroke\n" );
//
//    fprintf( output, "%u %u moveto\n", 107 + left + hShift, top );
//    fprintf( output, "%u %u lineto\n", 107 + left + hShift, top - 15 );
//    fprintf( output, "stroke\n" );
//    fprintf( output, "[] 0 setdash\n\n" );

      fprintf( output, "%% softstrip bitmap strip #%u\n", stripNumber );
      fprintf( output, "gsave\n" );

      fprintf( output, "   /strip%03u %u string def\n", stripNumber, ( field->colSize + 2 ) / 8 );

      fprintf( output, "   %2.6f %2.6f nearest translate\n",
               107.0f - ( field->colSize * 36.0f * density.bitWidth ) + left + hShift,
               ( ( float )top ) - 17.5f - field->rowSize * 72.0f * density.bitHeight );

      fprintf( output, "   %2.6f %2.6f nearest scale\n", density.bitWidth * field->colSize * 72.0f,
               density.bitHeight * field->rowSize * 72.0f );

      fprintf( output, "   %u %u 1 [%u 0 0 -%u 0 %u] { currentfile strip%03u readhexstring pop }\n",
               field->colSize, field->rowSize, field->colSize, field->rowSize, field->rowSize, stripNumber );

      rawBytes = field->colSize / 8;

      if ( 0 != ( field->colSize % 8 ) )
      {
         rawBytes += 1;
      }

      rawBytes *= field->rowSize;

      fprintf( output, "%%%%BeginData: %lu Hex Bytes\n", rawBytes );

      fputs( "image\n", output );

      for ( i = 0; i < field->rowSize; i++ )
      {
         imgByte = 0;

         for ( j = 0; j < field->colSize; j++ )
         {
            if ( BitField32_get( field, i, j, &bitOut ) )
            {
               imgByte |= bitOut;
            }

            if ( 0 != ( ( j + 1 ) % 8 ) )
            {
               imgByte <<= 1;
            }
            else
            {
               fprintf( output, "%02X", imgByte );

               imgByte = 0;
            }
         }

         imgByte <<= 2;
         imgByte |= 7;

         fprintf( output, "%02X\n", imgByte );
      }

      fputs( "%%EndData\n", output );

      fprintf( output, "grestore\n\n" );
   }

   if ( NULL != output )
   {
      if ( stripNumber == stripCount )
      {
         fprintf( output, "\npgsave restore\n" );
         fprintf( output, "showpage\n" );
         fputs( "%%PageTrailer\n", output );

         fputs( "\n%%Trailer\n", output );
         fputs( "%%EOF\n", output );
      }
      else if ( 0 == ( stripNumber % strips_per_page ) )
      {
         fprintf( output, "\npgsave restore\n" );
         fprintf( output, "showpage\n" );
         fputs( "%%PageTrailer\n", output );
      }

      fclose( output );
   }

   return result;
}
