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
 * @file matrix/Frag.hpp
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

struct Cell;
struct Hit;

class Frag
{
  public:

    static const float MAX_ROTVELOCITY;
    static const float DAMAGE_THRESHOLD;

    static Pool<Frag, 2048> pool;

    /*
     *  FIELDS
     */

    Frag*  prev[1];
    Frag*  next[1];

    Cell*  cell;
    int    index;       // position in world.objects vector

    int    type;        // pool of graphical models used
    Point3 p;           // position
    Vec3   velocity;
    Vec3   colour;

    float  restitution; // 1.0 < restitution < 2.0
    float  mass;
    float  lifeTime;

    Frag() : cell( null ), index( -1 )
    {}

    // no copying
    Frag( const Frag& ) = delete;
    Frag& operator = ( const Frag& ) = delete;

    explicit Frag( int index, const Point3& p, const Vec3& velocity, const Vec3& colour,
                   float restitution, float mass, float lifeTime );

    void damage( float damage )
    {
      damage -= DAMAGE_THRESHOLD;

      if( damage > 0.0f ) {
        lifeTime = 0.0f;
      }
    }

    /*
     *  SERIALISATION
     */

    void readFull( InputStream* istream );
    void writeFull( BufferStream* ostream );
    void readUpdate( InputStream* istream );
    void writeUpdate( BufferStream* ostream );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
