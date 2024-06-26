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
#include "Log.hh"

#include <csignal>
#include <cstdio>
#include <cstdlib>

#if defined(__EMSCRIPTEN__)
# include <pthread.h>
# include <AL/al.h>
# include <AL/alc.h>
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

namespace
{

constexpr float BELL_TIME       = 0.30f;
constexpr float BELL_AMPLITUDE  = 0.30f;
constexpr float BELL_FREQUENCY  = 500.0f;
constexpr int   BELL_RATE       = 44100;
constexpr int   INITIALISED_BIT = 0x80;

enum BellState
{
  NONE,
  PLAYING,
  FINISHED
};

pthread_mutex_t       bellMutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t        bellCond      = PTHREAD_COND_INITIALIZER;
Atomic<bool>          hasBellThread = {false};
System::CrashHandler* crashHandler  = nullptr;
int                   initFlags     = 0;

OZ_NORETURN
void abort(bool doHalt);

OZ_NORETURN
void signalHandler(int sigNum)
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

void resetSignals()
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

void catchSignals()
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
void generateBellSamples(float* buffer, int nSamples, int rate)
{
  for (int i = 0; i < nSamples; ++i) {
    float theta  = float(i) / float(rate) * BELL_FREQUENCY * Math::TAU;
    float sample = BELL_AMPLITUDE * Math::sin(theta);

    *buffer++ = sample;
  }
}

#if defined(__EMSCRIPTEN__)

constexpr ALenum FORMAT_MONO_FLOAT32 = 0x10010;
constexpr int    BELL_SAMPLES        = int(BELL_TIME * float(BELL_RATE));

void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  float data[BELL_SAMPLES];
  generateBellSamples(data, BELL_SAMPLES, BELL_RATE);

  ALCdevice* device = alcOpenDevice(nullptr);
  if (device != nullptr) {
    ALCcontext* context = alcCreateContext(device, nullptr);
    if (context != nullptr) {
      alcMakeContextCurrent(context);

      ALuint buffer = 0, source = 0;
      alGenBuffers(1, &buffer);
      alGenSources(1, &source);

      alBufferData(buffer, FORMAT_MONO_FLOAT32, data, sizeof(data), BELL_RATE);
      alSourcei(source, AL_BUFFER, buffer);
      alSourcePlay(source);
      Thread::sleepFor(BELL_TIME * 1.5_s);
      alSourceStop(source);

      alDeleteSources(1, &source);
      alDeleteBuffers(1, &buffer);

      alcDestroyContext(context);
    }

    alcCloseDevice(device);
  }

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#elif defined(_WIN32)

constexpr int BELL_SAMPLES = int(BELL_TIME * float(BELL_RATE));

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
  float samples[BELL_SAMPLES];
};

void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  Wave wave = {
    {'R', 'I', 'F', 'F'},
    36 + sizeof(wave.samples),
    {'W', 'A', 'V', 'E'},

    {'f', 'm', 't', ' '},
    16,
    3,
    1,
    BELL_RATE,
    BELL_RATE * sizeof(float),
    int16(sizeof(float)),
    int16(sizeof(float) * 8),

    {'d', 'a', 't', 'a'},
    sizeof(wave.samples),
    {}
  };

  generateBellSamples(wave.samples, BELL_SAMPLES, BELL_RATE);
  PlaySound(reinterpret_cast<LPCSTR>(&wave), nullptr, SND_MEMORY | SND_SYNC);

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#else

void* bellMain(void*)
{
  pthread_mutex_lock(&bellMutex);

  snd_pcm_t* alsa = nullptr;
  if (snd_pcm_open(&alsa, "default", SND_PCM_STREAM_PLAYBACK, 0) == 0) {
    snd_pcm_hw_params_t* params = nullptr;
    if (snd_pcm_hw_params_malloc(&params) == 0) {
      snd_pcm_hw_params_any(alsa, params);

      uint rate = BELL_RATE;
      if (snd_pcm_hw_params_set_access(alsa, params, SND_PCM_ACCESS_RW_INTERLEAVED) == 0 &&
          snd_pcm_hw_params_set_format(alsa, params, SND_PCM_FORMAT_FLOAT) == 0 &&
          snd_pcm_hw_params_set_channels(alsa, params, 1) == 0 &&
          snd_pcm_hw_params_set_rate_resample(alsa, params, 0) == 0 &&
          snd_pcm_hw_params_set_rate_near(alsa, params, &rate, nullptr) == 0 &&
          snd_pcm_hw_params(alsa, params) == 0 &&
          snd_pcm_prepare(alsa) == 0)
      {
        int    nSamples = int(BELL_TIME * float(rate));
        float* bellData = static_cast<float*>(malloc(nSamples * sizeof(float)));

        if (bellData != nullptr) {
          generateBellSamples(bellData, nSamples, rate);
          snd_pcm_writei(alsa, bellData, snd_pcm_uframes_t(nSamples));
          snd_pcm_drain(alsa);

          free(bellData);
        }
      }

      snd_pcm_hw_params_free(params);
    }

    snd_pcm_close(alsa);
  }

  hasBellThread.store<RELEASE>(false);
  pthread_cond_signal(&bellCond);
  pthread_mutex_unlock(&bellMutex);
  return nullptr;
}

#endif

void waitBell()
{
  pthread_mutex_lock(&bellMutex);
  while (hasBellThread.load<RELAXED>()) {
    pthread_cond_wait(&bellCond, &bellMutex);
  }
  pthread_mutex_unlock(&bellMutex);
}

void abort(bool doHalt)
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
}

}
