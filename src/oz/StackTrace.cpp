/*
 *  StackTrace.hpp
 *
 *  Class for generating stack trace for the current function call.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "StackTrace.hpp"

#include "common.hpp"
#include "Log.hpp"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef OZ_UNIX
# include <unistd.h>
# include <cxxabi.h>
# include <execinfo.h>
#endif

// prevent old-style cast warning due to a bug in <bits/signum.h>
#ifdef __GNUC__
# undef SIG_DFL
# undef SIG_IGN
# define SIG_DFL reinterpret_cast<__sighandler_t>( 0 )            /* Default action.  */
# define SIG_IGN reinterpret_cast<__sighandler_t>( 1 )            /* Ignore signal.  */
#endif

namespace oz
{

  const char* const StackTrace::SIGNALS[][2] =
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

  void* StackTrace::framePtrs[StackTrace::TRACE_SIZE + 1];
  char  StackTrace::output[StackTrace::TRACE_BUFFER_SIZE];

  void StackTrace::signalHandler( int signum )
  {
    signal( signum, SIG_DFL );

    if( signum < 1 || signum > 31 ) {
      signum = 0;
    }

    fprintf( stderr, "\nCaught signal %d %s (%s)\n",
             signum, SIGNALS[signum][0], SIGNALS[signum][1] );

    char* frames;
    int nFrames = StackTrace::get( &frames );
    const char* entry = frames;

    for( int i = 0; i < nFrames; ++i ) {
      fprintf( stderr, "  %s\n", entry );
      entry += strlen( entry ) + 1;
    }

    if( log.isFile() ) {
      log.resetIndent();
      log.println();
      log.println( "Caught %s (%d, %s). Stack trace:",
                  SIGNALS[signum][0], signum, SIGNALS[signum][1] );
      log.indent();
      log.printTrace( frames, nFrames );
    }

    raise( signum );
  }

  void StackTrace::init()
  {
    signal( SIGILL,     signalHandler );
    signal( SIGABRT,    signalHandler );
    signal( SIGFPE,     signalHandler );
    signal( SIGKILL,    signalHandler );
    signal( SIGSEGV,    signalHandler );
    signal( SIGTERM,    signalHandler );
  }

#ifndef OZ_UNIX

  int StackTrace::get( char** bufferPtr )
  {
    signalHelper.dummy();

    *bufferPtr = null;
    return 0;
  }

#else

  int StackTrace::get( char** bufferPtr )
  {
    int    nFrames = backtrace( framePtrs, TRACE_SIZE + 1 ) - 1;
    char** frames  = backtrace_symbols( framePtrs + 1, nFrames );

    if( frames == null ) {
      return 0;
    }

    const char* outEnd = output + TRACE_BUFFER_SIZE;
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

      // demangle name
      char*  demangleBuf = reinterpret_cast<char*>( malloc( STRING_BUFFER_SIZE ) );
      char*  demangleOut;
      char*  demangled;
      size_t size = STRING_BUFFER_SIZE;
      int    status = 0;

      demangleOut = abi::__cxa_demangle( func, demangleBuf, &size, &status );
      demangleBuf = demangleOut != null ? demangleOut : demangleBuf;
      demangled   = status == 0 ? demangleOut : func;

      size_t fileLen      = strnlen( file, size );
      size_t demangledLen = strnlen( demangled, size );
      size_t offsetLen    = strnlen( offset, size );
      size_t addressLen   = strnlen( address, size );

      size = 1 + fileLen + 2 + addressLen;
      size = demangledLen != 0 && offsetLen != 0 ? size + demangledLen + 3 + offsetLen : size;

      if( out + size > outEnd ) {
        free( demangleBuf );
        break;
      }

      memcpy( out, file, fileLen );
      out += fileLen;

      *out = '(';
      ++out;

      if( demangledLen != 0 && offsetLen != 0 ) {
        *out = ' ';
        ++out;

        memcpy( out, demangled, demangledLen );
        out += demangledLen;

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

      free( demangleBuf );
    }

    *bufferPtr = reinterpret_cast<char*>( realloc( frames, out - output ) );
    memcpy( *bufferPtr, output, out - output );
    return nFrames;
  }

#endif

}
