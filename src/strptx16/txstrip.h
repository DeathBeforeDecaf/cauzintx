#ifndef TXSTRIP_H
#define TXSTRIP_H

#include <stdio.h>
#include <stdlib.h>  // malloc()
#include <string.h>  // strlen(), _strnicmp()
#include <time.h>    // time_t

#include "bitfld32.h"
#include "crc16.h"
#include "filelist.h"
#include "globals.h"
#include "platform.h"
#include "settings.h"  // struct SystemSettings
#include "stdtypes.h"
#include "txdirect.h"  // struct DevicePixelationRow, pixelRowCount
#include "txreduce.h"  // struct DataDensityEntry, populateDataDensityTable(), displayDataDensityTable()

struct FileEntryType
{
   uint8_t   category;     // 3.4.13 (1)
   uint8_t   os_type;      // 3.4.14 (1)
// uint32_t  length : 24;  // 3.4.15 (3) file length on disk(max 16,777,215 bytes)
   uint8_t   length[3];    // 3.4.15 (3) file length on disk(max 16,777,215 bytes)
   char*     name;         // 3.4.16 (1<-->n bytes) null terminated filename
   uint8_t   terminator;   // 3.4.17 (1) filename terminator, 0xFF indicates executable content
   uint8_t   adjunct_size; // 3.4.18 (1) size of file metadata trailing
   uint8_t*  adjunct;      // 3.4.18+ (0-255 bytes) metadata (expansion block)

   uint16_t  size;         // total size (in bytes) of this composite entry data
                           // 1+1+3+len(name)+1+1+len(adjunct)
};


struct InputFileType
{
   char*                  name;
   uint32_t               sizeBytes;
   time_t                 lastModified;

   struct FileEntryType   entry;

   char                   path[];
};


struct HeaderSegmentType
{
   uint8_t  checksum;         // 3.4.6  (1 byte) parity-based checksum
   char     id[ 6 ];          // 3.4.7  (6 bytes)
   uint8_t  number;           // 3.4.8  (1 byte) Sequence Number (7 bit, 1-127)
   uint8_t  type;             // 3.4.9  (1 byte) Strip Type (0x00 data strip, 0x01 key strip, 0x02-0xFF undefined)
   uint16_t attributes;       // 3.4.10 (2 byte) optional CRC enabled iff ( attributes & 0x8000 )
};


struct DataPrefixType
{
   // header fields - all strips
   uint8_t  data_sync;        // 3.4.3  (1 byte)  0x00
   uint16_t reader_expansion; // 3.4.4  (2 bytes) 0x0000
   uint16_t length;           // 3.4.5  (2 bytes -> strip length)

   struct HeaderSegmentType header;
};


struct EncodedExtentType
{
   uint8_t  rows_hSyncHeader;
   uint8_t  rows_vSyncHeader;
   uint16_t cols_stripBody;
   uint16_t rows_stripBody;
};


struct DataStripType
{
   uint8_t hSync;             // 3.4.1  0x04-0x0C (4-12 nibbles per row)
   uint8_t vSync;             // 3.4.2  0x40-0x80 (4R0*.0025") to (8R0*.0025")

   struct DataPrefixType prefix;

   uint16_t optionalCRC;      // fld20: 0 or 2 bytes

   struct EncodedExtentType extent;

   struct BitField32Type* field;

   uint8_t* buffer;

   struct DataStripType* nextStrip;
};


struct MetaPrefixType
{
   // header fields - all strips
   uint8_t  data_sync;        // 3.4.3  (1 byte)  0x00
   uint16_t reader_expansion; // 3.4.4  (2 bytes) 0x0000
   uint16_t length;           // 3.4.5  (2 bytes -> strip length)

   struct HeaderSegmentType header;

   // header fields - logical strip
   uint8_t host_os;           // 3.4.11
   uint8_t file_count;        // 3.4.12
};


struct MetaStripType
{
   uint8_t hSync;             // 3.4.1  0x04-0x0C (4-12 nibbles per row)
   uint8_t vSync;             // 3.4.2  0x40-0x80 (8R0*.0025") to (4R0*.0025")

