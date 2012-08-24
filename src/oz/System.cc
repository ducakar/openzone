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
#include "Math.hh"
#include "Log.hh"

#include "windefs.h"
#include <clocale>
#include <csignal>
#include <cstdlib>

#if defined( __native_client__ )
# include <climits>
# include <ppapi/cpp/audio.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <pthread.h>
# include <unistd.h>
#elif defined( __ANDROID__ )
#elif defined( _WIN32 )
# include <windows.h>
# include <mmsystem.h>
#else
# include <ctime>
# include <dlfcn.h>
# include <pthread.h>
# include <pulse/simple.h>
# include <unistd.h>
#endif

#if defined( __native_client__ ) && !defined( __GLIBC__ )

// Fake implementations for signal() and raise() functions missing in newlib library. signal() is
// referenced by SDL hence must be present if we link with it. Those fake implementations also spare
// us several #ifdefs in this file.

extern "C"
void ( * signal( int, void ( * )( int ) ) )( int )
{
  return oz::null;
}

extern "C"
int raise( int )
{
  return 0;
}

#endif

namespace oz
{

#if defined( __native_client__ )

static const timespec TIMESPEC_10MS    = { 0, 10 * 1000000 };
static const float    SAMPLE_LENGTH    = 0.28f;
static const float    SAMPLE_GUARD     = 0.28f;
static const float    SAMPLE_FREQUENCY = 800.0f;

struct SampleInfo
{
  pp::Audio* audio;
  int        nFrameSamples;
  int        nSamples;
  float      quotient;
  int        end;
  int        offset;
};

#elif defined( __ANDROID__ )
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
  { 'R', 'I', 'F', 'F' }, 36 + 3087, { 'W', 'A', 'V', 'E' },
  { 'f', 'm', 't', ' ' }, 16, 1, 1, 11025, 11025, 1, 8,
  { 'd', 'a', 't', 'a' }, 3087, {
# include "bellSample.inc"
  }
};

#else

static const timespec       TIMESPEC_10MS = { 0, 10 * 1000000 };
static const pa_sample_spec BELL_SPEC     = { PA_SAMPLE_U8, 11025, 1 };
static const ubyte          BELL_SAMPLE[] = {
# include "bellSample.inc"
};
static const timespec       BELL_TIMESPEC = {
  0, long( float( sizeof( BELL_SAMPLE ) ) / float( BELL_SPEC.rate ) * 1e9f )
};

static decltype( ::pa_simple_new )*   pa_simple_new;   // = null
static decltype( ::pa_simple_free )*  pa_simple_free;  // = null
static decltype( ::pa_simple_write )* pa_simple_write; // = null

#endif

static bool               isConstructed; // = false
static volatile bool      isBellPlaying; // = false
static int                initFlags;     // = 0
#if defined( __native_client__ )
static pthread_mutex_t    bellLock;
#elif defined( __ANDROID__ )
#elif defined( _WIN32 )
static CRITICAL_SECTION   bellLock;
#else
static pthread_spinlock_t bellLock;
#endif

OZ_NORETURN
static void signalHandler( int signum );
static void construct();

OZ_NORETURN
static void terminate()
{
  OZ_ERROR( "Exception handling aborted" );
}

OZ_NORETURN
static void unexpected()
{
  OZ_ERROR( "Exception specification violation" );
}

static void resetSignals()
{
  signal( SIGINT,  SIG_DFL );
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
}

static void catchSignals()
{
  signal( SIGINT,  signalHandler );
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
}

OZ_NORETURN
static void signalHandler( int sigNum )
{
  if( !isConstructed ) {
    construct();
  }

  resetSignals();

  Log::verboseMode = false;
  Log::printSignal( sigNum );

  StackTrace st = StackTrace::current( 0 );
  Log::printTrace( st );
  Log::println();

  System::bell();
  System::abort( sigNum == SIGINT );
}

