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
 * @file matrix/Dynamic.hh
 */

#pragma once

#include "matrix/Object.hh"
#include "matrix/DynamicClass.hh"

namespace oz
{
namespace matrix
{

class Dynamic : public Object
{
  public:

    static Pool<Dynamic, 4096> pool; ///< Memory pool.

    Vec3  velocity;
    Vec3  momentum; ///< Desired velocity
    Vec3  floor;    ///< Floor normal.

    int   parent;   ///< Index of container object or -1 if object in positioned in the world.
    int   lower;    ///< Index of structure entity or object below this object, -1 otherwise.
    float depth;    ///< Depth of object's lower bound when intersecting liquid, 0 otherwise.

    float mass;     ///< Mass.
    float lift;     ///< Lift / weight (water only, for lava Physics::LAVA_LIFT is used).

  public:

    OZ_ALWAYS_INLINE
    void splash( float hitMomentum )
    {
      float hitMomentum2 = hitMomentum*hitMomentum;

      addEvent( EVENT_SPLASH, hitMomentum2 * MOMENTUM_INTENSITY_COEF );
      addEvent( EVENT_SPLASH, 1.0f );
    }

  protected:

    virtual void onDestroy();

  public:

    explicit Dynamic( const DynamicClass* clazz, int index, const Point3& p, Heading heading );
    explicit Dynamic( const DynamicClass* clazz, InputStream* istream );

    virtual void write( BufferStream* ostream ) const;

    virtual void readUpdate( InputStream* istream );
    virtual void writeUpdate( BufferStream* ostream ) const;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
}
