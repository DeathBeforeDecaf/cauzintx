// Platform-Specific implementation (DOS)

#if defined(_DOS) || defined(__MSDOS__)

#include "pformdos.h"

float strtof( char* inputStr, char** mark )
{
   if ( NULL != mark )
   {
      *mark = inputStr;

      // [whitespace] [sign] [digits] [.digits] [ {d | D | e | E }[sign]digits]
      while ( ( ' ' == **mark ) || ( '\t' == **mark ) )
      {
         ++( *mark );
      }

      if ( ( '-' == **mark ) || ( '+' == **mark ) )
      {
         ++( *mark );
      }

      while ( ( '0' <= **mark ) && ( '9' >= **mark ) )
      {
         ++( *mark );
      }

      if ( '.' == **mark )
      {
         ++( *mark );

         while ( ( '0' <= **mark ) && ( '9' >= **mark ) )
         {
            ++( *mark );
         }
      }

      if ( ( 'E' == **mark ) || ( 'e' == **mark ) || ( 'd' == **mark ) || ( 'D' == **mark ) )
      {
         ++( *mark );
      }

      if ( ( '-' == **mark ) || ( '+' == **mark ) )
      {
         ++( *mark );
      }

      while ( ( '0' <= **mark ) && ( '9' >= **mark ) )
      {
         ++( *mark );
      }
   }

   return ( float )atof( inputStr );
}


int makeDirectory( const char* pathname )
{
   return mkdir( pathname );
}



bool appendFilesInDirectory( char* path, char* wildcard, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct _find_t allContent  = { 0 };

   struct stat status = { 0 };

   char* workingPath;

   struct InputFileType* file;

   bool result = true;

   size_t length = strlen( path );

   workingPath = ( char* )malloc( FILENAME_MAX + 4 );

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
      if ( ( 0 == stat( workingPath, &status ) ) && ( status.st_mode & S_IFREG ) )
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
         if ( ( 0 == stat( workingPath, &status ) ) && ( status.st_mode & S_IFREG ) )
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
            if ( 0 == stat( workingPath, &status ) )
            {
               if ( ( status.st_mode & S_IFDIR ) && ( status.st_mode & S_IEXEC ) )

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
            if ( 0 == stat( workingPath, &status ) )
            {
               if ( ( status.st_mode & S_IFDIR ) && ( status.st_mode & S_IEXEC ) )
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
      if ( 0 == stat( inputSource, &status ) )
      {
         if ( status.st_mode & S_IFREG )
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

uint32_t getVersionID( const char* filename )
{
   uint32_t versionID = 0;

   struct stat status = { 0 };

   if ( ( 0 == stat( filename, &status ) ) && ( status.st_mode & S_IFREG ) )
   {
      versionID = status.st_mtime;

      versionID /= status.st_size;
   }

   return versionID;
}


char prompt( const char* promptStr, const char option[], unsigned char optionCount, const char* optionDefault )
{
   char result = '\0';

   char selection;
   char extended = ' ';

   char choice;

   unsigned char i;

   // display choices
   fputs( promptStr, stdout );

   fflush( stdout );

   // absorb pending key press before prompt
   while ( kbhit() )
   {
      selection = getch();

      if ( ( 0 == selection ) || ( 0xE0 == selection ) )
      {
         selection = 0;
         extended = getch();
      }
   }


   do
   {
      selection = getch();

      if ((0 == selection) || (0xE0 == selection))
      {
         selection = 0;
         extended = getch();
      }

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

   return result;
}

#if defined(__BORLANDC__)
int snprintf( char* buffer, size_t bufferSize, const char* format, ... )
{
/*
   va_list argptr;
   int result;

   va_start(argptr, format);
   result = vsprintf(buffer, format, argptr);
   va_end(argptr);

   return result;
*/

   int result;
   va_list argptr;

   FILE fakery = { 0 };

   size_t outBytes = bufferSize;

   if ( bufferSize != 0 )
   {
      if ( ( bufferSize - 1 ) > bufferSize )
      {
         errno = EFAULT;
         *buffer = '\0';

         return (EOF);
      }

      bufferSize = bufferSize - 1;
   }

   fakery.flags = _F_RDWR | _F_BUF; // File status flags
//   fakery.fd = 5;                   // File descriptor
   fakery.fd = -1;                   // File descriptor
   fakery.bsize = bufferSize;       // Buffer size
   fakery.buffer = fakery.curp = buffer; // Data transfer buffer, Current active pointer

   va_start(argptr, format);
   result = vfprintf(&fakery, format, argptr);
   if ( outBytes > 0 )
   {
      *(fakery.curp) = '\0';
   }
   va_end(argptr);

   return result;
}
#endif

void platformInitialize() { }

void platformRelinquish() { }

#endif
