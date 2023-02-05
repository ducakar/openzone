/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include "System.hh"

#include "Atomic.hh"
#include "SharedLib.hh"
#include "Log.hh"
#include "Pepper.hh"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(__EMSCRIPTEN__)
# include <pthread.h>
# include <SDL2/SDL.h>
#elif defined(__native_client__)
# include <ppapi/cpp/audio.h>
# include <ppapi/cpp/completion_callback.h>
# include <ppapi/cpp/core.h>
# include <ppapi_simple/ps.h>
# include <ppapi_simple/ps_interface.h>
# include <pthread.h>
#elif defined(_WIN32)
# include <pthread.h>
# include <windows.h>
# include <mmsystem.h>
#else
# include <pulse/simple.h>
# include <pthread.h>
#endif

namespace oz
{

static constexpr float BELL_TIME       = 0.30f;
static constexpr float BELL_FREQUENCY  = 1000.0f;
static constexpr int   BELL_RATE       = 44100;
static constexpr int   BELL_SAMPLES    = int(BELL_TIME * float(BELL_RATE));
static constexpr int   INITIALISED_BIT = 0x80;

enum BellState
{
  NONE,
  PLAYING,
  FINISHED
};

static pthread_mutex_t       bellMutex     = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t        bellCond      = PTHREAD_COND_INITIALIZER;
static Atomic<bool>          hasBellThread = {false};
static System::CrashHandler* crashHandler  = nullptr;
static int                   initFlags     = 0;

OZ_NORETURN
static void abort(bool doHalt);

OZ_NORETURN
static void signalHandler(int sigNum)
{
  Log::verboseMode = false;

  Log::resetIndent();
  Log::println();
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L
  Log::println("Signal %d (%s)", sigNum, strsignal(sigNum));
#else
  Log::println("Signal %d", sigNum);
#endif
  Log::printTrace(StackTrace::current(1));
  Log::println();

  System::bell();
  abort((initFlags & System::HALT_BIT) && sigNum != SIGINT);
}

static void resetSignals()
{
  signal(SIGILL,  SIG_DFL);
  signal(SIGABRT, SIG_DFL);
  signal(SIGFPE,  SIG_DFL);
  signal(SIGSEGV, SIG_DFL);
#ifndef _WIN32
  signal(SIGQUIT, SIG_DFL);
  signal(SIGTRAP, SIG_DFL);
#endif
}

static void catchSignals()
{
  signal(SIGILL,  signalHandler);
  signal(SIGABRT, signalHandler);
  signal(SIGFPE,  signalHandler);
  signal(SIGSEGV, signalHandler);
#ifndef _WIN32
  signal(SIGQUIT, signalHandler);
  // Disable default handler for SIGTRAP that terminates the process.
  signal(SIGTRAP, SIG_IGN);
#endif
}

// Buffer should contain space for (nSamples * 2) 16-bit samples.
static void generateBellSamples(int16* buffer, int nSamples, int rate, int fromSample, int toSample)
{
  float length   = float(nSamples);
  float quotient = BELL_FREQUENCY / float(rate) * Math::TAU;

  for (; fromSample < toSample; ++fromSample) {
    float i = float(fromSample);

    float amplitude = 0.8f * Math::fastSqrt(max<float>(0.0f, (length - i) / length));
    float theta     = i * quotient;
    float value     = amplitude * Math::sin(theta);
    int16 sample    = int16(Math::lround(value * SHRT_MAX));

    buffer[0] = sample;
    buffer[1] = sample;
    buffer   += 2;
  }
}

#if defined(__EMSCRIPTEN__)

static void* bellMain(void*)
{
  return nullptr;
}

#elif defined(__native_client__)

struct SampleInfo
{
  PP_AudioSampleRate rate;
  int                nFrameSamples;
  int                nSamples;
  int                offset;
  pthread_mutex_t    finishMutex   = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t     finishCond    = PTHREAD_COND_INITIALIZER;
  bool               isFinished    = false;
};

static void bellCallback(void* buffer, uint, void* info_)
{
  SampleInfo* info    = static_cast<SampleInfo*>(info_);
  int16*      samples = static_cast<int16*>(buffer);

  if (info->offset >= info->nSamples) {
    pthread_mutex_lock(&info.finishMutex);
    info->isFinished = true;
    pthread_mutex_unlock(&info.finishMutex);
    pthread_cond_signal(&info.finishCond);
  }
  else {
    generateBellSamples(samples, info->nSamples, info->rate, info->offset,
                        info->offset + info->nFrameSamples);
    info->offset += info->nFrameSamples;
  }
}

static void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  pp::InstanceHandle ppInstance(PSGetInstanceId());
  PP_AudioSampleRate rate = pp::AudioConfig::RecommendSampleRate(ppInstance);

