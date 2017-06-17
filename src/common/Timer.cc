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

constexpr long64 Timer::TICKS_PER_SEC;
constexpr Duration Timer::TICK_DURATION;
constexpr float Timer::TICK_TIME;

void Timer::reset()
{
  runDuration   = Duration::ZERO;

  ticks         = 0;
  time          = Duration::ZERO;

  nFrames       = 0;
  frameTicks    = 0;
  frameDuration = Duration::ZERO;
  frameTime     = 0.0f;
}

void Timer::tick()
{
  runDuration   += TICK_DURATION;

  ticks         += 1;
  time          += TICK_DURATION;

  frameTicks    += 1;
  frameDuration += TICK_DURATION;
  frameTime     += TICK_TIME;
}

void Timer::frame()
{
  nFrames      += 1;
  frameTicks    = 0;
  frameDuration = Duration::ZERO;
  frameTime     = 0.0f;
}

void Timer::drop(Duration duration)
{
  runDuration += duration;
}

Timer timer;

}
