#include "bitfld32.h"

void BitField32_initialize( struct BitField32Type** field, uint16_t rowCount, uint16_t colCount )
{
   size_t bucketCount;
   size_t length;

   uint32_t mask;
   uint8_t remainder;

   uint32_t bitCount = rowCount;

   bitCount *= colCount;

   mask = ( sizeof( uint32_t ) * 8 ) - 1;

   remainder = ( uint8_t )( bitCount & mask );

   bucketCount = ( 0 != remainder ) ? 1 : 0;

   if ( bitCount < ( 0xFFFFUL << 5 ) )
   {
      bucketCount += ( size_t )( bitCount >> 5 );

      length = sizeof( struct BitField32Type ) + sizeof( uint32_t ) * bucketCount;

      *field = ( struct BitField32Type* )malloc( length );

      if ( *field )
      {
         memset( *field, 0, length );

         ( *field )->rowSize = rowCount;
         ( *field )->colSize = colCount;
         ( *field )->mask = mask;
         ( *field )->bitCount = bitCount;
         ( *field )->bucketCount = bucketCount;
      }
   }
   else
   {
      fprintf( stderr, LNFEED "ERROR: Cannot allocate more than %lu bits for this implementation." LNFEED, ( 0xFFFFUL << 5 ) );

      *field = NULL;
   }
}


void BitField32_relinquish( struct BitField32Type** field )
{
   if ( field )
   {
      free( *field );

      *field = NULL;
   }
}


bool BitField32_save( struct BitField32Type* field, char* filePath )
{
   bool result = true;

   char* index = filePath;

   char subpath = '\0';

   size_t itemsWritten;

   struct stat info;

   FILE* output = NULL;

   if ( NULL != filePath )
   {
      while ( ( true == result ) && ( '\0' != *index ) )
      {
         if ( ( '\\' == *index ) || ( '/' == *index ) )
         {
            subpath = *index;

            *index = '\0';

            if ( 0 != stat( filePath, &info ) )
            {
               if ( 0 != makeDirectory( filePath ) )
               {
                   result = false;
               }
            }
            else if ( info.st_mode & S_IFREG )
            {
               result = false;
            }

            *index = subpath;

            subpath = '\0';
         }

         ++index;
      }

      if ( true == result )
      {
         if ( NULL != ( output = fopen( filePath, "wb" ) ) )
         {
            itemsWritten = fwrite( field->bitStore, sizeof( uint32_t ), field->bucketCount, output );

            if ( itemsWritten < field->bucketCount )
            {
               result = false;
            }

            fclose( output );
         }
         else
         {
            result = false;
         }
      }
   }
   else
   {
      result = false;
   }

   return result;
}

bool BitField32_load( struct BitField32Type* field, char* filePath )
{
   bool result = true;

   FILE* input = NULL;

   size_t itemsRead;

   if ( NULL != filePath )
   {
      if ( NULL != ( input = fopen( filePath, "rb" ) ) )
      {
         itemsRead = fread( field->bitStore, sizeof( uint32_t ), field->bucketCount, input );

         if ( itemsRead < field->bucketCount )
         {
            result = false;
         }

         fclose( input );

         remove( filePath );
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

   return result;
}


bool BitField32_get( struct BitField32Type* field, uint16_t row, uint16_t col, uint8_t* bitOut )
{
   uint8_t result = 5; // MSVC1.5 BUGBUG workaround

   uint32_t bucket;

   uint32_t bitOffset = row;

   if ( ( row < field->rowSize ) && ( col < field->colSize ) )
   {
      bitOffset *= field->colSize;

      bitOffset += col;

//    bucket = bitOffset >> 5;

      bucket = bitOffset >> result;

      result = ( uint8_t )( field->bitStore[ bucket ] >> ( field->mask - ( bitOffset & field->mask ) ) );

      *bitOut = 0x01 & result;

      return true;
   }

   return false;
}


bool BitField32_put( struct BitField32Type* field, uint16_t row, uint16_t col, uint8_t bitIn )
{
   if ( ( row < field->rowSize ) && ( col < field->colSize ) )
   {
      uint8_t remainder;

      uint32_t bucket;

      uint32_t bitOffset = row;

      bitOffset *= field->colSize;

      bitOffset += col;

      bucket = bitOffset >> 5;

      remainder = ( uint8_t )( bitOffset & field->mask );

      if ( 0 == bitIn )
      {
         field->bitStore[ bucket ] &= ~( 1UL << ( field->mask - remainder ) );
      }
      else
      {
         field->bitStore[ bucket ] |= 1UL << ( field->mask - remainder );
      }

      return true;
   }

   return false;
}


bool BitField32_set( struct BitField32Type* field, uint16_t row, uint16_t col )
{
   if ( ( row < field->rowSize ) && ( col < field->colSize ) )
   {
      uint8_t remainder;

      uint32_t bucket;

      uint32_t bitOffset = row;

      bitOffset *= field->colSize;

      bitOffset += col;

      bucket = bitOffset >> 5;

      remainder = ( uint8_t )( bitOffset & field->mask );

      field->bitStore[ bucket ] |= 1UL << ( field->mask - remainder );

      return true;
   }

   return false;
}


bool BitField32_clear( struct BitField32Type* field, uint16_t row, uint16_t col )
{
   if ( ( row < field->rowSize ) && ( col < field->colSize ) )
   {
      uint8_t remainder;

      uint32_t bucket;

      uint32_t bitOffset = row;

      bitOffset *= field->colSize;

      bitOffset += col;

      bucket = bitOffset >> 5;  // offset / 32

      remainder = ( uint8_t )( bitOffset & field->mask );

      field->bitStore[ bucket ] &= ~( 1UL << ( field->mask - remainder ) );

      return true;
   }

   return false;
}

// bitwise inversion (not) of bitfield, 0 is black in PostScript image
void BitField32_not( struct BitField32Type* field )
{
   size_t i;

   if ( NULL != field )
   {
      for ( i = 0; i < field->bucketCount; i++ )
      {
         field->bitStore[ i ] = ~( field->bitStore[ i ] );
      }
   }
}


void BitField32_empty( struct BitField32Type* field )
{
   memset( field->bitStore, 0, sizeof( uint32_t ) * field->bucketCount );
}
