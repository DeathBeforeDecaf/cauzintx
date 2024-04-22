#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdlib.h>  // NULL

#include "stdtypes.h"

enum LinearUnits
{
   linear_unit_unknown = 0,
   unit_inch,
   unit_mm
};


// Media Layout
/////////////////////////////////////////////////////////////////////////////////

struct MarginAreaType
{
   float top;
   float bottom;
   float left;
   float right;
};


// Physical properties describing the page of the finished copy
struct PublishedPageType
{
   enum LinearUnits units;

   float width;
   float height;

   struct MarginAreaType margin;
};


enum FeedType
{
   feed_unknown = 0,
   sheet,        // single sheets
   continuous    // roll/fanfold paper
};


struct PrintResolutionType
{
   uint16_t horizontal;
   uint16_t vertical;
};


// Physical properties describing the page created by the printer
// which may require photographic post-processing
struct PrintedPageType
{
   enum LinearUnits units;

   float width;
   float height;

   struct MarginAreaType margin;

   enum FeedType    feed;

   struct PrintResolutionType dots; // dpi
};


struct MediaLayoutType
{
   uint8_t lastPublishSelection;
   uint8_t lastPrintSelection;

   struct PublishedPageType publish;
   struct PrintedPageType print;
};


extern const char* linearUnitsName[];
extern const uint16_t linearUnitsCount;

extern const char* pageName[];
extern const uint16_t pageCount;

extern const struct PublishedPageType publishStandard[];
extern const struct PrintedPageType printStandard[];

/// Architectural Sizes
// Size      Width x Height (mm)    Width x Height (in)    Aspect Ratio
// Arch A      229 x 305 mm           9.0 x 12.0 in            4:3
// Arch B      305 x 457 mm          12.0 x 18.0 in            3:2
// Arch C      457 x 610 mm          18.0 x 24.0 in            4:3
// Arch D      610 x 914 mm          24.0 x 36.0 in            3:2
// Arch E      914 x 1219 mm         36.0 x 48.0 in            4:3
// Arch E1     762 x 1067 mm         30.0 x 42.0 in            7:5

/// Ansi Sizes
// Size    Width x Height (mm)    Width x Height (in)    Aspect Ratio    Nearest ISO
// A         216 x 279 mm           8.5 x 11.0 in          1:1.2941          A4
// B         279 x 432 mm          11.0 x 17.0 in          1:1.5455          A3
// C         432 x 559 mm          17.0 x 22.0 in          1:1.2941          A2
// D         559 x 864 mm          22.0 x 34.0 in          1:1.5455          A1
// E         864 x 1118 mm         34.0 x 44.0 in          1:1.2941          A0

// see also:
//    http://www.printernational.org/american-paper-sizes.php
//    http://www.printernational.org/iso-paper-sizes.php
//    http://www.printernational.org/english-paper-sizes.php


#endif
