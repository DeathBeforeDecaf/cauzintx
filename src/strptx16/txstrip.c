
#include "txstrip.h"

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

   struct FileEntryType* entry;

   char* index;

   char* extension;

   uint16_t i;

   size_t length;

   bool hasMatched;

   length = strlen( filePath ) + 1;

   length += sizeof( struct InputFileType );

   result = ( struct InputFileType* )malloc( length );

   if ( NULL != result )
   {
      memset( result, 0, length );

      entry = &(result->entry);

      result->sizeBytes = sizeBytes;

//    result->entry.length = sizeBytes;
      result->entry.length[0] = ( uint8_t )sizeBytes;
      result->entry.length[1] = ( uint8_t )( sizeBytes >> 8 );
      result->entry.length[2] = ( uint8_t )( sizeBytes >> 16 );

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

         for ( i = 0; i < executableExtensionCount; i++ )
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

         for ( i = 0; i < binaryExtensionCount; i++ )
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
            for ( i = 0; i < textExtensionCount; i++ )
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
         metaHeaderSize += ( uint16_t )strlen( item->path ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
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

      printf( LNFEED "  Meta data consumed %" FSTR_UINT16_T " bytes out of %" FSTR_UINT16_T " meta strip bytes.",
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
            fprintf( stderr, LNFEED "ERROR: Strip sequence requires more than %d strips to encode." LNFEED, SEQUENCE_LIMIT );
         }
      }
   }
   else
   {
      fprintf( stderr, LNFEED "ERROR: Meta strip is too large to fit in a single strip as configured." LNFEED );
      fprintf( stderr, "   Bytes required: %" FSTR_UINT16_T "   Bytes available: %" FSTR_UINT16_T LNFEED,
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
            metaHeaderSize += ( uint16_t )strlen( item->path ) + 1; // 3.4.16 + 3.4.17 Filename + Terminator
            metaHeaderSize += 1; // 3.4.18 BlockExpand (adjunct size)

            if ( NULL != item->entry.adjunct )
            {
               metaHeaderSize += item->entry.adjunct_size;
            }

            totalSizeBytes += item->sizeBytes;
         }
         else
         {
            fprintf( stderr, "\n   ERROR: cannot get item %" FSTR_UINT16_T " from CZFList_get()", i + 1 );
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
         fprintf( stderr, "   Bytes required: %" FSTR_UINT16_T "   Bytes available: %" FSTR_UINT16_T LNFEED,
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

   struct InputFileType* item = { 0 };

   struct DataStripType* dStrip = { 0 };

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
         fprintf( stderr, LNFEED "   ERROR: strip# %" FSTR_UINT8_T " does not exist in this sequence.", stripNumber );

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
      // left boundary
      BitField32_set( field, rowIndex, 0 );
      BitField32_set( field, rowIndex, 1 );

      // left block
      for ( col = 4; col <= 9; col++ )
      {
         BitField32_set( field, rowIndex, col );
      }

      while ( hSync_bars != 0 )
      {
         --hSync_bars;

         BitField32_set( field, rowIndex, hBars[ hSync_bars ] - 2 );
         BitField32_set( field, rowIndex, hBars[ hSync_bars ] - 1 );

         BitField32_set( field, rowIndex, colSize - hBars[ hSync_bars ] - 2 );
         BitField32_set( field, rowIndex, colSize - hBars[ hSync_bars ] - 3 );
      }

      // right block
      for ( col = colSize - 13; col <= colSize - 8; col++ )
      {
         BitField32_set( field, rowIndex, col );
      }

      // right boundary
      BitField32_set( field, rowIndex, colSize - 4 );
      BitField32_set( field, rowIndex, colSize - 3 );

      rowIndex++;

      hSync_bars = hSync - 4;
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

// TODO:
//          buffer[ byteIndex ] = ( uint8_t )( item->entry.length & 0x0000FF );
            buffer[ byteIndex ] = ( uint8_t )( item->entry.length[0] );
            ++byteIndex;

//          buffer[ byteIndex ] = ( uint8_t )( ( item->entry.length >> 8 ) & 0x0000FF );
            buffer[ byteIndex ] = ( uint8_t )( item->entry.length[1] );
            ++byteIndex;

//          buffer[ byteIndex ] = ( uint8_t )( ( item->entry.length >> 16 ) & 0x0000FF );
            buffer[ byteIndex ] = ( uint8_t )( item->entry.length[2] );
            ++byteIndex;

//          memcpy( buffer + byteIndex, item->entry.name, strlen( item->entry.name ) );
            memcpy( buffer + byteIndex, item->path, strlen( item->path ) );

//          byteIndex += strlen( item->entry.name );
            byteIndex += ( uint16_t )strlen( item->path );

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
            fprintf( stderr, LNFEED "   ERROR: cannot get item %" FSTR_UINT16_T " from CZFList_get()", i + 1 );
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
            fprintf( stderr, LNFEED "   ERROR: cannot get item %" FSTR_UINT16_T " from CZFList_get()", *inputFile + 1 );

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

                  *inputByteOffset += ( uint32_t )bytesRead;

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

                  *inputByteOffset += ( uint32_t )bytesRead;

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
                  fprintf( stderr, LNFEED "   remainingBytes = %" FSTR_UINT16_T ", bytesRead = %"
                                   FSTR_SIZE_T ", item->sizeBytes = %" FSTR_UINT32_T,
                                   remainingBytes, bytesRead, item->sizeBytes );

                  fclose( input );

                  input = NULL;

                  result = false;
               }
               else if ( ferror( input ) )
               {
                  byteIndex += bytesRead;

                  *inputByteOffset += ( uint32_t )bytesRead;

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

   snprintf( path, sizeof( path ) - 1, "_cauzin_.tx" PATH_SEPARATOR_STR "s%03" FSTR_UINT8_T ".czn", stripNumber );

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

   snprintf( path, sizeof( path ) - 1, "_cauzin_.tx" PATH_SEPARATOR_STR "s%03" FSTR_UINT8_T ".czn", stripNumber );

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
                           fprintf( stderr, LNFEED "   sorry, could not save strip# %" FSTR_UINT8_T " intermediate results.",
                                    stripNumber );
                        }
                     }
                     else
                     {
                        // strip encode error
                        fprintf( stderr, LNFEED "   sorry, could not encode strip# %" FSTR_UINT8_T " for postscript conversion.",
                                 stripNumber );
                     }
                  }
                  else
                  {
                     // buffer allocation error
                     fprintf( stderr, LNFEED "   sorry, could not allocate byte buffer for strip# %" FSTR_UINT8_T " storage.",
                              stripNumber );
                  }
               }
               else
               {
                  // bitfield allocation error
                  fprintf( stderr, LNFEED "   sorry, could not allocate bitfield for strip# %" FSTR_UINT8_T " storage.", stripNumber );
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
                     fprintf( stderr, LNFEED "   sorry, could not allocate bitfield for strip# %" FSTR_UINT8_T " storage.", stripNumber );
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
         fprintf( stderr, LNFEED "   sorry, cannot encode more than %d strips in a sequence.", SEQUENCE_LIMIT );
      }
   }
   else
   {
      result = false;

      fprintf( stderr, LNFEED "ERROR: No files selected to render - nothing to encode." LNFEED );
   }

   return result;
}


