#ifndef BITFLD32_H
#define BITFLD32_H

#include <memory.h>  // memset()
#include <stdio.h>   // fprintf(), fopen(), fclose()
#include <stdlib.h>  // NULL, malloc(), free()
#include <sys/stat.h>
#include <sys/types.h>

#include "platform.h"
#include "stdtypes.h"

struct BitField32Type
{
   uint16_t rowSize;
   uint16_t colSize;

   size_t bucketCount;

   uint32_t mask;
   uint32_t bitCount;
   uint32_t bitStore[];  // Row-major order
};

void BitField32_initialize( struct BitField32Type** field, uint16_t rowCount, uint16_t colCount );
void BitField32_relinquish( struct BitField32Type** field );

bool BitField32_save( struct BitField32Type* field, char* filePath );
bool BitField32_load( struct BitField32Type* field, char* filePath );

bool BitField32_get( struct BitField32Type* field, uint16_t row, uint16_t col, uint8_t* bitOut );
bool BitField32_put( struct BitField32Type* field, uint16_t row, uint16_t col, uint8_t bitIn );
bool BitField32_set( struct BitField32Type* field, uint16_t row, uint16_t col );
bool BitField32_clear( struct BitField32Type* field, uint16_t row, uint16_t col );

void BitField32_not( struct BitField32Type* field );

void BitField32_empty( struct BitField32Type* field );

#endif
