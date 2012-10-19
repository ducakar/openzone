/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file common/Timer.cc
 */

#include <stable.hh>
#include <common/Timer.hh>

namespace oz
{
namespace common
{

Timer::Timer()
{
  reset();
}

void Timer::reset()
{
  runMicros   = 0;

  ticks       = 0;
  micros      = 0;
  time        = 0.0f;

  nFrames     = 0;
  frameTicks  = 0;
  frameMicros = 0;
  frameTime   = 0.0f;
}

void Timer::tick()
{
  runMicros   += TICK_MICROS;

  ticks       += 1;
  micros      += TICK_MICROS;
  time         = float( micros ) / 1.0e6f;

  frameTicks  += 1;
  frameMicros += TICK_MICROS;
  frameTime    = float( frameMicros ) / 1.0e6f;
}

void Timer::frame()
{
  nFrames    += 1;
  frameTicks  = 0;
  frameMicros = 0;
  frameTime   = 0.0f;
}

void Timer::drop( uint micros_ )
{
  runMicros += micros_;
}

Timer timer;

}
}