  int nFrameSamples = pp::AudioConfig::RecommendSampleFrameCount(ppInstance, rate, 4096);
  int nSamples      = min<int>(int(BELL_TIME * float(rate)), 2 * BELL_SAMPLES);

  SampleInfo      info = {rate, nFrameSamples, nSamples, 0, false};
  pp::AudioConfig config(ppInstance, rate, nFrameSamples);
  pp::Audio       audio(ppInstance, config, bellCallback, &info);

  if (audio.StartPlayback() == PP_TRUE) {
    pthread_mutex_lock(&info.finishMutex);
    while (!info.isFinished) {
      pthread_cond_wait(&info.finishCond, &info.finishMutex);
    }
    pthread_mutex_unlock(&info.finishMutex);

    audio.StopPlayback();
  }

  pthread_cond_destroy(&info.finishCond);
  pthread_mutex_destroy(&info.finishMutex);

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#elif defined(_WIN32)

struct Wave
{
  char  chunkId[4];
  int   chunkSize;
  char  format[4];

  char  subchunk1Id[4];
  int   subchunk1Size;
  int16 audioFormat;
  int16 nChannels;
  int   sampleRate;
  int   byteRate;
  int16 blockAlign;
  int16 bitsPerSample;

  char  subchunk2Id[4];
  int   subchunk2Size;
  int16 samples[BELL_SAMPLES * 2];
};

static void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  Wave* wave = static_cast<Wave*>(alloca(sizeof(Wave)));

  wave->chunkId[0]     = 'R';
  wave->chunkId[1]     = 'I';
  wave->chunkId[2]     = 'F';
  wave->chunkId[3]     = 'F';
  wave->chunkSize      = 36 + sizeof(wave->samples);
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
  wave->sampleRate     = BELL_RATE;
  wave->byteRate       = BELL_RATE * 2 * sizeof(int16);
  wave->blockAlign     = int16(2 * sizeof(int16));
  wave->bitsPerSample  = int16(sizeof(int16) * 8);

  wave->subchunk2Id[0] = 'd';
  wave->subchunk2Id[1] = 'a';
  wave->subchunk2Id[2] = 't';
  wave->subchunk2Id[3] = 'a';
  wave->subchunk2Size  = sizeof(wave->samples);

