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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/Frag.hpp
 */

#pragma once

#include "matrix/FragPool.hpp"

namespace oz
{
namespace matrix
{

struct Cell;
struct Hit;

class Frag
{
  public:

    static const float DAMAGE_THRESHOLD;

    static Pool<Frag, 2048> mpool;

    /*
     *  FIELDS
     */

    Frag*  prev[1];
    Frag*  next[1];

    Cell*  cell;
    int    index;         // position in world.objects vector

    const FragPool* pool;

    Point3 p;
    Vec3   velocity;

    float  life;
    float  mass;
    float  restitution;

    // no copying
    Frag( const Frag& ) = delete;
    Frag& operator = ( const Frag& ) = delete;

    void damage( float damage )
    {
      damage -= DAMAGE_THRESHOLD;

      if( damage > 0.0f ) {
        life = 0.0f;
      }
    }

  public:

    explicit Frag( const FragPool* pool, int index, const Point3& p, const Vec3& velocity );
    explicit Frag( const FragPool* pool, InputStream* istream );

    void write( BufferStream* ostream );

    void readUpdate( InputStream* istream );
    void writeUpdate( BufferStream* ostream );

  OZ_STATIC_POOL_ALLOC( mpool )

};

}
}
