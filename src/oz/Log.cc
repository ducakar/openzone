/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Log.cc
 */

#include "Log.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace oz
{

static const int OUT_BUFFER_SIZE = 4096;

static const char* const SIGNALS[][2] =
{
  { "SIG???",    "[invalid signal number]"    },
  { "SIGHUP",    "Hangup"                     }, //  1
  { "SIGINT",    "Interrupt"                  }, //  2
  { "SIGQUIT",   "Quit"                       }, //  3
  { "SIGILL",    "Illegal instruction"        }, //  4
  { "SIGTRAP",   "Trace trap"                 }, //  5
  { "SIGABRT",   "Abort"                      }, //  6
  { "SIGBUS",    "BUS error"                  }, //  7
  { "SIGFPE",    "Floating-point exception"   }, //  8
  { "SIGKILL",   "Kill, unblockable"          }, //  9
  { "SIGUSR1",   "User-defined signal 1"      }, // 10
  { "SIGSEGV",   "Segmentation violation"     }, // 11
  { "SIGUSR2",   "User-defined signal 2"      }, // 12
  { "SIGPIPE",   "Broken pipe"                }, // 13
  { "SIGALRM",   "Alarm clock"                }, // 14
  { "SIGTERM",   "Termination"                }, // 15
  { "SIGSTKFLT", "Stack fault"                }, // 16
  { "SIGCHLD",   "Child status has changed"   }, // 17
  { "SIGCONT",   "Continue"                   }, // 18
  { "SIGSTOP",   "Stop, unblockable"          }, // 19
  { "SIGTSTP",   "Keyboard stop"              }, // 20
  { "SIGTTIN",   "Background read from tty"   }, // 21
  { "SIGTTOU",   "Background write to tty"    }, // 22
  { "SIGURG",    "Urgent condition on socket" }, // 23
  { "SIGXCPU",   "CPU limit exceeded"         }, // 24
  { "SIGXFSZ",   "File size limit exceeded"   }, // 25
  { "SIGVTALRM", "Virtual alarm clock"        }, // 26
  { "SIGPROF",   "Profiling alarm clock"      }, // 27
  { "SIGWINCH",  "Window size change"         }, // 28
  { "SIGIO",     "I/O now possible"           }, // 29
  { "SIGPWR",    "Power failure restart"      }, // 30
  { "SIGSYS",    "Bad system call"            }  // 31
};

static char  filePath[256] = "";
static FILE* file          = nullptr;
static int   tabs          = 0;

bool Log::showVerbose      = false;
bool Log::verboseMode      = false;
Log  Log::out;

const char* Log::logFile()
{
  return filePath;
}

void Log::resetIndent()
{
  tabs = 0;
}

void Log::indent()
{
  ++tabs;
}

void Log::unindent()
{
  if( tabs > 0 ) {
    --tabs;
  }
}

void Log::putsRaw( const char* s )
{
  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( s, stdout );
  }

  if( file != nullptr ) {
    fputs( s, file );
    fflush( file );
  }
}

void Log::vprintRaw( const char* s, va_list ap )
{
  char buffer[OUT_BUFFER_SIZE];

  vsnprintf( buffer, OUT_BUFFER_SIZE, s, ap );

  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( buffer, stdout );
  }

  if( file != nullptr ) {
    fputs( buffer, file );
    fflush( file );
  }
}

void Log::printRaw( const char* s, ... )
{
  char buffer[OUT_BUFFER_SIZE];

  va_list ap;
  va_start( ap, s );
  vsnprintf( buffer, OUT_BUFFER_SIZE, s, ap );
  va_end( ap );

  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( buffer, stdout );
  }

  if( file != nullptr ) {
    fputs( buffer, file );
    fflush( file );
  }
}

void Log::print( const char* s, ... )
{
  char buffer[OUT_BUFFER_SIZE];

  va_list ap;
  va_start( ap, s );
  vsnprintf( buffer, OUT_BUFFER_SIZE, s, ap );
  va_end( ap );

  if( !verboseMode || showVerbose || file == nullptr ) {
    for( int i = 0; i < tabs; ++i ) {
      fputs( "  ", stdout );
    }
    fputs( buffer, stdout );
  }

  if( file != nullptr ) {
    for( int i = 0; i < tabs; ++i ) {
      fputs( "  ", file );
    }
    fputs( buffer, file );
    fflush( file );
  }
}

void Log::printEnd( const char* s, ... )
{
  char buffer[OUT_BUFFER_SIZE];

  va_list ap;
  va_start( ap, s );
  vsnprintf( buffer, OUT_BUFFER_SIZE, s, ap );
  va_end( ap );

  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( buffer, stdout );
    fputc( '\n', stdout );
  }

  if( file != nullptr ) {
    fputs( buffer, file );
    fputc( '\n', file );
    fflush( file );
  }
}

void Log::printEnd()
{
  if( !verboseMode || showVerbose || file == nullptr ) {
    fputc( '\n', stdout );
  }

  if( file != nullptr ) {
    fputc( '\n', file );
    fflush( file );
  }
}

void Log::println( const char* s, ... )
{
  char buffer[OUT_BUFFER_SIZE];

  va_list ap;
  va_start( ap, s );
  vsnprintf( buffer, OUT_BUFFER_SIZE, s, ap );
  va_end( ap );

  if( !verboseMode || showVerbose || file == nullptr ) {
    for( int i = 0; i < tabs; ++i ) {
      fputs( "  ", stdout );
    }
    fputs( buffer, stdout );
    fputc( '\n', stdout );
  }

  if( file != nullptr ) {
    for( int i = 0; i < tabs; ++i ) {
      fputs( "  ", file );
    }
    fputs( buffer, file );
    fputc( '\n', file );
    fflush( file );
  }
}

