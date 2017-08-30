/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include "SpinLock.hh"
#include "Log.hh"
#include "Pepper.hh"

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(__ANDROID__)
# include <android/log.h>
# include <pthread.h>
# include <SLES/OpenSLES.h>
# include <unistd.h>
# define _Exit(c) _exit(c)
#elif defined(__EMSCRIPTEN__)
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
# include <alsa/asoundlib.h>
# include <pthread.h>
#endif

namespace oz
{

static const float BELL_TIME       = 0.30f;
static const float BELL_FREQUENCY  = 1000.0f;
static const int   BELL_RATE       = 48000;
static const int   BELL_SAMPLES    = int(BELL_TIME * float(BELL_RATE));
static const int   INITIALISED_BIT = 0x80;

enum BellState
{
  NONE,
  PLAYING,
  FINISHED
};

#if defined(__native_client__)

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

#elif defined(_WIN32)

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
  short samples[BELL_SAMPLES * 2];
};

#endif

static pthread_mutex_t       bellMutex     = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t        bellCond      = PTHREAD_COND_INITIALIZER;
static bool                  hasBellThread = false;
static System::CrashHandler* crashHandler  = nullptr;
static int                   initFlags     = 0;

OZ_NORETURN
static void abort(bool doHalt);

OZ_NORETURN
static void signalHandler(int sigNum)
{
  Log::verboseMode = false;

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L
  Log::println("Signal %d (%s)", sigNum, strsignal(sigNum));
#else
  Log::println("Signal %d", sigNum);
#endif
  Log::printTrace(StackTrace::current(1));
  Log::println();

#ifdef __ANDROID__
  __android_log_print(ANDROID_LOG_FATAL, "oz", "Signal %d\n", sigNum);
#endif

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

static void genBellSamples(short* samples, int nSamples_, int rate, int begin, int end)
{
  float nSamples = float(nSamples_);
  float quotient = BELL_FREQUENCY / float(rate) * Math::TAU;

  for (; begin < end; ++begin) {
    float i = float(begin);

    float amplitude = 0.8f * Math::fastSqrt(max<float>(0.0f, (nSamples - i) / nSamples));
    float theta     = i * quotient;
    float value     = amplitude * Math::sin(theta);
    short sample    = short(Math::lround(value * SHRT_MAX));

    samples[0] = sample;
    samples[1] = sample;
    samples   += 2;
  }
}

#if defined(__ANDROID__)

static void* bellMain(void*)
{
  static_cast<void>(genBellSamples);

  SLObjectItf engine;
  SLEngineOption engineOptions[] = {SL_ENGINEOPTION_THREADSAFE, true};
  slCreateEngine(&engine, 1, engineOptions, 0, nullptr, nullptr);
  (*engine)->Realize(engine, false);

  SLEngineItf iEngine;
  (*engine)->GetInterface(engine, SL_IID_ENGINE, &iEngine);

  SLObjectItf outputMix;
  (*iEngine)->CreateOutputMix(iEngine, &outputMix, 0, nullptr, nullptr);
  (*outputMix)->Realize(outputMix, false);

  SLObjectItf               player;
  SLDataFormat_PCM          pcmFormat          = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_48,
                                                  SL_PCMSAMPLEFORMAT_FIXED_16, 16,
                                                  SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                                  SL_BYTEORDER_LITTLEENDIAN
                                                 };
  SLDataLocator_BufferQueue bufferQueueLocator = {SL_DATALOCATOR_BUFFERQUEUE, 1};
  SLDataLocator_OutputMix   outputMixLocator   = {SL_DATALOCATOR_OUTPUTMIX, outputMix};
  SLDataSource              audioSource        = {&bufferQueueLocator, &pcmFormat};
  SLDataSink                audioSink          = {&outputMixLocator, nullptr};

  (*iEngine)->CreateAudioPlayer(iEngine, &player, &audioSource, &audioSink, 0, nullptr, nullptr);
  (*player)->Realize(player, false);

  SLPlayItf iPlay;
  (*player)->GetInterface(player, SL_IID_PLAY, &iPlay);

  SLBufferQueueItf iBufferQueue;
  (*player)->GetInterface(player, SL_IID_BUFFERQUEUE, &iBufferQueue);

  int    size     = BELL_SAMPLES * 2 * sizeof(short);
  short* samples  = static_cast<short*>(alloca(size));

  genBellSamples(samples, BELL_SAMPLES, BELL_RATE, 0, BELL_SAMPLES);
  (*iBufferQueue)->Enqueue(iBufferQueue, samples, size);
  (*iPlay)->SetPlayState(iPlay, SL_PLAYSTATE_PLAYING);

  SLBufferQueueState state;
  do {
    Time::sleep(10);
    (*iBufferQueue)->GetState(iBufferQueue, &state);
  }
  while (state.count != 0);
  (*iPlay)->SetPlayState(iPlay, SL_PLAYSTATE_STOPPED);

  (*player)->Destroy(player);
  (*outputMix)->Destroy(outputMix);
  (*engine)->Destroy(engine);

  return nullptr;
}

