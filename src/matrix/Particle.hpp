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
 * @file matrix/Particle.hpp
 */

#pragma once

#include "matrix/common.hpp"

namespace oz
{
namespace matrix
{

struct Cell;
struct Hit;

class Particle
{
  public:

    static const float MAX_ROTVELOCITY;
    static const float DAMAGE_THRESHOLD;

    static Pool<Particle, 2048> pool;

    /*
     *  FIELDS
     */

    Particle* prev[1];
    Particle* next[1];

    Cell*     cell;
    int       index;        // position in world.objects vector

    Point3    p;            // position
    Vec3      velocity;

    // graphics data
    Vec3      colour;
    Vec3      rot;
    Vec3      rotVelocity;

    float     restitution;    // 1.0 < restitution < 2.0
    float     mass;
    float     lifeTime;

    Particle() : cell( null ), index( -1 )
    {}

    // no copying
    Particle( const Particle& ) = delete;
    Particle& operator = ( const Particle& ) = delete;

    explicit Particle( int index, const Point3& p, const Vec3& velocity, const Vec3& colour,
                       float restitution, float mass, float lifeTime );

    void damage( float damage )
    {
      damage -= DAMAGE_THRESHOLD;

      if( damage > 0.0f ) {
        lifeTime = 0.0f;
      }
    }

    void update()
    {
      rot += rotVelocity;
    }

    /*
     *  SERIALISATION
     */

    void readFull( InputStream* istream );
    void writeFull( OutputStream* ostream );
    void readUpdate( InputStream* istream );
    void writeUpdate( OutputStream* ostream );

  OZ_STATIC_POOL_ALLOC( pool )

};

}
}
