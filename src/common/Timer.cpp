/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file common/Timer.cpp
 */

#include "common/Timer.hpp"

namespace oz
{

const float Timer::TICK_TIME = float( TICK_MILLIS ) / 1000.0f;

Timer timer;

Timer::Timer()
{
  reset();
}

void Timer::reset()
{
  runMillis   = 0;

  ticks       = 0;
  millis      = 0;
  time        = 0.0f;

  nFrames     = 0;
  frameTicks  = 0;
  frameMillis = 0;
  frameTime   = 0.0f;
}

void Timer::tick()
{
  runMillis   += TICK_MILLIS;

  ++ticks;
  millis      += TICK_MILLIS;
  time        = float( millis ) / 1000.0f;

  ++frameTicks;
  frameMillis += TICK_MILLIS;
  frameTime   += TICK_TIME;
}

void Timer::frame()
{
  ++nFrames;

  frameTicks  = 0;
  frameMillis = 0;
  frameTime   = 0.0f;
}

void Timer::drop( uint millis_ )
{
  runMillis += millis_;
}

}
