#include "platform.h"
#include "settings.h"
#include "txstrip.h"
#include "txreduce.h"  // struct DataDensityEntry

extern struct SystemSettings settings;

extern struct DevicePixelationRow pixelTable[];

extern struct DataDensityEntry density[];


// Column Density (rendered width of individual bits, including left and right margin content)
//    From *Patent 4,728,783* Column 6, Row 23
//
// 4 encoded nibbles + 1 byte left margin + 1 byte right margin - 2 bits empty space
//    = 4 encoded nibbles * 1 byte / encoded nibble + 2 bytes - 2 bits empty space
//    = 6 bytes * 8 = 48 bits - 2 bits empty space = 46 bits (total) per scan line
//
// Left Margin                                        Right Margin
//   Start Line (2 bits)                                Even Bit Parity (2 bits)
//   Space (1 bit)                                      Space (2 bits)
//   Checkerboard (2 bits)      + 4 to 12 nibbles +     Alignment Guideline (3 bits)
//   Odd Bit Parity(2 bits)                                  Total (7 bits)
//       Total (7 bits)
//
//  LMRGN     LSN=0    MSN=8    LSN=0    MSN=8    RMRGN
// _1101001 10101010 10101001 10101010 10101001 1000110_
// _1100101 10101010 10101001 10101010 10101001 0100111_


struct InputFileType* InputFile_initialize( char* filePath, uint32_t sizeBytes, time_t lastModified )
{
   struct InputFileType* result;

   char* index;

   char* extension;

   uint16_t i;

   size_t length;

   bool hasMatched;

   char* executableExtension[] =
   {
      "BAT",
      "COM",
      "EXE",
   };

   char* binaryExtension[] =
   {
      "BMP",
      "COM",
      "DLL",
      "DWG",
      "EXE",
      "GIF",
      "GZ",
      "ICO",
      "JPG",
      "LZ",
      "MOD",
      "MP3",
      "RAR",
      "TAR",
      "TGZ",
      "TIF",
      "TTF",
      "WAV",
      "ZIP",
   };

   char* textExtension[] =
   {
      "ASM",
      "BAS",
      "BAT",
      "C",
      "CPP",
      "CSS",
      "H",
      "HTM",
      "INI",
      "JS",
      "MD",
      "PAS",
      "SVG",
      "TXT",
      "XML",
   };

   length = strlen( filePath ) + 1;

   length += sizeof( struct InputFileType );

   result = ( struct InputFileType* )malloc( length );

   if ( NULL != result )
   {
      memset( result, 0, length );

      result->sizeBytes = sizeBytes;
      result->entry.length = sizeBytes;

      result->lastModified = lastModified;

      strcpy( result->path, filePath );

      result->name = NULL;
      index = result->path;

      while ( *index )
      {
         if ( ( '\\' == *index ) || ( '/' == *index ) )
         {
            result->name = index;
         }

         ++index;
      }

      if ( NULL != result->name )
      {
         ++result->name;
      }
      else
      {
         result->name = result->path;
      }

      result->entry.name = result->name;

      index = result->name;

      while ( *index && ( '.' != *index ) )
      {
         ++index;
      }

      if ( *index )
      {
         ++index;

         extension = index;

         hasMatched = false;

         for ( i = 0; i < sizeof( executableExtension ) / sizeof( executableExtension[ 0 ] ); i++ )
         {
            if ( 0 == stricmp( executableExtension[ i ], index ) )
            {
               result->entry.terminator = 0xFF;
               result->entry.os_type = cznpc_exe;
               hasMatched = true;

               break;
            }
         }

         if ( false == hasMatched )
         {
            result->entry.os_type = cznpc_other;
         }

         hasMatched = false;

         for ( i = 0; i < sizeof( binaryExtension ) / sizeof( binaryExtension[ 0 ] ); i++ )
         {
            if ( 0 == stricmp( binaryExtension[ i ], index ) )
            {
               result->entry.category = cznfile_binary;
               hasMatched = true;

               break;
            }
         }

         if ( false == hasMatched )
         {
            for ( i = 0; i < sizeof( textExtension ) / sizeof( textExtension[ 0 ] ); i++ )
            {
               if ( 0 == stricmp( textExtension[ i ], index ) )
               {
                  result->entry.category = cznfile_text;
                  hasMatched = true;

                  break;
               }
            }
         }
      }
//    else file has no extension
   }

   return result;
}


struct DataDensityEntry getDataDensity( uint8_t hSync, uint8_t vSync )
{
   uint8_t xResolution;
   uint8_t yResolution;

   uint8_t vSyncIndex;

   uint16_t stripLineCount;
   uint16_t watermarkedStripLineCount;

   struct DataDensityEntry result = { 0 };

   uint16_t index = 9 * ( 0xFF - vSync ) + hSync - 4;

   result = density[ index ];

