/*
 *  System.cpp
 *
 *  Class for generating stack trace for the current function call.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "System.hpp"

#include "common.hpp"
#include "Log.hpp"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#ifndef OZ_MINGW
# include <execinfo.h>
# ifndef __clang__
#  include <cxxabi.h>
# endif
#endif

// prevent old-style cast warning due to a bug in <bits/signum.h>
#ifdef __GNUC__
# undef SIG_ERR
# undef SIG_DFL
# undef SIG_IGN
# define SIG_DFL reinterpret_cast<__sighandler_t>( 0 )            /* Default action.  */
# define SIG_IGN reinterpret_cast<__sighandler_t>( 1 )            /* Ignore signal.  */
#endif

namespace oz
{

  const char* const System::SIGNALS[][2] =
  {
    { "?",              "[invalid signal number]"    },
    { "SIGHUP",         "Hangup"                     }, //  1
    { "SIGINT",         "Interrupt"                  }, //  2
    { "SIGQUIT",        "Quit"                       }, //  3
    { "SIGILL",         "Illegal instruction"        }, //  4
    { "SIGTRAP",        "Trace trap"                 }, //  5
    { "SIGABRT",        "Abort"                      }, //  6
    { "SIGBUS",         "BUS error"                  }, //  7
    { "SIGFPE",         "Floating-point exception"   }, //  8
    { "SIGKILL",        "Kill, unblockable"          }, //  9
    { "SIGUSR1",        "User-defined signal 1"      }, // 10
    { "SIGSEGV",        "Segmentation violation"     }, // 11
    { "SIGUSR2",        "User-defined signal 2"      }, // 12
    { "SIGPIPE",        "Broken pipe"                }, // 13
    { "SIGALRM",        "Alarm clock"                }, // 14
    { "SIGTERM",        "Termination"                }, // 15
    { "SIGSTKFLT",      "Stack fault"                }, // 16
    { "SIGCHLD",        "Child status has changed"   }, // 17
    { "SIGCONT",        "Continue"                   }, // 18
    { "SIGSTOP",        "Stop, unblockable"          }, // 19
    { "SIGTSTP",        "Keyboard stop"              }, // 20
    { "SIGTTIN",        "Background read from tty"   }, // 21
    { "SIGTTOU",        "Background write to tty"    }, // 22
    { "SIGURG",         "Urgent condition on socket" }, // 23
    { "SIGXCPU",        "CPU limit exceeded"         }, // 24
    { "SIGXFSZ",        "File size limit exceeded"   }, // 25
    { "SIGVTALRM",      "Virtual alarm clock"        }, // 26
    { "SIGPROF",        "Profiling alarm clock"      }, // 27
    { "SIGWINCH",       "Window size change"         }, // 28
    { "SIGIO",          "I/O now possible"           }, // 29
    { "SIGPWR",         "Power failure restart"      }, // 30
    { "SIGSYS",         "Bad system call"            }  // 31
  };

  OZ_THREAD_LOCAL void* System::framePtrs[System::TRACE_SIZE + 1];
  OZ_THREAD_LOCAL char  System::output[System::TRACE_BUFFER_SIZE];

  bool System::isHaltEnabled = false;

#ifdef OZ_MINGW

  void System::enableHalt( bool )
  {}

  void System::signalHandler( int )
  {}

  void System::catchSignals()
  {}

  void System::resetSignals()
  {}

  void System::trap()
  {}

#else

  void System::enableHalt( bool value )
  {
    isHaltEnabled = value;
  }

  void System::signalHandler( int signum )
  {
    System::resetSignals();

    if( signum < 1 || signum > 31 ) {
      signum = 0;
    }

    abort( "Caught signal %d %s (%s)", signum, SIGNALS[signum][0], SIGNALS[signum][1] );
  }

  void System::catchSignals()
  {
    signal( SIGINT,  signalHandler );
    signal( SIGQUIT, signalHandler );
    signal( SIGILL,  signalHandler );
    signal( SIGABRT, signalHandler );
    signal( SIGFPE,  signalHandler );
    signal( SIGSEGV, signalHandler );
    signal( SIGTERM, signalHandler );
  }

  void System::resetSignals()
  {
    signal( SIGINT,  SIG_DFL );
    signal( SIGQUIT, SIG_DFL );
    signal( SIGILL,  SIG_DFL );
    signal( SIGABRT, SIG_DFL );
    signal( SIGFPE,  SIG_DFL );
    signal( SIGSEGV, SIG_DFL );
    signal( SIGTERM, SIG_DFL );
  }

  void System::trap()
  {
    signal( SIGTRAP, SIG_IGN );
    raise( SIGTRAP );
    signal( SIGTRAP, SIG_DFL );
  }

#endif

  void System::halt()
  {
    fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
    fflush( stderr );
    while( sleep( 1 ) == 0 );
  }

  void System::error( const char* msg, ... )
  {
    va_list ap;
    va_start( ap, msg );

    fflush( stdout );

    fprintf( stderr, "\n" );
    vfprintf( stderr, msg, ap );
    fprintf( stderr, "\n" );

    fflush( stderr );

    if( log.isFile() ) {
      log.printEnd( "\n" );
      log.vprintRaw( msg, ap );
      log.printEnd( "\n" );
    }

    va_end( ap );
  }

#ifdef OZ_MINGW

