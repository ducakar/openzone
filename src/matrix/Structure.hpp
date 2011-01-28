/*
 *  Structure.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "matrix/BSP.hpp"

namespace oz
{

  class Object;
  class Structure;

  class Entity
  {
    friend class Structure;

    public:

      enum State
      {
        CLOSED,
        OPENING,
        OPENING_BLOCKED,
        OPENED,
        CLOSING,
        CLOSING_BLOCKED
      };

      static Vector<Object*> overlapingObjs;

      const EntityClass* clazz;
      Structure*         str;
      Vec3               offset;
      State              state;

      float              ratio;
      float              time;

    private:

      void updateIgnoring();
      void updateBlocking();
      void updatePushing();
      void updateCrushing();

  };

  class Structure : public Bounds
  {
    friend class Pool<Structure, 0, 256>;

    private:

      static const float DAMAGE_THRESHOLD;

    public:

      enum Rotation
      {
        R0   = 0,
        R90  = 1,
        R180 = 2,
        R270 = 3
      };

      static Pool<Structure, 0, 256> pool;

      static const Mat44 rotations[];
      static const Mat44 invRotations[];

    private:

      Structure* next[1];

    public:

      int        index;
      int        bsp;
      Point3     p;
      Rotation   rot;
      float      life;

      Entity*    entities;
      int        nEntities;

      explicit Structure( int index, int bsp, const Point3& p, Rotation rot );
      explicit Structure( int index, int bsp, InputStream* istream );

      ~Structure();

      /**
       * Rotate vector from absolute coordinate system to structure coordinate system.
       * @param v
       * @return
       */
      Vec3 toStructCS( const Vec3& v ) const;

      /**
       * Rotate vector from structure coordinate system to absolute coordinate system.
       * @param v
       * @return
       */
      Vec3 toAbsoluteCS( const Vec3& v ) const;

      /**
       * Rotate point from absolute coordinate system to structure coordinate system.
       * @param v
       * @return
       */
      Point3 toStructCS( const Point3& p, const Point3& origin ) const;

      /**
       * Rotate point from structure coordinate system to absolute coordinate system.
       * @param v
       * @return
       */
      Point3 toAbsoluteCS( const Point3& p, const Point3& origin ) const;

      /**
       * Rotate Bounds from absolute coordinate system to structure coordinate system.
       * @param v
       * @return
       */
      Bounds toStructCS( const Bounds& bb, const Point3& origin ) const;

      /**
       * Rotate Bounds from structure coordinate system to absolute coordinate system.
       * @param v
       * @return
       */
      Bounds toAbsoluteCS( const Bounds& bb, const Point3& origin ) const;

      static Bounds rotate( const Bounds& in, Rotation rot );
      void setRotation( const Bounds& in, Rotation rot );

      void damage( float damage );
      void destroy();
      void update();

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );

    OZ_STATIC_POOL_ALLOC( pool )

  };

  inline Vec3 Structure::toStructCS( const Vec3& v ) const
  {
    return invRotations[ int( rot ) ] * v;
  }

  inline Vec3 Structure::toAbsoluteCS( const Vec3& v ) const
  {
    return rotations[ int( rot ) ] * v;
  }

  inline Point3 Structure::toStructCS( const Point3& p, const Point3& origin ) const
  {
    return Point3::ORIGIN + invRotations[ int( rot ) ] * ( p - origin );
  }

  inline Point3 Structure::toAbsoluteCS( const Point3& p, const Point3& origin ) const
  {
    return origin + rotations[ int( rot ) ] * ( p - Point3::ORIGIN );
  }

  inline Bounds Structure::toStructCS( const Bounds& bb, const Point3& origin ) const
  {
    Point3 mins = bb.mins - ( Point3::ORIGIN - origin );
    Point3 maxs = bb.maxs - ( Point3::ORIGIN - origin );

    switch( rot ) {
      default: {
        assert( false );
      }
      case Structure::R0: {
        return Bounds( Point3( +bb.mins.x, +bb.mins.y, +bb.mins.z ),
                       Point3( +bb.maxs.x, +bb.maxs.y, +bb.maxs.z ) );
      }
      case Structure::R90: {
        return Bounds( Point3( +bb.mins.y, -bb.maxs.x, +bb.mins.z ),
                       Point3( +bb.maxs.y, -bb.mins.x, +bb.maxs.z ) );
      }
      case Structure::R180: {
        return Bounds( Point3( -bb.maxs.x, -bb.maxs.y, +bb.mins.z ),
                       Point3( -bb.mins.x, -bb.mins.y, +bb.maxs.z ) );
      }
      case Structure::R270: {
        return Bounds( Point3( -bb.maxs.y, +bb.mins.x, +bb.mins.z ),
                       Point3( -bb.mins.y, +bb.maxs.x, +bb.maxs.z ) );
      }
    }
  }

  inline Bounds Structure::toAbsoluteCS( const Bounds& bb, const Point3& origin ) const
  {
    switch( rot ) {
      default: {
        assert( false );
      }
      case Structure::R0: {
        return Bounds( origin + Vec3( +bb.mins.x, +bb.mins.y, +bb.mins.z ),
                       origin + Vec3( +bb.maxs.x, +bb.maxs.y, +bb.maxs.z ) );
      }
      case Structure::R90: {
        return Bounds( origin + Vec3( -bb.maxs.y, +bb.mins.x, +bb.mins.z ),
                       origin + Vec3( -bb.mins.y, +bb.maxs.x, +bb.maxs.z ) );
      }
      case Structure::R180: {
        return Bounds( origin + Vec3( -bb.maxs.x, -bb.maxs.y, +bb.mins.z ),
                       origin + Vec3( -bb.mins.x, -bb.mins.y, +bb.maxs.z ) );
      }
      case Structure::R270: {
        return Bounds( origin + Vec3( +bb.mins.y, -bb.maxs.x, +bb.mins.z ),
                       origin + Vec3( +bb.maxs.y, -bb.mins.x, +bb.maxs.z ) );
      }
    }
  }

  inline void Structure::damage( float damage )
  {
    damage -= DAMAGE_THRESHOLD;

    if( damage > 0.0f ) {
      life -= damage;
    }
  }

}
