/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/FragPool.hpp
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

class Frag;

class FragPool
{
  public:

    String name;

    float  velocitySpread; ///< Used when generating multiple frags from <code>Object::onDestroy()
                           ///< </code> or <code>Struct::destroy()</code>.

    float  life;
    float  lifeSpread;
    float  mass;
    float  restitution;

    Vector<int> models;

  public:

    explicit FragPool( const char* name );

    Frag* create( int index, const Point3& pos, const Vec3& velocity ) const;
    Frag* create( InputStream* istream ) const;

};

}
}
