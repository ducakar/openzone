/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/System.cpp
 */

#include "System.hpp"

#include "Log.hpp"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

// prevent old-style cast warning for GCC
#ifdef __GNUC__
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

void System::bell()
{}

void System::trap()
{}

void System::halt()
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

  if( signum == SIGINT ) {
    isHaltEnabled = false;
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

void System::bell()
{
  system( "paplay /usr/share/sounds/pop.wav &" );
}

void System::trap()
{
  bell();

  signal( SIGTRAP, SIG_IGN );
  raise( SIGTRAP );
  signal( SIGTRAP, SIG_DFL );
}

void System::halt()
{
  fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
  fflush( stderr );
  while( sleep( 1 ) == 0 );
}

#endif

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

void System::abort( const char* msg, ... )
{
  System::resetSignals();

  va_list ap;
  va_start( ap, msg );

  fflush( stdout );

  fprintf( stderr, "\n" );
  vfprintf( stderr, msg, ap );
  fprintf( stderr, "\n" );

  StackTrace st = StackTrace::current();
  log.printTrace( &st );

  fflush( stderr );

  if( log.isFile() ) {
    log.printEnd();
    log.vprintRaw( msg, ap );
    log.printEnd( "\n" );

    log.resetIndent();
    log.indent();
    log.printTrace( &st );
    log.unindent();
  }

  va_end( ap );

  if( isHaltEnabled ) {
    fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
    fflush( stderr );
    while( sleep( 1 ) == 0 );
  }

  ::abort();
}

#endif

}
