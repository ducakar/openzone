/*
 *  Bounds.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file common/Bounds.hpp
 */

#include "oz/oz.hpp"

namespace oz
{

class AABB;

class Bounds
{
  public:

    Point3 mins;
    Point3 maxs;

    Bounds() = default;

    OZ_ALWAYS_INLINE
    explicit Bounds( const Point3& mins_, const Point3& maxs_ ) : mins( mins_ ), maxs( maxs_ )
    {}

    OZ_ALWAYS_INLINE
    Bounds toBounds( float eps ) const
    {
      return Bounds( mins - Vec3( eps, eps, eps ), maxs + Vec3( eps, eps, eps ) );
    }

    Bounds& fromPointMove( const Point3& p, const Vec3& move, float eps = 0.0f )
    {
      mins = p - 2.0f * Vec3( eps, eps, eps );
      maxs = p + 2.0f * Vec3( eps, eps, eps );

      if( move.x < 0.0f ) {
        mins.x += move.x;
      }
      else {
        maxs.x += move.x;
      }
      if( move.y < 0.0f ) {
        mins.y += move.y;
      }
      else {
        maxs.y += move.y;
      }
      if( move.z < 0.0f ) {
        mins.z += move.z;
      }
      else {
        maxs.z += move.z;
      }
      return *this;
    }

    OZ_ALWAYS_INLINE
    Bounds operator + ( const Vec3& v ) const
    {
      return Bounds( mins + v, maxs + v );
    }

    OZ_ALWAYS_INLINE
    Bounds operator - ( const Vec3& v ) const
    {
      return Bounds( mins - v, maxs - v );
    }

    OZ_ALWAYS_INLINE
    Bounds& operator += ( const Vec3& v )
    {
      mins += v;
      maxs += v;
      return *this;
    }

    OZ_ALWAYS_INLINE
    Bounds& operator -= ( const Vec3& v )
    {
      mins -= v;
      maxs -= v;
      return *this;
    }

    OZ_ALWAYS_INLINE
    bool includes( const Point3& p, float eps = 0.0f ) const
    {
      return
        mins.x - eps <= p.x && p.x <= maxs.x + eps &&
        mins.y - eps <= p.y && p.y <= maxs.y + eps &&
        mins.z - eps <= p.z && p.z <= maxs.z + eps;
    }

    OZ_ALWAYS_INLINE
    bool isInside( const Bounds& b, float eps = 0.0f ) const
    {
      return
        b.mins.x - eps <= mins.x && maxs.x <= b.maxs.x + eps &&
        b.mins.y - eps <= mins.y && maxs.y <= b.maxs.y + eps &&
        b.mins.z - eps <= mins.z && maxs.z <= b.maxs.z + eps;
    }

    OZ_ALWAYS_INLINE
    bool includes( const Bounds& b, float eps = 0.0f ) const
    {
      return b.isInside( *this, eps );
    }

    OZ_ALWAYS_INLINE
    bool overlaps( const Bounds& b, float eps = 0.0f ) const
    {
      return
        b.mins.x - eps <= maxs.x && mins.x <= b.maxs.x + eps &&
        b.mins.y - eps <= maxs.y && mins.y <= b.maxs.y + eps &&
        b.mins.z - eps <= maxs.z && mins.z <= b.maxs.z + eps;
    }

    // implemented in AABB.hpp
    AABB    toAABB( float eps = 0.0f ) const;
    Bounds& fromAABBMove( const AABB& a, const Vec3& move, float eps = 0.0f );
    bool    isInside( const AABB& a, float eps = 0.0f ) const;
    bool    includes( const AABB& a, float eps = 0.0f ) const;
    bool    overlaps( const AABB& a, float eps = 0.0f ) const;

};

}
