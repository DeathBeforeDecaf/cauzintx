
#include "support.h"

char identity[ 64 ];

void generateIdentity( const char* filename )
{
   uint32_t versionID = getVersionID( filename );

   if ( 0 != versionID )
   {
      sprintf( identity, IDENTITY_STR " - " VERSION_STR " (%04X)", versionID );
   }

   printIdentity();
}

void printIdentity()
{
   fputs( identity, stdout );
   fputs( LNFEED, stdout );
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


char* rtrim( char* input, const char cutChar[], unsigned char cutCharSize )
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

   return input;
}


bool CZFListDataType_pathLessThan( CZFListDataType lhs, CZFListDataType rhs )
{
   struct InputFileType* left;
   struct InputFileType* right;

   left = lhs;
   right = rhs;

   return ( stricmp( left->path, right->path ) < 0 );
}


bool strtobool( const char* input, char** mark )
{
   char* index = ( char* )input;

   size_t length;

   bool result = false;

   if ( NULL != mark )
   {
      *mark = ( char* )input;
   }

   if ( NULL != index )
   {
      index = trim( index, hzWSpace, hzWSpaceCount );

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


bool softBreak( const char* lineIn, uint16_t colLimit, uint16_t rowLimit, char** linesOut )
{
   if ( ( NULL != lineIn ) && ( '\0' != *lineIn ) )
   {
      uint16_t rowCount = 0;

      char* lineStart = ( char* )lineIn;

      char* splitIndex;

      char* target;

      char* lastWhiteSpace;
      char* lastPunctuator;

      char* buffer;

      size_t byteCount = colLimit;

      if ( ( ( byteCount * rowLimit ) + rowLimit + 1 ) <= byteCount )
      {
         fprintf( stderr, "\n*** uint16_t overflow %s: %d, select a smaller buffer size\n", __FILE__, __LINE__ );

         return false;
      }

      byteCount *= rowLimit;
      byteCount += rowLimit + 1;

      buffer = ( char* )malloc( byteCount );

      if ( NULL != buffer )
      {
         target = buffer;

         do
         {
            // ltrim linestart
            do
            {
               lastWhiteSpace = NULL;

               switch ( *lineStart )
               {
                  case ' ':
                  case '\t':
                  case '\r':
                  case '\n':
                     lastWhiteSpace = lineStart;
                     ++lineStart;
                     break;
               }
            }
            while ( ( '\0' != *lineStart ) && ( NULL != lastWhiteSpace ) );

            splitIndex = lineStart;

            lastWhiteSpace = NULL;
            lastPunctuator = NULL;

            if ( ( '\0' != *lineStart ) && ( target != buffer ) )
            {
               *target = '\n';
               ++target;
            }

            // advance index until column limit
            while ( ( '\0' != *splitIndex ) && ( ( splitIndex - lineStart ) < colLimit ) )
            {
               switch ( *splitIndex )
               {
                  case ' ':
                  case '\t':
                  case '\n':
                     lastWhiteSpace = splitIndex;
                     break;

                  case '!':
                  case '?':
                  case ':':
                  case ';':
                  case '-':
                  case ',':
                  case '.':
                  case ')':
                  case '}':
                  case ']':
                  case '>':
                  case '|':
                  case '=':
                  case '*':
                  case '/':
                  case '%':
                     lastPunctuator = splitIndex;
                     break;
               }

               *target = *splitIndex;

               ++splitIndex;
               ++target;
            }

            // more lines?
            if ( '\0' != *splitIndex )
            {
               // terminate line in place of whitespace
               if ( NULL != lastWhiteSpace )
               {
                  target -= ( splitIndex - lastWhiteSpace );

                  lineStart = lastWhiteSpace + 1;
               }
               // terminate line to right of punctuator iff not start of line
               else if ( ( NULL != lastPunctuator ) && ( lastPunctuator != lineStart ) )
               {
                  target -= ( splitIndex - lastPunctuator - 1 );

                  lineStart = lastPunctuator + 1;
               }
               // terminate line at column limit
               else
               {
                  lineStart = splitIndex;
               }
            }
            else
            {
               break;
            }
         }
         while ( ++rowCount < rowLimit );

         *target = '\0';

         *linesOut = buffer;

         return true;
      }
   }

   return false;
}
