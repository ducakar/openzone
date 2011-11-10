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
 * @file matrix/Struct.hpp
 */

#pragma once

#include "matrix/BSP.hpp"

namespace oz
{
namespace matrix
{

class Object;

class Struct : public Bounds
{
  private:

    static const float DEMOLISH_SPEED;
    static const Vec3  DESTRUCT_FRAG_VELOCITY;
    static const Mat44 ROTATIONS[];

  public:

    class Entity
    {
      friend class Struct;

      public:

        enum State
        {
          CLOSED,
          OPENING,
          OPENED,
          CLOSING
        };

        Vec3              offset;

        const BSP::Model* model;
        Struct*           str;

        State             state;
        float             ratio;
        float             time;

      private:

        static void ( Entity::* const HANDLERS[] )();

        void updateIgnoring();
        void updateCrushing();
        void updateAutoDoor();

    };

    static Pool<Struct>    pool;
    static Vector<Object*> overlappingObjs;

  private:

    Mat44       transf;
    Mat44       invTransf;

  public:

    const BSP*  bsp;

    Point3      p;
    int         index;
    Heading     heading;

    float       life;
    float       resistance;
    float       demolishing;

    int         nEntities;
    Entity*     entities;

    Vector<int> boundObjects;

  private:

    void onDemolish();
    void onUpdate();

  public:

    ~Struct();

    // no copying
    Struct( const Struct& ) = delete;
    Struct& operator = ( const Struct& ) = delete;

    /**
     * Rotate vector from structure coordinate system to absolute coordinate system.
     * @param v
     * @return
     */
    Vec3 toAbsoluteCS( const Vec3& v ) const;

    /**
     * Rotate vector from absolute coordinate system to structure coordinate system.
     * @param v
     * @return
     */
    Vec3 toStructCS( const Vec3& v ) const;

    /**
     * Rotate point from structure coordinate system to absolute coordinate system.
     * @param point
     * @return
     */
    Point3 toAbsoluteCS( const Point3& point ) const;

    /**
     * Rotate point from absolute coordinate system to structure coordinate system.
     * @param point
     * @return
     */
    Point3 toStructCS( const Point3& point ) const;

    /**
     * Rotate Bounds from structure coordinate system to absolute coordinate system.
     * @param bb
     * @return
     */
    Bounds toAbsoluteCS( const Bounds& bb ) const;

    /**
     * Rotate Bounds from absolute coordinate system to structure coordinate system.
     * @param bb
     * @return
     */
    Bounds toStructCS( const Bounds& bb ) const;

    static Bounds rotate( const Bounds& in, Heading heading );

    void damage( float damage );
    void destroy();
    void update();

  public:

    explicit Struct( const BSP* bsp, int index, const Point3& p, Heading heading );
    explicit Struct( const BSP* bsp, InputStream* istream );

    void write( BufferStream* ostream );

  OZ_STATIC_POOL_ALLOC( pool )

};

OZ_ALWAYS_INLINE
inline Vec3 Struct::toAbsoluteCS( const Vec3& v ) const
{
  return transf * v;
}

OZ_ALWAYS_INLINE
inline Vec3 Struct::toStructCS( const Vec3& v ) const
{
  return invTransf * v;
}

OZ_ALWAYS_INLINE
inline Point3 Struct::toAbsoluteCS( const Point3& point ) const
{
  return transf * point;
}

OZ_ALWAYS_INLINE
inline Point3 Struct::toStructCS( const Point3& point ) const
{
  return invTransf * point;
}

OZ_ALWAYS_INLINE
inline void Struct::damage( float damage )
{
  damage -= resistance;

  if( damage > 0.0f ) {
    life -= damage;
  }
}

OZ_ALWAYS_INLINE
inline void Struct::update()
{
  if( nEntities != 0 || !boundObjects.isEmpty() || life <= 0.0f ) {
    onUpdate();
  }
}

}
}