void renderWatermarkLogo( FILE* output, float xPos, float yPos, float scale )
{
   fputs( "gsave\n\n", output );

   fprintf( output, "%1.2f %1.2f translate\n", xPos, yPos );
   fprintf( output, "%1.3f %1.3f scale\n\n", scale, scale );

   fputs( "0 setgray\n\n", output );

   // top right/top line
   fputs( "0 172.98 moveto\n", output );
   fputs( "18 0 rlineto\n", output );
   fputs( "0 -4.68 rlineto\n", output );
   fputs( "-18 0 rlineto\n", output );
   fputs( "closepath\n", output );
   fputs( "fill\n\n", output );

   fputs( "18 175.32 moveto\n", output );
   fputs( "702 0 rlineto\n", output );
   fputs( "0 -4.68 rlineto\n", output );
   fputs( "-702 0 rlineto\n", output );
   fputs( "closepath\n", output );
   fputs( "fill\n\n", output );

   fputs( "gsave\n", output );
   fputs( "   72 {\n", output );
   fputs( "      715.32 175.32 moveto\n", output );
   fputs( "      0 4.68 rlineto\n", output );
   fputs( "      4.68 0 rlineto\n", output );
   fputs( "      0 -4.68 rlineto\n", output );
   fputs( "     closepath fill\n\n", output );

   fputs( "      -9.755 0 translate\n", output );
   fputs( "   } repeat\n", output );
   fputs( "grestore\n\n", output );

   // background (field)
   fputs( "0 12 moveto\n", output );
   fputs( "0 37.74 lineto\n", output );
   fputs( "18 37.74 lineto\n", output );
   fputs( "18 135.06 lineto\n", output );
   fputs( "0 135.06 lineto\n", output );
   fputs( "0 161.28 lineto\n", output );
   fputs( "720 161.28 lineto\n", output );
   fputs( "720 12 lineto\n", output );
   fputs( "closepath fill\n\n", output );

   // bottom/left line
   fputs( "0 0 moveto\n", output );
   fputs( "0 3 rlineto\n", output );
   fputs( "720 0 rlineto\n", output );
   fputs( "720 0 lineto\n", output );
   fputs( "closepath fill\n\n", output );

   // text
   fputs( "1 setgray\n\n", output );

   fputs( "133.836 151.742 moveto\n", output );
   fputs( "152.98 151.742 170.234 144.285 169.73 121.27 curveto\n", output );
   fputs( "144.164 121.27 lineto\n", output );
   fputs( "144.289 125.063 143.031 127.418 141.016 128.988 curveto\n", output );
   fputs( "139 130.555 136.102 131.078 132.953 131.078 curveto\n", output );
   fputs( "128.293 131.078 122.246 129.641 122.246 123.492 curveto\n", output );
   fputs( "122.246 116.43 135.723 116.039 144.539 112.77 curveto\n", output );
   fputs( "160.16 107.016 165.449 98.121 165.449 87.004 curveto\n", output );
   fputs( "165.449 64.508 144.414 53.914 124.891 53.914 curveto\n", output );
   fputs( "104.238 53.914 85.594 63.727 86.73 87.527 curveto\n", output );
   fputs( "113.555 87.527 lineto\n", output );
   fputs( "113.555 82.426 114.691 79.289 116.957 76.934 curveto\n", output );
   fputs( "119.352 74.84 122.625 74.578 127.16 74.578 curveto\n", output );
   fputs( "132.574 74.578 139.25 77.457 139.25 83.605 curveto\n", output );
   fputs( "139.25 90.406 130.055 91.32 117.586 95.637 curveto\n", output );
   fputs( "106.629 99.43 96.051 105.445 96.051 121.008 curveto\n", output );
   fputs( "96.051 142.066 115.699 151.742 133.836 151.742 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "210.324 125.715 moveto\n", output );
   fputs( "229.469 125.715 242.066 116.563 242.066 96.027 curveto\n", output );
   fputs( "242.066 78.637 232.742 54.438 202.641 54.438 curveto\n", output );
   fputs( "184.633 54.438 170.02 63.461 170.02 86.219 curveto\n", output );
   fputs( "170.02 107.93 183.625 125.715 210.324 125.715 curveto\n", output );
   fputs( "closepath\n", output );
   fputs( "208.438 106.883 moveto\n", output );
   fputs( "198.484 106.883 194.961 91.32 194.961 84.258 curveto\n", output );
   fputs( "194.961 78.773 196.723 73.27 203.777 73.27 curveto\n", output );
   fputs( "213.602 73.27 217.125 88.313 217.125 94.199 curveto\n", output );
   fputs( "217.125 102.305 215.109 106.883 208.438 106.883 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "287.859 149.52 moveto\n", output );
   fputs( "292.266 149.52 296.551 149.129 300.961 148.867 curveto\n", output );
   fputs( "297.434 131.734 lineto\n", output );
   fputs( "295.793 131.996 293.652 132.125 290.758 132.125 curveto\n", output );
   fputs( "285.844 132.125 283.828 130.164 282.57 124.016 curveto\n", output );
   fputs( "295.289 124.016 lineto\n", output );
   fputs( "292.016 108.063 lineto\n", output );
   fputs( "279.422 108.063 lineto\n", output );
   fputs( "269.094 56.137 lineto\n", output );
   fputs( "244.152 56.137 lineto\n", output );
   fputs( "254.484 108.063 lineto\n", output );
   fputs( "244.152 108.063 lineto\n", output );
   fputs( "247.555 124.016 lineto\n", output );
   fputs( "257.633 124.016 lineto\n", output );
   fputs( "261.535 144.027 268.336 149.52 287.859 149.52 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "315.387 144.551 moveto\n", output );
   fputs( "340.324 144.551 lineto\n", output );
   fputs( "335.918 124.016 lineto\n", output );
   fputs( "349.395 124.016 lineto\n", output );
   fputs( "345.867 108.063 lineto\n", output );
   fputs( "332.641 108.063 lineto\n", output );
   fputs( "328.234 86.219 lineto\n", output );
   fputs( "327.73 83.996 327.102 81.641 327.102 79.418 curveto\n", output );
   fputs( "327.102 77.195 328.234 75.363 332.141 75.363 curveto\n", output );
   fputs( "336.168 75.363 338.309 75.625 340.324 75.758 curveto\n", output );
   fputs( "336.547 57.055 lineto\n", output );
   fputs( "332.141 56.27 327.605 55.617 323.07 55.355 curveto\n", output );
   fputs( "317.781 55.355 312.113 55.355 307.328 58.102 curveto\n", output );
   fputs( "303.047 60.586 300.527 65.945 300.527 70.918 curveto\n", output );
   fputs( "300.527 74.578 301.281 77.063 301.91 80.203 curveto\n", output );
   fputs( "307.707 108.063 lineto\n", output );
   fputs( "296.621 108.063 lineto\n", output );
   fputs( "300.148 124.016 lineto\n", output );
   fputs( "310.98 124.016 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "382.617 125.715 moveto\n", output );
   fputs( "396.473 125.715 413.348 122.578 411.965 104.266 curveto\n", output );
   fputs( "389.672 104.266 lineto\n", output );
   fputs( "389.922 106.883 389.293 108.582 387.906 109.629 curveto\n", output );
   fputs( "386.27 110.938 384.129 111.332 382.113 111.332 curveto\n", output );
   fputs( "378.711 111.332 375.059 109.891 375.059 105.836 curveto\n", output );
   fputs( "375.059 102.176 377.453 101.129 380.727 100.738 curveto\n", output );
   fputs( "399.242 98.121 408.438 93.934 408.438 79.68 curveto\n", output );
   fputs( "408.438 63.984 395.465 54.438 375.313 54.438 curveto\n", output );
   fputs( "361.207 54.438 343.699 57.969 344.203 78.371 curveto\n", output );
   fputs( "367.125 78.371 lineto\n", output );
   fputs( "367.125 75.625 367.879 73.535 369.391 72.094 curveto\n", output );
   fputs( "370.902 70.918 373.168 70.395 375.941 70.395 curveto\n", output );
   fputs( "382.617 70.395 383.496 74.84 383.496 76.41 curveto\n", output );
   fputs( "383.496 81.25 377.199 81.641 367.879 83.34 curveto\n", output );
   fputs( "363.473 84.125 350.121 86.09 350.121 100.867 curveto\n", output );
   fputs( "350.121 120.355 366.875 125.715 382.617 125.715 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "433.945 144.551 moveto\n", output );
   fputs( "458.883 144.551 lineto\n", output );
   fputs( "454.477 124.016 lineto\n", output );
   fputs( "467.953 124.016 lineto\n", output );
   fputs( "464.426 108.063 lineto\n", output );
   fputs( "451.199 108.063 lineto\n", output );
   fputs( "446.793 86.219 lineto\n", output );
   fputs( "446.289 83.996 445.66 81.641 445.66 79.418 curveto\n", output );
   fputs( "445.66 77.195 446.793 75.363 450.699 75.363 curveto\n", output );
   fputs( "454.727 75.363 456.871 75.625 458.883 75.758 curveto\n", output );
   fputs( "455.105 57.055 lineto\n", output );
   fputs( "450.699 56.27 446.164 55.617 441.629 55.355 curveto\n", output );
   fputs( "436.34 55.355 430.672 55.355 425.883 58.102 curveto\n", output );
   fputs( "421.602 60.586 419.082 65.945 419.082 70.918 curveto\n", output );
   fputs( "419.082 74.578 419.84 77.063 420.469 80.203 curveto\n", output );
   fputs( "426.262 108.063 lineto\n", output );
   fputs( "415.18 108.063 lineto\n", output );
   fputs( "418.703 124.016 lineto\n", output );
   fputs( "429.539 124.016 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "513.652 125.715 moveto\n", output );
   fputs( "517.809 125.715 520.199 125.457 522.469 124.672 curveto\n", output );
   fputs( "517.809 101.781 lineto\n", output );
   fputs( "514.785 102.961 511.383 103.484 508.234 103.484 curveto\n", output );
   fputs( "497.656 103.484 492.367 98.773 489.469 84.52 curveto\n", output );
   fputs( "483.801 56.137 lineto\n", output );
   fputs( "458.863 56.137 lineto\n", output );
   fputs( "472.34 124.016 lineto\n", output );
   fputs( "496.523 124.016 lineto\n", output );
   fputs( "494.379 113.422 lineto\n", output );
   fputs( "494.633 113.422 lineto\n", output );
   fputs( "499.039 120.617 507.227 125.715 513.652 125.715 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "532.234 149.52 moveto\n", output );
   fputs( "557.172 149.52 lineto\n", output );
   fputs( "553.52 131.34 lineto\n", output );
   fputs( "528.582 131.34 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "527.195 124.016 moveto\n", output );
   fputs( "552.133 124.016 lineto\n", output );
   fputs( "538.531 56.137 lineto\n", output );
   fputs( "513.59 56.137 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "598.566 125.707 moveto\n", output );
   fputs( "616.957 125.707 622.371 109.883 622.371 97.848 curveto\n", output );
   fputs( "622.371 77.711 612.676 54.43 590.508 54.43 curveto\n", output );
   fputs( "583.328 54.43 576.273 55.344 570.859 64.367 curveto\n", output );
   fputs( "562.215 24.891 lineto\n", output );
   fputs( "537.273 24.891 lineto\n", output );
   fputs( "558.137 124.008 lineto\n", output );
   fputs( "582.32 124.008 lineto\n", output );
   fputs( "580.809 116.289 lineto\n", output );
   fputs( "581.063 116.289 lineto\n", output );
   fputs( "585.344 122.566 590.883 125.707 598.566 125.707 curveto\n", output );
   fputs( "closepath\n", output );

   fputs( "588.367 106.875 moveto\n", output );
   fputs( "577.281 106.875 575.266 90.395 575.266 84.379 curveto\n", output );
   fputs( "575.266 81.895 576.148 73.262 584.336 73.262 curveto\n", output );
   fputs( "594.285 73.262 597.434 86.078 597.434 94.844 curveto\n", output );
   fputs( "597.434 101.512 595.672 106.875 588.367 106.875 curveto\n", output );
   fputs( "closepath\n\n\n", output );

   fputs( "102.73 46.719 moveto\n", output );
   fputs( "105.363 46.719 107.406 46.023 108.859 44.637 curveto\n", output );
   fputs( "110.316 43.25 111.121 41.672 111.281 39.906 curveto\n", output );
   fputs( "108.535 39.906 lineto\n", output );
   fputs( "108.223 41.246 107.602 42.309 106.668 43.094 curveto\n", output );
   fputs( "105.742 43.875 104.438 44.27 102.758 44.27 curveto\n", output );
   fputs( "100.711 44.27 99.055 43.547 97.789 42.102 curveto\n", output );
   fputs( "96.531 40.668 95.906 38.461 95.906 35.488 curveto\n", output );
   fputs( "95.906 33.055 96.473 31.074 97.605 29.555 curveto\n", output );
   fputs( "98.746 28.047 100.445 27.289 102.703 27.289 curveto\n", output );
   fputs( "104.777 27.289 106.359 28.086 107.445 29.684 curveto\n", output );
   fputs( "108.023 30.523 108.449 31.629 108.734 32.996 curveto\n", output );
   fputs( "111.48 32.996 lineto\n", output );
   fputs( "111.234 30.805 110.426 28.973 109.047 27.488 curveto\n", output );
   fputs( "107.395 25.703 105.164 24.813 102.363 24.813 curveto\n", output );
   fputs( "99.945 24.813 97.918 25.543 96.273 27.008 curveto\n", output );
   fputs( "94.113 28.941 93.031 31.93 93.031 35.969 curveto\n", output );
   fputs( "93.031 39.039 93.844 41.555 95.465 43.516 curveto\n", output );
   fputs( "97.223 45.652 99.645 46.719 102.73 46.719 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "123.887 46.719 moveto\n", output );
   fputs( "127.559 46.719 130.277 45.539 132.043 43.176 curveto\n", output );
   fputs( "133.422 41.336 134.109 38.98 134.109 36.113 curveto\n", output );
   fputs( "134.109 33.008 133.32 30.426 131.746 28.367 curveto\n", output );
   fputs( "129.895 25.949 127.254 24.742 123.828 24.742 curveto\n", output );
   fputs( "120.629 24.742 118.113 25.801 116.281 27.914 curveto\n", output );
   fputs( "114.648 29.953 113.832 32.531 113.832 35.645 curveto\n", output );
   fputs( "113.832 38.457 114.531 40.867 115.93 42.867 curveto\n", output );
   fputs( "117.723 45.434 120.375 46.719 123.887 46.719 curveto\n", output );
   fputs( "closepath\n", output );
   fputs( "124.055 44.211 moveto\n", output );
   fputs( "121.902 44.211 120.148 43.469 118.789 41.988 curveto\n", output );
   fputs( "117.43 40.516 116.75 38.34 116.75 35.461 curveto\n", output );
   fputs( "116.75 33.156 117.328 31.211 118.492 29.625 curveto\n", output );
   fputs( "119.66 28.051 121.555 27.262 124.168 27.262 curveto\n", output );
   fputs( "126.652 27.262 128.445 28.148 129.551 29.922 curveto\n", output );
   fputs( "130.664 31.707 131.219 33.758 131.219 36.07 curveto\n", output );
   fputs( "131.219 38.516 130.578 40.484 129.293 41.977 curveto\n", output );
   fputs( "128.02 43.465 126.273 44.211 124.055 44.211 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "137.395 46.152 moveto\n", output );
   fputs( "141.43 46.152 lineto\n", output );
   fputs( "147.406 28.563 lineto\n", output );
   fputs( "153.34 46.152 lineto\n", output );
   fputs( "157.332 46.152 lineto\n", output );
   fputs( "157.332 25.352 lineto\n", output );
   fputs( "154.656 25.352 lineto\n", output );
   fputs( "154.656 37.625 lineto\n", output );
   fputs( "154.656 38.051 154.664 38.754 154.684 39.738 curveto\n", output );
   fputs( "154.703 40.719 154.711 41.77 154.711 42.895 curveto\n", output );
   fputs( "148.781 25.352 lineto\n", output );
   fputs( "145.988 25.352 lineto\n", output );
   fputs( "140.016 42.895 lineto\n", output );
   fputs( "140.016 42.258 lineto\n", output );
   fputs( "140.016 41.746 140.023 40.969 140.043 39.922 curveto\n", output );
   fputs( "140.07 38.883 140.086 38.117 140.086 37.629 curveto\n", output );
   fputs( "140.086 25.352 lineto\n", output );
   fputs( "137.395 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "161.891 46.152 moveto\n", output );
   fputs( "171.25 46.152 lineto\n", output );
   fputs( "173.102 46.152 174.594 45.629 175.727 44.578 curveto\n", output );
   fputs( "176.859 43.539 177.426 42.078 177.426 40.191 curveto\n", output );
   fputs( "177.426 38.566 176.922 37.152 175.91 35.941 curveto\n", output );
   fputs( "174.898 34.742 173.348 34.145 171.25 34.145 curveto\n", output );
   fputs( "164.711 34.145 lineto\n", output );
   fputs( "164.711 25.352 lineto\n", output );
   fputs( "161.891 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "164.711 43.73 moveto\n", output );
   fputs( "164.711 36.523 lineto\n", output );
   fputs( "170.316 36.523 lineto\n", output );
   fputs( "171.582 36.523 172.605 36.793 173.391 37.328 curveto\n", output );
   fputs( "174.184 37.867 174.578 38.816 174.578 40.176 curveto\n", output );
   fputs( "174.578 41.703 174.012 42.742 172.879 43.293 curveto\n", output );
   fputs( "172.258 43.586 171.402 43.73 170.316 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "181.164 46.152 moveto\n", output );
   fputs( "184.023 46.152 lineto\n", output );
   fputs( "184.023 33.293 lineto\n", output );
   fputs( "184.023 31.785 184.309 30.527 184.875 29.527 curveto\n", output );
   fputs( "185.715 28.02 187.129 27.262 189.121 27.262 curveto\n", output );
   fputs( "191.512 27.262 193.133 28.078 193.992 29.711 curveto\n", output );
   fputs( "194.457 30.598 194.688 31.793 194.688 33.293 curveto\n", output );
   fputs( "194.688 46.152 lineto\n", output );
   fputs( "197.547 46.152 lineto\n", output );
   fputs( "197.547 34.469 lineto\n", output );
   fputs( "197.547 31.91 197.203 29.941 196.512 28.563 curveto\n", output );
   fputs( "195.246 26.055 192.859 24.797 189.348 24.797 curveto\n", output );
   fputs( "185.836 24.797 183.453 26.055 182.195 28.563 curveto\n", output );
   fputs( "181.508 29.941 181.164 31.91 181.164 34.469 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "200.168 46.152 moveto\n", output );
   fputs( "217.047 46.152 lineto\n", output );
   fputs( "217.047 43.676 lineto\n", output );
   fputs( "210.035 43.676 lineto\n", output );
   fputs( "210.035 25.352 lineto\n", output );
   fputs( "207.176 25.352 lineto\n", output );
   fputs( "207.176 43.672 lineto\n", output );
   fputs( "200.168 43.672 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "219.891 46.152 moveto\n", output );
   fputs( "235.059 46.152 lineto\n", output );
   fputs( "235.059 43.602 lineto\n", output );
   fputs( "222.641 43.602 lineto\n", output );
   fputs( "222.641 37.289 lineto\n", output );
   fputs( "234.125 37.289 lineto\n", output );
   fputs( "234.125 34.879 lineto\n", output );
   fputs( "222.641 34.879 lineto\n", output );
   fputs( "222.641 27.828 lineto\n", output );
   fputs( "235.27 27.828 lineto\n", output );
   fputs( "235.27 25.352 lineto\n", output );
   fputs( "219.891 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "239.305 46.152 moveto\n", output );
   fputs( "248.934 46.152 lineto\n", output );
   fputs( "250.52 46.152 251.828 45.918 252.855 45.457 curveto\n", output );
   fputs( "254.813 44.57 255.789 42.934 255.789 40.543 curveto\n", output );
   fputs( "255.789 39.297 255.527 38.277 255.008 37.484 curveto\n", output );
   fputs( "254.5 36.691 253.781 36.055 252.855 35.574 curveto\n", output );
   fputs( "253.668 35.242 254.277 34.809 254.684 34.273 curveto\n", output );
   fputs( "255.098 33.734 255.328 32.859 255.379 31.652 curveto\n", output );
   fputs( "255.477 28.863 lineto\n", output );
   fputs( "255.504 28.07 255.57 27.48 255.676 27.094 curveto\n", output );
   fputs( "255.844 26.43 256.145 26.008 256.582 25.816 curveto\n", output );
   fputs( "256.582 25.352 lineto\n", output );
   fputs( "253.125 25.352 lineto\n", output );
   fputs( "253.031 25.351 252.957 25.762 252.898 26.043 curveto\n", output );
   fputs( "252.844 26.328 252.793 26.875 252.758 27.688 curveto\n", output );
   fputs( "252.586 31.156 lineto\n", output );
   fputs( "252.52 32.516 252.016 33.426 251.074 33.891 curveto\n", output );
   fputs( "250.535 34.145 249.688 34.27 248.539 34.27 curveto\n", output );
   fputs( "242.121 34.27 lineto\n", output );
   fputs( "242.121 25.352 lineto\n", output );
   fputs( "239.305 25.392 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "242.121 43.73 moveto\n", output );
   fputs( "242.121 36.621 lineto\n", output );
   fputs( "248.637 36.621 lineto\n", output );
   fputs( "249.957 36.621 251 36.887 251.766 37.414 curveto\n", output );
   fputs( "252.539 37.941 252.926 38.898 252.926 40.277 curveto\n", output );
   fputs( "252.926 41.758 252.391 42.766 251.313 43.305 curveto\n", output );
   fputs( "250.738 43.586 249.969 43.73 249.008 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "276.363 46.152 moveto\n", output );
   fputs( "285.992 46.152 lineto\n", output );
   fputs( "287.578 46.152 288.883 45.918 289.914 45.457 curveto\n", output );
   fputs( "291.867 44.57 292.844 42.934 292.844 40.543 curveto\n", output );
   fputs( "292.844 39.297 292.586 38.277 292.066 37.484 curveto\n", output );
   fputs( "291.559 36.691 290.84 36.055 289.914 35.574 curveto\n", output );
   fputs( "290.727 35.242 291.332 34.809 291.738 34.273 curveto\n", output );
   fputs( "292.156 33.734 292.387 32.859 292.434 31.652 curveto\n", output );
   fputs( "292.531 28.863 lineto\n", output );
   fputs( "292.563 28.07 292.629 27.48 292.73 27.094 curveto\n", output );
   fputs( "292.902 26.43 293.203 26.008 293.637 25.816 curveto\n", output );
   fputs( "293.637 25.352 lineto\n", output );
   fputs( "290.184 25.352 lineto\n", output );
   fputs( "290.09 25.351 290.012 25.762 289.957 26.043 curveto\n", output );
   fputs( "289.898 26.328 289.852 26.875 289.816 27.688 curveto\n", output );
   fputs( "289.645 31.156 lineto\n", output );
   fputs( "289.578 32.516 289.074 33.426 288.129 33.891 curveto\n", output );
   fputs( "287.59 34.145 286.746 34.27 285.594 34.27 curveto\n", output );
   fputs( "279.18 34.27 lineto\n", output );
   fputs( "279.18 25.352 lineto\n", output );
   fputs( "276.363 25.392 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "279.18 43.73 moveto\n", output );
   fputs( "279.18 36.621 lineto\n", output );
   fputs( "285.695 36.621 lineto\n", output );
   fputs( "287.016 36.621 288.059 36.887 288.824 37.414 curveto\n", output );
   fputs( "289.598 37.941 289.984 38.898 289.984 40.277 curveto\n", output );
   fputs( "289.984 41.758 289.445 42.766 288.371 43.305 curveto\n", output );
   fputs( "287.793 43.586 287.023 43.73 286.063 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "297.234 46.152 moveto\n", output );
   fputs( "312.398 46.152 lineto\n", output );
   fputs( "312.398 43.602 lineto\n", output );
   fputs( "299.98 43.602 lineto\n", output );
   fputs( "299.98 37.289 lineto\n", output );
   fputs( "311.465 37.289 lineto\n", output );
   fputs( "311.465 34.879 lineto\n", output );
   fputs( "299.98 34.879 lineto\n", output );
   fputs( "299.98 27.828 lineto\n", output );
   fputs( "312.613 27.828 lineto\n", output );
   fputs( "312.613 25.352 lineto\n", output );
   fputs( "297.234 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "322.355 46.152 moveto\n", output );
   fputs( "325.543 46.152 lineto\n", output );
   fputs( "333.086 25.352 lineto\n", output );
   fputs( "330 25.352 lineto\n", output );
   fputs( "327.891 31.582 lineto\n", output );
   fputs( "319.664 31.582 lineto\n", output );
   fputs( "317.414 25.352 lineto\n", output );
   fputs( "314.523 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "323.828 43.063 moveto\n", output );
   fputs( "320.473 33.875 lineto\n", output );
   fputs( "326.984 33.875 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "335.777 46.152 moveto\n", output );
   fputs( "344.203 46.152 lineto\n", output );
   fputs( "347.066 46.152 349.281 45.137 350.859 43.105 curveto\n", output );
   fputs( "352.266 41.277 352.969 38.93 352.969 36.07 curveto\n", output );
   fputs( "352.969 33.859 352.555 31.863 351.723 30.082 curveto\n", output );
   fputs( "350.262 26.926 347.742 25.352 344.176 25.352 curveto\n", output );
   fputs( "335.777 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "338.609 43.73 moveto\n", output );
   fputs( "338.609 27.758 lineto\n", output );
   fputs( "343.637 27.758 lineto\n", output );
   fputs( "344.59  27.758 345.375 27.855 345.988 28.055 curveto\n", output );
   fputs( "347.082 28.422 347.98 29.129 348.68 30.18 curveto\n", output );
   fputs( "349.234 31.02 349.637 32.094 349.883 33.406 curveto\n", output );
   fputs( "350.023 34.191 350.094 34.918 350.094 35.586 curveto\n", output );
   fputs( "350.094 38.164 349.578 40.168 348.551 41.595 curveto\n", output );
   fputs( "347.531 43.02 345.883 43.73 343.609 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "362.641 46.152 moveto\n", output );
   fputs( "365.828 46.152 lineto\n", output );
   fputs( "373.375 25.352 lineto\n", output );
   fputs( "370.289 25.352 lineto\n", output );
   fputs( "368.176 31.582 lineto\n", output );
   fputs( "359.949 31.582 lineto\n", output );
   fputs( "357.699 25.352 lineto\n", output );
   fputs( "354.809 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "364.113 43.063 moveto\n", output );
   fputs( "360.758 33.875 lineto\n", output );
   fputs( "367.27 33.875 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "375.867 46.152 moveto\n", output );
   fputs( "384.801 46.152 lineto\n", output );
   fputs( "387.238 46.152 388.969 45.426 390 43.973 curveto\n", output );
   fputs( "390.602 43.113 390.902 42.121 390.902 40.996 curveto\n", output );
   fputs( "390.902 39.688 390.531 38.609 389.785 37.77 curveto\n", output );
   fputs( "389.398 37.324 388.84 36.919 388.113 36.551 curveto\n", output );
   fputs( "389.18 36.145 389.98 35.688 390.508 35.18 curveto\n", output );
   fputs( "391.441 34.273 391.91 33.02 391.91 31.426 curveto\n", output );
   fputs( "391.91 30.086 391.488 28.871 390.648 27.785 curveto\n", output );
   fputs( "389.395 26.164 387.398 25.352 384.66 25.352 curveto\n", output );
   fputs( "375.867 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "378.629 43.801 moveto\n", output );
   fputs( "378.629 37.355 lineto\n", output );
   fputs( "383.754 37.355 lineto\n", output );
   fputs( "384.941 37.355 385.867 37.523 386.527 37.852 curveto\n", output );
   fputs( "387.566 38.371 388.086 39.305 388.086 40.656 curveto\n", output );
   fputs( "388.086 42.016 387.535 42.934 386.43 43.406 curveto\n", output );
   fputs( "385.809 43.668 384.883 43.801 383.652 43.801 curveto\n", output );
   fputs( "closepath\n", output );
   fputs( "378.629 35.078 moveto\n", output );
   fputs( "378.629 27.758 lineto\n", output );
   fputs( "384.703 27.758 lineto\n", output );
   fputs( "386.43 27.758 387.66 28.258 388.398 29.258 curveto\n", output );
   fputs( "388.859 29.891 389.09 30.656 389.09 31.551 curveto\n", output );
   fputs( "389.09 33.063 388.418 34.09 387.066 34.637 curveto\n", output );
   fputs( "386.352 34.93 385.402 35.078 384.223 35.078 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "395.277 46.152 moveto\n", output );
   fputs( "398.098 46.152 lineto\n", output );
   fputs( "398.098 27.828 lineto\n", output );
   fputs( "408.633 27.828 lineto\n", output );
   fputs( "408.633 25.352 lineto\n", output );
   fputs( "395.277 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "411.676 46.152 moveto\n", output );
   fputs( "426.844 46.152 lineto\n", output );
   fputs( "426.844 43.602 lineto\n", output );
   fputs( "414.426 43.602 lineto\n", output );
   fputs( "414.426 37.289 lineto\n", output );
   fputs( "425.906 37.289 lineto\n", output );
   fputs( "425.906 34.879 lineto\n", output );
   fputs( "414.426 34.879 lineto\n", output );
   fputs( "414.426 27.828 lineto\n", output );
   fputs( "427.055 27.828 lineto\n", output );
   fputs( "427.055 25.352 lineto\n", output );
   fputs( "411.676 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "447.137 46.152 moveto\n", output );
   fputs( "456.492 46.152 lineto\n", output );
   fputs( "458.344 46.152 459.836 45.629 460.969 44.578 curveto\n", output );
   fputs( "462.102 43.539 462.668 42.078 462.668 40.191 curveto\n", output );
   fputs( "462.668 38.566 462.164 37.152 461.152 35.941 curveto\n", output );
   fputs( "460.145 34.742 458.59 34.145 456.492 34.145 curveto\n", output );
   fputs( "449.953 34.145 lineto\n", output );
   fputs( "449.953 25.352 lineto\n", output );
   fputs( "447.133 25.352 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "449.953 43.73 moveto\n", output );
   fputs( "449.953 36.523 lineto\n", output );
   fputs( "455.559 36.523 lineto\n", output );
   fputs( "456.824 36.523 457.848 36.793 458.633 37.328 curveto\n", output );
   fputs( "459.426 37.867 459.82 38.816 459.82 40.176 curveto\n", output );
   fputs( "459.82 41.703 459.254 42.742 458.121 43.293 curveto\n", output );
   fputs( "457.5 43.586 456.645 43.73 455.559 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "466.547  46.152 moveto\n", output );
   fputs( "476.176 46.152 lineto\n", output );
   fputs( "477.762 46.152 479.07  45.918 480.098 45.457 curveto\n", output );
   fputs( "482.055 44.57 483.031 42.934 483.031 40.543 curveto\n", output );
   fputs( "483.031 39.297 482.77 38.277 482.25 37.484 curveto\n", output );
   fputs( "481.742 36.691 481.023 36.055 480.098 35.574 curveto\n", output );
   fputs( "480.91 35.242 481.52 34.809 481.926 34.273 curveto\n", output );
   fputs( "482.34 33.734 482.57 32.859 482.617 31.652 curveto\n", output );
   fputs( "482.719 28.863 lineto\n", output );
   fputs( "482.746 28.07 482.813 27.48 482.918 27.094 curveto\n", output );
   fputs( "483.086 26.43 483.387 26.008 483.824 25.816 curveto\n", output );
   fputs( "483.824 25.352 lineto\n", output );
   fputs( "480.367 25.352 lineto\n", output );
   fputs( "480.273 25.531 480.199 25.762 480.141 26.043 curveto\n", output );
   fputs( "480.086 26.328 480.039 26.875 480 27.688 curveto\n", output );
   fputs( "479.832 31.156 lineto\n", output );
   fputs( "479.766 32.516 479.258 33.426 478.313 33.891 curveto\n", output );
   fputs( "477.777 34.145 476.934 34.27 475.781 34.27 curveto\n", output );
   fputs( "469.367 34.27 lineto\n", output );
   fputs( "469.367 25.352 lineto\n", output );
   fputs( "466.547 25.392 lineto\n", output );
   fputs( "closepath\n", output );
   fputs( "469.367 43.73 moveto\n", output );
   fputs( "469.367 36.621 lineto\n", output );
   fputs( "475.879 36.621 lineto\n", output );
   fputs( "477.199 36.621 478.242 36.887 479.008 37.414 curveto\n", output );
   fputs( "479.781 37.941 480.168 38.898 480.168 40.277 curveto\n", output );
   fputs( "480.168 41.758 479.633 42.766 478.555 43.305 curveto\n", output );
   fputs( "477.98 43.586 477.211 43.73 476.246 43.73 curveto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "487.789 46.152 moveto\n", output );
   fputs( "490.633 46.152 lineto\n", output );
   fputs( "490.633 25.352 lineto\n", output );
   fputs( "487.789 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "495.207 46.152 moveto\n", output );
   fputs( "498.535 46.152 lineto\n", output );
   fputs( "509.043 29.301 lineto\n", output );
   fputs( "509.043 46.152 lineto\n", output );
   fputs( "511.719 46.152 lineto\n", output );
   fputs( "511.719 25.352 lineto\n", output );
   fputs( "508.563 25.352 lineto\n", output );
   fputs( "497.898 42.188 lineto\n", output );
   fputs( "497.898 25.352 lineto\n", output );
   fputs( "495.207 25.352 lineto\n", output );
   fputs( "closepath\n\n", output );

   fputs( "514.41 46.152 moveto\n", output );
   fputs( "531.289 46.152 lineto\n", output );
   fputs( "531.289 43.672 lineto\n", output );
   fputs( "524.277 43.672 lineto\n", output );
   fputs( "524.277 25.352 lineto\n", output );
   fputs( "521.418 25.352 lineto\n", output );
   fputs( "521.418 43.672 lineto\n", output );
   fputs( "514.41 43.672 lineto\n", output );
   fputs( "closepath\n\n", output );
   fputs( "fill\n\n", output );
   fputs( "grestore\n", output );
}
