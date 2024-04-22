// Windows Platform-specific implementation
#if defined(_MSC_VER)
#include "pformwin.h"

int makeDirectory( const char* pathname )
{
   return mkdir( pathname );
}

bool appendFilesInDirectory( char* path, char* wildcard, uint16_t* insertOffset, bool searchSubdirectories )
{
   struct _finddata_t allContent = { 0 };

   intptr_t hSearch = -1;

   struct stat status = { 0 };

   struct InputFileType* file = NULL;

   bool result = true;

   size_t length = strlen( path );

   char* workingPath = ( char* )malloc( _MAX_PATH + 1 );

   if ( NULL == workingPath )
   {
      return false;
   }

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
   if ( -1 != ( hSearch = _findfirst( workingPath, &allContent ) ) )
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

      while ( 0 == _findnext( hSearch,&allContent ) )
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
                  else if (CZFList_insertBefore( *insertOffset, file ) )
                  {
                     ++( *insertOffset );
                  }
               }
            }
         }
      }
   }

   if ( -1 != hSearch )
   {
      _findclose(hSearch);

      hSearch = -1;
   }

   // search all subdirectory content for wildcard match
   if ( true == searchSubdirectories )
   {
      strcpy( workingPath + length, "\\*.*" );

      if ( -1 != ( hSearch = _findfirst( workingPath, &allContent ) ) )
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

         while ( 0 == _findnext( hSearch, &allContent ) )
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

   if ( -1 != hSearch )
   {
       _findclose( hSearch );

       hSearch = -1;
   }

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
         wildcard = ( char* )malloc( length + 1 );

         strcpy( wildcard, backslash );

         *backslash = '\0';
      }
      else // foo*.ext
      {
         wildcard = ( char* )malloc( length + 2 );

         if ( NULL != wildcard )
         {
            wildcard[ 0 ] = '\\';

            strcpy( wildcard + 1, inputSource );

            inputSource[ 0 ] = '.';
            inputSource[ 1 ] = '\0';
         }
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
      versionID = ( uint32_t )status.st_mtime;

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

void platformInitialize( void ) { }

void platformRelinquish( void ) { }

#endif