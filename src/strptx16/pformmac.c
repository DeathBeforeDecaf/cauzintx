// Platform-Specific implementation (Mac)

#include "pformmac.h"

static struct termios sysTerm;

struct termios* sysTermSet = NULL;

// derived from: https://opensource.apple.com/source/xnu/xnu-7195.81.3/bsd/sys/errno.h.auto.html

const char* errorName[] =
{
   "",                //  0: Error SUCCESS ( not used )

   "EPERM",           //  1: Operation not permitted. An attempt was made to perform an operation limited to processes
                      //     with appropriate privileges or to the owner of a file or other resources.

   "ENOENT",          //  2: No such file or directory. A component of a specified pathname did not exist, or the
                      //     pathname was an empty string.

   "ESRCH",           //  3: No such process could be found corresponding to that specified by the given process ID.

   "EINTR",           //  4: Interrupted function call. An asynchronous signal (such as SIGINT or SIGQUIT) was caught
                      //     by the process during the execution of an interruptible function. If the signal handler
                      //     performs a normal return, the interrupted function call will seem to have returned the
                      //     error condition.

   "EIO",             //  5: Input/output error. Some physical input or output error occurred.  This error will not be
                      //     reported until a subsequent operation on the same file descriptor and may be lost (over
                      //     written) by any subsequent errors.

   "ENXIO",           //  6: No such device or address. Input or output on a special file referred to a device that
                      //     did not exist, or made a request beyond the limits of the device.  This error may also
                      //     occur when, for example, a tape drive is not online or no disk pack is loaded on a drive.

   "E2BIG",           //  7: Arg list too long. The number of bytes used for the argument and environment list of the
                      //     new process exceeded the limit NCARGS (specified in ⟨sys/param.h⟩).

   "ENOEXEC",         //  8: Exec format error. A request was made to execute a file that, although it has the
                      //     appropriate permissions, was not in the format required for an executable file.

   "EBADF",           //  9: Bad file descriptor. A file descriptor argument was out of range, referred to no open
                      //     file, or a read (write) request was made to a file that was only open for writing
                      //     (reading).


   "ECHILD",          // 10: No child processes. A wait or waitpid function was executed by a process that had no
                      //     existing or unwaited-for child processes.

   "EDEADLK",         // 11: Resource deadlock avoided. An attempt was made to lock a system resource that would have
                      //     resulted in a deadlock situation.

   "ENOMEM",          // 12: Cannot allocate memory. The new process image required more memory than was allowed by
                      //     the hardware or by system-imposed memory management constraints.  A lack of swap space
                      //     is normally temporary; however, a lack of core is not.  Soft limits may be increased to
                      //     their corresponding hard limits.

   "EACCES",          // 13: Permission denied. An attempt was made to access a file in a way forbidden by its file
                      //     access permissions.

   "EFAULT",          // 14: Bad address. The system detected an invalid address in attempting to use an argument of
                      //     a call.

   "ENOTBLK",         // 15: Not a block device. A block device operation was attempted on a non-block device or file.

   "EBUSY",           // 16: Resource busy. An attempt to use a system resource which was in use at the time in a
                      //     manner which would have conflicted with the request.

   "EEXIST",          // 17: File exists. An existing file was mentioned in an inappropriate context, for instance,
                      //     as the new link name in a link function.

   "EXDEV",           // 18: Improper link. A hard link to a file on another file system was attempted.

   "ENODEV",          // 19: Operation not supported by device. An attempt was made to apply an inappropriate function
                      //     to a device, for example, trying to read a write-only device such as a printer.

   "ENOTDIR",         // 20: Not a directory. A component of the specified pathname existed, but it was not a
                      //     directory, when a directory was expected.

   "EISDIR",          // 21: Is a directory. An attempt was made to open a directory with write mode specified.

   "EINVAL",          // 22: Invalid argument. Some invalid argument was supplied. (For example, specifying an
                      //     undefined signal to a signal or kill function).

   "ENFILE",          // 23: Too many open files in system. Maximum number of file descriptors allowable on the system
                      //     has been reached and a requests for an open cannot be satisfied until at least one has
                      //     been closed.

   "EMFILE",          // 24: Too many open files. <As released, the limit on the number of open files per process is
                      //     64.> Getdtablesize(2) will obtain the current limit.

   "ENOTTY",          // 25: Inappropriate ioctl for device. A control function (see ioctl(2)) was attempted for a
                      //     file or special device for which the operation was inappropriate.

   "ETXTBSY",         // 26: Text file busy. The new process was a pure procedure (shared text) file which was open
                      //     for writing by another process, or while the pure procedure file was being executed an
                      //     open call requested write access.

   "EFBIG",           // 27: File too large. The size of a file exceeded the maximum (about 2.1E9 bytes on some
                      //     filesystems including UFS, 1.8E19 bytes on HFS+ and others).

   "ENOSPC",          // 28: Device out of space. A write to an ordinary file, the creation of a directory or symbolic
                      //     link, or the creation of a directory entry failed because no more disk blocks were
                      //     available on the file system, or the allocation of an inode for a newly created file
                      //     failed because no more inodes were available on the file system.

   "ESPIPE",          // 29: Illegal seek. An lseek function was issued on a socket, pipe or FIFO.

   "EROFS",           // 30: Read-only file system. An attempt was made to modify a file or directory was made on a
                      //     file system that was read-only at the time.

   "EMLINK",          // 31: Too many links. Maximum allowable hard links to a single file has been exceeded (limit of
                      //     32767 hard links per file).

   "EPIPE",           // 32: Broken pipe. A write on a pipe, socket or FIFO for which there is no process to read the
                      //     data.

   "EDOM",            // 33: Numerical argument out of domain. A numerical input argument was outside the defined
                      //     domain of the mathematical function.

   "ERANGE",          // 34: Numerical result out of range. A numerical result of the function was too large to fit in
                      //     the available space (perhaps exceeded precision).

   "EAGAIN",          // 35: Resource temporarily unavailable. This is a temporary condition and later calls to the
                      //     same routine may complete normally.

// "EWOULDBLOCK",     // 35: Operation would block (alias for EAGAIN).

   "EINPROGRESS",     // 36: Operation now in progress. An operation that takes a long time to complete (such as a
                      //     connect(2) or connectx(2)) was attempted on a non-blocking object (see fcntl(2)).

   "EALREADY",        // 37: Operation already in progress. An operation was attempted on a non-blocking object that
                      //     already had an operation in progress.

   "ENOTSOCK",        // 38: Socket operation on non-socket. Self-explanatory.

   "EDESTADDRREQ",    // 39: Destination address required. A required address was omitted from an operation on a
                      //     socket.

   "EMSGSIZE",        // 40: Message too long. A message sent on a socket was larger than the internal message buffer
                      //     or some other network limit.

   "EPROTOTYPE",      // 41: Protocol wrong type for socket. A protocol was specified that does not support the
                      //     semantics of the socket type requested. For example, you cannot use the ARPA Internet UDP
                      //     protocol with type SOCK_STREAM.

   "ENOPROTOOPT",     // 42: Protocol not available. A bad option or level was specified in a getsockopt(2) or
                      //     setsockopt(2) call.

   "EPROTONOSUPPORT", // 43: Protocol not supported. The protocol has not been configured into the system or no
                      //     implementation for it exists.

   "ESOCKTNOSUPPORT", // 44: Socket type not supported. The support for the socket type has not been configured
                      //     into the system or no implementation for it exists.

   "ENOTSUP",         // 45: Not supported. The attempted operation is not supported for the type of object
                      //     referenced.

   "EPFNOSUPPORT",    // 46: Protocol family not supported. The protocol family has not been configured into the
                      //     system or no implementation for it exists.

   "EAFNOSUPPORT",    // 47: Address family not supported by protocol family. An address incompatible with the
                      //     requested protocol was used.  For example, you shouldn't necessarily expect to be able to
                      //     use NS addresses with ARPA Internet protocols.

   "EADDRINUSE",      // 48: Address already in use. Only one usage of each address is normally permitted.

   "EADDRNOTAVAIL",   // 49: Cannot assign requested address. Normally results from an attempt to create a socket with
                      //     an address not on this machine.

   "ENETDOWN",        // 50: Network is down. A socket operation encountered a dead network.

   "ENETUNREACH",     // 51: Network is unreachable. A socket operation was attempted to an unreachable network.

   "ENETRESET",       // 52: Network dropped connection on reset. The host you were connected to crashed and rebooted.

   "ECONNABORTED",    // 53: Software caused connection abort. A connection abort was caused internal to your host
                      //     machine.

   "ECONNRESET",      // 54: Connection reset by peer. A connection was forcibly closed by a peer.  This normally
                      //     results from a loss of the connection on the remote socket due to a timeout or a reboot.

   "ENOBUFS",         // 55: No buffer space available. An operation on a socket or pipe was not performed because the
                      //     system lacked sufficient buffer space or because a queue was full.

   "EISCONN",         // 56: Socket is already connected. A connect or connectx request was made on an already
                      //     connected socket; or, a sendto or sendmsg request on a connected socket specified a
                      //     destination when already connected.

   "ENOTCONN",        // 57: Socket is not connected. An request to send or receive data was disallowed because the
                      //     socket was not connected and (when sending on a datagram socket) no address was supplied.

   "ESHUTDOWN",       // 58: Cannot send after socket shutdown. A request to send data was disallowed because the
                      //     socket had already been shut down with a previous shutdown(2) call.

   "ETIMEDOUT",       // 59: Operation timed out. A connect, connectx or send request failed because the connected
                      //     party did not properly respond after a period of time.  (The timeout period is dependent
                      //     on the communication protocol.)

   "ETIMEDOUT",       // 60: Operation timed out. A connect, connectx or send request failed because the connected
                      //     party did not properly respond after a period of time.  (The timeout period is dependent
                      //     on the communication protocol.)

   "ECONNREFUSED",    // 61: Connection refused. No connection could be made because the target machine actively
                      //     refused it.  This usually results from trying to connect to a service that is inactive on
                      //     the foreign host.

   "ELOOP",           // 62: Too many levels of symbolic links. A path name lookup involved more than 8 symbolic links.

   "ENAMETOOLONG",    // 63: File name too long. A component of a path name exceeded 255 (MAXNAMELEN) characters, or an
                      //     entire path name exceeded 1023 (MAXPATHLEN-1) characters.

   "EHOSTDOWN",       // 64: Host is down. A socket operation failed because the destination host was down.

   "EHOSTUNREACH",    // 65: No route to host. A socket operation was attempted to an unreachable host.

   "ENOTEMPTY",       // 66: Directory not empty. A directory with entries other than ‘.’ and ‘..’ was supplied to a
                      //     remove directory or rename call.

   "EPROCLIM",        // 67: Too many processes.

   "EUSERS",          // 68: Too many users. The quota system ran out of table entries.

   "EDQUOT",          // 69: Disc quota exceeded. A write to an ordinary file, the creation of a directory or symbolic
                      //     link, or the creation of a directory entry failed because the user's quota of disk blocks
                      //     was exhausted, or the allocation of an inode for a newly created file failed because the
                      //     user's quota of inodes was exhausted.

   "ESTALE",          // 70: Stale NFS file handle. An attempt was made to access an open file (on an NFS filesystem)
                      //     which is now unavailable as referenced by the file descriptor.  This may indicate the
                      //     file was deleted on the NFS server or some other catastrophic event occurred.

   "EREMOTE",         // 71: Too many levels of remote in path

   "EBADRPC",         // 72: RPC struct is bad. Exchange of RPC information was unsuccessful.

   "ERPCMISMATCH",    // 73: RPC version wrong. The version of RPC on the remote peer is not compatible with the local
                      //     version.

   "EPROGUNAVAIL",    // 74: RPC prog. not avail. The requested program is not registered on the remote host.

   "EPROGMISMATCH",   // 75: Program version wrong. The requested version of the program is not available on the remote
                      //     host (RPC).

   "EPROCUNAVAIL",    // 76: Bad procedure for program. An RPC call was attempted for a procedure which doesn't exist
                      //     in the remote program.

   "ENOLCK",          // 77: No locks available. A system-imposed limit on the number of simultaneous file locks was
                      //     reached.

   "ENOSYS",          // 78: Function not implemented. Attempted a system call that is not available on this system.

   "EFTYPE",          // 79: Inappropriate file type or format. The file was the wrong type for the operation, or a
                      //     data file had the wrong format.

   "EAUTH",           // 80: Authentication error. Attempted to use an invalid authentication ticket to mount an NFS
                      //     file system.

   "ENEEDAUTH",       // 81: Need authenticator. An authentication ticket must be obtained before the given NFS file
                      //     system may be mounted.

   "EPWROFF",         // 82: Device power is off. The device power is off.

   "EDEVERR",         // 83: Device error. A device error has occurred, e.g. a printer running out of paper.

   "EOVERFLOW",       // 84: Value too large to be stored in data type. A numerical result of the function was too
                      //     large to be stored in the caller provided space.

   "EBADEXEC",        // 85: Bad executable (or shared library). The executable or shared library being referenced was
                      //     malformed.

   "EBADARCH",        // 86: Bad CPU type in executable. The executable in question does not support the current CPU.

   "ESHLIBVERS",      // 87: Shared library version mismatch. The version of the shared library on the system does not
                      //     match the version which was expected.

   "EBADMACHO",       // 88: Malformed Mach-o file. The Mach object file is malformed.

   "ECANCELED",       // 89: Operation canceled. The scheduled operation was canceled.

   "EIDRM",           // 90: Identifier removed. An IPC identifier was removed while the current process was waiting.

   "ENOMSG",          // 91: No message of desired type. An IPC message queue does not contain a message of the desired
                      //     type, or a message catalog does not contain the requested message.

   "EILSEQ",          // 92: Illegal byte sequence. While decoding a multibyte character the function came along an
                      //     invalid or an incomplete sequence of bytes or the given wide character is invalid.

   "ENOATTR",         // 93: Attribute not found. The specified extended attribute does not exist.

   "EBADMSG",         // 94: Bad message. The message to be received is inapprorpiate for the operation being
                      //     attempted.

   "EMULTIHOP",       // 95: Reserved. This error is reserved for future use.

   "ENODATA",         // 96: No message available. No message was available to be received by the requested operation.

   "ENOLINK",         // 97: Reserved. This error is reserved for future use.

   "ENOSR",           // 98: No STREAM resources. This error is reserved for future use.
   "ENOSTR",          // 99: Not a STREAM. This error is reserved for future use.

   "EPROTO",          // 100: Protocol error. Some protocol error occurred. This error is device-specific, but is
                      //      generally not related to a hardware failure.

   "ETIME",           // 101: STREAM ioctl() timeout. This error is reserved for future use.

   "EOPNOTSUPP",      // 102: Operation not supported on socket. The attempted operation is not supported for the type
                      //      of socket referenced; for example, trying to accept a connection on a datagram socket.

   "ENOPOLICY",       // 103: No such policy registered
   "ENOTRECOVERABLE", // 104: State not recoverable

   "EOWNERDEAD",      // 105: Previous owner died

   "EQFULL",          // 106: Interface output queue is full
   "",                // 107:
   "",                // 108:
   "",                // 109:

   "",                // 110:
   "",                // 111:
   "",                // 112:
   "",                // 113:
   "",                // 114:

   "",                // 115:
   "",                // 116:
   "",                // 117:
   "",                // 118:
   "",                // 119:

   "",                // 120:
   "",                // 121:
   "",                // 122:
   "",                // 123:
   "",                // 124:

   "",                // 125:
   "",                // 126:
   "",                // 127:
   "",                // 128:
   "",                // 129:

   "",                // 130:
   "",                // 131:
   "",                // 132:
   ""                 // 133:
};