  int System::getStackTrace( char** bufferPtr )
  {
    *bufferPtr = null;
    return 0;
  }

  void System::abort( const char* msg, ... )
  {
    System::resetSignals();

    va_list ap;
    va_start( ap, msg );

    fflush( stdout );

    fprintf( stderr, "\n" );
    vfprintf( stderr, msg, ap );
    fprintf( stderr, "\n" );

    ::abort();
  }

#else

  int System::getStackTrace( char** bufferPtr )
  {
    int    nFrames = backtrace( framePtrs, TRACE_SIZE + 1 ) - 1;
    char** frames  = backtrace_symbols( framePtrs + 1, nFrames );

    if( frames == null ) {
      return 0;
    }

    const char* const outEnd = output + TRACE_BUFFER_SIZE;
    char* out = output;

    *out = '\0';

    for( int i = 0; i < nFrames; ++i ) {
      // file
      char* file = frames[i];

      // mangled function name
      char* func = strrchr( frames[i], '(' );

      if( func == null ) {
        size_t size = strlen( file ) + 1;

        if( out + size > outEnd ) {
          break;
        }

        memcpy( out, file, size );
        out[size - 1] = '\0';
        out += size;

        continue;
      }

      *func = '\0';
      ++func;

      // offset
      char* offset = strchr( func, '+' );

      if( offset == null ) {
        offset = func - 1;
      }

      *offset = '\0';
      ++offset;

      // address (plus a leading space)
      char* address = strchr( offset, ')' );

      if( address == null ) {
        --func;
        *func = '(';

        --offset;
        *offset = '+';

        size_t size = strlen( file ) + 1;

        if( out + size > outEnd ) {
          break;
        }

        memcpy( out, file, size );
        out[size - 1] = '\0';
        out += size;

        continue;
      }

      *address = '\0';
      ++address;

# ifdef __clang__
      size_t size;
# else
      // demangle name
      char*  demangleBuf = reinterpret_cast<char*>( malloc( STRING_BUFFER_SIZE ) );
      char*  demangleOut;
      size_t size = STRING_BUFFER_SIZE;
      int    status = 0;

      demangleOut = abi::__cxa_demangle( func, demangleBuf, &size, &status );
      demangleBuf = demangleOut != null ? demangleOut : demangleBuf;
      func        = status == 0 ? demangleOut : func;
# endif

      size_t fileLen    = strnlen( file, STRING_BUFFER_SIZE );
      size_t funcLen    = strnlen( func, STRING_BUFFER_SIZE );
      size_t offsetLen  = strnlen( offset, STRING_BUFFER_SIZE );
      size_t addressLen = strnlen( address, STRING_BUFFER_SIZE );

      size = fileLen + 2 + addressLen + 1;
      if( funcLen != 0 && offsetLen != 0 ) {
        size += 1 + funcLen + 3 + offsetLen + 1;
      }

      if( out + size > outEnd ) {
# ifndef __clang__
        free( demangleBuf );
# endif
        break;
      }

      memcpy( out, file, fileLen );
      out += fileLen;

      *out = '(';
      ++out;

      if( funcLen != 0 && offsetLen != 0 ) {
        *out = ' ';
        ++out;

        memcpy( out, func, funcLen );
        out += funcLen;

        *out = ' ';
        ++out;
        *out = '+';
        ++out;
        *out = ' ';
        ++out;

        memcpy( out, offset, offsetLen );
        out += offsetLen;

        *out = ' ';
        ++out;
      }

      *out = ')';
      ++out;

      memcpy( out, address, addressLen );
      out += addressLen;

      *out = '\0';
      ++out;

# ifndef __clang__
      free( demangleBuf );
# endif
    }

    *bufferPtr = reinterpret_cast<char*>( realloc( frames, size_t( out - output ) ) );
    memcpy( *bufferPtr, output, size_t( out - output ) );
    return nFrames;
  }

  void System::abort( const char* msg, ... )
  {
    System::resetSignals();

    va_list ap;
    va_start( ap, msg );

    fflush( stdout );

    fprintf( stderr, "\n" );
    vfprintf( stderr, msg, ap );
    fprintf( stderr, "\n" );

    char* frames;
    int nFrames = System::getStackTrace( &frames );
    const char* entry = frames;

    for( int i = 0; i < nFrames; ++i ) {
      fprintf( stderr, "  %s\n", entry );
      entry += strlen( entry ) + 1;
    }

    fflush( stderr );

    if( log.isFile() ) {
      log.printEnd();
      log.vprintRaw( msg, ap );
      log.printEnd( "\n" );

      log.resetIndent();
      log.indent();
      log.printTrace( frames, nFrames );
      log.unindent();
    }

    va_end( ap );

    free( frames );

    if( isHaltEnabled ) {
      fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
      fflush( stderr );
      while( sleep( 1 ) == 0 );
    }

    ::abort();
  }

#endif

}
