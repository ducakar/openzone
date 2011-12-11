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
 * @file common/Timer.hh
 */

#pragma once

#include "oz/oz.hh"

namespace oz
{

class Timer
{
  public:

    static const     int   TICK_MILLIS   = 17;
    static constexpr float TICK_TIME     = float( TICK_MILLIS ) / 1000.0f;
    static const     int   TICKS_PER_SEC = ( 1000 + TICK_MILLIS / 2 ) / TICK_MILLIS;

    uint  runMillis;

    int   ticks;
    uint  millis;
    float time;

    int   nFrames;
    int   frameTicks;
    int   frameMillis;
    float frameTime;

    Timer();

    void reset();
    void tick();
    void frame();
    void drop( uint millis );
};

extern Timer timer;

}