int makeDirectory( const char* pathname )
{
   return mkdir( pathname, 0744 );
}


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


void errorExit( const char* message, unsigned line, const char* filename )
{
   int errorID = errno;

   char lineStr[ 15 ];

   const char prefixStr[] = "ERROR [";

   const char unknownStr[] = "?UNKNOWN?";

   int offset = sizeof( lineStr[ 15 ] ) / sizeof( lineStr[ 0 ] ) - 1;

   char* index = ( char* )prefixStr;

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }

   if ( ( errorID > 0 ) && ( errorID <= ELAST ) )
   {
      index = ( char* )errorName[ errorID ];
   }
   else
   {
      index = ( char* )unknownStr;
   }

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }

   fputc( ' ', stderr );

   index = strerror( errorID );

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }

   fputc( ']', stderr );
   fputc( ' ', stderr );

   lineStr[ offset-- ] = '\0';

   while ( offset >= 0 )
   {
      lineStr[ offset-- ] = '0' + ( char )( line % 10 );

      line /= 10;

      if ( 0 == line )
      {
         break;
      }
   }

   index = &lineStr[ offset + 1 ];

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }

   fputc( ':', stderr );

   index = ( char* )message;

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }

   fputs( LNFEED, stderr );

   fflush( stderr );

   exit( EXIT_FAILURE );
}