   struct MetaPrefixType prefix;

   uint16_t optionalCRC;      // fld20: 0 or 2 bytes

   struct EncodedExtentType extent;

   struct BitField32Type* field;

   uint8_t* buffer;

   struct DataStripType* nextStrip;
};


// sequence host
enum CZNStripType
{
   cznstrip_data = 0x00,
   cznstrip_key  = 0x01, // (not supported) proprietary sequence header
}
; // const cznstrip;


// sequence hostOS
enum CZNOpSysType
{
   cznopsys_generic = 0x00,
   cznopsys_colos   = 0x01,
   cznopsys_apldos  = 0x10,
   cznopsys_prodos  = 0x11,
   cznopsys_aplcpm  = 0x12,
   cznossys_pcmsdos = 0x14,
   cznopsys_mac     = 0x15,
   cznopsys_reserved= 0x20, // accepted as PC/MS-DOS
}
; // const cznopsys;


// file content
enum CZNFileType
{
   cznfile_generic  = 0x00,  // if CZNOpSysType is generic, generic identifies file as text
   cznfile_text     = 0x01,
   cznfile_binary   = 0x02,
   cznfile_token    = 0x04,
   cznfile_compress = 0x10,
}
; // const cznfile;


enum CZNAplDOSType // clientOS: Apple DOS
{
   cznapl_text     = 0x00,
   cznapl_ibasic   = 0x01, // integer basic
   cznapl_aplsoft  = 0x02, // applesoft basic
   cznapl_binary   = 0x04,
   cznapl_obj      = 0x10, // relocatable object module file
   cznapl_a_type   = 0x20, // not supported
   cznapl_b_type   = 0x40, // not supported
}
; // const cznapldos;


enum CZNProDOSType // clientOS: Apple ProDOS
{
   cznpro_text     = 0x04,
   cznpro_binary   = 0x06,
   cznpro_ibasic   = 0xFA,
   cznpro_aplsoft  = 0xFC,
   cznpro_obj      = 0xFE,
   cznpro_sys      = 0xFF,
}
; // const cznprodos;


enum CZNMacType // clientOS: Apple Macintosh
{
   cznmac_binary   = 0x00,
   cznmac_text     = 0x01,
}
; // const cznmacdos;


enum CZNPCDOSType // clientOS: PC/MS-DOS
{
   cznpc_exe     = 0x00, // *.exe, *.com, *.bat
   cznpc_other   = 0x01, // (everything else)
}
; // const cznpcdos;


struct InputFileType* InputFile_initialize( char* filePath, uint32_t sizeBytes, time_t lastModified );

struct DataDensityEntry getDataDensity( uint8_t hSync, uint8_t vSync );

void calculateStripSequenceExtents( uint8_t* stripCount, float* lastStripHeight );

// initialize strip chain
// allocate bitfields
// encode strip data into a bitfield
// process encoded strip data into postscript output
// deallocate bitfields
// relinquish strip chain

bool initializeStripSequence( struct MetaStripType** lStrip );
bool initializeBitfield( struct MetaStripType* lStrip, uint8_t stripNumber );
bool initializeByteBuffer( struct MetaStripType* lStrip, uint8_t stripNumber );

bool encodeStripData( struct MetaStripType* lStrip, uint8_t stripNumber, uint16_t* inputFile, uint32_t* inputByteOffset );

bool saveBitfield( struct MetaStripType* lStrip, uint8_t stripNumber );
bool loadBitfield( struct MetaStripType* lStrip, uint8_t stripNumber );

bool relinquishByteBuffer( struct MetaStripType* lStrip, uint8_t stripNumber );
bool relinquishBitfield( struct MetaStripType* lStrip, uint8_t stripNumber );
void relinquishStripSequence( struct MetaStripType** lStrip );

bool renderStripSequence();

void renderWatermarkLogo( FILE* output, float xPos, float yPos, float scale );

#endif
