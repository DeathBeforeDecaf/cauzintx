#include <sys/types.h> // struct _stat
#include <sys/stat.h>  // stat()#include "support.h"
#ifndef _DOS
#include <dirent.h>  // DIR
#include <errno.h>
#include <regex.h>  // regcomp(), regexec(), regfree()
#include <sys/ioctl.h>
#include <termios.h>
#endif

#include "platform.h"
#include "support.h"

#ifndef _DOS
extern struct termios* sysTermSet;
#endif

#define IDENTITY_STR "The New Laser Archivist"
#define VERSION_STR "0.0.1"

char identity[ 64 ];

const char hzwspace[] =
{
   ' ',
   '\t',
   '\0'
};

const uint8_t hzwspace_count = sizeof( hzwspace ) / sizeof( hzwspace[ 0 ] ) - 1;

void generateIdentity( char* filename )
{
   uint32_t versionID;

#ifdef _DOS
   struct _stat status = { 0 };

   if ( ( 0 == _stat( filename, &status ) ) && ( status.st_mode & _S_IFREG ) )
   {
      versionID = status.st_mtime;
#else
   struct stat status = { 0 };

   if ( ( 0 == stat( filename, &status ) ) && ( status.st_mode & S_IFREG ) )
   {
      versionID = status.st_mtimespec.tv_nsec;
#endif
      versionID /= status.st_size;

      sprintf( identity, IDENTITY_STR " - " VERSION_STR " (%04X)", versionID, stdout );
   }

   printIdentity();
}

void printIdentity()
{
   fputs( identity, stdout );
   fputs( LNFEED, stdout );
}

char prompt( char* promptStr, char option[], unsigned char optionCount, char* optionDefault )
{
   char result = '\0';

   char selection;
   char extended;

   char choice;

   unsigned char i;

#ifdef _DOS
   // display choices
   fputs( promptStr, stdout );

   fflush( stdout );
#else
   int status;

   // display choices
   fputs( promptStr, stdout );

   fflush( stdout );

   while ( ( -1 == ( status = ttySetRaw( STDIN_FILENO ) ) ) && ( EINTR == errno ) )
      continue;
#endif

   // absorb pending key press before prompt
   while ( kbhit() )
   {
#ifdef _DOS
      selection = getch();

      if ( 0 == selection )
      {
         extended = getch();
      }
#else
      selection = getchar();
#endif
   }

   do
   {


#ifdef _DOS
      selection = getch();

      if ( 0 == selection )
      {
         extended = getch();
      }
#else
      selection = getchar();

      extended = ' ';
#endif

      choice = ( 0 < selection ) ? selection : extended;

      // keypress
      for ( i = 0; i < optionCount; i++ )
      {
         if ( option[ i ] == choice )
         {
            result = choice;

            break;
         }
      }

      // was the any key pressed?
      if ( ( '\0' == result ) && ( NULL != optionDefault ) )
      {
         result = *optionDefault;
      }

      // should a default selection be displayed? => y
      if ( ( 32 < result ) && ( 127 > result ) )
      {
         fputc( result, stdout );
      }

      fputs( LNFEED, stdout );

      if ( '\0' == result )
      {
         // display choices
         fputs( promptStr, stdout );

         fflush( stdout );
      }
   }
   while ( '\0' == result );

#ifndef _DOS
   if ( NULL != sysTermSet )
   {
      if ( -1 == tcsetattr( STDIN_FILENO, TCSAFLUSH, sysTermSet ) )
      {
         errorExit( "tcsetattr", __LINE__, __FILE__ );
      }
   }
#endif

   return result;
}


char* trim( char* input, const char cutChar[], unsigned char cutCharSize )
{
   char* result = input;

   unsigned char i;

   char* index;

   if ( ( NULL != input ) && ( '\0' != *input ) )
   {
      index = input;

      do
      {
         for ( i = 0; i < cutCharSize; i++ )
         {
            if ( cutChar[ i ] == *index )
            {
               result = index + 1;

               break;
            }
         }

         ++index;
      }
      while ( ( index == result ) && ( '\0' != *index ) );

      if ( '\0' != *index )
      {
         index = input + 1;

         while ( '\0' != *index ) { ++index; }

         i = 0;

         do
         {
            --index;

            for ( i = 0; i < cutCharSize; i++ )
            {
               if ( cutChar[ i ] == *index )
               {
                  *index = '\0';

                  break;
               }
            }
         }
         while ( ( '\0' == *index ) && ( index != input ) );
      }
   }

   return result;
}


char* ltrim( char* input, const char cutChar[], unsigned char cutCharSize )
{
   char* result = input;

   unsigned char i;

   char* index;

   if ( ( NULL != input ) && ( '\0' != *input ) )
   {
      index = input;

      do
      {
         for ( i = 0; i < cutCharSize; i++ )
         {
            if ( cutChar[ i ] == *index )
            {
               result = index + 1;

               break;
            }
         }

         ++index;
      }
      while ( ( index == result ) && ( '\0' != *index ) );
   }

   return result;
}


void rtrim( char* input, const char cutChar[], unsigned char cutCharSize )
{
   char* index;

   uint8_t i;

   if ( ( NULL != input ) && ( '\0' != *input ) )
   {
      index = input + 1;

      while ( '\0' != *index ) { ++index; }

      i = 0;

      do
      {
         --index;

         for ( i = 0; i < cutCharSize; i++ )
         {
            if ( cutChar[ i ] == *index )
            {
               *index = '\0';

               break;
            }
         }
      }
      while ( ( '\0' == *index ) && ( index != input ) );
   }
}


#ifdef _DOS
bool appendFilesInDirectory( char* path, char* wildcard, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct _find_t  allContent  = { 0 };

   struct _stat status = { 0 };

   char* workingPath;

//   char* index;

   struct InputFileType* file;

   bool result = true;

   size_t length = strlen( path );

   workingPath = ( char* )malloc( _MAX_PATH + 1 );

   strcpy( workingPath, path );

   if ( NULL != wildcard )
   {
      strcpy( workingPath + length, wildcard );
   }
   else
   {
      strcpy( workingPath + length, "\\*.*" );
   }

   // list all matching directory content
   if ( 0 == _dos_findfirst( workingPath, _A_RDONLY, &allContent ) )
   {
      strcpy( workingPath + length + 1, allContent.name );

      // what's in the box?
      if ( ( 0 == _stat( workingPath, &status ) ) && ( status.st_mode & _S_IFREG ) )
      {
         if ( ( '.' != workingPath[ 0 ] ) || ( '\\' != workingPath[ 1 ] ) )
         {
            file = InputFile_initialize( workingPath, status.st_size, status.st_atime );

            if ( NULL != file )
            {
               if ( NULL == insertOffset )
               {
                  CZFList_insertAtTail( file );
               }
               else if ( CZFList_insertBefore( *insertOffset, file ) )
               {
                  ++( *insertOffset );
               }
            }
         }
         else
         {
            file = InputFile_initialize( workingPath + 2, status.st_size, status.st_atime );

            if ( NULL != file )
            {
               if ( NULL == insertOffset )
               {
                  CZFList_insertAtTail( file );
               }
               else if ( CZFList_insertBefore( *insertOffset, file ) )
               {
                  ++( *insertOffset );
               }
            }
         }
      }

      while ( 0 == _dos_findnext( &allContent ) )
      {
         strcpy( workingPath + length + 1, allContent.name );

         // what's in the box??
         if ( ( 0 == _stat( workingPath, &status ) ) && ( status.st_mode & _S_IFREG ) )
         {
            if ( ( '.' != workingPath[ 0 ] ) || ( '\\' != workingPath[ 1 ] ) )
            {
               file = InputFile_initialize( workingPath, status.st_size, status.st_atime );

               if ( NULL != file )
               {
                  if ( NULL == insertOffset )
                  {
                     CZFList_insertAtTail( file );
                  }
                  else if ( CZFList_insertBefore( *insertOffset, file ) )
                  {
                     ++( *insertOffset );
                  }
               }
            }
            else
            {
               file = InputFile_initialize( workingPath + 2, status.st_size, status.st_atime );

               if ( NULL != file )
               {
                  if ( NULL == insertOffset )
                  {
                     CZFList_insertAtTail( file );
                  }
                  else if ( CZFList_insertBefore( *insertOffset, file ) )
                  {
                     ++( *insertOffset );
                  }
               }
            }
         }
      }
   }

   // search all subdirectory content for wildcard match
   if ( true == searchSubdirectories )
   {
      strcpy( workingPath + length, "\\*.*" );

      if ( 0 == _dos_findfirst( workingPath, _A_SUBDIR, &allContent ) )
      {
         if ( ( '.' != allContent.name[ 0 ] )
              || ( ( '\0' != allContent.name[ 1 ] )
                   && ( ( '.' != allContent.name[ 1 ] ) || ( '\0' != allContent.name[ 2 ] ) ) ) )
         {
            strcpy( workingPath + length + 1, allContent.name );

            // what's in the box???
            if ( 0 == _stat( workingPath, &status ) )
            {
               if ( ( status.st_mode & _S_IFDIR ) && ( status.st_mode & _S_IEXEC ) )

               {
                  if ( ( '.' != workingPath[ 0 ] ) || ( '\\' != workingPath[ 1 ] ) )
                  {
                     result &= appendFilesInDirectory( workingPath, wildcard, insertOffset, searchSubdirectories );
                  }
                  else
                  {
                     result &= appendFilesInDirectory( workingPath + 2, wildcard, insertOffset, searchSubdirectories );
                  }
               }
            }
         }

         while ( 0 == _dos_findnext( &allContent ) )
         {
            if ( '.' == allContent.name[ 0 ] )
            {
               if ( ( '\0' == allContent.name[ 1 ] )
                 || ( ( '.' == allContent.name[ 1 ] ) && ( '\0' == allContent.name[ 2 ] ) ) )
               {
                  continue; // to ignore . and .. entries
               }
            }

            strcpy( workingPath + length + 1, allContent.name );

            // what's in the box????
            if ( 0 == _stat( workingPath, &status ) )
            {
               if ( ( status.st_mode & _S_IFDIR ) && ( status.st_mode & _S_IEXEC ) )
               {
                  if ( ( '.' != workingPath[ 0 ] ) || ( '\\' != workingPath[ 1 ] ) )
                  {
                     result &= appendFilesInDirectory( workingPath, wildcard, insertOffset, searchSubdirectories );
                  }
                  else
                  {
                     result &= appendFilesInDirectory( workingPath + 2, wildcard, insertOffset, searchSubdirectories );
                  }
               }
            }
         }
      }
   }

   free( workingPath );

   return result;
}
#else
bool appendFilesInDirectory( char* path, char* wildcard, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct stat status = { 0 };

   DIR* directory = NULL;
   struct dirent* directoryEntry = NULL;

   char* workingPath;

   char* source;
   char* destination;
   char* endOfString;
   char* rhs;

   struct InputFileType* file = NULL;

   regex_t regex;

   bool result = true;

   workingPath = ( char* )malloc( _MAX_PATH + 1 );

   if ( ( '.' == *path ) && ( '/' == path[ 1 ] ) )
   {
      strcpy( workingPath, path + 2 );
   }
   else
   {
      strcpy( workingPath, path );
   }

   size_t length = strlen( workingPath );

   directory = opendir( workingPath );

   if ( NULL != directory )
   {
      if ( NULL != wildcard )
      {
         strcpy( workingPath + length, wildcard );
      }
      else
      {
         strcpy( workingPath + length, "/" "*.*" );
      }

      endOfString = workingPath + length;

      while ( '\0' != *endOfString )
      {
         ++endOfString;
      }

      // fix up '.../*.*' to mean '.../*' to match all files
      if ( ( workingPath <= ( endOfString - 3 ) ) && ( 0 == strcmp( endOfString - 3, "*.*" ) ) )
      {
         endOfString -= 2;
         *endOfString = '\0';
      }

      source = endOfString;

      while ( workingPath < source )
      {
         --source;

         switch ( *source )
         {
            case '*': // [^/]*
            {
               rhs = endOfString - 1;

               endOfString += 4;
               *endOfString = '\0';

               destination = endOfString - 1;

               while ( source < rhs )
               {
                  *destination = *rhs;

                  --rhs;
                  --destination;
               }

               memcpy( source, "[^/]*", 5 );
            }
               break;

            case '.': // \\.
            {
               rhs = endOfString - 1;

               endOfString += 1;
               *endOfString = '\0';

               destination = endOfString - 1;

               while ( source < rhs )
               {
                  *destination = *rhs;

                  --rhs;
                  --destination;
               }

               *source = '\\';
               *( source + 1 ) = '.';
            }
               break;

            case '?': // .
            {
               *source = '.';
            }
               break;
         }
      }

      if ( 0 == regcomp( &regex, workingPath, REG_ICASE | REG_NOSUB | REG_EXTENDED ) )
      {
         if ( ( '.' == *path ) && ( '/' == path[ 1 ] ) )
         {
            strcpy( workingPath, path + 2 );
         }
         else
         {
            strcpy( workingPath, path );
         }

         length = strlen( workingPath );

         workingPath[ length + 1 ] = '\0';
         workingPath[ length ] = '/';

         while ( NULL != ( directoryEntry = readdir( directory ) ) )
         {
            if ( '\0' != directoryEntry->d_name[ 0 ] )
            {
               if ( '.' != directoryEntry->d_name[ 0 ] )
               {
                  strcpy( workingPath + length + 1, directoryEntry->d_name );

                  // what's in the box?
                  if ( ( 0 == stat( workingPath, &status ) ) && ( status.st_mode & S_IFREG ) )
                  {
                     if ( 0 == regexec( &regex, workingPath, ( size_t )0, NULL, 0 ) )
                     {
                        file = InputFile_initialize( workingPath, status.st_size, status.st_atimespec.tv_sec );
                     }

                     if ( NULL != file )
                     {
                        if ( NULL == insertOffset )
                        {
                           CZFList_insertAtTail( file );
                        }
                        else if ( CZFList_insertBefore( *insertOffset, file ) )
                        {
                           ++( *insertOffset );
                        }

                        file = NULL;
                     }
                  }
               }
            }
         }

         regfree( &regex );
      }
      else
      {
         fprintf( stderr, "ERROR: Path regex compile: %s", workingPath );

         // regex compile error
         result = false;
      }

      if ( true == searchSubdirectories )
      {
         rewinddir( directory );

         while ( NULL != ( directoryEntry = readdir( directory ) ) )
         {
            if ( '\0' != directoryEntry->d_name[ 0 ] )
            {
               if ( '.' != directoryEntry->d_name[ 0 ] )
               {
                  strcpy( workingPath + length + 1, directoryEntry->d_name );

                  // what's in the box???
                  if ( 0 == stat( workingPath, &status ) )
                  {
                     if ( ( status.st_mode & S_IFDIR ) && ( status.st_mode & S_IEXEC ) )
                     {
                        if ( ( '.' != workingPath[ 0 ] ) || ( '/' != workingPath[ 1 ] ) )
                        {
                           result &= appendFilesInDirectory( workingPath, wildcard, insertOffset, searchSubdirectories );
                        }
                        else
                        {
                           result &= appendFilesInDirectory( workingPath + 2, wildcard, insertOffset, searchSubdirectories );
                        }
                     }
                  }
               }
               else if ( '/' == directoryEntry->d_name[ 1 ] )
               {
                  fputs( directoryEntry->d_name, stdout );

                  fputs( LNFEED, stdout );
               }
            }
         }
      }

      closedir( directory );
   }

   free( workingPath );

   return result;
}
#endif


#ifdef _DOS
bool appendInputFiles( char* inputSource, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct _stat status = { 0 };

   char* backslash = NULL;
   char* wildcard = NULL;

   uint16_t wildcardCounter = 0;

   size_t length = 0;

   char* index = inputSource;

   struct InputFileType* file;

   bool result = true;

   while ( *index )
   {
      if ( '\\' == *index )
      {
         backslash = index;

         wildcardCounter = 0;
      }
      else if ( ( '*' == *index ) || ( '?' == *index ) )
      {
         ++wildcardCounter;
      }

      ++index;
   }

   length = index - inputSource;

   if ( 0 < wildcardCounter )
   {
      if ( NULL != backslash )
      {
         // ...\foo*.ext
         wildcard = malloc( length + 1 );

         strcpy( wildcard, backslash );

         *backslash = '\0';
      }
      else // foo*.ext
      {
         wildcard = malloc( length + 2 );

         wildcard[ 0 ] = '\\';

         strcpy( wildcard + 1, inputSource );

         inputSource[ 0 ] = '.';
         inputSource[ 1 ] = '\0';
      }
   }

   if ( 0 == wildcardCounter )
   {
      if ( 0 == _stat( inputSource, &status ) )
      {
         if ( status.st_mode & _S_IFREG )
         {
            file = InputFile_initialize( inputSource, status.st_size, status.st_atime );

            if ( NULL != file )
            {
               if ( NULL == insertOffset )
               {
                  CZFList_insertAtTail( file );
               }
               else if ( CZFList_insertBefore( *insertOffset, file ) )
               {
                  ++( *insertOffset );
               }
            }
         }
         else if ( ( status.st_mode & S_IFDIR ) && ( status.st_mode & S_IEXEC ) )
         {
            result &= appendFilesInDirectory( inputSource, wildcard, insertOffset, searchSubdirectories );
         }
      }
      else
      {
         result = false;
      }
   }
   else
   {
      result &= appendFilesInDirectory( inputSource, wildcard, insertOffset, searchSubdirectories );
   }

   if ( NULL != wildcard )
   {
      free( wildcard );

      wildcard = NULL;
   }

   return result;
}
#else
bool appendInputFiles( char* inputSource, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct stat status = { 0 };

   char* backslash = NULL;
   char* wildcard = NULL;

   uint16_t wildcardCounter = 0;

   size_t length = 0;

   char* index = inputSource;

   struct InputFileType* file;

   bool result = true;

   while ( *index )
   {
      if ( '/' == *index )
      {
         backslash = index;

         wildcardCounter = 0;
      }
      else if ( ( '*' == *index ) || ( '?' == *index ) )
      {
         ++wildcardCounter;
      }

      ++index;
   }

   length = index - inputSource;

   if ( 0 < wildcardCounter )
   {
      if ( NULL != backslash )
      {
         // ...\foo*.ext
         wildcard = malloc( length + 1 );

         strcpy( wildcard, backslash );

         *backslash = '\0';
      }
      else // foo*.ext
      {
         wildcard = malloc( length + 2 );

         wildcard[ 0 ] = '/';

         strcpy( wildcard + 1, inputSource );

         inputSource[ 0 ] = '.';
         inputSource[ 1 ] = '\0';
      }
   }

   if ( 0 == wildcardCounter )
   {
      if ( 0 == stat( inputSource, &status ) )
      {
         if ( status.st_mode & S_IFREG )
         {
            file = InputFile_initialize( inputSource, status.st_size, status.st_atimespec.tv_sec );

            if ( NULL != file )
            {
               if ( NULL == insertOffset )
               {
                  CZFList_insertAtTail( file );
               }
               else if ( CZFList_insertBefore( *insertOffset, file ) )
               {
                  ++( *insertOffset );
               }
            }
         }
         else if ( ( status.st_mode & S_IFDIR ) && ( status.st_mode & S_IEXEC ) )
         {
            result &= appendFilesInDirectory( inputSource, wildcard, insertOffset, searchSubdirectories );
         }
      }
      else
      {
         result = false;
      }
   }
   else
   {
      result &= appendFilesInDirectory( inputSource, wildcard, insertOffset, searchSubdirectories );
   }

   if ( NULL != wildcard )
   {
      free( wildcard );

      wildcard = NULL;
   }

   return result;
}
#endif



bool CZFListDataType_pathLessThan( CZFListDataType lhs, CZFListDataType rhs )
{
   return ( stricmp( lhs->path, rhs->path ) < 0 );
}


bool strtobool( char* input, char** mark )
{
   char* index = input;

   size_t length;

   bool result = false;

   if ( NULL != mark )
   {
      *mark = input;
   }

   if ( NULL != index )
   {
      index = trim( index, hzwspace, hzwspace_count );

      length = strlen( index );

      if ( 1 < length )
      {
         switch( length )
         {
            case 5:
               if ( ( 0 == stricmp( index, "false" ) ) && ( NULL != mark ) )
               {
                  *mark = index + 5;
               }
               break;

            case 4:
               if ( 0 == stricmp( index, "true" ) )
               {
                  if ( NULL != mark )
                  {
                     *mark = index + 4;
                  }

                  result = true;
               }
               break;

            case 3:
               if ( ( 0 == stricmp( index, "off" ) ) && ( NULL != mark ) )
               {
                  *mark = index + 3;
               }
               else if ( 0 == stricmp( index, "yes" ) )
               {
                  if ( NULL != mark )
                  {
                     *mark = index + 3;
                  }

                  result = true;
               }
               break;

            case 2:
               if ( ( 0 == stricmp( index, "no" ) ) && ( NULL != mark ) )
               {
                  *mark = index + 2;
               }
               else if ( 0 == stricmp( index, "on" ) )
               {
                  if ( NULL != mark )
                  {
                     *mark = index + 2;
                  }

                  result = true;
               }
               break;
         }
      }
      else if ( 1 == length )
      {
         switch( *index )
         {
            case 'T':
            case 'Y':
            case 't':
            case 'y':
            case '1':
            {
               if ( NULL != mark )
               {
                  *mark = index + 1;
               }

               result = true;
            }
            break;

            case 'F':
            case 'f':
            case 'N':
            case 'n':
            case '0':
            {
               if ( NULL != mark )
               {
                  *mark = index + 1;
               }
            }
            break;

         }
      }
   }

   return result;
}
