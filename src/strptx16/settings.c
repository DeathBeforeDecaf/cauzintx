#include "settings.h"

const char whitespace[] =
{
   ' ',
   '\t',
   '\r',
   '\n',
};

const uint8_t ws_count = sizeof( whitespace ) / sizeof( whitespace[ 0 ] );


struct SystemSettings settings =
{
   1,                                                        // isInteractive
   0,                                                        // searchRecursively
   direct_render,
   {                                                         // StripLayoutType
      unit_inch,
      {                                                      // AlignmentMarks
         ( 2.0f + 3.0f ) / 32.0f, // top: 1/16 + 3/32
         ( 8.0f + 1.0f ) / 32.0f, // bottom: 1/4 + 1/32
         1.0f + 5.0f / 16.0f,     // left
         0.11f                    // right: minimum?
      },
      {  // ~ 1/4" total                                     // StripHeader
         0.05f,                   // top_m
         0.07f,                   // hsync_h
         0.13f                    // vsync_h
      },
      {                                                      // StripFooter
         0.08f,                   // top_m                   // top of bottom
         0.15f,                   // wmark_h                 // (optional)
         0.08f                    // bottom_m                // (only with wmark)
      },
      0.620f,                     // MIN_PUBLISHED_WIDTH
      0.668f,                     // MAX_PUBLISHED_WIDTH
      0.639089f,                  // apparent_width (weirdly specific)
      0.0f                        // apparent_height (to be calculated)
   },
   {                                                         // PageLayoutType
      1.0f,                       // reductionFactor (direct_render)
      0.0f,                       // inkSpreadIndex (direct_render)

      9.0f,                       // MAX_APERTURE_SIZE_INCHES,
                                  //   (8.75f probably closer to reality)

      // postscript strip horizontal placement
      131U,                       // FIRST_STRIP_WIDTH_POINTS
      57U,                        // NEXT_STRIP_WIDTH_POINTS
   },
   { 0 },                                                    // MediaLayoutType
   {
      { 'S', 'T', 'R', 'P', 'I', 'D', '\0' }, // stripID
      { 'o', 'u', 't', 'p', 'u', 't', '.', 'p', 's', '\0' }, // outputFilename
      NULL,                       // verboseDescription
      { 0 },                      // encoding
      5UL * 1024UL * 127UL        // MAX_STRIP_SEQUENCE_BYTES: ~127 5K strips
   },
   { 0 }                          // workingDirectory
};
