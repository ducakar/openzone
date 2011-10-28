/*
 *  Loader.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Timer.hpp"

namespace oz
{
namespace client
{

  class Loader
  {
    private:

      static const int TICK_PERIOD               = 180 * Timer::TICKS_PER_SEC;  // 2 min

      static const int IMAGO_CLEAR_INTERVAL      = 10  * Timer::TICKS_PER_SEC;  // 10 s (+ 3 s)
      static const int IMAGO_CLEAR_LAG           = 3   * Timer::TICKS_PER_SEC;

      static const int IMAGOCLASS_CLEAR_INTERVAL = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 0 min)
      static const int IMAGOCLASS_CLEAR_LAG      = 0   * Timer::TICKS_PER_SEC;

      static const int BSP_CLEAR_INTERVAL        = 120 * Timer::TICKS_PER_SEC;  // 2 min (+ 1 min)
      static const int BSP_CLEAR_LAG             = 60  * Timer::TICKS_PER_SEC;

      static const int SOURCE_CLEAR_INTERVAL     = 1   * Timer::TICKS_PER_SEC;  //  1 s (+ ~0.5 s)
      static const int SOURCE_CLEAR_LAG          = 1   * Timer::TICKS_PER_SEC / 2;

      static const int AUDIO_CLEAR_INTERVAL      = 10  * Timer::TICKS_PER_SEC;  // 10 s (+ 6 s)
      static const int AUDIO_CLEAR_LAG           = 6   * Timer::TICKS_PER_SEC;

      int tick;

      // clean unused imagines and handle screenshots
      void cleanupRender();
      // stop playing stopped continuous sounds, clean up unused audios
      void cleanupSound();

  public:

      void cleanup();
      void update();

      void makeScreenshot();

      void init();
      void free();

  };

  extern Loader loader;

}
}
