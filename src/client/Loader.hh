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

/**
 * @file client/Loader.hh
 */

#pragma once

#include <client/common.hh>

namespace oz::client
{

class Loader
{
private:

  static constexpr uint TICK_PERIOD               = 180 * Timer::TICKS_PER_SEC;  // 2 min

  static constexpr uint IMAGO_CLEAR_INTERVAL      = 10  * Timer::TICKS_PER_SEC;  //  10 s (+ 2 s)
  static constexpr uint IMAGO_CLEAR_LAG           = 2   * Timer::TICKS_PER_SEC;

  static constexpr uint FRAG_CLEAR_INTERVAL       = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 0 s)
  static constexpr uint FRAG_CLEAR_LAG            = 0   * Timer::TICKS_PER_SEC;

  static constexpr uint BSP_CLEAR_INTERVAL        = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 20 s)
  static constexpr uint BSP_CLEAR_LAG             = 20  * Timer::TICKS_PER_SEC;

  static constexpr uint MODEL_CLEAR_INTERVAL      = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 40 s)
  static constexpr uint MODEL_CLEAR_LAG           = 40  * Timer::TICKS_PER_SEC;

  static constexpr uint PARTICLE_CLEAR_INTERVAL   = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 60 s)
  static constexpr uint PARTICLE_CLEAR_LAG        = 60  * Timer::TICKS_PER_SEC;

  static constexpr uint BSPAUDIO_CLEAR_INTERVAL   = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 80 s)
  static constexpr uint BSPAUDIO_CLEAR_LAG        = 80  * Timer::TICKS_PER_SEC;

  static constexpr uint AUDIO_CLEAR_INTERVAL      = 5   * Timer::TICKS_PER_SEC;  //   5 s (+ 3 s)
  static constexpr uint AUDIO_CLEAR_LAG           = 3   * Timer::TICKS_PER_SEC;

  static constexpr uint SOURCE_CLEAR_INTERVAL     = 1   * Timer::TICKS_PER_SEC;  //   1 s (+ ~0.5 s)
  static constexpr uint SOURCE_CLEAR_LAG          = 1   * Timer::TICKS_PER_SEC / 2;

  static constexpr uint SOUND_CLEAR_INTERVAL      = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 100 s)
  static constexpr uint SOUND_CLEAR_LAG           = 100 * Timer::TICKS_PER_SEC;

private:

  Thread       preloadThread;

  Semaphore    preloadMainSemaphore;
  Semaphore    preloadAuxSemaphore;

  Atomic<bool> isPreloadAlive;

  uint         tick;

private:

  static void* preloadMain(void*);

  // Clean unused imagines.
  void updateRender();
  // Stop playing stopped continuous sounds, clean up unused audios.
  void updateSound();

  // Remove unused models, BSPs, FragPools.
  void cleanupRender();
  // Remove unused sound buffers.
  void cleanupSound();

  // preload scheduled models
  void preloadRender();
  // load scheduled models
  void uploadRender(bool isOneShot);
  // Reload terra and/or caelum if changed.
  void updateEnvironment();

  void preloadRun();

public:

  void makeScreenshot();

  void syncUpdate();
  void update();

  void load();
  void unload();

  void init();
  void destroy();

};

extern Loader loader;

}
