/*
 *  Timer.h
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

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

      void init();
      void tick();
      void frame();
  };

  extern Timer timer;

}
