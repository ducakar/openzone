/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file nirvana/Mind.hh
 */

#pragma once

#include <nirvana/common.hh>

namespace oz
{

class Mind
{
  public:

    static const int FORCE_UPDATE_BIT = 0x00000001;

    Mind* prev[1];
    Mind* next[1];

    int bot;
    int flags;
    int side;

  public:

    explicit Mind( int bot );
    explicit Mind( int bot, InputStream* istream );
    ~Mind();

    void update();

    void write( OutputStream* ostream ) const;

};

}
