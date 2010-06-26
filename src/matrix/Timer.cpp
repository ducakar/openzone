/*
 *  Timer.cpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Timer.hpp"

namespace oz
{

  const float Timer::TICK_TIME = float( Timer::TICK_MILLIS ) / 1000.0f;

  Timer timer;

  void Timer::init()
  {
    millis        = 0;
    time          = 0.0f;
    nFrames       = 0;
    nirvanaMillis = 0;
    frameMillis   = 0;
    frameTime     = 0.0f;
    frameTicks    = 0;
  }

  void Timer::tick()
  {
    millis      += TICK_MILLIS;
    time        = float( millis ) / 1000.0f;

    frameMillis += TICK_MILLIS;
    frameTime   += TICK_TIME;
    ++frameTicks;
  }

  void Timer::frame()
  {
    ++nFrames;

    frameMillis = 0;
    frameTime   = 0.0f;
    frameTicks  = 0;
  }

}
