#ifndef SETTINGS_H
#define SETTINGS_H

#include "globals.h"
#include "layout.h" // LinearUnits, MediaLayoutType
#include "platform.h"
#include "stdtypes.h"

extern struct SystemSettings settings;

enum OutputModeType
{
   output_unknown = 0,
   direct_render, // reductionFactor = 1.0f, printMedia = sheet
   photo_reduce   // reductionFactor = 6-12f, printMedia = continuous
};

// Published Strip Layout (in physical units)
//  1.666 inches width of first strip & alignment marks
// ~0.775 inches horizontal displacement
// ~0.775 * 2 + 0.11 + 1.66
//  0.775 = offset between strips on multistrip page
//  0.11 = strip right margin

// Strip Layout
/////////////////////////////////////////////////////////////////////////////////

struct AlignmentMarks // Extent for Strip Alignment Marks
{
   float top;    // 2/32in + 3/32in = 5/32in;
   float bottom; // 9/32in or 0?;
   float left;   // 1in + 5/16in
   float right;  // 0.33in (8.4mm)
};


struct StripHeader  // ~0.25 inches (6 mm)
{
   float top_m;    // (margin top) space before strip
   float hsync_h;  // Horizontal Sync height
   float vsync_h;  // Vertical Sync height
};


// margin top = 0.0625, 0.068, 0.06 ~ 0.0635 ~ 1/16
// wm_height = 0.161, 0.158, 0.151 ~ .1567 ~ 5/32

struct StripFooter
{
   float top_m;     // (margin top) space after strip and before watermark
   float wmark_h;   // watermark height
   float bottom_m;  // (margin bottom) space after watermark
};


struct StripLayoutType
{
// strip length = published page size - top margin - bottom margin - alignment markers - strip header
//              = 11.0" - 0.75" - 0.75" - 7/16" - (0.206 to 0.2125)"

   enum LinearUnits units;

   struct AlignmentMarks align; // Alignment Marks (dot and line)

   struct StripHeader header;  // Horizontal Sync, Vertical Sync

   struct StripFooter footer;  // Optional Watermark

   const float MIN_PUBLISHED_WIDTH; // 0.620f
   const float MAX_PUBLISHED_WIDTH; // 0.668f

   // as rendered in the publication (optionally post-processed)
   float apparentWidth;  // physical width
   float apparentHeight; // physical length
   float apparentWatermarkedHeight; // physical width minus watermark
};

struct PageLayoutType
{
   float reductionFactor;
   float inkSpreadIndex;

   float limitApertureSizeInches; // 8.75f probably closer to reality

   // postscript horizontal strip placement
   uint16_t FIRST_STRIP_WIDTH_POINTS;
   uint16_t NEXT_STRIP_WIDTH_POINTS;
};

// The SequencedEncoding structure contains the per-strip attributes
// that directly influence the strip size, such as the data density
// (hSync: nibbles per scan line) and (vSync: optical reader step count
// indicating the expected physical scan line height) and the presence
// of the optional CRC at the end of strip.  Allowing unique per-strip
// encodings permits non-uniform pagination (or uniform pagination if
// that is desired).
struct SequencedEncodingType
{
   uint8_t  override;  // 0x01 == hSync, 0x02 == vSync, 0x04 == crc, 0x08 == watermark
   uint8_t  hSync;
   uint8_t  vSync;
};

enum EncodingOverrideType
{
   override_none    = 0x00,
   strip_hsync      = 0x01,
   strip_vsync      = 0x02,
   strip_crc        = 0x04,
   strip_watermark  = 0x08
}
; // const encode;


struct StripSequenceType
{
   char stripID[ 7 ]; // first six letters of first encoded file (default)

   char outputFilename[ _MAX_PATH + 14 ]; // output.ps (default)

   char* verboseDescription;

   struct SequencedEncodingType encoding[ 1 + SEQUENCE_LIMIT ];

   uint32_t limitMaxSequenceBytes;
};

// encapsulates the data model for encoding strips from a list of files into a sequence of bitfields,
// each bitfield is inverted and rendered into a postscript bitmap to emit a page of strips within
// the output postscript file

struct SystemSettings // (1448 bytes)
{
   bool isInteractive;
   bool searchRecursively;
   bool generateWatermark;

   enum OutputModeType outputMode;

   struct StripLayoutType stripLayout;

   struct PageLayoutType pageLayout;

   struct MediaLayoutType media;

   struct StripSequenceType sequence;

   char workingDirectory[ _MAX_PATH + 14 ];
};


extern const char whitespace[];

extern const uint8_t whitespaceCount;

void calculateStripHeight( struct StripLayoutType* dfltStrip, struct MediaLayoutType media );

#endif
