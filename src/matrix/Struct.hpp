/*
 *  Struct.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
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

          static void ( Entity::* handlers[] )();

          void updateIgnoring();
          void updateCrushing();
          void updateAutoDoor();

      };

      static Pool<Struct> pool;

      static const Mat44 rotations[];
      static const Mat44 invRotations[];

      static Vector<Object*> overlappingObjs;

    public:

      Point3     p;
      int        index;
      int        id;
      const BSP* bsp;
      Heading    heading;
      float      life;
      float      resistance;

      int        nEntities;
      Entity*    entities;

      ~Struct();

    private:

      // no copying
      Struct( const Struct& );
      Struct& operator = ( const Struct& );

    public:

      explicit Struct( int index, int bspId, const Point3& p, Heading heading );
      explicit Struct( int index, int bpsId, InputStream* istream );

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

      static Bounds rotate( const Bounds& in, Heading heading );

      void damage( float damage );
      void destroy();
      void update();

      void readFull( InputStream* istream );
      void writeFull( OutputStream* ostream );

    OZ_STATIC_POOL_ALLOC( pool )

  };

  OZ_ALWAYS_INLINE
  inline Vec3 Struct::toStructCS( const Vec3& v ) const
  {
    return invRotations[heading] * v;
  }

  OZ_ALWAYS_INLINE
  inline Vec3 Struct::toAbsoluteCS( const Vec3& v ) const
  {
    return rotations[heading] * v;
  }

  OZ_ALWAYS_INLINE
  inline Point3 Struct::toStructCS( const Point3& point ) const
  {
    return Point3::ORIGIN + invRotations[heading] * ( point - p );
  }

  OZ_ALWAYS_INLINE
  inline Point3 Struct::toAbsoluteCS( const Point3& point ) const
  {
    return p + rotations[heading] * ( point - Point3::ORIGIN );
  }

  OZ_ALWAYS_INLINE
  inline void Struct::damage( float damage )
  {
    damage -= resistance;

    if( damage > 0.0f ) {
      life -= damage;
      printf( "%g\n", life );
    }
  }

}
