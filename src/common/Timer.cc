/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <common/Timer.hh>

namespace oz
{

void Timer::reset()
{
  nTicksInSecond_  = 0;

  nTicks           = 0;
  duration         = Duration::ZERO;

  nFrames          = 0;
  frameTicks       = 0;
  frameDuration    = Duration::ZERO;
  frameTime        = 0.0f;

  realTickDuration = Duration::ZERO;
  realDuration     = Duration::ZERO;
}

void Timer::tick()
{
  nTicks          += 1;
  duration        += TICK_DURATION;

  frameTicks      += 1;
  frameDuration   += TICK_DURATION;
  frameTime       += frameDuration.t();

  realTickDuration = (1_s * (nTicksInSecond_ + 1)) / TICKS_PER_SEC -
                     (1_s * nTicksInSecond_) / TICKS_PER_SEC;
  realDuration    += realTickDuration;

  nTicksInSecond_  = (nTicksInSecond_ + 1) % TICKS_PER_SEC;
}

void Timer::frame()
{
  nFrames      += 1;
  frameTicks    = 0;
  frameDuration = Duration::ZERO;
  frameTime     = 0.0f;
}

void Timer::drop(Duration skipDuration)
{
  realDuration += skipDuration;
}

Timer timer;

}
