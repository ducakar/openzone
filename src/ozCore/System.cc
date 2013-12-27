/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/System.cc
 */

#include "System.hh"

#include "StackTrace.hh"
#include "Math.hh"
#include "Log.hh"
#include "Java.hh"
#include "Pepper.hh"

#include <clocale>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined( EMSCRIPTEN )
# include <SDL.h>
#elif defined( __ANDROID__ )
# include <android/log.h>
# include <ctime>
# include <pthread.h>
#elif defined( __native_client__ )
# include <ctime>
# include <ppapi/cpp/audio.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <pthread.h>
#elif defined( _WIN32 )
# include <windows.h>
# include <io.h>
# include <mmsystem.h>
# define isatty( fd ) _isatty( fd )
#else
# include <ctime>
# include <fcntl.h>
# include <pthread.h>
# ifndef __linux__
#  include <sys/ioctl.h>
#  include <sys/soundcard.h>
# else
#  include <alsa/asoundlib.h>
# endif
# ifdef OZ_PULSE_BELL
#  include <cerrno>
#  include <pulse/simple.h>
# endif
#endif

#if defined( __native_client__ ) && !defined( __GLIBC__ )

using namespace oz;

// Fake implementations for signal() and raise() functions missing in newlib library. signal() is
// referenced by SDL hence must be present if we link with it. Those fake implementations also spare
// us several #ifdefs in this file.

extern "C" OZ_WEAK
void ( * signal( int, void ( * )( int ) ) )( int )
{
  return nullptr;
}

extern "C" OZ_WEAK
int raise( int )
{
  return 0;
}

#endif

namespace oz
{

static const float    BELL_TIME           = 0.30f;
static const float    BELL_FREQUENCY      = 1000.0f;
static const int      BELL_PREFERRED_RATE = 44100;
#ifndef _WIN32
static const timespec TIMESPEC_10MS       = { 0, 10 * 1000000 };
#endif

#if defined( __native_client__ )

struct SampleInfo
{
  pp::Audio* audio;
  int        rate;
  int        nFrameSamples;
  int        nSamples;
  int        end;
  int        offset;
};

#elif defined( _WIN32 )

static const int BELL_WAVE_SAMPLES = int( BELL_TIME * BELL_PREFERRED_RATE );

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
  short samples[BELL_WAVE_SAMPLES * 2];
};

#endif

static volatile int          bellLock           = 0;
static int                   initFlags          = 0;
static System::CrashHandler* crashHandler       = nullptr;
static bool                  isDebuggerAttached = false;

OZ_NORETURN
static void abort( bool doHalt );

OZ_NORETURN
static void signalHandler( int sigNum )
{
  Log::verboseMode = false;
  Log::printSignal( sigNum );
  Log::printTrace( StackTrace::current( 1 ) );
  Log::println();

#ifdef __ANDROID__
  __android_log_print( ANDROID_LOG_FATAL, "oz", "Signal %d\n", sigNum );
#endif

  System::bell();
  abort( ( initFlags & System::HALT_BIT ) && sigNum != SIGINT );
}

static void resetSignals()
{
  signal( SIGILL,  SIG_DFL );
  signal( SIGABRT, SIG_DFL );
  signal( SIGFPE,  SIG_DFL );
  signal( SIGSEGV, SIG_DFL );
#ifndef _WIN32
  signal( SIGQUIT, SIG_DFL );
  signal( SIGTRAP, SIG_DFL );
#endif
}

static void catchSignals()
{
  signal( SIGILL,  signalHandler );
  signal( SIGABRT, signalHandler );
  signal( SIGFPE,  signalHandler );
  signal( SIGSEGV, signalHandler );
#ifndef _WIN32
  signal( SIGQUIT, signalHandler );
  // Disable default handler for SIGTRAP that terminates the process.
  signal( SIGTRAP, SIG_IGN );
#endif
}

OZ_NORETURN
static void terminate()
{
  System::trap();

  Log::verboseMode = false;
  Log::putsRaw( "\n\nException handling aborted\n" );
  Log::printTrace( StackTrace::current( 1 ) );
  Log::println();

#ifdef __ANDROID__
  __android_log_write( ANDROID_LOG_FATAL, "oz", "Exception handling aborted\n" );
#endif

  System::bell();
  abort( initFlags & System::HALT_BIT );
}

OZ_NORETURN
static void unexpected()
{
  System::trap();

  Log::verboseMode = false;
  Log::putsRaw( "\n\nException specification violation\n" );
  Log::printTrace( StackTrace::current( 1 ) );
  Log::println();

#ifdef __ANDROID__
  __android_log_write( ANDROID_LOG_FATAL, "oz", "Exception specification violation\n" );
#endif

  System::bell();
  abort( initFlags & System::HALT_BIT );
}

