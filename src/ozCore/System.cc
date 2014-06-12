/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
#include "SpinLock.hh"
#include "Log.hh"
#include "Java.hh"
#include "Pepper.hh"

#include <clocale>
#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined( __ANDROID__ )
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
#else
# include <ctime>
# include <pthread.h>
# ifndef __linux__
#  include <fcntl.h>
#  include <sys/ioctl.h>
#  include <sys/soundcard.h>
# else
#  include <alsa/asoundlib.h>
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
#ifndef __native_client__
static const int      BELL_PREFERRED_RATE = 48000;
#endif
#ifndef _WIN32
static const timespec TIMESPEC_10MS       = { 0, 10 * 1000000 };
#endif

#if defined( __native_client__ )

struct SampleInfo
{
  pp::Audio audio;
  int       rate;
  int       nFrameSamples;
  int       nSamples;
  int       end;
  int       offset;
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

static SpinLock              bellLock;
static System::CrashHandler* crashHandler       = nullptr;
static int                   initFlags          = 0;
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

static void genBellSamples( short* samples, int nSamples_, int rate, int begin, int end )
{
  float nSamples = float( nSamples_ );
  float quotient = BELL_FREQUENCY / float( rate ) * Math::TAU;

  for( ; begin < end; ++begin ) {
    float i = float( begin );

    float amplitude = 0.8f * Math::fastSqrt( max<float>( 0.0f, ( nSamples - i ) / nSamples ) );
    float theta     = i * quotient;
    float value     = amplitude * Math::sin( theta );
    short sample    = short( Math::lround( value * SHRT_MAX ) );

    samples[0] = sample;
    samples[1] = sample;
    samples   += 2;
  }
}

#if defined( __ANDROID__ )

static void* bellMain( void* )
{
  static_cast<void>( genBellSamples );

  // TODO: Implement bell for OpenSL ES.
  __android_log_write( ANDROID_LOG_DEFAULT, "oz", "*** BELL ***\n" );

  bellLock.unlock();
  return nullptr;
}

#elif defined( __native_client__ )

static void bellCallback( void* buffer, uint, void* info_ )
{
  SampleInfo* info    = static_cast<SampleInfo*>( info_ );
  short*      samples = static_cast<short*>( buffer );

  if( info->offset >= info->end ) {
    bellLock.unlock();
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

  PP_AudioSampleRate rate = pp::AudioConfig::RecommendSampleRate( ppInstance );
  uint nFrameSamples = pp::AudioConfig::RecommendSampleFrameCount( ppInstance, rate, 4096 );

  SampleInfo info;
  info.rate          = rate;
  info.nFrameSamples = int( nFrameSamples );
  info.nSamples      = Math::lround( BELL_TIME * float( rate ) );
  info.end           = info.nSamples + 2 * info.nFrameSamples;
  info.offset        = 0;

  pp::AudioConfig config( ppInstance, rate, nFrameSamples );
  pp::Audio       audio( ppInstance, config, bellCallback, &info );

  if( audio.StartPlayback() == PP_FALSE ) {
    bellLock.unlock();
    return nullptr;
  }

  while( !bellLock.tryLock() ) {
    nanosleep( &TIMESPEC_10MS, nullptr );
  }
  bellLock.unlock();

  audio.StopPlayback();
  return nullptr;
}

#elif defined( _WIN32 )

static DWORD WINAPI bellMain( void* )
{
  Wave* wave = static_cast<Wave*>( alloca( sizeof( Wave ) ) );

  wave->chunkId[0]     = 'R';
  wave->chunkId[1]     = 'I';
  wave->chunkId[2]     = 'F';
  wave->chunkId[3]     = 'F';
  wave->chunkSize      = int( 36 + sizeof( wave->samples ) );
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
  wave->byteRate       = int( BELL_PREFERRED_RATE * 2 * sizeof( short ) );
  wave->blockAlign     = short( 2 * sizeof( short ) );
  wave->bitsPerSample  = short( sizeof( short ) * 8 );

  wave->subchunk2Id[0] = 'd';
  wave->subchunk2Id[1] = 'a';
  wave->subchunk2Id[2] = 't';
  wave->subchunk2Id[3] = 'a';
  wave->subchunk2Size  = sizeof( wave->samples );

  genBellSamples( wave->samples, BELL_WAVE_SAMPLES, BELL_PREFERRED_RATE, 0, BELL_WAVE_SAMPLES );
  PlaySound( reinterpret_cast<LPCSTR>( wave ), nullptr, SND_MEMORY | SND_SYNC );

  bellLock.unlock();
  return 0;
}

#else

static void* bellMain( void* )
{
#ifndef __linux__

  int fd;
  if( ( fd = open( "/dev/dsp", O_WRONLY, 0 ) ) < 0 &&
      ( fd = open( "/dev/dsp0", O_WRONLY, 0 ) ) < 0 &&
      ( fd = open( "/dev/dsp1", O_WRONLY, 0 ) ) < 0 )
  {
    bellLock.unlock();
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

    bellLock.unlock();
    return nullptr;
  }

  int    nSamples = int( BELL_TIME * float( rate ) );
  size_t size     = size_t( nSamples * channels ) * sizeof( short );
  short* samples  = static_cast<short*>( alloca( size ) );

  genBellSamples( samples, nSamples, int( rate ), 0, nSamples );
  write( fd, samples, size );

  close( fd );

#else

  snd_pcm_t* alsa;
  if( snd_pcm_open( &alsa, "default", SND_PCM_STREAM_PLAYBACK, 0 ) != 0 ) {
    bellLock.unlock();
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

    bellLock.unlock();
    return nullptr;
  }

  int    nSamples = int( BELL_TIME * float( rate ) );
  size_t size     = size_t( nSamples * 2 ) * sizeof( short );
  short* samples  = static_cast<short*>( alloca( size ) );

  genBellSamples( samples, nSamples, int( rate ), 0, nSamples );
  snd_pcm_writei( alsa, samples, snd_pcm_uframes_t( nSamples ) );

  snd_pcm_drain( alsa );
  snd_pcm_close( alsa );

#endif

  bellLock.unlock();
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

  while( !bellLock.tryLock() ) {
# ifdef _WIN32
    Sleep( 10 );
# else
    nanosleep( &TIMESPEC_10MS, nullptr );
# endif
  }
  bellLock.unlock();
}

// Wait bell to finish playing on (normal) process termination.
struct BellFinaliser
{
  volatile bool isFinalised = false;

  OZ_HIDDEN
  ~BellFinaliser()
  {
    isFinalised = true;

    waitBell();
  }
};

static BellFinaliser bellFinaliser;

OZ_NORETURN
static void abort( bool doHalt )
{
  static_cast<void>( doHalt );

  resetSignals();

  if( crashHandler != nullptr ) {
    crashHandler();
  }

#ifdef __ANDROID__
  __android_log_write( ANDROID_LOG_FATAL, "liboz", doHalt ? "HALTED\n" : "ABORTED\n"  );
#endif

  fflush( stdout );
  fputs( doHalt ? "Halted. Attach a debugger or press Enter to abort ... " : "ABORTED\n", stderr );
  fflush( stderr );

  if( doHalt ) {
    fgetc( stdin );
  }

  waitBell();
  _Exit( EXIT_FAILURE );
}

const int System::HANDLER_BIT;
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
#ifdef _WIN32

  if( bellLock.tryLock() ) {
    HANDLE bellThread = CreateThread( nullptr, 0, bellMain, nullptr, 0, nullptr );

    if( bellThread == nullptr ) {
      bellLock.unlock();
    }
    else {
      CloseHandle( bellThread );
    }
  }

#else

  if( bellLock.tryLock() ) {
    pthread_t      bellThread;
    pthread_attr_t bellThreadAttr;

    pthread_attr_init( &bellThreadAttr );
    pthread_attr_setdetachstate( &bellThreadAttr, PTHREAD_CREATE_DETACHED );

    if( pthread_create( &bellThread, &bellThreadAttr, bellMain, nullptr ) != 0 ) {
      bellLock.unlock();
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
  bell();

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
}

void System::error( const char* function, const char* file, int line, int nSkippedFrames,
                    const char* msg, ... )
{
  trap();
  bell();

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

  abort( initFlags & HALT_BIT );
}

void System::threadInit()
{
  if( initFlags & HANDLER_BIT ) {
    catchSignals();
  }
}

void System::init( int flags, CrashHandler* crashHandler_ )
{
  initFlags    = flags;
  crashHandler = crashHandler_;

#if !defined( __ANDROID__ ) && !defined( __native_client__ ) && !defined( _WIN32 )

  int fd = open( "/", O_RDONLY );
  close( fd );

  isDebuggerAttached = fd >= 5;

  if( initFlags & LOCALE_BIT ) {
    setlocale( LC_ALL, "" );
  }

#endif

  threadInit();
}

}