   // keyhole optimization for Device-Dependent Rendering
   // enables bitmap rendering at the printer resolution
   if ( 1.0f == settings.pageLayout.reductionFactor )
   {
      if ( ( ( settings.media.print.width == settings.media.publish.width ) && ( settings.media.print.height == settings.media.publish.height ) )
           || ( ( settings.media.print.width == settings.media.publish.height ) && ( settings.media.print.height == settings.media.publish.width ) ) )
      {
         if ( ( settings.media.print.margin.top <= settings.media.publish.margin.top )
              && ( settings.media.print.margin.bottom <= settings.media.publish.margin.bottom )
              && ( settings.media.print.margin.left <= settings.media.publish.margin.left )
              && ( settings.media.print.margin.right <= settings.media.publish.margin.right ) )
         {
            if ( ( 0 == ( settings.media.print.dots.horizontal % 300U ) )
                 && ( 0 == ( settings.media.print.dots.vertical % 300U ) ) )
            {
               switch ( settings.media.print.dots.horizontal / 300U )
               {
                  case  1: xResolution = 0;
                     break;
                  case  2: xResolution = 1;
                     break;
                  case  4: xResolution = 2;
                     break;
                  case  8: xResolution = 3;
                     break;
                  case 16: xResolution = 4;
                     break;
                  case 32: xResolution = 5;
                     break;

                  default: xResolution = 0xF0;
                     break;
               };

               switch ( settings.media.print.dots.vertical / 300U )
               {
                  case  1: yResolution = 0;
                     break;
                  case  2: yResolution = 1;
                     break;
                  case  4: yResolution = 2;
                     break;
                  case  8: yResolution = 3;
                     break;
                  case 16: yResolution = 4;
                     break;
                  case 32: yResolution = 5;
                     break;

                  default: yResolution = 0xF0;
                     break;
               };

               if ( ( xResolution < 0xF0 ) && ( yResolution < 0xF0 ) )
               {
                  if ( 0 != pixelTable[ xResolution ].hPixel[ hSync - 4 ].dotsPerHPixel )
                  {
                     for ( vSyncIndex = 0; vSyncIndex < 8; vSyncIndex++ )
                     {
                        if ( ( 0 != pixelTable[ yResolution ].vPixel[ vSyncIndex ].dotsPerVPixel )
                             && ( vSync == pixelTable[ yResolution ].vPixel[ vSyncIndex ].vSync ) )
                        {
                           result.bitWidth = pixelTable[ xResolution ].hPixel[ hSync - 4 ].bitWidth;
                           result.bitHeight = pixelTable[ yResolution ].vPixel[ vSyncIndex ].bitHeight;

                           stripLineCount =
                              ( uint16_t )( settings.stripLayout.apparentHeight / result.bitHeight );

                           watermarkedStripLineCount =
                              ( uint16_t )( settings.stripLayout.apparentWatermarkedHeight / result.bitHeight );

                           // discard the last odd nibble (half-byte strip size not allowed :^)
                           if ( ( stripLineCount * hSync ) & 0x0001 )
                           {
                              stripLineCount--;
                           }

                           if ( ( watermarkedStripLineCount * hSync ) & 0x0001 )
                           {
                              watermarkedStripLineCount--;
                           }

                           result.lineCountPerStrip = stripLineCount;
                           result.lineCountPerWatermarkedStrip = watermarkedStripLineCount;

                           result.byteCountPerStrip = ( stripLineCount * hSync ) / 2;
                           result.byteCountPerWatermarkedStrip = ( watermarkedStripLineCount * hSync ) / 2;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return result;
}


void calculateStripSequenceExtents( uint8_t* stripCount, float* lastStripHeight )
{
   uint16_t i;

   struct InputFileType* item;

   uint8_t result = 0;

   uint8_t hSync;
   uint8_t vSync;
   uint16_t attributes;

   uint16_t lineCount = 0;

   float lastStrip = 0.0f;

   uint32_t totalSizeBytes = 0;
   uint32_t bytesRemaining = 0;

   uint16_t crcByteSize = 0;

   struct DataDensityEntry density = { 0 };

   uint16_t stripByteCount = 0;

   uint16_t metaHeaderSize = 1 + 6 + 1 + 1 + 2 + 1 + 1;

   uint16_t dataStripHeaderSize = 1 + 6 + 1 + 1 + 2;

   for ( i = 0; i < CZFList_itemCount(); i++ )
   {
      if ( CZFList_get( i, &item ) )
      {
         metaHeaderSize += 1 + 1 + 3; // 3.4.13 Cauzin Type + 3.4.14 OS FileType + 3.4.15 File Length
//       metaHeaderSize += strlen( item->entry.name ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
         metaHeaderSize += strlen( item->path ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
         metaHeaderSize += 1; // 3.4.18 BlockExpand (adjunct size)

         if ( NULL != item->entry.adjunct )
         {
            metaHeaderSize += item->entry.adjunct_size;
         }

         totalSizeBytes += item->sizeBytes;
      }
   }

   attributes =
      ( ( settings.sequence.encoding[ 1 ].override & strip_crc )
      || ( settings.sequence.encoding[ 0 ].override & strip_crc ) ) ? 0x8000 : 0x0000;

   if ( 0x8000 & attributes )
   {
      // CRC present
      metaHeaderSize += 2;
   }

   // lookup hSync + vSync
   // find the number of bytes in strip

   hSync =
      ( settings.sequence.encoding[ 1 ].override & strip_hsync )
      ? settings.sequence.encoding[ 1 ].hSync : settings.sequence.encoding[ 0 ].hSync;

   vSync =
      ( settings.sequence.encoding[ 1 ].override & strip_vsync )
      ? settings.sequence.encoding[ 1 ].vSync : settings.sequence.encoding[ 0 ].vSync;

   density = getDataDensity( hSync, vSync );

   // 0 = HSync, VSync (strip prefix)
   // 5 = Data Sync (1), Expansion Bytes (2), Strip Length (2)

   stripByteCount =
      ( 0 == ( settings.sequence.encoding[ 1 ].override & strip_watermark ) )
      ? density.byteCountPerStrip : density.byteCountPerWatermarkedStrip;

   // REQUIRE: all metadata fits in first strip
   if ( ( metaHeaderSize + 5 ) <= stripByteCount )
   {
      bytesRemaining = totalSizeBytes + ( metaHeaderSize + 5 );

      printf( "\n  Meta data consumed %u bytes out of %u meta strip bytes.",
              ( metaHeaderSize + 5 ), stripByteCount );

      // everything fits in first strip?
      if ( bytesRemaining <= stripByteCount )
      {
         lineCount =
            ( 0 != ( bytesRemaining << 1 ) % density.hsync )
            ? ( uint16_t )( 1 + ( bytesRemaining << 1 ) / density.hsync ) : ( uint16_t )( bytesRemaining << 1 ) / density.hsync;

         lineCount +=
            ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

         lineCount +=
            ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );

         bytesRemaining = 0;

         lastStrip = ( float )( density.bitHeight * lineCount );

         result = 1;
      }
      else
      {
         bytesRemaining -= stripByteCount;

         for ( i = 2; ( 0 < bytesRemaining ) && ( i < 255 ); i++ )
         {
            if ( i < 128 )
            {
               hSync =
                  ( settings.sequence.encoding[ i ].override & strip_hsync )
                  ? settings.sequence.encoding[ i ].hSync : settings.sequence.encoding[ 0 ].hSync;
            }
            else
            {
               hSync = settings.sequence.encoding[ 0 ].hSync;
            }

            if ( i < 128 )
            {
               vSync =
                  ( settings.sequence.encoding[ i ].override & strip_vsync )
                  ? settings.sequence.encoding[ i ].vSync : settings.sequence.encoding[ 0 ].vSync;
            }
            else
            {
               vSync = settings.sequence.encoding[ 0 ].vSync;
            }

            density = getDataDensity( hSync, vSync );

            crcByteSize = 0;

            if ( i < 128 )
            {
               attributes =
                  ( ( settings.sequence.encoding[ i ].override & strip_crc )
                  || ( settings.sequence.encoding[ 0 ].override & strip_crc ) )
                  ? 0x8000 : 0x0000;
            }
            else
            {
               attributes = ( settings.sequence.encoding[ 0 ].override & strip_crc ) ? 0x8000 : 0x0000;
            }

            if ( 0x8000 & attributes )
            {
               // CRC present
               crcByteSize += 2;
            }

            if ( i < 128 )
            {
               stripByteCount =
                  ( 0 == ( settings.sequence.encoding[ i ].override & strip_watermark ) )
                  ? density.byteCountPerStrip : density.byteCountPerWatermarkedStrip;
            }
            else
            {
               stripByteCount = density.byteCountPerStrip;
            }

            // one or more additional strips remain?
            if ( ( dataStripHeaderSize + 5 + crcByteSize + bytesRemaining ) > stripByteCount )
            {
               bytesRemaining -= stripByteCount - ( dataStripHeaderSize + 5 + crcByteSize );
            }
            else
            {
               lineCount =
                  ( 0 != ( ( dataStripHeaderSize + 5 + crcByteSize + bytesRemaining ) << 1 ) % hSync )
                  ? ( uint16_t )( 1 + ( ( dataStripHeaderSize + 5 + crcByteSize + bytesRemaining ) << 1 ) / hSync )
                  : ( uint16_t )( ( dataStripHeaderSize + 5 + crcByteSize + bytesRemaining ) << 1 ) / hSync;

               lineCount +=
                  ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

               lineCount +=
                  ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );

               bytesRemaining = 0;

               lastStrip = ( float )( density.bitHeight * lineCount );

               result = ( uint8_t )i;

               break;
            }
         }

         if ( 0 != bytesRemaining )
         {
            // error too many strips
            fprintf( stderr, LNFEED "ERROR: Strip sequence requires more than 127 strips to encode." LNFEED );
         }
      }
   }
   else
   {
      fprintf( stderr, LNFEED "ERROR: Meta strip is too large to fit in a single strip as configured." LNFEED );
      fprintf( stderr, "   Bytes required: %u   Bytes available: %u" LNFEED,
               ( metaHeaderSize + 5 ), stripByteCount );
   }

   *stripCount = result;

   *lastStripHeight = lastStrip;
}


// initialize strip header chain
// allocate bitfields
// encode strip data
// process encoded strip data into postscript output
// deallocate bitfields
// relinquish strip header chain

bool initializeStripSequence( struct MetaStripType** lStrip )
{
   bool result = true;

   uint16_t i;

   struct InputFileType* item;

   uint32_t totalSizeBytes = 0;
   uint32_t bytesRemaining = 0;

   uint16_t metaHeaderSize = 0;

   struct DataStripType* dStrip;

   struct DataDensityEntry density = { 0 };

   uint16_t stripByteCount = 0;
   uint16_t stripLineCount = 0;

   *lStrip = ( struct MetaStripType* )malloc( sizeof( struct MetaStripType ) );

   if ( NULL != lStrip )
   {
      memset( *lStrip, 0, sizeof( struct MetaStripType ) );

      ( *lStrip )->hSync =
        ( settings.sequence.encoding[ 1 ].override & strip_hsync )
        ? settings.sequence.encoding[ 1 ].hSync : settings.sequence.encoding[ 0 ].hSync;

      ( *lStrip )->vSync =
        ( settings.sequence.encoding[ 1 ].override & strip_vsync )
        ? settings.sequence.encoding[ 1 ].vSync : settings.sequence.encoding[ 0 ].vSync;

      density = getDataDensity( ( *lStrip )->hSync, ( *lStrip )->vSync );

      ( *lStrip )->prefix.data_sync = 0x00;
      ( *lStrip )->prefix.reader_expansion = 0x0000;

      memcpy( ( *lStrip )->prefix.header.id, settings.sequence.stripID, 6 );

      ( *lStrip )->prefix.header.number = 1;
      ( *lStrip )->prefix.header.type = cznstrip_data;

      ( *lStrip )->prefix.header.attributes =
        ( ( settings.sequence.encoding[ 1 ].override & strip_crc )
        || ( settings.sequence.encoding[ 0 ].override & strip_crc ) )
        ? 0x8000 : 0x0000;

      ( *lStrip )->prefix.host_os = cznossys_pcmsdos; // (0x20 also works)

      ( *lStrip )->prefix.file_count = ( uint8_t )CZFList_itemCount();

      ( *lStrip )->prefix.length = 1 + 6 + 1 + 1 + 2 + 1 + 1; // 3.4.6 thru 3.4.12

      // meta data size for all files

      for ( i = 0; i < CZFList_itemCount(); i++ )
      {
         if ( CZFList_get( i, &item ) )
         {
            metaHeaderSize += 1 + 1 + 3; // 3.4.13 Cauzin Type + 3.4.14 OS FileType + 3.4.15 File Length
//          metaHeaderSize += strlen( item->entry.name ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
            metaHeaderSize += strlen( item->path ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
            metaHeaderSize += 1; // 3.4.18 BlockExpand (adjunct size)

            if ( NULL != item->entry.adjunct )
            {
               metaHeaderSize += item->entry.adjunct_size;
            }

            totalSizeBytes += item->sizeBytes;
         }
         else
         {
            fprintf( stderr, "\n   ERROR: cannot get item %u from CZFList_get()", i + 1 );
         }
      }

      ( *lStrip )->prefix.length += metaHeaderSize;

      if ( 0x8000 & ( *lStrip )->prefix.header.attributes ) // CRC if present
      {
         ( *lStrip )->prefix.length += 2;
      }

      if ( 0 == ( settings.sequence.encoding[ 1 ].override & strip_watermark ) )
      {
         stripByteCount = density.byteCountPerStrip;
         stripLineCount = density.lineCountPerStrip;
      }
      else
      {
         stripByteCount = density.byteCountPerWatermarkedStrip;
         stripLineCount = density.lineCountPerWatermarkedStrip;
      }

      // meta data fits in first strip?
      if ( ( ( *lStrip )->prefix.length + 5 ) <= stripByteCount )
      {
         bytesRemaining = totalSizeBytes + ( ( *lStrip )->prefix.length + 5 );

         // one strip for everything?
         if ( bytesRemaining <= stripByteCount )
         {
            bytesRemaining = 0;

            ( *lStrip )->prefix.length += ( uint16_t )totalSizeBytes;

            // total nibbles / nibbles per row == total rows for strip body
            ( *lStrip )->extent.rows_stripBody =
               ( ( ( *lStrip )->prefix.length + 5 ) << 1 ) / ( *lStrip )->hSync;

            if ( 0 != ( ( ( ( *lStrip )->prefix.length + 5 ) << 1 ) % ( *lStrip )->hSync ) )
            {
               ( *lStrip )->extent.rows_stripBody += 1;
            }

            ( *lStrip )->extent.cols_stripBody = ( ( *lStrip )->hSync << 3 ) + 14; // bitsPerLine

            ( *lStrip )->extent.rows_hSyncHeader =
               ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

            ( *lStrip )->extent.rows_vSyncHeader =
               ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );
         }
         else // more than single strip required for encode
         {
            ( *lStrip )->prefix.length = stripByteCount - 5;

            ( *lStrip )->extent.rows_stripBody = stripLineCount;

            ( *lStrip )->extent.cols_stripBody = ( ( *lStrip )->hSync << 3 ) + 14; //bitsPerLine

            ( *lStrip )->extent.rows_hSyncHeader =
               ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

            ( *lStrip )->extent.rows_vSyncHeader =
               ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );

            bytesRemaining -= stripByteCount;

            ( *lStrip )->nextStrip = ( struct DataStripType* )malloc( sizeof( struct DataStripType ) );

            dStrip = ( *lStrip )->nextStrip;

            for ( i = 2; ( 0 < bytesRemaining ) && ( i < 128 ); i++ )
            {
               if ( NULL != dStrip )
               {
                  memset( dStrip, 0, sizeof( struct DataStripType ) );

                  dStrip->prefix.length = 1 + 6 + 1 + 1 + 2;  // 3.4.6 thru 3.4.10

                  dStrip->hSync =
                     ( settings.sequence.encoding[ i ].override & strip_hsync )
                     ? settings.sequence.encoding[ i ].hSync : settings.sequence.encoding[ 0 ].hSync;

                  dStrip->vSync =
                     ( settings.sequence.encoding[ i ].override & strip_vsync )
                     ? settings.sequence.encoding[ i ].vSync : settings.sequence.encoding[ 0 ].vSync;

                  density = getDataDensity( dStrip->hSync, dStrip->vSync );

                  dStrip->prefix.data_sync = 0x00;
                  dStrip->prefix.reader_expansion = 0x0000;

                  memcpy( dStrip->prefix.header.id, settings.sequence.stripID, 6 );

                  dStrip->prefix.header.number = ( uint8_t )i;
                  dStrip->prefix.header.type = cznstrip_data;

                  dStrip->prefix.header.attributes =
                     ( ( settings.sequence.encoding[ i ].override & strip_crc )
                     || ( settings.sequence.encoding[ 0 ].override & strip_crc ) )
                     ? 0x8000 : 0x0000;

                  if ( 0x8000 & dStrip->prefix.header.attributes ) // CRC suffix?
                  {
                     dStrip->prefix.length += 2;
                  }

                  if ( 0 == ( settings.sequence.encoding[ i ].override & strip_watermark ) )
                  {
                     stripByteCount = density.byteCountPerStrip;
                     stripLineCount = density.lineCountPerStrip;
                  }
                  else
                  {
                     stripByteCount = density.byteCountPerWatermarkedStrip;
                     stripLineCount = density.lineCountPerWatermarkedStrip;
                  }

                  // last strip?
                  if ( ( dStrip->prefix.length + 5 + bytesRemaining ) <= stripByteCount )
                  {
                     dStrip->prefix.length += ( uint16_t )bytesRemaining;

                     bytesRemaining = 0;

                     // total nibbles / nibbles per row == total rows for strip body
                     dStrip->extent.rows_stripBody =
                        ( ( dStrip->prefix.length + 5 ) << 1 ) / dStrip->hSync;

                     if ( 0 != ( ( ( dStrip->prefix.length + 5 ) << 1 ) % dStrip->hSync ) )
                     {
                        dStrip->extent.rows_stripBody += 1;
                     }

                     dStrip->extent.cols_stripBody = ( dStrip->hSync << 3 ) + 14; // bitsPerLine

                     dStrip->extent.rows_hSyncHeader =
                        ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

                     dStrip->extent.rows_vSyncHeader =
                        ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );
                  }
                  else // more strips, fresh out of the bit bucket
                  {
                     bytesRemaining -= stripByteCount - dStrip->prefix.length - 5;

                     dStrip->prefix.length = stripByteCount - 5;

                     dStrip->extent.rows_stripBody = stripLineCount;

                     dStrip->extent.cols_stripBody = ( dStrip->hSync << 3 ) + 14; // bitsPerLine

                     dStrip->extent.rows_hSyncHeader =
                        ( uint8_t )floor( settings.stripLayout.header.hsync_h / density.bitHeight + 0.5 );

                     dStrip->extent.rows_vSyncHeader =
                        ( uint8_t )floor( settings.stripLayout.header.vsync_h / density.bitHeight + 0.5 );

                     dStrip->nextStrip = ( struct DataStripType* )malloc( sizeof( struct DataStripType ) );

                     dStrip = dStrip->nextStrip;
                  }
               }
               else
               {
                  result = false;

                  bytesRemaining = 0;

                  fprintf( stderr, LNFEED "ERROR: Failed allocation for data strip." LNFEED );

                  break;
               }
            }
         }
      }
      else
      {
         fprintf( stderr, LNFEED "ERROR: Meta strip is too large to fit in a single strip as configured." LNFEED );
         fprintf( stderr, "   Bytes required: %u   Bytes available: %u" LNFEED,
                  ( ( *lStrip )->prefix.length + 5 ), stripByteCount );

         result = false;

         free( *lStrip );

         *lStrip = NULL;
      }
   }
   else
   {
      result = false;
   }

   return result;
}


bool initializeBitfield( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint16_t totalRows;
   uint16_t totalCols;

   uint8_t i;

   struct DataStripType* dStrip;

   struct BitField32Type** field = NULL;

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

         totalRows =
            dStrip->extent.rows_stripBody + dStrip->extent.rows_hSyncHeader
            + dStrip->extent.rows_vSyncHeader;

         totalCols = dStrip->extent.cols_stripBody;

         field = &( dStrip->field );
      }
      else
      {
         result = false;

         return result;
      }
   }
   else
   {
      totalRows =
         lStrip->extent.rows_stripBody + lStrip->extent.rows_hSyncHeader
         + lStrip->extent.rows_vSyncHeader;

      totalCols = lStrip->extent.cols_stripBody;

      field = &( lStrip->field );
   }

   BitField32_initialize( field, totalRows, totalCols );

   if ( NULL == field )
   {
      // failed allocation
      result = false;
   }

   return result;
}


bool initializeByteBuffer( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint8_t i;

   size_t byteCount;

   struct DataStripType* dStrip = NULL;

   if ( 1 < stripNumber )
   {
      dStrip = lStrip->nextStrip;

      i = 2;

      while ( NULL != dStrip )
      {
         if ( i < stripNumber )
         {
            dStrip = dStrip->nextStrip;

            ++i;
         }
         else
         {
            break;
         }
      }

      if ( NULL != dStrip )
      {
         byteCount = ( size_t )( ( ( ( uint32_t )dStrip->extent.rows_stripBody ) * dStrip->hSync ) >> 1 );

         dStrip->buffer = ( uint8_t* )malloc( byteCount );

         if ( NULL != dStrip->buffer )
         {
            memset( dStrip->buffer, 0, byteCount );
         }
         else
         {
            result = false;
         }
      }
      else
      {
         result = false;
      }
   }
   else
   {
      byteCount = ( size_t )( ( ( ( uint32_t )lStrip->extent.rows_stripBody ) * lStrip->hSync ) >> 1 );

      lStrip->buffer = ( uint8_t* )malloc( byteCount );

      if ( NULL != lStrip->buffer )
      {
         memset( lStrip->buffer, 0, byteCount );
      }
      else
      {
         result = false;
      }
   }

   return result;
}


bool encodeStripData( struct MetaStripType* lStrip, uint8_t stripNumber, uint16_t* inputFile, uint32_t* inputByteOffset )
{
   bool result = true;

   uint8_t hSync;
   uint8_t vSync;

   uint8_t hSync_bars;
   uint8_t checkerboard = 0;
   uint8_t displacement = 0;

   uint8_t nibble[ 12 ] = { 0 };

   uint8_t bitIndex;

   uint8_t oddParity;
   uint8_t evenParity;

   uint8_t ferrorCount = 0;

   uint16_t i;

   uint16_t rowIndex = 0;
   uint16_t row;
   uint16_t col;

   uint16_t colSize = 0;

   uint16_t byteCount = 0;
   uint16_t byteIndex = 0;
   uint16_t byteLimit = 0;

   uint16_t remainingBytes;

   uint16_t checksum;
   uint16_t attributes;

   uint16_t hBars[] = { 14, 18, 22, 26, 30, 34, 38, 42 };

   size_t bytesRead;

   FILE* input = NULL;

   struct InputFileType* item;

   struct DataStripType* dStrip;

   struct BitField32Type* field;

   struct EncodedExtentType stripExtent;

   uint8_t* buffer;

   crc16_clear();

   if ( 1 < stripNumber )
   {
      i = 2;

      dStrip = lStrip->nextStrip;

      while ( NULL != dStrip )
      {
         if ( i < stripNumber )
         {
            dStrip = dStrip->nextStrip;

            ++i;
         }
         else
         {
            break;
         }
      }

      if ( NULL != dStrip )
      {
         hSync = dStrip->hSync;
         vSync = dStrip->vSync;

         field = dStrip->field;

         stripExtent = dStrip->extent;

         byteCount = dStrip->prefix.length + 5;

         attributes = dStrip->prefix.header.attributes;

         if ( 0 != ( 0x8000 & attributes ) )
         {
            byteLimit = 2;
         }

         buffer = dStrip->buffer;
      }
      else
      {
         fprintf( stderr, LNFEED "   ERROR: strip# %u does not exist in this sequence.", stripNumber );

         result = false;

         return result;
      }
   }
   else
   {
      hSync = lStrip->hSync;
      vSync = lStrip->vSync;

      field = lStrip->field;

      stripExtent = lStrip->extent;

      byteCount = lStrip->prefix.length + 5;

      attributes = lStrip->prefix.header.attributes;

      if ( 0 != ( 0x8000 & attributes ) )
      {
         byteLimit = 2;
      }

      buffer = lStrip->buffer;

      *inputFile = 0;
      *inputByteOffset = 0;
   }

   hSync_bars = hSync - 4;

   colSize = ( hSync << 3 ) + 14; // bitsPerLine;


   // paint hSync
   for ( row = 0; row < stripExtent.rows_hSyncHeader; row++ )
   {
      // start line
      BitField32_set( field, rowIndex, 0 );
      BitField32_set( field, rowIndex, 1 );

      for ( col = 4; col <= 9; col++ )
      {
         BitField32_set( field, rowIndex, col );
      }

      while ( hSync_bars != 0 )
      {
         --hSync_bars;

         BitField32_set( field, rowIndex, hBars[ hSync_bars ] - 2 );
         BitField32_set( field, rowIndex, hBars[ hSync_bars ] - 1 );

         BitField32_set( field, rowIndex, colSize - hBars[ hSync_bars ] - 1 );
         BitField32_set( field, rowIndex, colSize - hBars[ hSync_bars ] - 2 );
      }

      hSync_bars = hSync - 4;

      for ( col = colSize - 12; col <= colSize - 7; col++ )
      {
         BitField32_set( field, rowIndex, col );
      }

      BitField32_set( field, rowIndex, colSize - 4 );
      BitField32_set( field, rowIndex, colSize - 3 );

      rowIndex++;
   }


   // stripe vSync byte into nibble scanLine
   oddParity = evenParity = 0;

   for ( i = 0; i < hSync; i++ )
   {
      if ( 0 == ( i & 0x01 ) )
      {
         nibble[ i ] = vSync & 0x0F;
      }
      else
      {
         nibble[ i ] = vSync >> 4;
      }

      evenParity ^= ( nibble[ i ] & 0x01 );      // 0th
      evenParity ^= ( nibble[ i ] & 0x04 ) >> 2; // 2nd

      oddParity ^= ( nibble[ i ] & 0x02 ) >> 1;  // 1st
      oddParity ^= ( nibble[ i ] & 0x08 ) >> 3;  // 3rd
   }

   // paint vSync from nibble scanLine
   for ( row = 0; row < stripExtent.rows_vSyncHeader; row++ )
   {
      // left margin
      for ( col = 0; col < 7; col++ )
      {
         // start line
         if ( ( 0 <= col ) && ( 1 >= col ) )
         {
            BitField32_set( field, rowIndex, col );
         }
         // checkerboard
         else if ( 3U + checkerboard == col )
         {
            BitField32_set( field, rowIndex, col );
         }
         // odd bit parity
         else if ( 5U + oddParity == col )
         {
            BitField32_set( field, rowIndex, col );
         }
      }

      // encode least significant nibble, most significant nibble of vSync byte
      for ( i = 0, bitIndex = 7; i < hSync; i++, bitIndex += 8 )
      {
         BitField32_set( field, rowIndex, bitIndex + ( nibble[ i ] & 0x01 ) );
         BitField32_set( field, rowIndex, bitIndex + 2 + ( ( nibble[ i ] & 0x02 ) >> 1 ) );
         BitField32_set( field, rowIndex, bitIndex + 4 + ( ( nibble[ i ] & 0x04 ) >> 2 ) );
         BitField32_set( field, rowIndex, bitIndex + 6 + ( ( nibble[ i ] & 0x08 ) >> 3 ) );
      }

      // right margin
      for ( col = colSize - 7; col < colSize; col++ )
      {
         // even bit parity
         if ( colSize - 7 + evenParity == col )
         {
            BitField32_set( field, rowIndex, col );
         }
         // alignment guideline
         else if ( ( colSize - 3 <= col ) && ( colSize - 2 >= col ) )
         {
            BitField32_set( field, rowIndex, col );
         }
         // zipper
         else if ( ( colSize - 1 == col ) && ( 1 == checkerboard ) )
         {
            BitField32_set( field, rowIndex, col );
         }
      }

      checkerboard = ( 0 == checkerboard ) ? 1 : 0;

      rowIndex++;
   }


   if ( 1 < stripNumber ) // data strip 3.4.1 thru 3.4.10
   {
      buffer[ byteIndex ] = dStrip->prefix.data_sync;
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( dStrip->prefix.reader_expansion & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( dStrip->prefix.reader_expansion >> 8 ) & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( dStrip->prefix.length & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( dStrip->prefix.length >> 8 ) & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = dStrip->prefix.header.checksum;
      ++byteIndex;

      memcpy( buffer + byteIndex, dStrip->prefix.header.id, 6 );  // stripID
      byteIndex += 6;

      buffer[ byteIndex ] = dStrip->prefix.header.number;
      ++byteIndex;

      buffer[ byteIndex ] = dStrip->prefix.header.type;
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( dStrip->prefix.header.attributes & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( dStrip->prefix.header.attributes >> 8 ) & 0x00FF );
      ++byteIndex;
   }
   else // meta strip 3.4.1 thru 3.4.12
   {
      buffer[ byteIndex ] = lStrip->prefix.data_sync;
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( lStrip->prefix.reader_expansion & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( lStrip->prefix.reader_expansion >> 8 ) & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( lStrip->prefix.length & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( lStrip->prefix.length >> 8 ) & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = lStrip->prefix.header.checksum;
      ++byteIndex;

      memcpy( buffer + byteIndex, lStrip->prefix.header.id, 6 );  // stripID
      byteIndex += 6;

      buffer[ byteIndex ] = lStrip->prefix.header.number;
      ++byteIndex;

      buffer[ byteIndex ] = lStrip->prefix.header.type;
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( lStrip->prefix.header.attributes & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = ( uint8_t )( ( lStrip->prefix.header.attributes >> 8 ) & 0x00FF );
      ++byteIndex;

      buffer[ byteIndex ] = lStrip->prefix.host_os;
      ++byteIndex;

      buffer[ byteIndex ] = lStrip->prefix.file_count;
      ++byteIndex;

      // append file metadata 3.4.13 thru 3.4.18 + adjunct
      for ( i = 0; i < CZFList_itemCount(); i++ )
      {
         if ( CZFList_get( i, &item ) )
         {
            buffer[ byteIndex ] = item->entry.category;
            ++byteIndex;

            buffer[ byteIndex ] = item->entry.os_type;
            ++byteIndex;

            buffer[ byteIndex ] = ( uint8_t )( item->entry.length & 0x0000FF );
            ++byteIndex;

            buffer[ byteIndex ] = ( uint8_t )( ( item->entry.length >> 8 ) & 0x0000FF );
            ++byteIndex;

            buffer[ byteIndex ] = ( uint8_t )( ( item->entry.length >> 16 ) & 0x0000FF );
            ++byteIndex;

//          memcpy( buffer + byteIndex, item->entry.name, strlen( item->entry.name ) );
            memcpy( buffer + byteIndex, item->path, strlen( item->path ) );

//          byteIndex += strlen( item->entry.name );
            byteIndex += strlen( item->path );

            buffer[ byteIndex ] = item->entry.terminator;
            ++byteIndex;

            buffer[ byteIndex ] = item->entry.adjunct_size;
            ++byteIndex;

            if ( 0 < item->entry.adjunct_size )
            {
               memcpy( buffer + byteIndex, item->entry.adjunct, item->entry.adjunct_size );

               byteIndex += item->entry.adjunct_size;
            }
         }
         else
         {
            fprintf( stderr, LNFEED "   ERROR: cannot get item %u from CZFList_get()", i + 1 );
         }
      }
   }

   remainingBytes = byteCount - ( byteIndex + byteLimit );

   // append file contents
   while ( ( true == result ) && ( 0 < remainingBytes ) )
   {
      if ( NULL == input )
      {
         if ( CZFList_get( *inputFile, &item ) )
         {
            input = fopen( item->path, "rb" );
         }
         else
         {
            fprintf( stderr, LNFEED "   ERROR: cannot get item %u from CZFList_get()", *inputFile + 1 );

            result = false;
         }
      }

      if ( NULL != input )
      {
         if ( ftell( input ) != ( signed long )*inputByteOffset )
         {
            if ( 0 != fseek( input, ( signed long )*inputByteOffset, SEEK_SET ) )
            {
               fclose( input );

               input = NULL;

               result = false;
            }
         }

         if ( true == result )
         {
            // partial file fills strip
            if ( remainingBytes < item->sizeBytes - *inputByteOffset )
            {
               bytesRead = fread( buffer + byteIndex, 1, remainingBytes, input );

               if ( bytesRead == remainingBytes )
               {
                  if ( 0 != ( 0x8000 & attributes ) )
                  {
                     for ( i = 0; i < bytesRead; i++ )
                     {
                        crc16_put( *( buffer + byteIndex + i ) );
                     }
                  }

                  byteIndex += bytesRead;

                  ferrorCount = 0;

                  remainingBytes = 0;

                  *inputByteOffset += bytesRead;

                  fclose( input );

                  input = NULL;
               }
               else if ( feof( input ) )
               {
                  fprintf( stderr, LNFEED "   ERROR: input file %s smaller than expected.", item->path );

                  fclose( input );

                  input = NULL;

                  result = false;
               }
               else if ( ferror( input ) )
               {
                  byteIndex += bytesRead;

                  *inputByteOffset += bytesRead;

                  fclose( input );

                  input = NULL;

                  ++ferrorCount;

                  if ( 3 == ferrorCount )
                  {
                     result = false;
                  }
               }
            }
            else // buffer entire file remnant
            {
               bytesRead = fread( buffer + byteIndex, 1, ( size_t )( item->sizeBytes - *inputByteOffset ), input );

               if ( bytesRead == ( item->sizeBytes - *inputByteOffset ) )
               {
                  if ( 0 != ( 0x8000 & attributes ) )
                  {
                     for ( i = 0; i < bytesRead; i++ )
                     {
                        crc16_put( *( buffer + byteIndex + i ) );
                     }
                  }

                  byteIndex += bytesRead;

                  remainingBytes -= bytesRead;

                  ferrorCount = 0;

                  *inputByteOffset = 0;

                  *inputFile += 1;

                  fclose( input );

                  input = NULL;
               }
               else if ( feof( input ) )
               {
                  fprintf( stderr, LNFEED "   ERROR: input file %s smaller than expected.", item->path );
                  fprintf( stderr, LNFEED "   remainingBytes = %u, bytesRead = %u, item->sizeBytes = %lu",
                                   remainingBytes, bytesRead, item->sizeBytes );

                  fclose( input );

                  input = NULL;

                  result = false;
               }
               else if ( ferror( input ) )
               {
                  byteIndex += bytesRead;

                  *inputByteOffset += bytesRead;

                  fclose( input );

                  input = NULL;

                  ++ferrorCount;

                  if ( 3 == ferrorCount )
                  {
                     result = false;
                  }
               }
            }
         }
      }
      else
      {
         result = false;
      }
   }

   // paranoia
   if ( NULL != input )
   {
      fclose( input );

      input = NULL;
   }

   if ( false == result )
   {
      return result;
   }

   // calculate CRC16
   if ( 0 != ( 0x8000 & attributes ) )
   {
      // Least Significant Byte
      buffer[ byteIndex ] = ( uint8_t )( crc16_get() & 0x00FF );
      ++byteIndex;

      // Most Significant Byte
      buffer[ byteIndex ] = ( uint8_t )( ( crc16_get() >> 8 ) & 0x00FF );
      ++byteIndex;
   }


   // fixup checksum byte
   checksum = 0;

   for ( i = 6; i < byteCount; i++ )
   {
      checksum = 0x01FF & ( buffer[ i ] + ( checksum & 0x00FF ) + ( checksum >> 8 ) );
   }

   // finalize calculated checksum (twos complement)
   checksum = ( ~( ( checksum >> 8 ) + ( 0xFF & checksum ) ) + 1 ) & 0xFF;

   buffer[ 5 ] = ( uint8_t )checksum;

   // encode bytes into nibble scanLines
   byteCount = byteIndex;

   byteIndex = 0;

   displacement = checkerboard;

   // paint scanLine into bitfield from nibble array
   for ( row = 0; row < stripExtent.rows_stripBody; row++ )
   {
      // load scanLine into nibble array
      oddParity = evenParity = 0;

      for ( i = 0; i < hSync; i++ )
      {
         if ( ( 0 == ( hSync & 0x01 ) ) || ( displacement == checkerboard ) )
         {
            if ( 0 == ( i & 0x01 ) )
            {
               if ( byteIndex < byteCount )
               {
                  nibble[ i ] = buffer[ byteIndex ] & 0x0F;
               }
               else
               {
                  nibble[ i ] = 0x00;
               }
            }
            else
            {
               if ( byteIndex < byteCount )
               {
                  nibble[ i ] = buffer[ byteIndex++ ] >> 4;
               }
               else
               {
                  nibble[ i ] = 0x00;
               }
            }
         }
         else // odd hSync, beware the odd nibble! (you'll break your tooth :^)
         {
            if ( 0 == ( i & 0x01 ) )
            {
               if ( byteIndex < byteCount )
               {
                  nibble[ i ] = buffer[ byteIndex++ ] >> 4;
               }
               else
               {
                  nibble[ i ] = 0x00;
               }
            }
            else
            {
               if ( byteIndex < byteCount )
               {
                  nibble[ i ] = buffer[ byteIndex ] & 0x0F;
               }
               else
               {
                  nibble[ i ] = 0x00;
               }
            }
         }

         evenParity ^= ( nibble[ i ] & 0x01 );      // 0th
         evenParity ^= ( nibble[ i ] & 0x04 ) >> 2; // 2nd

         oddParity ^= ( nibble[ i ] & 0x02 ) >> 1;  // 1st
         oddParity ^= ( nibble[ i ] & 0x08 ) >> 3;  // 3rd
      }


      // left margin
      for ( col = 0; col < 7; col++ )
      {
         // start line
         if ( ( 0 <= col ) && ( 1 >= col ) )
         {
            BitField32_set( field, rowIndex, col );
         }
         // checkerboard
         else if ( 3U + checkerboard == col )
         {
            BitField32_set( field, rowIndex, col );
         }
         // odd bit parity
         else if ( 5U + oddParity == col )
         {
            BitField32_set( field, rowIndex, col );
         }
      }

      // encode least significant nibble, most significant nibble of scanLine
      for ( i = 0, bitIndex = 7; i < hSync; i++, bitIndex += 8 )
      {
         BitField32_set( field, rowIndex, bitIndex + ( nibble[ i ] & 0x01 ) );
         BitField32_set( field, rowIndex, bitIndex + 2 + ( ( nibble[ i ] & 0x02 ) >> 1 ) );
         BitField32_set( field, rowIndex, bitIndex + 4 + ( ( nibble[ i ] & 0x04 ) >> 2 ) );
         BitField32_set( field, rowIndex, bitIndex + 6 + ( ( nibble[ i ] & 0x08 ) >> 3 ) );
      }

      // right margin
      for ( col = colSize - 7; col < colSize; col++ )
      {
         // even bit parity
         if ( colSize - 7 + evenParity == col )
         {
            BitField32_set( field, rowIndex, col );
         }
         // alignment guideline
         else if ( ( colSize - 3 <= col ) && ( colSize - 2 >= col ) )
         {
            BitField32_set( field, rowIndex, col );
         }
         // zipper
         else if ( ( colSize - 1 == col ) && ( 1 == checkerboard ) )
         {
            BitField32_set( field, rowIndex, col );
         }
      }

      checkerboard = ( 0 == checkerboard ) ? 1 : 0;

      rowIndex++;
   }

   return result;
}


bool saveBitfield( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint8_t i;

   struct DataStripType* dStrip;

   struct BitField32Type* field = NULL;

   char path[ 26 ] = { 0 };

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
   }

#ifdef _DOS
   _snprintf( path, sizeof( path ) - 1, "_CAUZIN_.TX\\S%03u.CZN", stripNumber );
#else
   snprintf( path, sizeof( path ) - 1, "_CAUZIN_.TX/S%03u.CZN", stripNumber );
#endif

   result &= BitField32_save( field, path );

   return result;
}


bool loadBitfield( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint8_t i;

   struct DataStripType* dStrip;

   struct BitField32Type* field = NULL;

   char path[ 26 ] = { 0 };

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
   }

#ifdef _DOS
   _snprintf( path, sizeof( path ) - 1, "_CAUZIN_.TX\\S%03u.CZN", stripNumber );
#else
   snprintf( path, sizeof( path ) - 1, "_CAUZIN_.TX/S%03u.CZN", stripNumber );
#endif

   result &= BitField32_load( field, path );

   return result;
}


bool relinquishByteBuffer( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint8_t i;

   struct DataStripType* dStrip = NULL;

   if ( 1 < stripNumber )
   {
      dStrip = lStrip->nextStrip;

      i = 2;

      while ( NULL != dStrip )
      {
         if ( i < stripNumber )
         {
            dStrip = dStrip->nextStrip;

            ++i;
         }
         else
         {
            break;
         }
      }

      if ( NULL != dStrip )
      {
         free( dStrip->buffer );
      }
      else
      {
         result = false;
      }
   }
   else
   {
      free( lStrip->buffer );
   }

   return result;
}


bool relinquishBitfield( struct MetaStripType* lStrip, uint8_t stripNumber )
{
   bool result = true;

   uint8_t i;

   struct DataStripType* dStrip;

   if ( 1 < stripNumber )
   {
      dStrip = lStrip->nextStrip;

      i = 2;

      while ( NULL != dStrip )
      {
         if ( i < stripNumber )
         {
            dStrip = dStrip->nextStrip;

            ++i;
         }
         else
         {
            break;
         }
      }

      if ( NULL != dStrip )
      {
         BitField32_relinquish( &( dStrip->field ) );
      }
      else
      {
         result = false;
      }
   }
   else
   {
      BitField32_relinquish( &( lStrip->field ) );
   }

   return result;
}


void relinquishStripSequence( struct MetaStripType** lStrip )
{
   struct DataStripType* dStrip;
   struct DataStripType* next;

   if ( NULL != *lStrip )
   {
      dStrip = ( *lStrip )->nextStrip;

      free( *lStrip );

      *lStrip = NULL;

      while ( NULL != dStrip )
      {
         next = dStrip->nextStrip;

         free( dStrip );

         dStrip = next;
      }
   }
}


bool renderStripSequence()
{
   bool result = true;

   uint8_t stripCount = 0;

   uint8_t stripNumber = 0;

   uint16_t inputFile = 0;
   uint32_t inputByteOffset = 0;

   float lengthLastStrip = 0.0f;

   struct MetaStripType* lStrip = NULL;

   if ( 0 < CZFList_itemCount() )
   {
      calculateStripHeight( &settings.stripLayout, settings.media );

      populateDataDensityTable( settings.stripLayout );

      calculateStripSequenceExtents( &stripCount, &lengthLastStrip );

      if ( stripCount < 128 )
      {
         result &= initializeStripSequence( &lStrip );

         if ( true == result )
         {
            for ( stripNumber = 1; stripNumber <= stripCount; stripNumber++ )
            {
               result &= initializeBitfield( lStrip, stripNumber );

               if ( true == result )
               {
                  result &= initializeByteBuffer( lStrip, stripNumber );

                  if ( true == result )
                  {
                     result &= encodeStripData( lStrip, stripNumber, &inputFile, &inputByteOffset );

                     if ( true == result )
                     {
                        result &= saveBitfield( lStrip, stripNumber );

                        if ( false == result )
                        {
                           // strip encode error
                           fprintf( stderr, LNFEED "   sorry, could not save strip# %u intermediate results.",
                                    stripNumber );
                        }
                     }
                     else
                     {
                        // strip encode error
                        fprintf( stderr, LNFEED "   sorry, could not encode strip# %u for postscript conversion.",
                                 stripNumber );
                     }
                  }
                  else
                  {
                     // buffer allocation error
                     fprintf( stderr, LNFEED "   sorry, could not allocate byte buffer for strip# %u storage.",
                              stripNumber );
                  }
               }
               else
               {
                  // bitfield allocation error
                  fprintf( stderr, LNFEED "   sorry, could not allocate bitfield for strip# %u storage.", stripNumber );
               }

               result &= relinquishByteBuffer( lStrip, stripNumber );

               result &= relinquishBitfield( lStrip, stripNumber );
            }

            if ( result )
            {
               fprintf( stderr, LNFEED LNFEED "*** renderStripSequence() returned TRUE ***" );

               for ( stripNumber = 1; stripNumber <= stripCount; stripNumber++ )
               {
                  result &= initializeBitfield( lStrip, stripNumber );

                  if ( true == result )
                  {
                     result &= loadBitfield( lStrip, stripNumber );

                     if ( true == result )
                     {
                        result &= generateDirectPostscriptOutput( lStrip, stripNumber, stripCount );

                        if ( true != result )
                        {
                           // write error
                           fprintf( stderr, LNFEED "   sorry, could not write postscript output file." );
                        }
                     }
                     else
                     {
                        // error reading file
                        fprintf( stderr, LNFEED "   sorry, could not read all input files to convert to postscript." );
                     }
                  }
                  else
                  {
                     // bitfield allocation error
                     fprintf( stderr, LNFEED "   sorry, could not allocate bitfield for strip# %u storage.", stripNumber );
                  }

                  relinquishBitfield( lStrip, stripNumber );
               }
            }
            else
            {
               fprintf( stderr, LNFEED LNFEED "*** renderStripSequence() returned FALSE ***" );
            }
         }
         else
         {
            // strip header allocation error
            fprintf( stderr, LNFEED "   sorry, could not allocate strip file info to begin encode." );
         }

         relinquishStripSequence( &lStrip );
      }
      else
      {
         // strip header allocation error
         fprintf( stderr, LNFEED "   sorry, cannot encode more than 127 strips in a sequence." );
      }
   }
   else
   {
      result = false;

      fprintf( stderr, LNFEED "ERROR: No files selected to render - nothing to encode." LNFEED );
   }

   return result;
}
