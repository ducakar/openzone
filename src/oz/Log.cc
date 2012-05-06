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

#include "arrays.hh"
#include "Exception.hh"
#include "Time.hh"

#include "windefs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace oz
{

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

static char  filePath[256];
static FILE* file = 0;
static int   tabs = 0;

bool Log::showVerbose = false;
bool Log::verboseMode = false;

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

void Log::vprintRaw( const char* s, va_list ap )
{
  va_list ap2;
  va_copy( ap2, ap );

  if( !verboseMode || showVerbose || file == null ) {
    vprintf( s, ap );
  }
  if( file != null ) {
    vfprintf( file, s, ap2 );

    fflush( file );
  }
}

void Log::printRaw( const char* s, ... )
{
  va_list ap;

  if( !verboseMode || showVerbose || file == null ) {
    va_start( ap, s );

    vprintf( s, ap );

    va_end( ap );
  }
  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );

    va_end( ap );

    fflush( file );
  }
}

void Log::print( const char* s, ... )
{
  va_list ap;

  if( !verboseMode || showVerbose || file == null ) {
    for( int i = 0; i < tabs; ++i ) {
      printf( "  " );
    }

    va_start( ap, s );

    vprintf( s, ap );

    va_end( ap );
  }
  if( file != null ) {
    for( int i = 0; i < tabs; ++i ) {
      fprintf( file, "  " );
    }

    va_start( ap, s );

    vfprintf( file, s, ap );

    va_end( ap );

    fflush( file );
  }
}

void Log::printEnd( const char* s, ... )
{
  va_list ap;

  if( !verboseMode || showVerbose || file == null ) {
    va_start( ap, s );

    vprintf( s, ap );
    printf( "\n" );

    va_end( ap );
  }
  if( file != null ) {
    va_start( ap, s );

    vfprintf( file, s, ap );
    fprintf( file, "\n" );

    va_end( ap );

    fflush( file );
  }
}

void Log::printEnd()
{
  if( !verboseMode || showVerbose || file == null ) {
    printf( "\n" );
  }
  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::println( const char* s, ... )
{
  va_list ap;

  if( !verboseMode || showVerbose || file == null ) {
    for( int i = 0; i < tabs; ++i ) {
      printf( "  " );
    }

    va_start( ap, s );

    vprintf( s, ap );
    printf( "\n" );

    va_end( ap );
  }
  if( file != null ) {
    for( int i = 0; i < tabs; ++i ) {
      fprintf( file, "  " );
    }

    va_start( ap, s );

    vfprintf( file, s, ap );
    fprintf( file, "\n" );

    va_end( ap );

    fflush( file );
  }
}

void Log::println()
{
  if( !verboseMode || showVerbose || file == null ) {
    printf( "\n" );
  }
  if( file != null ) {
    fprintf( file, "\n" );

    fflush( file );
  }
}

void Log::printTime()
{
  Time time = Time::local();

  if( !verboseMode || showVerbose || file == null ) {
    printf( "%04d-%02d-%02d %02d:%02d:%02d",
            time.year, time.month, time.day, time.hour, time.minute, time.second );
  }
  if( file != null ) {
    fprintf( file, "%04d-%02d-%02d %02d:%02d:%02d",
             time.year, time.month, time.day, time.hour, time.minute, time.second );

    fflush( file );
  }
}

void Log::printSignal( int signum )
{
  int index = uint( signum ) >= uint( aLength( SIGNALS ) ) ? 0 : signum;

  if( !verboseMode || showVerbose || file == null ) {
    printf( "\n\nCaught signal %d %s (%s)\n", signum, SIGNALS[index][0], SIGNALS[index][1] );
  }
  if( file != null ) {
    fprintf( file, "\n\nCaught signal %d %s (%s)\n", signum, SIGNALS[index][0], SIGNALS[index][1] );

    fflush( file );
  }
}

void Log::printTrace( const StackTrace* st )
{
  if( st->nFrames == 0 ) {
    if( !verboseMode || showVerbose || file == null ) {
      printf( "    [empty stack trace]\n" );
    }
    if( file != null ) {
      fprintf( file, "    [empty stack trace]\n" );
    }
  }
  else {
    char** entries = st->symbols();

    for( int i = 0; i < st->nFrames; ++i ) {
      if( !verboseMode || showVerbose || file == null ) {
        printf( "    %s\n", entries[i] );
      }
      if( file != null ) {
        fprintf( file, "    %s\n", entries[i] );
      }
    }

    ::free( entries );
  }

  if( file != null ) {
    fflush( file );
  }
}

void Log::printException( const std::exception* e )
{
  const Exception* oe = dynamic_cast<const Exception*>( e );

  if( oe == null ) {
    if( !verboseMode || showVerbose || file == null ) {
      printf( "\n\nEXCEPTION: %s\n", e->what() );
    }
    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n", e->what() );

      fflush( file );
    }
  }
  else {
    if( !verboseMode || showVerbose || file == null ) {
      printf( "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
              oe->message, oe->function, oe->file, oe->line );
    }
    if( file != null ) {
      fprintf( file, "\n\nEXCEPTION: %s\n  in %s\n  at %s:%d\n  stack trace:\n",
               oe->message, oe->function, oe->file, oe->line );
    }

    printTrace( &oe->stackTrace );

    if( !verboseMode || showVerbose || file == null ) {
      printf( "\n" );
    }
    if( file != null ) {
      fprintf( file, "\n" );
    }
  }
}

bool Log::init( const char* filePath_, bool doClear )
{
  if( filePath_ == null ) {
    filePath[0] = '\0';
  }
  else {
    strncpy( filePath, filePath_, 256 );
    filePath[255] = '\0';
  }

  tabs = 0;

  if( file != null ) {
    fclose( file );
    file = null;
  }
  if( filePath[0] != '\0' ) {
    file = fopen( filePath, doClear ? "w" : "a" );
  }

  return file != null;
}

void Log::free()
{
  if( file != null ) {
    fclose( file );
    file = null;
  }
}

}