static void genBellSamples( short* samples, int nSamples_, int rate, int begin, int end )
{
  float nSamples = float( nSamples_ );
  float quotient = BELL_FREQUENCY / float( rate ) * Math::TAU;

  for( ; begin < end; ++begin ) {
    float i = float( begin );

    float amplitude = 0.8f * Math::fastSqrt( max( ( nSamples - i ) / nSamples, 0.0f ) );
    float theta     = i * quotient;
    float value     = amplitude * Math::sin( theta );
    short sample    = short( Math::lround( value * SHRT_MAX ) );

    samples[0] = sample;
    samples[1] = sample;
    samples   += 2;
  }
}

#if defined( EMSCRIPTEN )

static void bellMain()
{
  static_cast<void>( genBellSamples );
}

#elif defined( __ANDROID__ )

static void* bellMain( void* )
{
  static_cast<void>( genBellSamples );

  // TODO: Implement bell for OpenSL ES.
  __android_log_write( ANDROID_LOG_DEFAULT, "oz", "*** BELL ***\n" );

  __sync_lock_release( &bellLock );
  return nullptr;
}

#elif defined( __native_client__ )

static void bellCallback( void* buffer, uint, void* info_ )
{
  SampleInfo* info    = static_cast<SampleInfo*>( info_ );
  short*      samples = static_cast<short*>( buffer );

  if( info->offset >= info->end ) {
    __sync_lock_release( &bellLock );
  }
  else {
    genBellSamples( samples, info->nSamples, info->rate, info->offset,
                    info->offset + info->nFrameSamples );
    info->offset += info->nFrameSamples;
  }
}

static void* bellMain( void* )
{
  pp::Instance* ppInstance = Pepper::instance();

  if( ppInstance == nullptr ) {
    return nullptr;
  }

  SampleInfo* info  = static_cast<SampleInfo*>( malloc( sizeof( SampleInfo ) ) );
  pp::Audio*  audio = static_cast<pp::Audio*>( malloc( sizeof( pp::Audio ) ) );

  if( info == nullptr || audio == nullptr ) {
    free( audio );
    free( info );

    __sync_lock_release( &bellLock );
    return nullptr;
  }

  PP_AudioSampleRate rate = pp::AudioConfig::RecommendSampleRate( ppInstance );
  uint nFrameSamples = pp::AudioConfig::RecommendSampleFrameCount( ppInstance, rate, 4096 );
  pp::AudioConfig config( ppInstance, rate, nFrameSamples );

  info->audio         = new( audio ) pp::Audio( ppInstance, config, bellCallback, info );
  info->rate          = rate;
  info->nFrameSamples = int( nFrameSamples );
  info->nSamples      = Math::lround( BELL_TIME * float( rate ) );
  info->end           = info->nSamples + 2*info->nFrameSamples;
  info->offset        = 0;

  if( info->audio->StartPlayback() == PP_FALSE ) {
    info->audio->~Audio();
    free( info->audio );
    free( info );

    __sync_lock_release( &bellLock );
    return nullptr;
  }

  while( bellLock != 0 ) {
    nanosleep( &TIMESPEC_10MS, nullptr );
  }

  info->audio->StopPlayback();
  info->audio->~Audio();
  free( info->audio );
  free( info );
  return nullptr;
}

#elif defined( _WIN32 )

static DWORD WINAPI bellMain( void* )
{
  Wave* wave = static_cast<Wave*>( malloc( sizeof( Wave ) ) );

  wave->chunkId[0]     = 'R';
  wave->chunkId[1]     = 'I';
  wave->chunkId[2]     = 'F';
  wave->chunkId[3]     = 'F';
  wave->chunkSize      = 36 + sizeof( wave->samples );
  wave->format[0]      = 'W';
  wave->format[1]      = 'A';
  wave->format[2]      = 'V';
  wave->format[3]      = 'E';

  wave->subchunk1Id[0] = 'f';
  wave->subchunk1Id[1] = 'm';
  wave->subchunk1Id[2] = 't';
  wave->subchunk1Id[3] = ' ';
  wave->subchunk1Size  = 16;
  wave->audioFormat    = 1;
  wave->nChannels      = 2;
  wave->sampleRate     = BELL_PREFERRED_RATE;
  wave->byteRate       = BELL_PREFERRED_RATE * 2 * sizeof( short );
  wave->blockAlign     = 2 * sizeof( short );
  wave->bitsPerSample  = sizeof( short ) * 8;

  wave->subchunk2Id[0] = 'd';
  wave->subchunk2Id[1] = 'a';
  wave->subchunk2Id[2] = 't';
  wave->subchunk2Id[3] = 'a';
  wave->subchunk2Size  = sizeof( wave->samples );

  genBellSamples( wave->samples, BELL_WAVE_SAMPLES, BELL_PREFERRED_RATE, 0, BELL_WAVE_SAMPLES );

  PlaySound( reinterpret_cast<LPCSTR>( wave ), nullptr, SND_MEMORY | SND_SYNC );
  free( wave );

  __sync_lock_release( &bellLock );
  return 0;
}

