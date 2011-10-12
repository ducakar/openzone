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

      static const int TICK_CLEAR_PERIOD         = 60 * Timer::TICKS_PER_SEC;  // 60 s

      static const int MODEL_CLEAR_INTERVAL      = 20 * Timer::TICKS_PER_SEC;  // 20 s (mod  0 ms)
      static const int MODEL_CLEAR_LAG           = 0;

      static const int BSP_CLEAR_INTERVAL        = 40 * Timer::TICKS_PER_SEC;  // 40 s (mod 10 ms)
      static const int BSP_CLEAR_LAG             = 10;

      static const int AUDIO_CLEAR_INTERVAL      = 20 * Timer::TICKS_PER_SEC;  // 20 s (mod 20 ms)
      static const int AUDIO_CLEAR_LAG           = 20;

      static const int SOURCE_CLEAR_INTERVAL     =  1 * Timer::TICKS_PER_SEC;  //  1 s (mod 30 ms)
      static const int SOURCE_CLEAR_LAG          = 30;

      static const int CONTSOURCE_CLEAR_INTERVAL =  1 * Timer::TICKS_PER_SEC;  //  1 s (mod 40 ms)
      static const int CONTSOURCE_CLEAR_LAG      = 40;

      int tick;

      // clean up unused models, handle screenshots
      void cleanupRender();
      // stop playing stopped continuous sounds, clean up unused audio models
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
