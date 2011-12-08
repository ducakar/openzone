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

    static Pool<Dynamic, 4096> pool;

    Vec3  velocity;
    Vec3  momentum; // desired velocity
    Vec3  floor;    // if on ground, used as floor normal, it is not set if on another object

    int   parent;   // index of container object (if object isn't positioned in the world,
                    // it has to be contained in an another object, otherwise it will be removed)
    int   lower;    // index of the lower object
    float depth;    // how deep under water the object's lower bound is

    float mass;
    float lift;

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
