/*
 *  Timer.cpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Timer.h"

namespace oz
{

  Timer timer;

  void Timer::init()
  {
    millis      = 0;
    time        = 0.0f;
    nFrames     = 0;
    frameMillis = 0;
    frameTime   = 0.0f;
    frameTicks  = 0;
  }

  void Timer::tick()
  {
    millis      += TICK_MILLIS;
    time        = (float) ( millis / 1000.0f );

    frameMillis += TICK_MILLIS;
    frameTime   += TICK_TIME;
    frameTicks++;
  }

  void Timer::frame()
  {
    nFrames++;

    frameMillis = 0;
    frameTime   = 0.0f;
    frameTicks  = 0;
  }

}
