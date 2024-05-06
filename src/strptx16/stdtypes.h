#ifndef STDTYPES_H
#define STDTYPES_H

#if defined(_DOS) || defined(__MSDOS__)
#else
#include <stdbool.h> // bool, true, false
#endif

// -128 < 0 < 127
#ifndef _INT8_T
#define _INT8_T
typedef signed char           int8_t;
#endif // _INT8_T

// -32,768 < 0 < 32767
#ifndef _INT16_T
#define _INT16_T
typedef signed short         int16_t;
#endif // _INT16_T

// -2,147,483,648 < 0 < 2,147,483,647
#if defined(__LP64__) || defined(__ILP32__) || defined(_WIN64)
#ifndef _INT32_T
#define _INT32_T
typedef signed int           int32_t;
#endif // _INT32_T

#else

#ifndef _INT32_T
#define _INT32_T
typedef signed long          int32_t;
#endif // _INT32_T

#endif

// -9,223,372,036,854,775,808 < 0 < 9,223,372,036,854,775,807
#if defined(__LP64__) || defined(_WIN64)
#ifndef _INT64_T
#define _INT64_T
typedef signed long long     int64_t;
#endif // _INT64_T
#endif

// 0 < 255
#ifndef _UINT8_T
#define _UINT8_T
typedef unsigned char        uint8_t;
#endif // _UINT8_T

// 0 < 65535
#ifndef _UINT16_T
#define _UINT16_T
#ifndef __BORLANDC__
typedef unsigned short      uint16_t;
#else
typedef unsigned            uint16_t;
#endif
#endif // _UINT16_T

// 0 < 4,294,967,295
#if defined(__LP64__) || defined(__ILP32__) || defined(_WIN64)
#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned int        uint32_t;
#endif // _UINT32_T
#else
#ifndef _UINT32_T
#define _UINT32_T
typedef unsigned long       uint32_t;
#endif // _UINT32_T
#endif

// 0 < 18,446,744,073,709,551,615
#if defined(__LP64__) || defined(__ILP32__) || defined(_WIN64)
#ifndef _UINT64_T
#define _UINT64_T
typedef unsigned long long  uint64_t;
#endif // _UINT64_T
#else // 8/16 bit
#ifndef _UINT64_T
#define _UINT64_T
typedef union _little_endian_uint64_t       \
{                                           \
   struct                                   \
   {                                        \
      unsigned long             low;        \
      unsigned long             high;       \
   }                                        \
   part;                                    \
                                            \
   unsigned char           byte[ 8 ];       \
                                            \
   unsigned char          octet[ 8 ];       \
                                            \
   unsigned short        hextet[ 4 ];       \
                                            \
   unsigned short       doublet[ 4 ];       \
                                            \
   unsigned long        quadlet[ 2 ];       \
                                            \
/* unsigned long long   octlet; */          \
}                                           \
uint64_t;
#endif // _UINT64_T
#endif


// UTF8 code point
#ifndef _CHAR8_T
#define _CHAR8_T
typedef unsigned char        char8_t;
#endif // _CHAR8_T

// UTF16 code point
#ifndef _CHAR16_T
#define _CHAR16_T
typedef unsigned short      char16_t;
#endif // _CHAR16_T

// UTF32 code point
#if defined(__LP64__) || defined(__ILP32__) || defined(_WIN64)
#ifndef _CHAR32_T
#define _CHAR32_T
typedef unsigned int        char32_t;
#endif // _CHAR32_T
#else
#ifndef _CHAR32_T
#define _CHAR32_T
typedef unsigned long       char32_t;
#endif // _CHAR32_T
#endif

#if defined(_DOS) || defined(__MSDOS__)
#ifndef _BOOL_T
#define _BOOL_T
typedef unsigned char       bool;
typedef enum { false = 0, true = 1 } bool16_t;
#endif // _BOOL_T
#endif


#if defined(__LP64__) || defined(_WIN64)// 64_bit
#ifndef _FSTR64
#define _FSTR64
#define FSTR_INT8_T "d"
#define FSTR_UINT8_T "u"
#define FSTR_INT16_T "d"
#define FSTR_UINT16_T "u"
#define FSTR_INT32_T "d"
#define FSTR_UINT32_T "u"
#define FSTR_INT64_T "ld"
#define FSTR_UINT64_T "lu"
#define FSTR_SIZE_T "zu"
#endif // _FSTR64
#elif defined(_DOS) || defined(__MSDOS__) // 16_bit
#ifndef _FSTR16
#define _FSTR16
#define FSTR_INT8_T "d"
#define FSTR_UINT8_T "u"
#define FSTR_INT16_T "d"
#define FSTR_UINT16_T "u"
#define FSTR_INT32_T "ld"
#define FSTR_UINT32_T "lu"
#define FSTR_SIZE_T "lu"
#endif // _FSTR16
#else // 32 bit (no 16 bit windows by default) 
#ifndef _FSTR32
#define _FSTR32
#define FSTR_INT8_T "d"
#define FSTR_UINT8_T "u"
#define FSTR_INT16_T "d"
#define FSTR_UINT16_T "u"
#define FSTR_INT32_T "d"
#define FSTR_UINT32_T "u"
#define FSTR_INT64_T "ld"
#define FSTR_UINT64_T "lu"
#define FSTR_SIZE_T "zu"
#endif // _FSTR32
#endif

#endif