#else

static void* bellMain( void* )
{
#ifdef OZ_PULSE_BELL

  const pa_sample_spec PA_SAMPLE_SPEC = { PA_SAMPLE_S16NE, BELL_PREFERRED_RATE, 2 };
# ifndef _GNU_SOURCE
  const char* program_invocation_short_name = "liboz";
# endif

  pa_simple* pa = pa_simple_new( nullptr, program_invocation_short_name, PA_STREAM_PLAYBACK,
                                 nullptr, "bell", &PA_SAMPLE_SPEC, nullptr, nullptr, nullptr );
  if( pa != nullptr ) {
    int    nSamples = int( BELL_TIME * float( BELL_PREFERRED_RATE ) );
    size_t size     = size_t( nSamples * 2 ) * sizeof( short );
    short* samples  = static_cast<short*>( malloc( size ) );

    genBellSamples( samples, nSamples, BELL_PREFERRED_RATE, 0, nSamples );
    pa_simple_write( pa, samples, size, nullptr );
    free( samples );

    pa_simple_drain( pa, nullptr );
    pa_simple_free( pa );

    __sync_lock_release( &bellLock );
    return nullptr;
  }

#endif
#ifndef __linux__

  int fd;
  if( ( fd = open( "/dev/dsp", O_WRONLY, 0 ) ) < 0 &&
      ( fd = open( "/dev/dsp0", O_WRONLY, 0 ) ) < 0 &&
      ( fd = open( "/dev/dsp1", O_WRONLY, 0 ) ) < 0 )
  {
    __sync_lock_release( &bellLock );
    return nullptr;
  }

# if OZ_BYTE_ORDER == 4321
  int format   = AFMT_S16_BE;
# else
  int format   = AFMT_S16_LE;
# endif
  int channels = 2;
  int rate     = BELL_PREFERRED_RATE;

  if( ioctl( fd, SNDCTL_DSP_SETFMT, &format ) < 0 ||
      ioctl( fd, SNDCTL_DSP_CHANNELS, &channels ) < 0 ||
      ioctl( fd, SNDCTL_DSP_SPEED, &rate ) < 0 )
  {
    close( fd );

    __sync_lock_release( &bellLock );
    return nullptr;
  }

  int    nSamples = int( BELL_TIME * float( rate ) );
  size_t size     = size_t( nSamples * channels ) * sizeof( short );
  short* samples  = static_cast<short*>( malloc( size ) );

  genBellSamples( samples, nSamples, int( rate ), 0, nSamples );
  write( fd, samples, size );
  free( samples );

  close( fd );

#else

  snd_pcm_t* alsa;
  if( snd_pcm_open( &alsa, "default", SND_PCM_STREAM_PLAYBACK, 0 ) != 0 ) {
    __sync_lock_release( &bellLock );
    return nullptr;
  }

  snd_pcm_hw_params_t* params;
  snd_pcm_hw_params_alloca( &params );
  snd_pcm_hw_params_any( alsa, params );

  uint rate = BELL_PREFERRED_RATE;

  if( snd_pcm_hw_params_set_access( alsa, params, SND_PCM_ACCESS_RW_INTERLEAVED ) != 0 ||
      snd_pcm_hw_params_set_format( alsa, params, SND_PCM_FORMAT_S16 ) != 0 ||
      snd_pcm_hw_params_set_channels( alsa, params, 2 ) != 0 ||
      snd_pcm_hw_params_set_rate_resample( alsa, params, 0 ) != 0 ||
      snd_pcm_hw_params_set_rate_near( alsa, params, &rate, nullptr ) != 0 ||
      snd_pcm_hw_params( alsa, params ) != 0 ||
      snd_pcm_prepare( alsa ) != 0 )
  {
    snd_pcm_close( alsa );

    __sync_lock_release( &bellLock );
    return nullptr;
  }

  int    nSamples = int( BELL_TIME * float( rate ) );
  size_t size     = size_t( nSamples * 2 ) * sizeof( short );
  short* samples  = static_cast<short*>( malloc( size ) );

  genBellSamples( samples, nSamples, int( rate ), 0, nSamples );
  snd_pcm_writei( alsa, samples, snd_pcm_uframes_t( nSamples ) );
  free( samples );

  snd_pcm_drain( alsa );
  snd_pcm_close( alsa );

#endif

  __sync_lock_release( &bellLock );
  return nullptr;
}

#endif