static void construct()
{
  setlocale( LC_CTYPE, "" );

#if defined( __native_client__ )

  if( pthread_mutex_init( &bellLock, null ) != 0 ) {
    OZ_ERROR( "Bell mutex creation failed" );
  }

#elif defined( __ANDROID__ )
#elif defined( _WIN32 )

  InitializeCriticalSection( &bellLock );

#else

  // Disable default handler for TRAP signal that crashes the process.
  signal( SIGTRAP, SIG_IGN );

  if( pthread_spin_init( &bellLock, PTHREAD_PROCESS_PRIVATE ) != 0 ) {
    OZ_ERROR( "Bell spin lock creation failed" );
  }

  void* library = dlopen( "libpulse-simple.so.0", RTLD_NOW );

  if( library != null ) {
    *( void** )( &pa_simple_new )   = dlsym( library, "pa_simple_new" );
    *( void** )( &pa_simple_free )  = dlsym( library, "pa_simple_free" );
    *( void** )( &pa_simple_write ) = dlsym( library, "pa_simple_write" );

    if( pa_simple_new == null || pa_simple_free == null || pa_simple_write == null ) {
      pa_simple_new   = null;
      pa_simple_free  = null;
      pa_simple_write = null;

      dlclose( library );
    }
  }

#endif

  isConstructed = true;
}

#if defined( __native_client__ )

static void stopBellCallback( void* info_, int )
{
  SampleInfo* info = static_cast<SampleInfo*>( info_ );

  info->audio->StopPlayback();
  info->audio->~Audio();
  free( info->audio );

  isBellPlaying = false;
}

static void bellPlayCallback( void* buffer, uint size, void* info_ )
{
  static_cast<void>( size );

  SampleInfo* info    = static_cast<SampleInfo*>( info_ );
  short*      samples = static_cast<short*>( buffer );

  hard_assert( size / uint( sizeof( short[2] ) ) >= uint( info->nFrameSamples ) );

  if( info->offset >= info->end ) {
    System::core->CallOnMainThread( 0, pp::CompletionCallback( stopBellCallback, info ) );
  }

  int begin = info->offset;
  info->offset += info->nFrameSamples;

  for( int i = begin; i < info->offset; ++i ) {
    float position  = float( info->nSamples - 1 - i ) / float( info->nSamples - 1 );
    float amplitude = Math::fastSqrt( max( position, 0.0f ) );
    float value     = amplitude * Math::sin( float( i ) * info->quotient );
    short sample    = short( float( SHRT_MAX ) * value );

    samples[0] = sample;
    samples[1] = sample;
    samples   += 2;
  }
}

static void bellInitCallback( void* info_, int )
{
  SampleInfo* info = static_cast<SampleInfo*>( info_ );

  PP_AudioSampleRate rate = pp::AudioConfig::RecommendSampleRate( System::instance );
  uint nFrameSamples = pp::AudioConfig::RecommendSampleFrameCount( System::instance, rate, 4096 );

  pp::AudioConfig config( System::instance, rate, nFrameSamples );

  info->nFrameSamples = int( nFrameSamples );
  info->nSamples      = int( SAMPLE_LENGTH * float( rate ) + 0.5f );
  info->quotient      = SAMPLE_FREQUENCY * Math::TAU / float( rate );
  info->end           = info->nSamples + int( SAMPLE_GUARD * float( rate ) + 0.5f );
  info->offset        = 0;

  void* audioPtr = malloc( sizeof( pp::Audio ) );
  if( audioPtr == null ) {
    OZ_ERROR( "pp::Audio object allocation failed" );
  }

  info->audio = new( audioPtr ) pp::Audio( System::instance, config, bellPlayCallback, info );
  if( info->audio->StartPlayback() == PP_FALSE ) {
    info->audio->~Audio();
    free( info->audio );

    isBellPlaying = false;
  }
}

static void* bellThread( void* )
{
  SampleInfo info;
  System::core->CallOnMainThread( 0, pp::CompletionCallback( bellInitCallback, &info ) );

  while( isBellPlaying ) {
    nanosleep( &TIMESPEC_10MS, null );
  }
  return null;
}

#elif defined( __ANDROID__ )
#elif defined( _WIN32 )

static DWORD WINAPI bellThread( LPVOID )
{
  PlaySound( reinterpret_cast<LPCSTR>( &WAVE_SAMPLE ), null, SND_MEMORY | SND_SYNC );

  isBellPlaying = false;
  return 0;
}

#else

