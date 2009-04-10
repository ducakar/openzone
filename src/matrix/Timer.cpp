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

  void Timer::update( int frameMillis_ )
  {
    frameMillis = frameMillis_;
    frameTime   = (float) ( frameMillis / 1000.0f );

    millis      += frameMillis;
    time        = (float) ( millis / 1000.0f );
  }

}
