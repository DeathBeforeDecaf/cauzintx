#include "layout.h"

const char* linearUnitsName[] =
{
   "",
   "inch",
   "mm",
   NULL
};

const uint16_t linearUnitsCount = sizeof( linearUnitsName ) / sizeof( linearUnitsName[ 0 ] ) - 1;


// Media Layout
/////////////////////////////////////////////////////////////////////////////////

const char* pageName[] =
{
   "letter",  // ansi a
   "legal",
   "tabloid", // ansi b, ledger
   "custom1",
/*
   "a4",
   "a3",
   "custom2",
*/
   NULL
};

const uint16_t pageCount = sizeof( pageName ) / sizeof( pageName[ 0 ] ) - 1;

// Layout of Published Page

// /pub=letter, legal, tabloid, custom1, a4, a3, custom2
const struct PublishedPageType publishStandard[] =
{
   {             // letter
      unit_inch,
      8.5f,
      11.0f,
      {
         0.5f,
         0.75f,
         0.75f,
         0.5f
      }
   },
   {             // legal
      unit_inch,
      8.5f,
      14.0f,
      {
         0.5f,
         0.75f,
         0.75f,
         0.5f
      }
   },
   {             // tabloid
      unit_inch,
      11.0f,
      17.0f,
      {
         0.5f,
         0.75f,
         0.75f,
         0.5f
      }
   },
   {             // custom1
      unit_inch,
      8.5f,
      11.0f,
      {
         0.75f,
         0.75f,
         0.75f,
         0.5f
      }
   },
   {             // a4
      unit_mm,
      210.0f,
      297.0f,
      {
         19.0f,
         19.0f,
         19.0f,
         12.5f
      }
   },
   {             // a3
      unit_mm,
      297.0f,
      420.0f,
      {
         19.0f,
         19.0f,
         19.0f,
         12.5f
      }
   },
   {             // custom2
      unit_mm,
      210.0f,
      297.0f,
      {
         19.0f,
         19.0f,
         19.0f,
         12.5f
      }
   },
};

// Layout of Printed Page (before reduction?)

// /prt=letter,legal,tabloid,a4,a3
const struct PrintedPageType printStandard[] =
{
   {              // letter
      unit_inch,
      8.5f,
      11.0f,
      {         // keepout (device limits)
         0.25f,
         0.25f,
         0.25f,
         0.25f
      },
      sheet,
      {
         600,
         600
      }
   },
   {              // legal
      unit_inch,
      8.5f,
      14.0f,
      {
         0.25f,
         0.25f,
         0.25f,
         0.25f
      },
      sheet,
      {
         600,
         600
      }
   },
   {              // ledger
      unit_inch,
      11.0f,
      17.0f,
      {
         0.25f,
         0.25f,
         0.25f,
         0.25f
      },
      sheet,
      {
         600,
         600
      }
   },
   {              // custom1
      unit_inch,
      8.5f,
      11.0f,
      {
         0.25f,
         0.25f,
         0.25f,
         0.25f
      },
      sheet,
      {
         600,
         600
      }
   },
   {              // a4
      unit_mm,
      210.0f,
      297.0f,
      {
         0.635f,
         0.635f,
         0.635f,
         0.635f
      },
      sheet,
      {           // dpcm
         236,
         236
      }
   },
   {              // a3
      unit_mm,
      297.0f,
      420.0f,
      {
         0.635f,
         0.635f,
         0.635f,
         0.635f
      },
      sheet,
      {           // dpcm
         236,
         236
      }
   },
   {              // custom2
      unit_mm,
      210.0f,
      297.0f,
      {
         0.635f,
         0.635f,
         0.635f,
         0.635f
      },
      sheet,
      {           // dpcm
         236,
         236
      }
   },
};
