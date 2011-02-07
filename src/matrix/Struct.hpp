/*
 *  Struct.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/BSP.hpp"

namespace oz
{

  class Object;

  class Struct : public Bounds
  {
    private:

      static const float DAMAGE_THRESHOLD;

    public:

      class Entity
      {
        friend class Struct;

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

          static Vector<Object*> overlappingObjs;

          const BSP::Model* model;
          Struct*           str;
          Vec3              offset;
          State             state;

          float             ratio;
          float             time;

        private:

          void updateIgnoring();
          void updateBlocking();
          void updatePushing();
          void updateCrushing();

      };

      enum Rotation
      {
        R0   = 0,
        R90  = 1,
        R180 = 2,
        R270 = 3
      };

      static Pool<Struct, 256> pool;

      static const Mat44 rotations[];
      static const Mat44 invRotations[];

    public:

      int      index;
      int      bsp;
      Point3   p;
      Rotation rot;
      float    life;

      int      nEntities;
      Entity*  entities;

      explicit Struct( int index, int bsp, const Point3& p, Rotation rot );
      explicit Struct( int index, int bsp, InputStream* istream );

      ~Struct();

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
       * @param point
       * @return
       */
      Point3 toStructCS( const Point3& point ) const;

      /**
       * Rotate point from structure coordinate system to absolute coordinate system.
       * @param point
       * @return
       */
      Point3 toAbsoluteCS( const Point3& point ) const;

      /**
       * Rotate Bounds from absolute coordinate system to structure coordinate system.
       * @param bb
       * @return
       */
      Bounds toStructCS( const Bounds& bb ) const;

      /**
       * Rotate Bounds from structure coordinate system to absolute coordinate system.
       * @param bb
       * @return
       */
      Bounds toAbsoluteCS( const Bounds& bb ) const;

      static Bounds rotate( const Bounds& in, Rotation rot );
      void setRotation( const Bounds& in, Rotation rot );

      void damage( float damage );
      void destroy();
      void update();

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );

    OZ_STATIC_POOL_ALLOC( pool )

  };

  inline Vec3 Struct::toStructCS( const Vec3& v ) const
  {
    return invRotations[ int( rot ) ] * v;
  }

  inline Vec3 Struct::toAbsoluteCS( const Vec3& v ) const
  {
    return rotations[ int( rot ) ] * v;
  }

  inline Point3 Struct::toStructCS( const Point3& point ) const
  {
    return Point3::ORIGIN + invRotations[ int( rot ) ] * ( point - p );
  }

  inline Point3 Struct::toAbsoluteCS( const Point3& point ) const
  {
    return p + rotations[ int( rot ) ] * ( point - Point3::ORIGIN );
  }

  inline void Struct::damage( float damage )
  {
    damage -= DAMAGE_THRESHOLD;

    if( damage > 0.0f ) {
      life -= damage;
    }
  }

}
