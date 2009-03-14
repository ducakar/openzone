/*
 *  Timer.cpp
 *
 *  Timer utility
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

#include "Timer.hpp"

namespace oz
{

  Timer timer;

  void Timer::update( int frameMillis_ )
  {
    frameMillis = frameMillis_;
    frameTime   = frameMillis / 1000.0f;

    millis      += frameMillis;
    time        = millis / 1000.0f;
  }

}
