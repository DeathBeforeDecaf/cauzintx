#ifndef CZSTRIP_H
#define CZSTRIP_H

#include <math.h>  // floor()

#include "globals.h"
#include "layout.h" // struct PublishedPageType, struct PrintedPageType
#include "platform.h"
#include "settings.h"
#include "stdtypes.h"
#include "txreduce.h"
#include "txstrip.h"

/// TXDirect.h : Render bitfields directly in integral, device-dependent pixel
///              resolution printing postscript bitmaps on individual sheets
///////////////////////////////////////////////////////////////////////////////

// Given a collection of common device resolutions, calculate valid hSync/vSync settings
// that divide evenly into an integral number of pixels and remain within the
// min and max values for observed (published) strip widths

enum PixelUnits
{
   pixel_unit_unknown = 0,
   dots_per_inch,
   dots_per_cm
};

struct HPixelationEntry
{
// minWidthDotRatio = MIN_PUBLISHED_WIDTH * dotsPerUnitWidth / bitsPerLine
// maxWidthDotRatio = MAX_PUBLISHED_WIDTH * dotsPerUnitWidth / bitsPerLine

   uint16_t maxDotsPerHPixel;
   uint16_t minDotsPerHPixel;
   uint16_t dotsPerHPixel;        // best approximation for pixel dot count between min/max

   float    bitWidth;            // dotsPerPixel * dotsPerUnitWidth (resolution)
   float    stripWidth;          // bitsPerLine[ hSync - 4 ] * realizedBitWidth
};


struct VPixelationEntry
{
   uint8_t  vSync;

   uint16_t dotsPerVPixel;        // best approximation for pixel dot count

   float    bitHeight;            // dotsPerPixel * dotsPerUnitHeight (resolution)
};


struct DevicePixelationRow
{
   enum PixelUnits units;

   uint16_t resolution; // dotsPerUnitWidth ( 300, 600, 1200, 2400, 4800, 9600 dpi )

   struct HPixelationEntry hPixel[ 9 ]; // nibbles per scanline, 4 nibbles = density[ 0 ]

   struct VPixelationEntry vPixel[ 8 ];
};

extern struct DevicePixelationRow pixelTable[6]; // 300, 600, 1200, 2400, 4800, 9600

extern const uint8_t pixelRowCount;

void initializeDeviceDependentPixelTable();

void nearestDeviceDependentResolution( uint16_t xDPI, uint16_t yDPI );

bool generateDirectPostscriptOutput( struct MetaStripType* lStrip, uint8_t stripNumber, uint8_t stripCount );

#endif
