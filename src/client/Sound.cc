/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
 */

#include <client/Sound.hh>

#include <client/Context.hh>
#include <client/Camera.hh>

#include <SDL2/SDL.h>

namespace oz::client
{

void* Sound::musicMain(void*)
{
  sound.musicRun();
  return nullptr;
}

void* Sound::soundMain(void*)
{
  sound.soundRun();
  return nullptr;
}

void Sound::musicRun()
{
  streamedTrack = -1;

  while (isMusicAlive.load<RELAXED>()) {
    musicMainSemaphore.post();
    musicAuxSemaphore.wait();

    int oldSelectedTrack = selectedTrack.exchange<RELAXED>(-1);
    if (oldSelectedTrack != -1) {
      if (streamedTrack >= 0) {
        musicDecoder = AL::Decoder();
      }

      streamedTrack = oldSelectedTrack == -2 ? -1 : oldSelectedTrack;

      if (streamedTrack >= 0) {
        musicDecoder = AL::Decoder(liber.musicTracks[streamedTrack].path, true);
      }
    }

    if (streamedTrack >= 0) {
      hasStreamedBytes.store<RELEASE>(musicDecoder.decode());
    }
  }
}

void Sound::playCell(int cellX, int cellY)
{
  const Cell& cell = orbis.cells[cellX][cellY];

  for (int strIndex : cell.structs) {
    if (!playedStructs.get(strIndex)) {
      playedStructs.set(strIndex);

      const Struct* str = orbis.str(strIndex);
      float radius = SOUND_DISTANCE + str->dim().fastN();

      if ((str->p - camera.p).sqN() <= radius*radius) {
        context.playBSP(str);
      }
    }
  }

  OZ_AL_CHECK_ERROR();

  for (const Object& obj : cell.objects) {
    if (obj.flags & Object::AUDIO_BIT) {
      float radius = SOUND_DISTANCE + obj.dim.fastN();

      if ((obj.p - camera.p).sqN() <= radius*radius) {
        context.playAudio(&obj, &obj);
      }
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Sound::updateMusic()
{
  if (!musicMainSemaphore.tryWait()) {
    return;
  }

  if (selectedTrack.load<RELAXED>() != -1) {
    musicBuffersQueued = 0;

    alSourceStop(musicSource);

    int nQueued = 0;
    alGetSourcei(musicSource, AL_BUFFERS_QUEUED, &nQueued);

    if (nQueued != 0) {
      uint buffers[2];
      alSourceUnqueueBuffers(musicSource, nQueued, buffers);
    }

    musicAuxSemaphore.post();
  }
  else if (streamedTrack < 0) {
    musicMainSemaphore.post();
  }
  else {
    bool hasLoaded = false;

    int nProcessed = 0;
    alGetSourcei(musicSource, AL_BUFFERS_PROCESSED, &nProcessed);

    if (nProcessed != 0) {
      if (hasStreamedBytes.load<ACQUIRE>()) {
        hasLoaded = true;

        uint buffer = 0;
        alSourceUnqueueBuffers(musicSource, 1, &buffer);
        musicDecoder.load(buffer);
        alSourceQueueBuffers(musicSource, 1, &buffer);
      }
      else {
        --musicBuffersQueued;

        if (musicBuffersQueued == 0) {
          streamedTrack = -1;
        }
      }
    }
    // If beginning of a track.
    else if (musicBuffersQueued != 2 && hasStreamedBytes.load<ACQUIRE>()) {
      hasLoaded = true;

      int i = musicBuffersQueued;
      ++musicBuffersQueued;

      musicDecoder.load(musicBufferIds[i]);
      alSourceQueueBuffers(musicSource, 1, &musicBufferIds[i]);
      alSourcePlay(musicSource);
    }

    if (musicBuffersQueued != 0) {
      ALint value = 0;
      alGetSourcei(musicSource, AL_SOURCE_STATE, &value);

      if (value == AL_STOPPED) {
        alSourcePlay(musicSource);
      }
    }

    if (hasLoaded) {
      musicAuxSemaphore.post();
    }
    else {
      musicMainSemaphore.post();
    }
  }

  OZ_AL_CHECK_ERROR();
}

void Sound::soundRun()
{
  soundAuxSemaphore.wait();

  while (isSoundAlive.load<RELAXED>()) {
    Instant<STEADY> currentInstant = Instant<STEADY>::now();
    Instant<STEADY> beginInstant   = currentInstant;

    float orientation[] = {
      camera.at.x, camera.at.y, camera.at.z,
      camera.up.x, camera.up.y, camera.up.z
    };

    OZ_AL_CHECK_ERROR();

    // add new sounds
    alListenerfv(AL_ORIENTATION, orientation);
    alListenerfv(AL_POSITION, camera.p);
    alListenerfv(AL_VELOCITY, camera.velocity);

    playedStructs.clear();

    Span span = orbis.getInters(camera.p, SOUND_DISTANCE + Math::sqrt(3.0f) * Object::MAX_DIM);

    for (int x = span.minX ; x <= span.maxX; ++x) {
      for (int y = span.minY; y <= span.maxY; ++y) {
        playCell(x, y);
      }
    }

    currentInstant = Instant<STEADY>::now();
    effectsDuration += currentInstant - beginInstant ;
    beginInstant  = currentInstant;

    updateMusic();

    currentInstant = Instant<STEADY>::now();
    musicDuration += currentInstant - beginInstant ;

    soundMainSemaphore.post();
    soundAuxSemaphore.wait();
  }
}

void Sound::setVolume(float volume)
{
  volume_ = volume;
  alListenerf(AL_GAIN, volume);
}

void Sound::setMusicVolume(float volume) const
{
  alSourcef(musicSource, AL_GAIN, volume);
}

bool Sound::isMusicPlaying() const
{
  return streamedTrack >= 0;
}

int Sound::getCurrentTrack() const
{
  return streamedTrack;
}

void Sound::playMusic(int track)
{
  OZ_ASSERT(track >= 0);

  selectedTrack.store<RELAXED>(track);
}

void Sound::stopMusic()
{
  selectedTrack.store<RELAXED>(-2);
}

void Sound::resume() const
{
  alListenerf(AL_GAIN, volume_);
}

void Sound::suspend() const
{
  alListenerf(AL_GAIN, 0.0f);
}

void Sound::play()
{
  soundAuxSemaphore.post();
}

void Sound::sync()
{
  soundMainSemaphore.wait();
}

void Sound::load()
{
  effectsDuration = Duration::ZERO;
  musicDuration   = Duration::ZERO;
}

void Sound::unload()
{}

void Sound::init()
{
  Log::println("Initialising Sound {");
  Log::indent();

  const char* deviceSpec = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);

  Log::verboseMode = true;
  Log::println("Available OpenAL devices {");
  Log::indent();

  for (const char* s = deviceSpec; *s != '\0'; s += String::length(s) + 1) {
    Log::println("%s", s);
  }

  Log::unindent();
  Log::println("}");
  Log::verboseMode = false;

  OZ_AL_CHECK_ERROR();

  ALCcontext* soundContext = alcGetCurrentContext();
  ALCdevice*  soundDevice  = alcGetContextsDevice(soundContext);

  Log::println("OpenAL device: %s", alcGetString(soundDevice, ALC_DEVICE_SPECIFIER));
  Log::println("OpenAL vendor: %s", alGetString(AL_VENDOR));
  Log::println("OpenAL renderer: %s", alGetString(AL_RENDERER));
  Log::println("OpenAL version: %s", alGetString(AL_VERSION));

  int nAttributes = 0;
  alcGetIntegerv(soundDevice, ALC_ATTRIBUTES_SIZE, 1, &nAttributes);

  List<int> attributes(nAttributes);
  alcGetIntegerv(soundDevice, ALC_ALL_ATTRIBUTES, nAttributes, attributes.begin());

  Log::println("OpenAL attributes {");
  Log::indent();

  for (int i = 0; i < nAttributes; i += 2) {
    switch (attributes[i]) {
      case ALC_FREQUENCY: {
        Log::println("ALC_FREQUENCY: %d Hz", attributes[i + 1]);
        break;
      }
      case ALC_REFRESH: {
        Log::println("ALC_REFRESH: %d Hz", attributes[i + 1]);
        break;
      }
      case ALC_SYNC: {
        Log::println("ALC_SYNC: %s", attributes[i + 1] != 0 ? "on" : "off");
        break;
      }
      case ALC_MONO_SOURCES: {
        Log::println("ALC_MONO_SOURCES: %d", attributes[i + 1]);
        break;
      }
      case ALC_STEREO_SOURCES: {
        Log::println("ALC_STEREO_SOURCES: %d", attributes[i + 1]);
        break;
      }
      default: {
        break;
      }
    }
  }

  Log::unindent();
  Log::println("}");

  const char* sExtensions = alGetString(AL_EXTENSIONS);
  List<String> extensions = String::trim(sExtensions).split(' ');

  Log::verboseMode = true;
  Log::println("OpenAL extensions {");
  Log::indent();

  for (const String& extension : extensions) {
    Log::println("%s", extension.c());
  }

  Log::unindent();
  Log::println("}");
  Log::verboseMode = false;

  selectedTrack.store<RELAXED>(-1);
  streamedTrack = -1;

  alGenBuffers(2, musicBufferIds);
  alGenSources(1, &musicSource);

  musicBuffersQueued = 0;

  alSourcei(musicSource, AL_SOURCE_RELATIVE, AL_TRUE);

  setVolume(appConfig.include("sound.volume", 1.0f).get(0.0f));
  setMusicVolume(0.5f);

  isMusicAlive.store<RELAXED>(true);
  isSoundAlive.store<RELAXED>(true);

  musicThread = Thread("music", musicMain);
  soundThread = Thread("sound", soundMain);

  Log::unindent();
  Log::println("}");

  OZ_AL_CHECK_ERROR();
}

void Sound::destroy()
{
  Log::print("Destroying Sound ...");

  isSoundAlive.store<RELAXED>(false);
  isMusicAlive.store<RELAXED>(false);

  soundAuxSemaphore.post();
  musicAuxSemaphore.post();
  soundThread.join();
  musicThread.join();

  alSourceStop(musicSource);
  alDeleteSources(1, &musicSource);
  alDeleteBuffers(2, musicBufferIds);

  selectedTrack.store<RELAXED>(-1);
  streamedTrack = -1;

  OZ_AL_CHECK_ERROR();

  Log::printEnd(" OK");
}

Sound sound;

}