static void* bellThread( void* )
{
  pa_simple* pa = pa_simple_new( null, "liboz", PA_STREAM_PLAYBACK, null, "bell", &BELL_SPEC, null,
                                 null, null );
  if( pa != null ) {
    pa_simple_write( pa, BELL_SAMPLE, sizeof( BELL_SAMPLE ), null );

    // pa_simple_drain() takes much longer (~ 1-2 s) than the sample is actually playing, so we use
    // this sleep to ensure the sample has finished playing.
    nanosleep( &BELL_TIMESPEC, null );
    pa_simple_free( pa );
  }

  isBellPlaying = false;
  return null;
}

#endif

static void waitBell()
{
#if defined( __native_client__ )
  if( System::core->IsMainThread() ) {
    return;
  }
#endif

  // Delay termination until bell finishes.
  while( isBellPlaying ) {
#ifdef _WIN32
    Sleep( 10 );
#else
    nanosleep( &TIMESPEC_10MS, null );
#endif
  }
}

#ifdef __native_client__
pp::Module*   System::module;   // = null
pp::Instance* System::instance; // = null
pp::Core*     System::core;     // = null
#endif

OZ_HIDDEN
System System::system;

OZ_HIDDEN
System::System()
{
  if( !isConstructed ) {
    construct();
  }
}

OZ_HIDDEN
System::~System()
{
  waitBell();
}

void System::abort( bool preventHalt )
{
  resetSignals();

  if( !preventHalt && ( initFlags & HALT_BIT ) ) {
    Log::printHalt();

#ifdef _WIN32
    while( true ) {
      Sleep( 10 );
    }
#else
    while( nanosleep( &TIMESPEC_10MS, null ) == 0 );
#endif
  }

  waitBell();
  ::abort();
}

void System::trap()
{
  if( !isConstructed ) {
    construct();
  }

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
    construct();
  }

#if defined( __native_client__ )

  if( instance == null || core == null || pthread_mutex_trylock( &bellLock ) != 0 ) {
    return;
  }

  if( isBellPlaying ) {
    pthread_mutex_unlock( &bellLock );
  }
  else {
    isBellPlaying = true;
    pthread_mutex_unlock( &bellLock );

    pthread_t thread;
    if( pthread_create( &thread, null, bellThread, null ) != 0 ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
  }

#elif defined( __ANDROID__ )
#elif defined( _WIN32 )

  EnterCriticalSection( &bellLock );

  if( isBellPlaying ) {
    LeaveCriticalSection( &bellLock );
  }
  else {
    isBellPlaying = true;
    LeaveCriticalSection( &bellLock );

    HANDLE thread = CreateThread( null, 0, bellThread, null, 0, null );
    if( thread == null ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
    CloseHandle( thread );
  }

#else

  if( pa_simple_new == null || pthread_spin_trylock( &bellLock ) != 0 ) {
    return;
  }

  if( isBellPlaying ) {
    pthread_spin_unlock( &bellLock );
  }
  else {
    isBellPlaying = true;
    pthread_spin_unlock( &bellLock );

    pthread_t thread;
    if( pthread_create( &thread, null, bellThread, null ) != 0 ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
  }

#endif
}

void System::warning( const char* function, const char* file, int line, int nSkippedFrames,
                      const char* msg, ... )
{
  trap();

  va_list ap;
  va_start( ap, msg );

  bool verboseMode = Log::verboseMode;

  Log::verboseMode = false;
  Log::putsRaw( "\n\n" );
  Log::vprintRaw( msg, ap );
  Log::printRaw( "\n  in %s\n  at %s:%d\n", function, file, line );

  Log::verboseMode = verboseMode;

  va_end( ap );

  StackTrace st = StackTrace::current( nSkippedFrames + 1 );
  Log::printTrace( st );
  Log::println();

  bell();
}

void System::error( const char* function, const char* file, int line, int nSkippedFrames,
                    const char* msg, ... )
{
  trap();

  va_list ap;
  va_start( ap, msg );

  Log::verboseMode = false;
  Log::putsRaw( "\n\n" );
  Log::vprintRaw( msg, ap );
  Log::printRaw( "\n  in %s\n  at %s:%d\n", function, file, line );

  va_end( ap );

  StackTrace st = StackTrace::current( nSkippedFrames + 1 );
  Log::printTrace( st );
  Log::println();

  bell();
  abort();
}

void System::init( int flags )
{
#ifdef __native_client__
  flags &= ~HALT_BIT;
#endif

  if( !isConstructed ) {
    construct();
  }

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