static void quitHandler( int signalID )
{
   platformRelinquish();

   _exit( EXIT_SUCCESS );
}

static void restartHandler( int signalID )
{
   char label[] = LNFEED "restart application()" LNFEED;

   char* index = label;

   while ( *index )
   {
      if ( EOF == fputc( *index, stderr ) )
      {
         break;
      }

      ++index;
   }
}

static void detachHandler( int signalID ) // SIGTSTP / SIGCONT
{
   struct termios appTerm;        // save current tty settings

   sigset_t stopSignalMask;
   sigset_t appSignalMask;

   struct sigaction signalMap;

   int invariantErrno = errno;             // prevent errno changes

   // Save current terminal settings, restore terminal to
   // state at time of program startup

   if ( -1 == tcgetattr( STDIN_FILENO, &appTerm ) )
   {
      errorExit( "tcgetattr", __LINE__, __FILE__ );
   }

   if ( -1 == tcsetattr( STDIN_FILENO, TCSAFLUSH, &sysTerm ) )
   {
      errorExit( "tcsetattr", __LINE__, __FILE__ );
   }

   // Set the disposition of SIGTSTP to the default, raise the signal
   // once more, and then unblock it so that we actually stop

   if ( SIG_ERR == signal( SIGTSTP, SIG_DFL ) )
   {
      errorExit( "signal", __LINE__, __FILE__ );
   }

   raise( SIGTSTP );

   sigemptyset( &stopSignalMask );

   sigaddset( &stopSignalMask, SIGTSTP );

   if ( -1 == sigprocmask( SIG_UNBLOCK, &stopSignalMask, &appSignalMask ) )
   {
      errorExit( "sigprocmask", __LINE__, __FILE__ );
   }

   // execution resumes here after SIGCONT

   if ( -1 == sigprocmask( SIG_SETMASK, &appSignalMask, NULL ) )
   {
      errorExit( "sigprocmask", __LINE__, __FILE__ );       // Reblock SIGTSTP
   }

   sigemptyset( &signalMap.sa_mask );         // Reestablish handler

   signalMap.sa_flags = SA_RESTART;
   signalMap.sa_handler = detachHandler;

   if ( -1 == sigaction( SIGTSTP, &signalMap, NULL ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }

   // The user may have changed the terminal settings while we were
   // stopped; save the settings so we can restore them later

   if ( -1 == tcgetattr( STDIN_FILENO, &sysTerm ) )
   {
      errorExit( "tcgetattr", __LINE__, __FILE__ );
   }

   // Restore our terminal settings
   if ( -1 == tcsetattr( STDIN_FILENO, TCSAFLUSH, &appTerm ) )
   {
      errorExit( "tcsetattr", __LINE__, __FILE__ );
   }

   errno = invariantErrno;
}

int ttySetCBreak( int fildes )
{
   struct termios termSettings;

   if ( ( NULL == sysTermSet ) && ( 0 == tcgetattr( fildes, &sysTerm ) ) )
   {
      sysTermSet = &sysTerm;
   }

   if ( NULL != sysTermSet )
   {
      // non-canonical mode, disable echoing, enable ctrl-key signal generation
      termSettings.c_lflag &= ~( ICANON | ECHO );
      termSettings.c_lflag |= ISIG;

      // Disable carriage return to newline translation
      termSettings.c_iflag &= ~ICRNL;

      termSettings.c_cc[ VMIN ] = 1;               // single character input
      termSettings.c_cc[ VTIME ] = 0;              // with blocking

      // TCSAFLUSH: drain output, flush input
      return tcsetattr( fildes, TCSAFLUSH, &termSettings );
   }

   return -1;
}


int ttySetRaw( int fildes )
{
   struct termios termSettings;

   if ( ( NULL == sysTermSet ) && ( 0 == tcgetattr( fildes, &sysTerm ) ) )
   {
      sysTermSet = &sysTerm;
   }

   if ( NULL != sysTermSet )
   {
      // noncanonical mode, disable signals, extended input processing, and echoing
      termSettings.c_lflag &= ~( ICANON | ISIG | IEXTEN | ECHO );

      // Disable special handling of CR, NL, and BREAK.
      // No 8th-bit stripping or parity error handling. Disable START/STOP output flow control.
      termSettings.c_iflag &=
         ~( BRKINT | ICRNL | IGNBRK | IGNCR | INLCR | INPCK | ISTRIP | IXON | PARMRK );


      termSettings.c_oflag &= ~OPOST;          // disable output processing

      termSettings.c_cc[ VMIN ] = 1;           // single character input
      termSettings.c_cc[ VTIME ] = 0;          // with blocking

      // TCSAFLUSH: drain output, flush input
      return tcsetattr( fildes, TCSAFLUSH, &termSettings );
   }

   return -1;
}


int stricmp( const char* lhs, const char* rhs )
{
   return strcasecmp( lhs, rhs );
}


int strnicmp( const char* lhs, const char* rhs, size_t length )
{
   return strncasecmp( lhs, rhs, length );
}


int kbhit( void )
{
   struct timeval timeout = { 0 };

   fd_set fildes;

   int status;

   FD_ZERO( &fildes );
   FD_SET( STDIN_FILENO, &fildes );

   while ( ( -1 == ( status = select( STDIN_FILENO + 1, &fildes, NULL, NULL, &timeout ) ) )
           && ( EINTR == errno ) )
   {
      bzero( &timeout, sizeof( struct timeval ) );
   }

   if ( 0 == status )
   {
      return FD_ISSET( STDIN_FILENO, &fildes );
   }

   return 0;
}


uint32_t getVersionID( const char* filename )
{
   uint32_t versionID = 0;

   struct stat status = { 0 };

   if ( ( 0 == stat( filename, &status ) ) && ( status.st_mode & S_IFREG ) )
   {
      versionID = status.st_mtimespec.tv_nsec;

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

   int status;

   while ( ( -1 == ( status = ttySetRaw( STDIN_FILENO ) ) ) && ( EINTR == errno ) )
      continue;

   // absorb pending key press before prompt
   while ( kbhit() )
   {
      selection = getchar();
   }

   do
   {
      selection = getchar();

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

   if ( NULL != sysTermSet )
   {
      if ( -1 == tcsetattr( STDIN_FILENO, TCSAFLUSH, sysTermSet ) )
      {
         errorExit( "tcsetattr", __LINE__, __FILE__ );
      }
   }

   return result;
}


void platformInitialize( void )
{
   struct sigaction appSignal;
   struct sigaction sysSignal;

   sigemptyset( &appSignal.sa_mask );

   appSignal.sa_flags = SA_RESTART;

   int status;
/*
   if ( ( NULL == sysTermSet ) && ( 0 == tcgetattr( STDIN_FILENO, &sysTerm ) ) )
   {
      sysTermSet = &sysTerm;
   }
*/
/*
   while ( ( -1 == ( status = ttySetRaw( STDIN_FILENO ) ) ) && ( EINTR == errno ) )
      continue;


   if ( -1 == status )
   {
      errorExit( "ttySetRaw", __LINE__, __FILE__ );
   }
*/
   fflush( stdout );

// setvbuf( stdout, ( char * )NULL, _IOLBF, 0 );     // disable stdout buffering

   // establish handlers only if the signals are not being ignored

   appSignal.sa_handler = quitHandler;

   if ( -1 == sigaction( SIGQUIT, NULL, &sysSignal ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }

   if ( SIG_IGN != sysSignal.sa_handler )
   {
      if ( -1 == sigaction( SIGQUIT, &appSignal, NULL ) )
      {
         errorExit( "sigaction", __LINE__, __FILE__ );
      }
   }

   if ( -1 == sigaction( SIGINT, NULL, &sysSignal ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }

   if ( SIG_IGN != sysSignal.sa_handler )
   {
      if ( -1 == sigaction( SIGINT, &appSignal, NULL ) )
      {
         errorExit( "sigaction", __LINE__, __FILE__ );
      }
   }

   appSignal.sa_handler = restartHandler;

   if ( -1 == sigaction( SIGHUP, NULL, &sysSignal ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }

   if ( SIG_IGN != sysSignal.sa_handler )
   {
      if ( -1 == sigaction( SIGHUP, &appSignal, NULL ) )
      {
         errorExit( "sigaction", __LINE__, __FILE__ );
      }
   }

   appSignal.sa_handler = detachHandler;

   if ( -1 == sigaction( SIGTSTP, NULL, &sysSignal ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }

   if ( SIG_IGN != sysSignal.sa_handler )
   {
      if ( -1 == sigaction( SIGTSTP, &appSignal, NULL ) )
      {
         errorExit( "sigaction", __LINE__, __FILE__ );
      }
   }

   appSignal.sa_handler = quitHandler;

   if ( -1 == sigaction( SIGTERM, &appSignal, NULL ) )
   {
      errorExit( "sigaction", __LINE__, __FILE__ );
   }
}


void platformRelinquish( void )
{
   if ( NULL != sysTermSet )
   {
      if ( -1 == tcsetattr( STDIN_FILENO, TCSAFLUSH, sysTermSet ) )
      {
         errorExit( "tcsetattr", __LINE__, __FILE__ );
      }
   }
}
