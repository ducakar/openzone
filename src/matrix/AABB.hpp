/*
 *  AABB.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Sphere.hpp"
#include "matrix/Bounds.hpp"

namespace oz
{

  class AABB
  {
    public:

      // max allowed dimension for an object plus 1 cm for epsilon
      static const float MAX_DIM;
      static const float REAL_MAX_DIM;

      /**
       * Position
       */
      Point3 p;

      /**
       * Extents
       */
      Vec3   dim;

      OZ_ALWAYS_INLINE
      explicit AABB()
      {}

      OZ_ALWAYS_INLINE
      explicit AABB( const Point3& p_, const Vec3& dim_ ) : p( p_ ), dim( dim_ )
      {}

      OZ_ALWAYS_INLINE
      Bounds toBounds( float eps = 0.0f ) const
      {
        return Bounds( p - dim - Vec3( eps, eps, eps ),
                       p + dim + Vec3( eps, eps, eps ) );
      }

      OZ_ALWAYS_INLINE
      AABB toAABB( float eps ) const
      {
        return AABB( p, dim + Vec3( eps, eps, eps ) );
      }

      OZ_ALWAYS_INLINE
      AABB operator + ( const Vec3& v ) const
      {
        return AABB( p + v, dim );
      }

      OZ_ALWAYS_INLINE
      AABB operator - ( const Vec3& v ) const
      {
        return AABB( p - v, dim );
      }

      OZ_ALWAYS_INLINE
      AABB& operator += ( const Vec3& v )
      {
        p += v;
        return *this;
      }

      OZ_ALWAYS_INLINE
      AABB& operator -= ( const Vec3& v )
      {
        p -= v;
        return *this;
      }

      OZ_ALWAYS_INLINE
      AABB operator * ( float k ) const
      {
        return AABB( p, dim * k );
      }

      OZ_ALWAYS_INLINE
      AABB operator / ( float k ) const
      {
        return AABB( p, dim / k );
      }

      OZ_ALWAYS_INLINE
      AABB& operator *= ( float k )
      {
        dim *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      AABB& operator /= ( float k )
      {
        dim /= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      bool includes( const Point3& point, float eps = 0.0f ) const
      {
        Vec3 relPos = point - p;
        Vec3 d = dim + Vec3( eps, eps, eps );

        return
            -d.x <= relPos.x && relPos.x <= d.x &&
            -d.y <= relPos.y && relPos.y <= d.y &&
            -d.z <= relPos.z && relPos.z <= d.z;
      }

      OZ_ALWAYS_INLINE
      bool isInside( const AABB& a, float eps = 0.0f ) const
      {
        Vec3 relPos = p - a.p;
        Vec3 d = a.dim - dim + Vec3( eps, eps, eps );

        return
            -d.x <= relPos.x && relPos.x <= d.x &&
            -d.y <= relPos.y && relPos.y <= d.y &&
            -d.z <= relPos.z && relPos.z <= d.z;
      }

      OZ_ALWAYS_INLINE
      bool includes( const AABB& a, float eps = 0.0f ) const
      {
        return a.isInside( *this, eps );
      }

      OZ_ALWAYS_INLINE
      bool overlaps( const AABB& a, float eps = 0.0f ) const
      {
        Vec3 relPos = a.p - p;
        Vec3 d = a.dim + dim + Vec3( eps, eps, eps );

        return
            -d.x <= relPos.x && relPos.x <= d.x &&
            -d.y <= relPos.y && relPos.y <= d.y &&
            -d.z <= relPos.z && relPos.z <= d.z;
      }

      OZ_ALWAYS_INLINE
      bool isInside( const Bounds& b, float eps = 0.0f ) const
      {
        return
            b.mins.x - eps <= p.x - dim.x && p.x + dim.x <= b.maxs.x + eps &&
            b.mins.y - eps <= p.y - dim.y && p.y + dim.y <= b.maxs.y + eps &&
            b.mins.z - eps <= p.z - dim.z && p.z + dim.z <= b.maxs.z + eps;
      }

      OZ_ALWAYS_INLINE
      bool includes( const Bounds& b, float eps = 0.0f ) const
      {
        return
            b.mins.x + eps >= p.x - dim.x && p.x + dim.x >= b.maxs.x - eps &&
            b.mins.y + eps >= p.y - dim.y && p.y + dim.y >= b.maxs.y - eps &&
            b.mins.z + eps >= p.z - dim.z && p.z + dim.z >= b.maxs.z - eps;
      }

      OZ_ALWAYS_INLINE
      bool overlaps( const Bounds& b, float eps = 0.0f ) const
      {
        return
            b.mins.x - eps <= p.x + dim.x && p.x - dim.x <= b.maxs.x + eps &&
            b.mins.y - eps <= p.y + dim.y && p.y - dim.y <= b.maxs.y + eps &&
            b.mins.z - eps <= p.z + dim.z && p.z - dim.z <= b.maxs.z + eps;
      }

  };

  OZ_ALWAYS_INLINE
  inline AABB Bounds::toAABB( float eps ) const
  {
    return AABB( Point3( ( mins.x + maxs.x ) * 0.5f,
                         ( mins.y + maxs.y ) * 0.5f,
                         ( mins.z + maxs.z ) * 0.5f ),
                 Vec3( ( maxs.x - mins.x ) * 0.5f + eps,
                       ( maxs.y - mins.y ) * 0.5f + eps,
                       ( maxs.z - mins.z ) * 0.5f + eps ) );
  }

  inline Bounds& Bounds::fromAABBMove( const AABB& a, const Vec3& move, float eps )
  {
    if( move.x < 0.0f ) {
      mins.x = a.p.x - a.dim.x - 2.0f * eps + move.x;
      maxs.x = a.p.x + a.dim.x + 2.0f * eps;
    }
    else {
      mins.x = a.p.x - a.dim.x - 2.0f * eps;
      maxs.x = a.p.x + a.dim.x + 2.0f * eps + move.x;
    }
    if( move.y < 0.0f ) {
      mins.y = a.p.y - a.dim.y - 2.0f * eps + move.y;
      maxs.y = a.p.y + a.dim.y + 2.0f * eps;
    }
    else {
      mins.y = a.p.y - a.dim.y - 2.0f * eps;
      maxs.y = a.p.y + a.dim.y + 2.0f * eps + move.y;
    }
    if( move.z < 0.0f ) {
      mins.z = a.p.z - a.dim.z - 2.0f * eps + move.z;
      maxs.z = a.p.z + a.dim.z + 2.0f * eps;
    }
    else {
      mins.z = a.p.z - a.dim.z - 2.0f * eps;
      maxs.z = a.p.z + a.dim.z + 2.0f * eps + move.z;
    }
    return *this;
  }

  OZ_ALWAYS_INLINE
  inline bool Bounds::isInside( const AABB& a, float eps ) const
  {
    return a.includes( *this, eps );
  }

  OZ_ALWAYS_INLINE
  inline bool Bounds::includes( const AABB& a, float eps ) const
  {
    return a.isInside( *this, eps );
  }

  OZ_ALWAYS_INLINE
  inline bool Bounds::overlaps( const AABB& a, float eps ) const
  {
    return a.overlaps( *this, eps );
  }

}
