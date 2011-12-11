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
 * @file oz/System.cc
 */

#include "System.hh"

#include "Log.hh"
#include "Math.hh"

#include <csignal>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
# include <windows.h>
# include <mmsystem.h>
#else
# include <dlfcn.h>
# include <unistd.h>
# include <pthread.h>
# include <pulse/simple.h>
#endif

namespace oz
{

static const char* const SIGNALS[][2] =
{
  { "?",         "[invalid signal number]"    },
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

#ifndef _WIN32
static const ubyte BELL_SAMPLE[] = {
# include "bellSample.inc"
};

static const pa_sample_spec BELL_SPEC = { PA_SAMPLE_U8, 11025, 1 };

static decltype( ::pa_simple_new   )* pa_simple_new   = null;
static decltype( ::pa_simple_free  )* pa_simple_free  = null;
static decltype( ::pa_simple_write )* pa_simple_write = null;

static void* libpulse = null;
#endif

int System::initFlags = 0;

void System::signalHandler( int signum )
{
  System::resetSignals();

  if( signum < 1 || signum > 31 ) {
    signum = 0;
  }

  if( signum == SIGINT ) {
    System::initFlags &= ~System::HALT_BIT;
  }

  System::abort( "Caught signal %d %s (%s)", signum, SIGNALS[signum][0], SIGNALS[signum][1] );
}

#ifndef _WIN32

static void* bellThread( void* )
{
  pa_simple* pa = pa_simple_new( null, "liboz", PA_STREAM_PLAYBACK, null, "bell", &BELL_SPEC,
                                 null, null, null );
  if( pa != null ) {
    pa_simple_write( pa, BELL_SAMPLE, sizeof( BELL_SAMPLE ), null );
    pa_simple_free( pa );
  }

  return null;
}

#endif

void System::catchSignals()
{
#ifndef _WIN32
  signal( SIGQUIT, signalHandler );
#endif
  signal( SIGINT,  signalHandler );
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
  signal( SIGTERM, signalHandler );
}

void System::resetSignals()
{
#ifndef _WIN32
  signal( SIGQUIT, SIG_DFL );
#endif
  signal( SIGINT,  SIG_DFL );
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
  signal( SIGTERM, SIG_DFL );
}

void System::bell( bool isSync )
{
#ifdef _WIN32
  DWORD flags = SND_ALIAS_ID;

  if( !isSync ) {
    flags |= SND_ASYNC;
  }

  PlaySound( reinterpret_cast<LPCSTR>( SND_ALIAS_SYSTEMDEFAULT ), null, flags );
#else
  if( isSync ) {
    bellThread( null );
  }
  else {
    pthread_t thread;
    pthread_create( &thread, null, bellThread, null );
  }
#endif
}

void System::trap()
{
#ifndef _WIN32
  signal( SIGTRAP, SIG_IGN );
  raise( SIGTRAP );
  signal( SIGTRAP, SIG_DFL );
#endif
}

void System::halt()
{
  fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
  fflush( stderr );

#ifdef _WIN32
  while( true ) {
    Sleep( 1000 );
  }
#else
  while( sleep( 1 ) == 0 );
#endif
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

  if( initFlags & HALT_BIT ) {
    fprintf( stderr, "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );
    fflush( stderr );

#ifdef _WIN32
    while( true ) {
      Sleep( 1000 );
    }
#else
    while( sleep( 1 ) == 0 );
#endif
  }

  ::abort();
}

void System::init( int flags )
{
  if( initFlags & CATCH_SIGNALS_BIT ) {
    resetSignals();
  }

#ifndef _WIN32
  if( libpulse != null ) {
    dlclose( libpulse );

    pa_simple_new   = null;
    pa_simple_free  = null;
    pa_simple_write = null;
    libpulse        = null;
  }
#endif

  initFlags = 0;

  if( flags & CATCH_SIGNALS_BIT ) {
    initFlags |= CATCH_SIGNALS_BIT;
    catchSignals();
  }

  if( flags & HALT_BIT ) {
    initFlags |= HALT_BIT;
  }

#ifndef _WIN32
  libpulse = dlopen( "libpulse-simple.so", RTLD_NOW );

  if( libpulse == null ) {
    return;
  }

  *reinterpret_cast<void**>( &pa_simple_new   ) = dlsym( libpulse, "pa_simple_new" );
  *reinterpret_cast<void**>( &pa_simple_free  ) = dlsym( libpulse, "pa_simple_free" );
  *reinterpret_cast<void**>( &pa_simple_write ) = dlsym( libpulse, "pa_simple_write" );

  if( pa_simple_new == null || pa_simple_free == null || pa_simple_write == null ) {
    dlclose( libpulse );

    pa_simple_new   = null;
    pa_simple_free  = null;
    pa_simple_write = null;
    libpulse        = null;

    return;
  }
#endif
}

}
