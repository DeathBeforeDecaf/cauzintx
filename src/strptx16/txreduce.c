
#include "txreduce.h"

/// TXReduce.c : Render bitfields scaled for photographic reduction, with given
///              ink spread index into postscript for continuous roll printing
///////////////////////////////////////////////////////////////////////////////

// Table of density entries enumerating byte counts of HSync/VSync selection
// per col increasing from HSync 0x04 to 0x0C (4-12 nibbles across)
// per row decreasing from VSync 0xFF to 0x20 (min: 0x2R0 x 0.0025" line height)

struct DataDensityEntry density[ ( 256 - 32 ) * 9 ] = { 0 };

const uint16_t densityCount = sizeof( density ) / sizeof( density[ 0 ] );


// hSync 0x04 to 0x0C (4 to 12 nibbles/2 to 6 bytes) per line
//    Strip widths range from 0.620" to 0.668"
//    0x04  // 4 encoded nibbles (ultra-low density)
//    0x06  // 6 encoded nibbles (low density)
//    0x08  // 8 encoded nibbles (medium density)
//    0x0A  // 10 encoded nibbles (high density)
//    0x0C  // 12 encoded nibbles (ultra-high density)

// vSync 0x01 = 0.00015625" = 5 / 32000" (microstep)
// vSync 0x10 =     0.0025" =   1 / 400" (step)
// vSync 0x40 =     0.0100" =   1 / 100" (low density)
// vSync 0x60 =     0.0150" =   3 / 200" (medium density)
// vSync 0x80 =     0.0200" =   1 /  50" (high density)

void populateDataDensityTable( struct StripLayoutType dfltStrip )
{
   double scanLineHeight;

   uint16_t colOffset;
   uint16_t nibbleCount;

   uint16_t stripLineCount;
   uint16_t byteCount;

   uint16_t watermarkedStripLineCount;
   uint16_t watermarkedByteCount;

   uint16_t index;

   uint16_t vSync = 256;

   for ( index = 0; index < densityCount; index++ )
   {
      colOffset = ( index % 9 );

      if ( 0 == colOffset )
      {
         vSync--;

         scanLineHeight = vSync * 5.0 / 32000.0;
      }

      nibbleCount = 4 + colOffset;

      stripLineCount =
         ( uint16_t )( dfltStrip.apparentHeight / scanLineHeight );

      // discard the last odd nibble (half-byte strip size not allowed :^)
      if ( ( stripLineCount * nibbleCount ) & 0x0001 )
      {
         stripLineCount--;
      }

      watermarkedStripLineCount =
         ( uint16_t )( dfltStrip.apparentWatermarkedHeight / scanLineHeight );

      if ( ( watermarkedStripLineCount * nibbleCount ) & 0x0001 )
      {
         watermarkedStripLineCount--;
      }

      byteCount = ( ( stripLineCount * nibbleCount ) / 2 );

      watermarkedByteCount = ( ( watermarkedStripLineCount * nibbleCount ) / 2 );

      density[ index ].hsync = ( uint8_t )nibbleCount;
      density[ index ].vsync = ( uint8_t )vSync;
      density[ index ].lineCountPerStrip = stripLineCount;
      density[ index ].lineCountPerWatermarkedStrip = watermarkedStripLineCount;
      density[ index ].byteCountPerStrip = byteCount;
      density[ index ].byteCountPerWatermarkedStrip = watermarkedByteCount;
      density[ index ].bitWidth =
         ( double )dfltStrip.apparentWidth / ( ( nibbleCount << 3 ) + 14 ); // bitsPerLine

      density[ index ].bitHeight = scanLineHeight;
   }
}


// According to the StripWare Stripper manual:
//    A high density strip stores 1065 bytes in each strip
//    A medium density strip stores 844 bytes in each strip

void displayDataDensityTable()
{
   uint16_t stepCount;
   uint16_t remainder;

   uint16_t colOffset;

   uint16_t index;

   uint16_t lowerLimit;
   uint16_t upperLimit;

   uint16_t vSync = 256;

   calculateStripHeight( &settings.stripLayout, settings.media );

   populateDataDensityTable( settings.stripLayout );

   printf( "   Apparent Strip Width = %f", settings.stripLayout.apparentWidth );
   printf( "   Apparent Strip Length = %f" LNFEED, settings.stripLayout.apparentHeight );

   lowerLimit = ( 0x00FF - 0x009F ) * 9 - 1;  // 863
   upperLimit = densityCount - ( 9 * 16 );    // 1872

   for ( index = 0; index < densityCount; index++ )
   {
      colOffset = ( index % 9 );

      if ( 0 == colOffset )
      {
         vSync--;

         stepCount = vSync >> 4;
         remainder = ( vSync & 0x0F );
      }

      if ( ( 0x0F == remainder ) && ( 0 == colOffset ) && ( index > lowerLimit ) && ( index < upperLimit ) )
      {
         printf( LNFEED );

         if ( ( index > 1727 ) || ( index < 1007 ) ) // vsync from >=0x8F to <=0x40
         {
            printf( "            /-x-x-x-x-x-x-x-x-x--- HSync ---x-x-x-x-x-x-x-x-x-\\" LNFEED );
         }
         else
         {
            printf( "            /--------------------- HSync ---------------------\\" LNFEED );
         }

         printf( "         4      5      6      7      8      9     10     11     12" LNFEED );
         printf( "VSync  -----  -----  -----  -----  -----  -----  -----  -----  -----" );
      }

      if ( ( 0 == colOffset ) && ( index > lowerLimit ) && ( index < upperLimit ) )
      {
         printf( LNFEED " %Xr%X ", stepCount, remainder );
      }

      if ( ( index > lowerLimit ) && ( index < upperLimit ) )
      {
         printf( " % 6u", density[ index ].byteCountPerStrip );
      }

      if ( ( 0 == ( ( index + 1 ) % 144 ) ) && ( index > lowerLimit ) && ( ( index + 1 ) < upperLimit ) )
      {
         fflush( stdout );

         prompt( LNFEED "[Press any key to continue] ", " ", 1, " " );
      }
   }
}
