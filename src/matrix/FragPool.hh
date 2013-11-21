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
 * @file matrix/FragPool.hh
 */

#pragma once

#include <matrix/common.hh>

namespace oz
{

class Frag;

class FragPool
{
  public:

    // OR'ed to the client::FragPoo::flags, so we must assure bits don't overlap
    static const int FADEOUT_BIT = 0x0100;

    String    name;
    int       id;

    int       flags;

    float     velocitySpread; ///< Used when generating multiple frags in `Object::onDestroy()` or
                              ///< `Struct::destroy()`.

    float     life;
    float     lifeSpread;

    float     mass;
    float     elasticity;

    List<int> models;

  public:

    explicit FragPool( const JSON& config, const char* name, int id );

    Frag* create( int index, const Point& pos, const Vec3& velocity ) const;
    Frag* create( InputStream* is ) const;

};

}
