#include "globals.h"

const uint32_t MAX_SEQUENCE_SIZE =  10UL * 1024UL * 1024UL;
const uint32_t MIN_SEQUENCE_SIZE = 16UL * 1024UL;

// ~127 x 5K strips
const uint32_t DEFAULT_SEQUENCE_LIMIT = 5UL * 1024UL * 127UL;

// limit for calculating max strip length (if page size is larger)
const float READER_MIN_APERTURE_SIZE_INCHES = 2.99f;
const float READER_MAX_APERTURE_SIZE_INCHES = 10.01f;

// observed limit for calculating valid direct rendering dibits
const float PUBLISHED_STRIP_MIN_SIZE_INCHES = 0.620f;
const float PUBLISHED_STRIP_MAX_SIZE_INCHES = 0.668f;

// media size limits
const float PUBLISHED_HEIGHT_MIN_SIZE_INCHES = 2.0f;
const float PUBLISHED_HEIGHT_MAX_SIZE_INCHES = 120.0f;
const float PUBLISHED_WIDTH_MIN_SIZE_INCHES = 2.0f;
const float PUBLISHED_WIDTH_MAX_SIZE_INCHES = 120.0f;

const float PRINT_HEIGHT_MIN_SIZE_INCHES = 2.0f;
const float PRINT_HEIGHT_MAX_SIZE_INCHES = 120.0f;
const float PRINT_WIDTH_MIN_SIZE_INCHES = 2.0f;
const float PRINT_WIDTH_MAX_SIZE_INCHES = 120.0f;

const char* executableExtension[] =
{
   "BAT",
   "COM",
   "EXE",
};

const uint16_t executableExtensionCount = sizeof( executableExtension ) / sizeof( executableExtension[ 0 ] );

const char* binaryExtension[] =
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

const uint16_t binaryExtensionCount = sizeof( binaryExtension ) / sizeof( binaryExtension[ 0 ] );


const char* textExtension[] =
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

const uint16_t textExtensionCount = sizeof( textExtension ) / sizeof( textExtension[ 0 ] );

// trimmable whitespace
const char hzWSpace[] =
{
   ' ',
   '\t',
   '\0'
};

const uint8_t hzWSpaceCount = sizeof( hzWSpace ) / sizeof( hzWSpace[ 0 ] ) - 1;

const char whitespace[] =
{
   ' ',
   '\t',
   '\r',
   '\n',
};

const uint8_t whitespaceCount = sizeof( whitespace ) / sizeof( whitespace[ 0 ] );