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
 * @file oz/System.cc
 */

#include "System.hh"

#include "arrays.hh"
#include "Log.hh"

#include "windefs.h"
#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined( __native_client__ ) || defined( __ANDROID__ )
# include <ppapi/cpp/module.h>
# include <unistd.h>
#elif defined( _WIN32 )
# include <windows.h>
# include <mmsystem.h>
#else
# include <pthread.h>
# include <pulse/simple.h>
# include <unistd.h>
#endif

#ifdef __native_client__

// Fake implementations for signal() and raise() functions missing in newlib library. signal() is
// referenced by SDL hence must be present if we link with SDL. Those fake implementations also
// spare several #ifdefs in this file.
extern "C"
{

void ( * signal( int, void ( * )( int ) ) )( int )
{
  return oz::null;
}

int raise( int )
{
  return 0;
}

}

#endif

namespace oz
{

#if defined( __native_client__ ) || defined( __ANDROID__ )
#elif defined( _WIN32 )

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

static bool               isConstructed = false;
static int                initFlags = 0;
static volatile int       nBellUsers = 0;
#if defined( __native_client__ ) || defined( __ANDROID__ )
#elif defined( _WIN32 )
static CRITICAL_SECTION   bellCounterLock;
#else
static pthread_spinlock_t bellCounterLock;
#endif

static void resetSignals()
{
  signal( SIGINT,  SIG_DFL );
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
}

static void signalHandler( int sigNum )
{
  resetSignals();

  Log::verboseMode = false;
  Log::printSignal( sigNum );

  StackTrace st = StackTrace::current( 1 );
  Log::printTrace( &st );
  Log::println();

  System::bell();
  System::abort( sigNum == SIGINT );
}

static void catchSignals()
{
  signal( SIGINT,  signalHandler );
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
}

static void terminate()
{
  System::error( 0, "EXCEPTION HANDLING ABORTED" );
}

static void unexpected()
{
  System::error( 0, "EXCEPTION SPECIFICATION VIOLATION" );
}

#if defined( __native_client__ ) || defined( __ANDROID__ )
#elif defined( _WIN32 )

static DWORD WINAPI bellThread( LPVOID )
{
  PlaySound( reinterpret_cast<LPCSTR>( &WAVE_SAMPLE ), null, SND_MEMORY | SND_SYNC );

  EnterCriticalSection( &bellCounterLock );
  --nBellUsers;
  LeaveCriticalSection( &bellCounterLock );

  return 0;
}

#else

static void* bellThread( void* )
{
  pa_simple* pa = pa_simple_new( null, "liboz", PA_STREAM_PLAYBACK, null, "bell", &BELL_SPEC,
                                 null, null, null );
  if( pa != null ) {
    pa_simple_write( pa, BELL_SAMPLE, sizeof( BELL_SAMPLE ), null );
    pa_simple_drain( pa, null );
    pa_simple_free( pa );
  }

  pthread_spin_lock( &bellCounterLock );
  --nBellUsers;
  pthread_spin_unlock( &bellCounterLock );

  return null;
}

#endif

System        System::system;
pp::Module*   System::module   = null;
pp::Instance* System::instance = null;
pp::Core*     System::core     = null;
int           System::width    = 0;
int           System::height   = 0;

System::System()
{
#if defined( __native_client__ ) || defined( __ANDROID__ )
#elif defined( _WIN32 )
  InitializeCriticalSection( &bellCounterLock );
#else
  // Disable default handler for TRAP signal that crashes the process.
  signal( SIGTRAP, SIG_IGN );

  pthread_spin_init( &bellCounterLock, PTHREAD_PROCESS_PRIVATE );
#endif

  isConstructed = true;
}

System::~System()
{
#ifdef _WIN32
  while( nBellUsers != 0 ) {
    Sleep( 10 );
  }
#else
  while( nBellUsers != 0 ) {
    usleep( 10 * 1000 );
  }
#endif
}

void System::abort( bool preventHalt )
{
  resetSignals();

  if( !preventHalt && ( initFlags & HALT_BIT ) ) {
    Log::printHalt();

#ifdef _WIN32
    while( true ) {
      Sleep( 1000 );
    }
#else
    while( sleep( 1 ) == 0 );
#endif
  }

  system.~System();
  ::abort();
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

void System::bell()
{
  if( !isConstructed ) {
    return;
  }

#if defined( __native_client__ ) || defined( __ANDROID__ )
#elif defined( _WIN32 )

  EnterCriticalSection( &bellCounterLock );
  ++nBellUsers;
  LeaveCriticalSection( &bellCounterLock );

  HANDLE thread = CreateThread( null, 0, bellThread, null, 0, null );
  CloseHandle( thread );

#else

  pthread_spin_lock( &bellCounterLock );
  ++nBellUsers;
  pthread_spin_unlock( &bellCounterLock );

  pthread_t thread;
  pthread_create( &thread, null, bellThread, null );

#endif
}

void System::warning( int nSkippedFrames, const char* msg, ... )
{
  trap();

  va_list ap;
  va_start( ap, msg );

  Log::verboseMode = false;
  Log::printRaw( "\n\n" );
  Log::vprintRaw( msg, ap );
  Log::printRaw( "\n" );

  va_end( ap );

  StackTrace st = StackTrace::current( 1 + nSkippedFrames );
  Log::printTrace( &st );
  Log::println();

  bell();
}

void System::error( int nSkippedFrames, const char* msg, ... )
{
  trap();

  va_list ap;
  va_start( ap, msg );

  Log::verboseMode = false;
  Log::printRaw( "\n\n" );
  Log::vprintRaw( msg, ap );
  Log::printRaw( "\n" );

  va_end( ap );

  StackTrace st = StackTrace::current( 1 + nSkippedFrames );
  Log::printTrace( &st );
  Log::println();

  bell();
  abort();
}

void System::init( int flags )
{
  if( initFlags & SIGNAL_HANDLER_BIT ) {
    resetSignals();
  }
  if( initFlags & EXCEPTION_HANDLERS_BIT ) {
    std::set_unexpected( std::unexpected );
    std::set_terminate( std::terminate );
  }

  initFlags = flags;

  if( initFlags & SIGNAL_HANDLER_BIT ) {
    catchSignals();
  }
  if( initFlags & EXCEPTION_HANDLERS_BIT ) {
    std::set_terminate( terminate );
    std::set_unexpected( unexpected );
  }
}

void System::free()
{
  std::set_unexpected( std::unexpected );
  std::set_terminate( std::terminate );

  resetSignals();

  initFlags = 0;
}

}