  generateBellSamples(wave->samples, BELL_SAMPLES, BELL_RATE, 0, BELL_SAMPLES);
  PlaySound(reinterpret_cast<LPCSTR>(wave), nullptr, SND_MEMORY | SND_SYNC);

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#else

enum PulseStatus
{
  NOT_INITIALISED,
  INITIALISED,
  INITIALISATION_FAILED
};

static PulseStatus pulseStatus                = NOT_INITIALISED;
static int16       bellData[BELL_SAMPLES * 2] = {};

static OZ_DL_DEFINE(pa_simple_new)
static OZ_DL_DEFINE(pa_simple_free)
static OZ_DL_DEFINE(pa_simple_write)
static OZ_DL_DEFINE(pa_simple_drain)

static void initialisePulse()
{
  SharedLib libPulseSimple("libpulse-simple.so.0");

  if (libPulseSimple.isOpened()) {
    generateBellSamples(bellData, BELL_SAMPLES, BELL_RATE, 0, BELL_SAMPLES);

    OZ_DL_LOAD(libPulseSimple, pa_simple_new)
    OZ_DL_LOAD(libPulseSimple, pa_simple_free)
    OZ_DL_LOAD(libPulseSimple, pa_simple_write)
    OZ_DL_LOAD(libPulseSimple, pa_simple_drain)

    pulseStatus = INITIALISED;
  }
  else {
    pulseStatus = INITIALISATION_FAILED;
  }
}

static void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  if (pulseStatus == NOT_INITIALISED) {
    initialisePulse();
  }

  if (pulseStatus == INITIALISED) {
    pa_sample_spec sampleSpec = {PA_SAMPLE_S16NE, BELL_RATE, 2};
    pa_simple*     pa         = pa_simple_new(nullptr, "liboz", PA_STREAM_PLAYBACK, nullptr, "bell",
                                              &sampleSpec, nullptr, nullptr, nullptr);

    if (pa != nullptr) {
      pa_simple_write(pa, bellData, sizeof(bellData), nullptr);
      pa_simple_drain(pa, nullptr);
      pa_simple_free(pa);
    }
  }

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#endif

static void waitBell()
{
#ifdef __native_client__
  if (pp::Module::Get()->core()->IsMainThread()) {
    return;
  }
#endif

  pthread_mutex_lock(&bellMutex);
  while (hasBellThread.load<RELAXED>()) {
    pthread_cond_wait(&bellCond, &bellMutex);
  }
  pthread_mutex_unlock(&bellMutex);
}

static void abort(bool doHalt)
{
  resetSignals();

  if (crashHandler != nullptr) {
    crashHandler();
  }

  fflush(stdout);
  fputs(doHalt ? "Halted. Attach a debugger or press Enter to abort ... " : "ABORTED\n", stderr);
  fflush(stderr);

  if (doHalt) {
    fgetc(stdin);
  }

  waitBell();
  _Exit(EXIT_FAILURE);
}

void System::trap()
{
#ifdef _WIN32
  if (IsDebuggerPresent()) {
    DebugBreak();
  }
#else
  raise(SIGTRAP);
#endif
}

void System::bell()
{
  if (!hasBellThread.load<ACQUIRE>() && pthread_mutex_trylock(&bellMutex) == 0) {
    if (!hasBellThread.load<RELAXED>()) {
      pthread_t      bellThread = {};
      pthread_attr_t bellThreadAttr;

      pthread_attr_setdetachstate(&bellThreadAttr, PTHREAD_CREATE_DETACHED);
      if (pthread_create(&bellThread, nullptr, bellMain, nullptr) == 0) {
        hasBellThread.store<RELEASE>(true);
      }
    }
    pthread_mutex_unlock(&bellMutex);
  }
}

void System::error(const char* function, const char* file, int line, int nSkippedFrames,
                   const char* message, ...)
{
  trap();

  va_list ap;
  va_start(ap, message);

  Log::verboseMode = false;

  Log::putsRaw("\n");
  Log::vprintRaw(message, ap);
  Log::printRaw("\n  at %s\n  in %s:%d\n", function, file, line);

  va_end(ap);

  Log::printTrace(StackTrace::current(nSkippedFrames + 1));
  Log::println();

  bell();
  abort(initFlags & HALT_BIT);
}

void System::init(int flags, CrashHandler* handler)
{
  initFlags    = flags | INITIALISED_BIT;
  crashHandler = handler;

  atexit(waitBell);

  if (initFlags & HANDLER_BIT) {
    catchSignals();
  }

#ifdef __native_client__
  PSInterfaceInit();
#endif
}

}