void Log::println()
{
  if( !verboseMode || showVerbose || file == nullptr ) {
    fputc( '\n', stdout );
  }

  if( file != nullptr ) {
    fputc( '\n', file );
    fflush( file );
  }
}

void Log::printTime( const Time& time )
{
  char buffer[OUT_BUFFER_SIZE];

  snprintf( buffer, OUT_BUFFER_SIZE, "%04d-%02d-%02d %02d:%02d:%02d",
            time.year, time.month, time.day, time.hour, time.minute, time.second );

  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( buffer, stdout );
  }

  if( file != nullptr ) {
    fputs( buffer, file );
    fflush( file );
  }
}

void Log::printTrace( const StackTrace& st )
{
  if( st.nFrames == 0 ) {
    if( !verboseMode || showVerbose || file == nullptr ) {
      fprintf( stdout, "  thread: %s\n  stack trace:\n    [no stack trace]\n", st.threadName );
    }

    if( file != nullptr ) {
      fprintf( file, "  thread: %s\n  stack trace:\n    [no stack trace]\n", st.threadName );
    }
  }
  else {
    char** entries = st.symbols();

    if( !verboseMode || showVerbose || file == nullptr ) {
      fprintf( stdout, "  thread: %s\n  stack trace:\n", st.threadName );
    }

    if( file != nullptr ) {
      fprintf( file, "  thread: %s\n  stack trace:\n", st.threadName );
    }

    for( int i = 0; i < st.nFrames; ++i ) {
      if( !verboseMode || showVerbose || file == nullptr ) {
        fputs( "    ", stdout );
        fputs( entries[i], stdout );
        fputc( '\n', stdout );
      }

      if( file != nullptr ) {
        fputs( "    ", file );
        fputs( entries[i], file );
        fputc( '\n', file );
      }
    }

    ::free( entries );
  }

  if( file != nullptr ) {
    fflush( file );
  }
}

void Log::printSignal( int sigNum )
{
  char buffer[OUT_BUFFER_SIZE];

  int index = uint( sigNum ) >= uint( aLength( SIGNALS ) ) ? 0 : sigNum;

  snprintf( buffer, OUT_BUFFER_SIZE, "\n\nSignal %d %s (%s)\n",
            sigNum, SIGNALS[index][0], SIGNALS[index][1] );

  if( !verboseMode || showVerbose || file == nullptr ) {
    fputs( buffer, stdout );
  }

  if( file != nullptr ) {
    fputs( buffer, file );
    fflush( file );
  }
}

bool Log::init( const char* filePath_, bool clearFile )
{
  tabs = 0;

#ifdef __native_client__

  static_cast<void>( filePath_ );
  static_cast<void>( clearFile );

  return false;

#else

  if( filePath_ == nullptr ) {
    filePath[0] = '\0';
  }
  else {
    strncpy( filePath, filePath_, 256 );
    filePath[255] = '\0';
  }

  if( file != nullptr ) {
    fclose( file );
    file = nullptr;
  }
  if( filePath[0] != '\0' ) {
    file = fopen( filePath, clearFile ? "w" : "a" );
  }

  return file != nullptr;

#endif
}

void Log::free()
{
  if( file != nullptr ) {
    fclose( file );
    file = nullptr;
  }
}

const Log& Log::operator << ( bool b ) const
{
  putsRaw( b ? "true" : "false" );
  return *this;
}

const Log& Log::operator << ( char c ) const
{
  printRaw( "%c", c );
  return *this;
}

const Log& Log::operator << ( byte b ) const
{
  printRaw( "%d", b );
  return *this;
}

const Log& Log::operator << ( ubyte b ) const
{
  printRaw( "%u", b );
  return *this;
}

const Log& Log::operator << ( short s ) const
{
  printRaw( "%hd", s );
  return *this;
}

const Log& Log::operator << ( ushort s ) const
{
  printRaw( "%hu", s );
  return *this;
}

const Log& Log::operator << ( int i ) const
{
  printRaw( "%d", i );
  return *this;
}

const Log& Log::operator << ( uint i ) const
{
  printRaw( "%u", i );
  return *this;
}

const Log& Log::operator << ( long l ) const
{
  printRaw( "%ld", l );
  return *this;
}

const Log& Log::operator << ( ulong l ) const
{
  printRaw( "%lu", l );
  return *this;
}

const Log& Log::operator << ( long64 l ) const
{
#ifdef _WIN32
  printRaw( "%ld", long( l ) );
#else
  printRaw( "%lld", l );
#endif
  return *this;
}

const Log& Log::operator << ( ulong64 l ) const
{
#ifdef _WIN32
  printRaw( "%lu", ulong( l ) );
#else
  printRaw( "%llu", l );
#endif
  return *this;
}

const Log& Log::operator << ( float f ) const
{
  printRaw( "%g", f );
  return *this;
}

const Log& Log::operator << ( double d ) const
{
  printRaw( "%g", d );
  return *this;
}

const Log& Log::operator << ( const String& s ) const
{
  putsRaw( s );
  return *this;
}

const Log& Log::operator << ( const char* s ) const
{
  putsRaw( s );
  return *this;
}

const Log& Log::operator << ( volatile const void* p ) const
{
  printRaw( "%p", p );
  return *this;
}

}
