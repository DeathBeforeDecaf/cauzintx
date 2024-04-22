#ifndef TXREDUCE_H
#define TXREDUCE_H

#include "layout.h" // calculateStripHeight()
#include "platform.h"
#include "settings.h"  // struct SystemSettings, struct StripLayoutType
#include "stdtypes.h"
#include "support.h"

/// TXReduce.h : Render bitfields scaled for photographic reduction, with given
///              ink spread index into postscript for continuous roll printing
///////////////////////////////////////////////////////////////////////////////

// for a given published strip height, generate data density table based
// on specific line height (vsync) and number of nibbles per line (hsync).

struct DataDensityEntry
{
   uint8_t hsync; // 0x04 - 0x0C (4 - 12 nibbles => 2 to 6 bytes)
   uint8_t vsync; // 0x40 - 0x80 (high to low) limits: 0x20 - 0xFF

   uint16_t lineCountPerStrip;
   uint16_t byteCountPerStrip;

   uint16_t lineCountPerWatermarkedStrip;
   uint16_t byteCountPerWatermarkedStrip;

   double bitWidth;
   double bitHeight;
};

void populateDataDensityTable( struct StripLayoutType );

void displayDataDensityTable();

// bool generatePostscriptOutputForReduction( struct MetaStripType* lStrip, uint8_t stripNumber, uint8_t stripCount );

extern struct DataDensityEntry density[];

#endif
