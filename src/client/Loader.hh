/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <common/Timer.hh>
#include <client/common.hh>

namespace oz
{
namespace client
{

class Loader
{
private:

  static const uint TICK_PERIOD               = 180 * Timer::TICKS_PER_SEC;  // 2 min

  static const uint IMAGO_CLEAR_INTERVAL      = 10  * Timer::TICKS_PER_SEC;  //  10 s (+ 2 s)
  static const uint IMAGO_CLEAR_LAG           = 2   * Timer::TICKS_PER_SEC;

  static const uint FRAG_CLEAR_INTERVAL       = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 0 s)
  static const uint FRAG_CLEAR_LAG            = 0  * Timer::TICKS_PER_SEC;

  static const uint BSP_CLEAR_INTERVAL        = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 20 s)
  static const uint BSP_CLEAR_LAG             = 20  * Timer::TICKS_PER_SEC;

  static const uint MODEL_CLEAR_INTERVAL      = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 40 s)
  static const uint MODEL_CLEAR_LAG           = 40  * Timer::TICKS_PER_SEC;

  static const uint PARTICLE_CLEAR_INTERVAL   = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 60 s)
  static const uint PARTICLE_CLEAR_LAG        = 60  * Timer::TICKS_PER_SEC;

  static const uint BSPAUDIO_CLEAR_INTERVAL   = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 80 s)
  static const uint BSPAUDIO_CLEAR_LAG        = 80  * Timer::TICKS_PER_SEC;

  static const uint AUDIO_CLEAR_INTERVAL      = 5   * Timer::TICKS_PER_SEC;  //   5 s (+ 3 s)
  static const uint AUDIO_CLEAR_LAG           = 3   * Timer::TICKS_PER_SEC;

  static const uint SOURCE_CLEAR_INTERVAL     = 1   * Timer::TICKS_PER_SEC;  //   1 s (+ ~0.5 s)
  static const uint SOURCE_CLEAR_LAG          = 1   * Timer::TICKS_PER_SEC / 2;

  static const uint SOUND_CLEAR_INTERVAL      = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 100 s)
  static const uint SOUND_CLEAR_LAG           = 100 * Timer::TICKS_PER_SEC;

  Thread preloadThread;

  Semaphore preloadMainSemaphore;
  Semaphore preloadAuxSemaphore;

  volatile bool isPreloadAlive;

  uint tick;

private:

  static void preloadMain( void* );

  // clean unused imagines and handle screenshots
  void cleanupRender();
  // stop playing stopped continuous sounds, clean up unused audios
  void cleanupSound();

  // preload scheduled models
  void preloadRender();
  // load scheduled models
  void uploadRender();

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
}
