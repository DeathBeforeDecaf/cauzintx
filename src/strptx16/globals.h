#ifndef GLOBALS_H
#define GLOBALS_H

#include "stdtypes.h"

#define IDENTITY_STR "The New Laser Archivist"
#define VERSION_STR  "0.0.2"

#define ISO8601_STR  "%04d-%02d-%02dT%02d:%02d:%02d"

// 127 strips per valid sequence
#define SEQUENCE_LIMIT 127

extern const uint32_t MAX_SEQUENCE_SIZE;
extern const uint32_t MIN_SEQUENCE_SIZE;
extern const uint32_t DEFAULT_SEQUENCE_LIMIT;

// physical limit for calculating max strip length (if page size is larger)
extern const float READER_MIN_APERTURE_SIZE_INCHES;
extern const float READER_MAX_APERTURE_SIZE_INCHES;

// observed limit for calculating valid direct rendering dibits
extern const float PUBLISHED_STRIP_MIN_SIZE_INCHES;
extern const float PUBLISHED_STRIP_MAX_SIZE_INCHES;

// media size limits
extern const float PUBLISHED_HEIGHT_MIN_SIZE_INCHES;
extern const float PUBLISHED_HEIGHT_MAX_SIZE_INCHES;
extern const float PUBLISHED_WIDTH_MIN_SIZE_INCHES;
extern const float PUBLISHED_WIDTH_MAX_SIZE_INCHES;

extern const float PRINT_HEIGHT_MIN_SIZE_INCHES;
extern const float PRINT_HEIGHT_MAX_SIZE_INCHES;
extern const float PRINT_WIDTH_MIN_SIZE_INCHES;
extern const float PRINT_WIDTH_MAX_SIZE_INCHES;

extern const char* executableExtension[];
extern const uint16_t executableExtensionCount;

extern const char* binaryExtension[];
extern const uint16_t binaryExtensionCount;

extern const char* textExtension[];
extern const uint16_t textExtensionCount;

extern const char hzWSpace[];
extern const uint8_t hzWSpaceCount;

extern const char whitespace[];
extern const uint8_t whitespaceCount;

#endif
