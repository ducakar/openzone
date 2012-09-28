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

#include "Math.hh"
#include "Log.hh"

#include <clocale>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined( __native_client__ )
# include <climits>
# include <ppapi/cpp/audio.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <pthread.h>
#elif defined( _WIN32 )
# include <windows.h>
# include <io.h>
# include <mmsystem.h>
#else
# include <alsa/asoundlib.h>
# include <ctime>
# include <dlfcn.h>
# include <pthread.h>
# include <pulse/simple.h>
# include <unistd.h>
#endif

#if defined( __native_client__ ) && !defined( __GLIBC__ )

using namespace oz;

// Fake implementations for signal() and raise() functions missing in newlib library. signal() is
// referenced by SDL hence must be present if we link with it. Those fake implementations also spare
// us several #ifdefs in this file.

extern "C"
void ( * signal( int, void ( * )( int ) ) )( int )
{
  return nullptr;
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

static const Wave BELL_SAMPLE = {
  { 'R', 'I', 'F', 'F' }, 36 + 3087, { 'W', 'A', 'V', 'E' },
  { 'f', 'm', 't', ' ' }, 16, 1, 1, 11025, 11025, 1, 8,
  { 'd', 'a', 't', 'a' }, 3087, {
# include "bellSample.inc"
  }
};

#else

static const timespec       TIMESPEC_10MS = { 0, 10 * 1000000 };
static const timespec       BELL_TIMESPEC = { 0, 280 * 1000000 };
static const pa_sample_spec BELL_SPEC     = { PA_SAMPLE_U8, 11025, 1 };
static const ubyte          BELL_SAMPLE[] = {
# include "bellSample.inc"
};

static decltype( ::pa_simple_new                  )* pa_simple_new                  = nullptr;
static decltype( ::pa_simple_free                 )* pa_simple_free                 = nullptr;
static decltype( ::pa_simple_write                )* pa_simple_write                = nullptr;

static decltype( ::snd_pcm_open                   )* snd_pcm_open                   = nullptr;
static decltype( ::snd_pcm_close                  )* snd_pcm_close                  = nullptr;
static decltype( ::snd_pcm_hw_params_malloc       )* snd_pcm_hw_params_malloc       = nullptr;
static decltype( ::snd_pcm_hw_params_free         )* snd_pcm_hw_params_free         = nullptr;
static decltype( ::snd_pcm_hw_params_any          )* snd_pcm_hw_params_any          = nullptr;
static decltype( ::snd_pcm_hw_params_set_access   )* snd_pcm_hw_params_set_access   = nullptr;
static decltype( ::snd_pcm_hw_params_set_format   )* snd_pcm_hw_params_set_format   = nullptr;
static decltype( ::snd_pcm_hw_params_set_rate     )* snd_pcm_hw_params_set_rate     = nullptr;
static decltype( ::snd_pcm_hw_params_set_channels )* snd_pcm_hw_params_set_channels = nullptr;
static decltype( ::snd_pcm_hw_params              )* snd_pcm_hw_params              = nullptr;
static decltype( ::snd_pcm_prepare                )* snd_pcm_prepare                = nullptr;
static decltype( ::snd_pcm_writei                 )* snd_pcm_writei                 = nullptr;

#endif

#if defined( __native_client__ )
static pthread_mutex_t    bellLock;
#elif defined( _WIN32 )
static CRITICAL_SECTION   bellLock;
#else
static pthread_spinlock_t bellLock;
#endif
static volatile bool      isBellPlaying     = false;
static bool               isBellInitialised = false;
static int                initFlags         = 0;

OZ_NORETURN
static void abort( bool doHalt );

#if !defined( __native_client__ )

OZ_NORETURN
static void signalHandler( int sigNum )
{
  Log::verboseMode = false;
  Log::printSignal( sigNum );
  Log::printTrace( StackTrace::current( 0 ) );
  Log::println();

  System::bell();
  abort( ( initFlags & System::HALT_BIT ) && sigNum != SIGINT );
}

#endif

static void resetSignals()
{
#if !defined( __native_client__ )
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
# ifndef _WIN32
  signal( SIGQUIT, SIG_DFL );
  signal( SIGTRAP, SIG_DFL );
# endif
#endif
}

static void catchSignals()
{
#if !defined( __native_client__ )
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
# ifndef _WIN32
  signal( SIGQUIT, signalHandler );
  // Disable default handler for SIGTRAP that terminates a process.
  signal( SIGTRAP, SIG_IGN );
# endif
#endif
}

OZ_NORETURN
static void terminate()
{
  System::trap();
  resetSignals();

  Log::verboseMode = false;
  Log::putsRaw( "\n\nException handling aborted\n" );
  Log::printTrace( StackTrace::current( 0 ) );
  Log::println();

  System::bell();
  abort( initFlags & System::HALT_BIT );
}

OZ_NORETURN
static void unexpected()
{
  System::trap();
  resetSignals();

  Log::verboseMode = false;
  Log::putsRaw( "\n\nException specification violation\n" );
  Log::printTrace( StackTrace::current( 0 ) );
  Log::println();

  System::bell();
  abort( initFlags & System::HALT_BIT );
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
  if( audioPtr == nullptr ) {
    OZ_ERROR( "pp::Audio object allocation failed" );
  }

  info->audio = new( audioPtr ) pp::Audio( System::instance, config, bellPlayCallback, info );
  if( info->audio->StartPlayback() == PP_FALSE ) {
    info->audio->~Audio();
    free( info->audio );

    isBellPlaying = false;
  }
}

static void* bellMain( void* )
{
  SampleInfo info;
  System::core->CallOnMainThread( 0, pp::CompletionCallback( bellInitCallback, &info ) );

  while( isBellPlaying ) {
    nanosleep( &TIMESPEC_10MS, nullptr );
  }
  return nullptr;
}

#elif defined( _WIN32 )

static DWORD WINAPI bellMain( LPVOID )
{
  PlaySound( reinterpret_cast<LPCSTR>( &BELL_SAMPLE ), nullptr, SND_MEMORY | SND_SYNC );

  isBellPlaying = false;
  return 0;
}

#else

static void* bellMain( void* )
{
  if( pa_simple_new != nullptr ) {
    pa_simple* pa = pa_simple_new( nullptr, "liboz", PA_STREAM_PLAYBACK, nullptr, "bell",
                                   &BELL_SPEC, nullptr, nullptr, nullptr );
    if( pa != nullptr ) {
      pa_simple_write( pa, BELL_SAMPLE, sizeof( BELL_SAMPLE ), nullptr );

      // pa_simple_drain() takes much longer (~ 1-2 s) than the sample is actually playing, so we
      // use this sleep to ensure the sample has finished playing.
      nanosleep( &BELL_TIMESPEC, nullptr );
      pa_simple_free( pa );

      isBellPlaying = false;
      return nullptr;
    }
  }

  if( snd_pcm_open != nullptr ) {
    snd_pcm_t*           alsa;
    snd_pcm_hw_params_t* params;

    if( snd_pcm_hw_params_malloc( &params ) < 0 ) {
      isBellPlaying = false;
      return nullptr;
    }

    if( snd_pcm_open( &alsa, "default", SND_PCM_STREAM_PLAYBACK, 0 ) < 0 ) {
      snd_pcm_hw_params_free( params );

      isBellPlaying = false;
      return nullptr;
    }

    snd_pcm_hw_params_any( alsa, params );
    snd_pcm_hw_params_set_access( alsa, params, SND_PCM_ACCESS_RW_INTERLEAVED );
    snd_pcm_hw_params_set_format( alsa, params, SND_PCM_FORMAT_U8 );
    snd_pcm_hw_params_set_rate( alsa, params, 11025, 0 );
    snd_pcm_hw_params_set_channels( alsa, params, 1 );
    snd_pcm_hw_params( alsa, params );

    if( snd_pcm_prepare( alsa ) >= 0 ) {
      snd_pcm_writei( alsa, BELL_SAMPLE, sizeof( BELL_SAMPLE ) );
      // Ensure the sample has finished playing.
      nanosleep( &BELL_TIMESPEC, nullptr );
    }

    snd_pcm_close( alsa );
    snd_pcm_hw_params_free( params );
  }

  isBellPlaying = false;
  return nullptr;
}

#endif

static void waitBell()
{
#if defined( __native_client__ )
  if( System::core == nullptr || System::core->IsMainThread() ) {
    return;
  }
#endif

  // Delay termination until bell finishes.
  while( isBellPlaying ) {
#ifdef _WIN32
    Sleep( 10 );
#else
    nanosleep( &TIMESPEC_10MS, nullptr );
#endif
  }
}

static void initBell()
{
#if defined( __native_client__ )

  if( pthread_mutex_init( &bellLock, nullptr ) != 0 ) {
    OZ_ERROR( "Bell mutex creation failed" );
  }

#elif defined( _WIN32 )

  InitializeCriticalSection( &bellLock );
  isBellInitialised = true;

#else

  if( pthread_spin_init( &bellLock, PTHREAD_PROCESS_PRIVATE ) != 0 ) {
    OZ_ERROR( "Bell spin lock creation failed" );
  }

  // Link PulseAudio client library.
  void* pulseLib = dlopen( "libpulse-simple.so.0", RTLD_NOW );

  if( pulseLib != nullptr ) {
    *( void** ) &pa_simple_new   = dlsym( pulseLib, "pa_simple_new" );
    *( void** ) &pa_simple_free  = dlsym( pulseLib, "pa_simple_free" );
    *( void** ) &pa_simple_write = dlsym( pulseLib, "pa_simple_write" );

    if( pa_simple_new == nullptr || pa_simple_free == nullptr || pa_simple_write == nullptr ) {
      pa_simple_new = nullptr;
      dlclose( pulseLib );
    }
  }

  // Link ALSA library.
  void* alsaLib = dlopen( "libasound.so.2", RTLD_NOW );

  if( alsaLib != nullptr ) {
    *( void** ) &snd_pcm_open                   = dlsym( alsaLib, "snd_pcm_open" );
    *( void** ) &snd_pcm_close                  = dlsym( alsaLib, "snd_pcm_close" );
    *( void** ) &snd_pcm_hw_params_malloc       = dlsym( alsaLib, "snd_pcm_hw_params_malloc" );
    *( void** ) &snd_pcm_hw_params_free         = dlsym( alsaLib, "snd_pcm_hw_params_free" );
    *( void** ) &snd_pcm_hw_params_any          = dlsym( alsaLib, "snd_pcm_hw_params_any" );
    *( void** ) &snd_pcm_hw_params_set_access   = dlsym( alsaLib, "snd_pcm_hw_params_set_access" );
    *( void** ) &snd_pcm_hw_params_set_format   = dlsym( alsaLib, "snd_pcm_hw_params_set_format" );
    *( void** ) &snd_pcm_hw_params_set_rate     = dlsym( alsaLib, "snd_pcm_hw_params_set_rate" );
    *( void** ) &snd_pcm_hw_params_set_channels = dlsym( alsaLib, "snd_pcm_hw_params_set_channels" );
    *( void** ) &snd_pcm_hw_params              = dlsym( alsaLib, "snd_pcm_hw_params" );
    *( void** ) &snd_pcm_prepare                = dlsym( alsaLib, "snd_pcm_prepare" );
    *( void** ) &snd_pcm_writei                 = dlsym( alsaLib, "snd_pcm_writei" );

    if( snd_pcm_open == nullptr || snd_pcm_close == nullptr ||
        snd_pcm_hw_params_malloc == nullptr || snd_pcm_hw_params_free == nullptr ||
        snd_pcm_hw_params_any == nullptr || snd_pcm_hw_params_set_access == nullptr ||
        snd_pcm_hw_params_set_format == nullptr || snd_pcm_hw_params_set_rate == nullptr ||
        snd_pcm_hw_params_set_channels == nullptr || snd_pcm_hw_params == nullptr ||
        snd_pcm_prepare == nullptr || snd_pcm_writei == nullptr )
    {
      snd_pcm_open = nullptr;
      dlclose( alsaLib );
    }
  }

#endif
}

OZ_NORETURN
static void abort( bool doHalt )
{
  resetSignals();

#ifdef _WIN32
  if( doHalt && _isatty( STDIN_FILENO ) && _isatty( STDERR_FILENO ) ) {
#else
  if( doHalt && isatty( STDIN_FILENO ) && isatty( STDERR_FILENO ) ) {
#endif
    fputs( "Halted. Attach a debugger or press Enter to quit ... ", stderr );
    fflush( stderr );
    fgetc( stdin );
  }

  waitBell();
  ::abort();
}

pp::Module*   System::module   = nullptr;
pp::Instance* System::instance = nullptr;
pp::Core*     System::core     = nullptr;

OZ_HIDDEN
System System::system;

OZ_HIDDEN
System::System()
{
  if( !isBellInitialised ) {
    initBell();
  }
}

OZ_HIDDEN
System::~System()
{
  waitBell();
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
  if( !isBellInitialised ) {
    initBell();
  }

#if defined( __native_client__ )

  if( instance == nullptr || core == nullptr || pthread_mutex_trylock( &bellLock ) != 0 ) {
    return;
  }

  if( isBellPlaying ) {
    pthread_mutex_unlock( &bellLock );
  }
  else {
    isBellPlaying = true;
    pthread_mutex_unlock( &bellLock );

    pthread_t bellThread;
    if( pthread_create( &bellThread, nullptr, bellMain, nullptr ) != 0 ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
    pthread_detach( bellThread );
  }

#elif defined( _WIN32 )

  EnterCriticalSection( &bellLock );

  if( isBellPlaying ) {
    LeaveCriticalSection( &bellLock );
  }
  else {
    isBellPlaying = true;
    LeaveCriticalSection( &bellLock );

    HANDLE bellThread = CreateThread( nullptr, 0, bellMain, nullptr, 0, nullptr );
    if( bellThread == nullptr ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
    CloseHandle( bellThread );
  }

#else

  if( ( pa_simple_new == nullptr && snd_pcm_open == nullptr ) ||
      pthread_spin_trylock( &bellLock ) != 0 )
  {
    return;
  }

  if( isBellPlaying ) {
    pthread_spin_unlock( &bellLock );
  }
  else {
    isBellPlaying = true;
    pthread_spin_unlock( &bellLock );

    pthread_t bellThread;
    if( pthread_create( &bellThread, nullptr, bellMain, nullptr ) != 0 ) {
      OZ_ERROR( "Bell thread creation failed" );
    }
    pthread_detach( bellThread );
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
  abort( initFlags & HALT_BIT );
}

void System::threadInit()
{
  if( initFlags & SIGNALS_BIT ) {
    catchSignals();
  }
}

void System::init( int flags )
{
#ifdef __native_client__
  flags &= ~HALT_BIT;
#endif
  initFlags = flags;

  if( initFlags & SIGNALS_BIT ) {
    catchSignals();
  }
  else {
    resetSignals();
  }

  if( initFlags & EXCEPTIONS_BIT ) {
    std::set_terminate( terminate );
    std::set_unexpected( unexpected );
  }
  else {
    std::set_unexpected( std::unexpected );
    std::set_terminate( std::terminate );
  }

  if( initFlags & LOCALE_BIT ) {
    setlocale( LC_ALL, "" );
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
