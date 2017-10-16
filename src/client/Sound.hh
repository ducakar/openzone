/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

/**
 * @file client/Sound.hh
 */

#pragma once

#include <client/common.hh>

#include <physfs.h>
#include <AL/alc.h>

namespace oz::client
{

class Sound
{
private:

  static const int   MUSIC_BUFFER_SIZE       = 64 * 1024;
  static const int   MUSIC_INPUT_BUFFER_SIZE = 64 * 1024;
  static const float SOUND_DISTANCE;

  SharedLib                   libeSpeak;

  SBitset<Orbis::MAX_STRUCTS> playedStructs;
  float                       volume_;

  uint                        musicSource;
  uint                        musicBufferIds[2];
  int                         musicBuffersQueued;
  AL::Decoder                 musicDecoder;

  // Music track id to switch to, -1 to do nothing, -2 stop playing.
  Atomic<int>                 selectedTrack;
  int                         streamedTrack;
  Atomic<bool>                hasStreamedBytes;

  Thread                      musicThread;
  Thread                      soundThread;

  Semaphore                   musicMainSemaphore;
  Semaphore                   musicAuxSemaphore;
  Semaphore                   soundMainSemaphore;
  Semaphore                   soundAuxSemaphore;

  Atomic<bool>                isMusicAlive;
  Atomic<bool>                isSoundAlive;

public:

  Duration                    effectsDuration;
  Duration                    musicDuration;

private:

  static void musicMain(void*);
  static void soundMain(void*);

  void musicRun();

  void playCell(int cellX, int cellY);
  void updateMusic();
  void soundRun();

public:

  void setVolume(float volume);
  void setMusicVolume(float volume) const;

  bool isMusicPlaying() const;
  int  getCurrentTrack() const;

  void playMusic(int track);
  void stopMusic();

  void resume() const;
  void suspend() const;

  void play();
  void sync();

  void load();
  void unload();

  void init();
  void destroy();

  void initLibs();

};

extern Sound sound;

}