#elif defined(__EMSCRIPTEN__)

static void* bellMain(void*)
{
  return nullptr;
}

#elif defined(__native_client__)

static void bellCallback(void* buffer, uint, void* info_)
{
  SampleInfo* info    = static_cast<SampleInfo*>(info_);
  short*      samples = static_cast<short*>(buffer);

  if (info->offset >= info->nSamples) {
    pthread_mutex_lock(&info.finishMutex);
    info->isFinished = true;
    pthread_mutex_unlock(&info.finishMutex);
    pthread_cond_signal(&info.finishCond);
  }
  else {
    genBellSamples(samples, info->nSamples, info->rate, info->offset,
                   info->offset + info->nFrameSamples);
    info->offset += info->nFrameSamples;
  }
}

static void* bellMain(void*)
{
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

  pthread_mutex_lock(&bellMutex);
  hasBellThread = false;
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#elif defined(_WIN32)

static void* bellMain(void*)
{
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
  wave->byteRate       = BELL_RATE * 2 * sizeof(short);
  wave->blockAlign     = short(2 * sizeof(short));
  wave->bitsPerSample  = short(sizeof(short) * 8);

  wave->subchunk2Id[0] = 'd';
  wave->subchunk2Id[1] = 'a';
  wave->subchunk2Id[2] = 't';
  wave->subchunk2Id[3] = 'a';
  wave->subchunk2Size  = sizeof(wave->samples);

  genBellSamples(wave->samples, BELL_SAMPLES, BELL_RATE, 0, BELL_SAMPLES);
  PlaySound(reinterpret_cast<LPCSTR>(wave), nullptr, SND_MEMORY | SND_SYNC);

  pthread_mutex_lock(&bellMutex);
  hasBellThread = false;
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#else

static void* bellMain(void*)
{
  snd_pcm_t* alsa;

  if (snd_pcm_open(&alsa, "default", SND_PCM_STREAM_PLAYBACK, 0) == 0) {
    snd_pcm_hw_params_t* params;
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(alsa, params);

    uint rate = BELL_RATE;

    if (snd_pcm_hw_params_set_access(alsa, params, SND_PCM_ACCESS_RW_INTERLEAVED) == 0 &&
        snd_pcm_hw_params_set_format(alsa, params, SND_PCM_FORMAT_S16) == 0 &&
        snd_pcm_hw_params_set_channels(alsa, params, 2) == 0 &&
        snd_pcm_hw_params_set_rate_resample(alsa, params, 0) == 0 &&
        snd_pcm_hw_params_set_rate_near(alsa, params, &rate, nullptr) == 0 &&
        snd_pcm_hw_params(alsa, params) == 0 &&
        snd_pcm_prepare(alsa) == 0)
    {

      int    nSamples = min<int>(int(BELL_TIME * float(rate)), 2 * BELL_SAMPLES);
      int    size     = nSamples * 2 * sizeof(short);
      short* samples  = static_cast<short*>(alloca(size));

      genBellSamples(samples, nSamples, rate, 0, nSamples);
      snd_pcm_writei(alsa, samples, snd_pcm_uframes_t(nSamples));
      snd_pcm_drain(alsa);
    }

    snd_pcm_close(alsa);
  }

  pthread_mutex_lock(&bellMutex);
  hasBellThread = false;
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
  while (hasBellThread) {
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

#ifdef __ANDROID__
  __android_log_write(ANDROID_LOG_FATAL, "liboz", doHalt ? "HALTED\n" : "ABORTED\n");
#endif

  fflush(stdout);
  fputs(doHalt ? "Halted. Attach a debugger or press Enter to abort ... " : "ABORTED\n", stderr);
  fflush(stderr);

  if (doHalt) {
    fgetc(stdin);
  }

  waitBell();
  _Exit(EXIT_FAILURE);
}

const int System::HANDLER_BIT;
const int System::HALT_BIT;

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
  if (pthread_mutex_trylock(&bellMutex) == 0) {
    if (!hasBellThread) {
      pthread_t      bellThread;
      pthread_attr_t bellThreadAttr;

      pthread_attr_setdetachstate(&bellThreadAttr, PTHREAD_CREATE_DETACHED);
      hasBellThread = pthread_create(&bellThread, nullptr, bellMain, nullptr) == 0;
    }
    pthread_mutex_unlock(&bellMutex);
  }
}

void System::error(const char* function, const char* file, int line, int nSkippedFrames,
                   const char* msg, ...)
{
  trap();

  va_list ap;
  va_start(ap, msg);

#ifdef __ANDROID__
  __android_log_vprint(ANDROID_LOG_FATAL, "oz", msg, ap);
  __android_log_print(ANDROID_LOG_FATAL, "oz", "  in %s\n  at %s:%d\n", function, file, line);
#endif

  Log::verboseMode = false;

  Log::putsRaw("\n\n");
  Log::vprintRaw(msg, ap);
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
