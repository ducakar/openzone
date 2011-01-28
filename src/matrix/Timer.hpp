/*
 *  Timer.hpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

namespace oz
{

  class Timer
  {
    public:

      static const int   TICK_MILLIS = 20;
      static const float TICK_TIME;

      int   millis;
      float time;
      int   nFrames;
      int   nirvanaMillis;

      int   frameMillis;
      float frameTime;
      int   frameTicks;

      explicit Timer();

      void reset();
      void tick();
      void frame();
  };

  extern Timer timer;

}
