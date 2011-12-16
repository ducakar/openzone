/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/System.cc
 */

#include "System.hh"

#include "Log.hh"

#include <csignal>
#include <cstdio>
#include <cstdlib>

#include <fcntl.h>
#include <unistd.h>

#ifdef _WIN32
# include <windows.h>
# include <mmsystem.h>
#else
# include <pthread.h>
# include <pulse/simple.h>
#endif

namespace oz
{

System System::system;

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

#ifdef _WIN32

struct Wave
{
  char  chunkId[4];
  int   chunkSize;
  char  format[4];

  char  subchunk1Id[4];
  int   subchunk1Size;
  short audioFormat;
  short nChannels;
  int   sampleRate;
  int   byteRate;
  short blockAlign;
  short bitsPerSample;

  char  subchunk2Id[4];
  int   subchunk2Size;
  ubyte data[4410];
};

static const Wave WAVE_SAMPLE = {
  { 'R', 'I', 'F', 'F' }, 36 + 4410, { 'W', 'A', 'V', 'E' },
  { 'f', 'm', 't', ' ' }, 16, 1, 1, 11025, 11025, 1, 8,
  { 'd', 'a', 't', 'a' }, 4410, {
# include "bellSample.inc"
  }
};

#else

static const pa_sample_spec BELL_SPEC = { PA_SAMPLE_U8, 11025, 1 };
static const ubyte BELL_SAMPLE[] = {
# include "bellSample.inc"
};

#endif

static volatile int bellUsers = 0;
static int initFlags = 0;

static void resetSignals();

static void signalHandler( int signum )
{
  resetSignals();

  if( signum < 1 || signum > 31 ) {
    signum = 0;
  }

  if( signum == SIGINT ) {
    initFlags &= ~System::HALT_BIT;
  }

  System::error( "Caught signal %d %s (%s)", signum, SIGNALS[signum][0], SIGNALS[signum][1] );
  System::bell();
  System::abort();
}

static void catchSignals()
{
  signal( SIGINT,  signalHandler );
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
  signal( SIGTERM, signalHandler );
#ifndef _WIN32
  signal( SIGQUIT, signalHandler );
#endif
}

static void resetSignals()
{
  signal( SIGINT,  SIG_DFL );
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
  signal( SIGTERM, SIG_DFL );
#ifndef _WIN32
  signal( SIGQUIT, SIG_DFL );
#endif
}

#ifdef _WIN32

static DWORD WINAPI bellThread( LPVOID )
{
  PlaySound( reinterpret_cast<LPCSTR>( &WAVE_SAMPLE ), null, SND_MEMORY | SND_SYNC );

  --bellUsers;
  return 0;
}

#else

static void* bellThread( void* )
{
  pa_simple* pa = pa_simple_new( null, "liboz", PA_STREAM_PLAYBACK, null, "bell", &BELL_SPEC,
                                 null, null, null );
  if( pa != null ) {
    pa_simple_write( pa, BELL_SAMPLE, sizeof( BELL_SAMPLE ), null );
    pa_simple_free( pa );
  }

  --bellUsers;
  return null;
}

#endif

System::System()
{
#ifndef _WIN32
  // Disable default handler for TRAP signal that crashes the process.
  signal( SIGTRAP, SIG_IGN );
#endif
}

System::~System()
{
  while( bellUsers != 0 ) {
#ifdef _WIN32
    Sleep( 100 );
#else
    usleep( 100 );
#endif
  }
}

void System::bell()
{
  ++bellUsers;

#ifdef _WIN32
  HANDLE thread = CreateThread( null, 0, bellThread, null, 0, null );
  CloseHandle( thread );
#else
  pthread_t thread;
  pthread_create( &thread, null, bellThread, null );
#endif
}

void System::trap()
{
#ifdef _WIN32
  if( IsDebuggerPresent() ) {
    DebugBreak();
  }
#else
  raise( SIGTRAP );
#endif
}

void System::halt()
{
  printf( "Attach a debugger or send a fatal signal (e.g. CTRL-C) to kill ...\n" );

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

  log.printEnd();
  log.vprintRaw( msg, ap );
  log.printEnd();

  va_end( ap );

  StackTrace st = StackTrace::current();
  log.printTrace( &st );
}

void System::abort( bool halt_ )
{
  resetSignals();

  // Wait until bell is played completely.
  system.~System();

  if( halt_ && ( initFlags & HALT_BIT ) ) {
    halt();
  }

  ::abort();
}

void System::init( int flags )
{
  if( initFlags & CATCH_SIGNALS_BIT ) {
    resetSignals();
  }

  initFlags = flags;

  if( initFlags & CATCH_SIGNALS_BIT ) {
    catchSignals();
  }
}

}