static void waitBell()
{
#if defined( __native_client__ )
  if( pp::Module::Get()->core()->IsMainThread() ) {
    return;
  }
#endif

#ifndef EMSCRIPTEN
  while( __sync_lock_test_and_set( &bellLock, 1 ) != 0 ) {
# ifdef _WIN32
    Sleep( 10 );
# else
    nanosleep( &TIMESPEC_10MS, nullptr );
# endif
  }
  __sync_lock_release( &bellLock );

#endif
}

// Wait bell to finish playing on (normal) process termination.
struct BellFinaliser
{
  volatile bool isFinalised;

  OZ_HIDDEN
  ~BellFinaliser()
  {
    isFinalised = true;

    waitBell();
  }
};

static BellFinaliser bellFinaliser = { false };

OZ_NORETURN
static void abort( bool doHalt )
{
  static_cast<void>( doHalt );

  resetSignals();

  if( crashHandler != nullptr ) {
    crashHandler();
  }

#if defined( __ANDROID__ )

  __android_log_write( ANDROID_LOG_FATAL, "liboz", "ABORTED\n"  );

#else

# if !defined( EMSCRIPTEN ) && !defined( __native_client )
  doHalt = doHalt && isatty( STDIN_FILENO ) && isatty( STDERR_FILENO );
# endif

  fflush( stdout );
  fputs( doHalt ? "Halted. Attach a debugger or press Enter to abort ... " : "ABORTED\n", stderr );
  fflush( stderr );

  if( doHalt ) {
    fgetc( stdin );
  }

#endif

  waitBell();
  ::abort();
}

const int System::HANDLERS_BIT;
const int System::HALT_BIT;
const int System::LOCALE_BIT;

void System::trap()
{
#if defined( __ANDROID__ ) || defined( __native_client__ )
#elif defined( _WIN32 )
  if( IsDebuggerPresent() ) {
    DebugBreak();
  }
#else
  raise( SIGTRAP );
#endif
}

bool System::isInstrumented()
{
  return isDebuggerAttached;
}

void System::bell()
{
#if defined( EMSCRIPTEN )

  static_cast<void>( bellLock );

  bellMain();

#elif defined( _WIN32 )

  if( __sync_lock_test_and_set( &bellLock, 1 ) == 0 ) {
    HANDLE bellThread = CreateThread( nullptr, 0, bellMain, nullptr, 0, nullptr );

    if( bellThread == nullptr ) {
      __sync_lock_release( &bellLock );
    }
    else {
      CloseHandle( bellThread );
    }
  }

#else

  if( __sync_lock_test_and_set( &bellLock, 1 ) == 0 ) {
    pthread_t      bellThread;
    pthread_attr_t bellThreadAttr;

    pthread_attr_init( &bellThreadAttr );
    pthread_attr_setdetachstate( &bellThreadAttr, PTHREAD_CREATE_DETACHED );

    if( pthread_create( &bellThread, &bellThreadAttr, bellMain, nullptr ) != 0 ) {
      __sync_lock_release( &bellLock );
    }
    pthread_attr_destroy( &bellThreadAttr );
  }

#endif

  // If this occurs during static finalisation bellFinaliser may already be destructed.
  if( bellFinaliser.isFinalised ) {
    waitBell();
  }
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

#ifdef __ANDROID__
  __android_log_vprint( ANDROID_LOG_WARN, "oz", msg, ap );
  __android_log_print( ANDROID_LOG_WARN, "oz", "  in %s\n  at %s:%d\n", function, file, line );
#endif

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

#ifdef __ANDROID__
  __android_log_vprint( ANDROID_LOG_FATAL, "oz", msg, ap );
  __android_log_print( ANDROID_LOG_FATAL, "oz", "  in %s\n  at %s:%d\n", function, file, line );
#endif

  va_end( ap );

  StackTrace st = StackTrace::current( nSkippedFrames + 1 );
  Log::printTrace( st );
  Log::println();

  bell();
  abort( initFlags & HALT_BIT );
}

void System::threadInit()
{
  if( initFlags & HANDLERS_BIT ) {
    catchSignals();
  }
}

void System::init( int flags, CrashHandler* crashHandler_ )
{
  initFlags    = flags;
  crashHandler = crashHandler_;

#if !defined( EMSCRIPTEN ) && !defined( __ANDROID__ ) && !defined( __native_client__ ) && \
    !defined( _WIN32 )

  int fd = open( "/", O_RDONLY );
  close( fd );

  isDebuggerAttached = fd >= 5;

  if( initFlags & LOCALE_BIT ) {
    setlocale( LC_ALL, "" );
  }

#endif

  if( initFlags & HANDLERS_BIT ) {
    std::set_terminate( terminate );
    std::set_unexpected( unexpected );
  }

  threadInit();
}

}
