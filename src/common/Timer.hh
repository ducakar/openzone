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
 * @file common/Timer.hh
 */

#pragma once

#include "oz/oz.hh"

namespace oz
{

class Timer
{
  public:

    static const     uint  TICKS_PER_SEC = 60;
    static constexpr float TICK_TIME     = 1.0f / float( TICKS_PER_SEC );
    static const     uint  TICK_MICROS   = ( 1000000 + TICKS_PER_SEC / 2 ) / TICKS_PER_SEC;
    static const     uint  TICK_MILLIS   = ( 1000 + TICKS_PER_SEC / 2 ) / TICKS_PER_SEC;

    ulong64 runMicros;

    ulong64 ticks;
    ulong64 micros;
    float   time;

    ulong64 nFrames;
    ulong64 frameTicks;
    ulong64 frameMicros;
    float   frameTime;

    Timer();

    void reset();
    void tick();
    void frame();
    void drop( uint micros );

};

extern Timer timer;

}
