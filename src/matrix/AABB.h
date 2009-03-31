/*
 *  AABB.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include "BSP.h"

namespace oz
{

  struct AABB
  {
    // max allowed dimension for an object plus 1 cm for epsilon
    static const float MAX_DIMXY;

    static const float REAL_MAX_DIMXY;

    Vec3 p;
    Vec3 dim;

    AABB() {}
    AABB( const Vec3 &p_, const Vec3 &dim_ ) : p( p_ ), dim( dim_ ) {}

    Bounds toBounds( float eps = 0.0f ) const
    {
      Bounds t;

      t.mins.x = p.x - dim.x - 2.0f * eps;
      t.mins.y = p.y - dim.y - 2.0f * eps;
      t.mins.z = p.z - dim.z - 2.0f * eps;

      t.maxs.x = p.x + dim.x + 2.0f * eps;
      t.maxs.y = p.y + dim.y + 2.0f * eps;
      t.maxs.z = p.z + dim.z + 2.0f * eps;

      return t;
    }

    Bounds toBounds( const Vec3 &move, float eps = 0.0f ) const
    {
      Bounds t;

      if( move.x < 0.0f ) {
        t.mins.x = p.x - dim.x - 2.0f * eps + move.x;
        t.maxs.x = p.x + dim.x + 2.0f * eps;
      }
      else {
        t.mins.x = p.x - dim.x - 2.0f * eps;
        t.maxs.x = p.x + dim.x + 2.0f * eps + move.x;
      }
      if( move.y < 0.0f ) {
        t.mins.y = p.y - dim.y - 2.0f * eps + move.y;
        t.maxs.y = p.y + dim.y + 2.0f * eps;
      }
      else {
        t.mins.y = p.y - dim.y - 2.0f * eps;
        t.maxs.y = p.y + dim.y + 2.0f * eps + move.y;
      }
      if( move.z < 0.0f ) {
        t.mins.z = p.z - dim.z - 2.0f * eps + move.z;
        t.maxs.z = p.z + dim.z + 2.0f * eps;
      }
      else {
        t.mins.z = p.z - dim.z - 2.0f * eps;
        t.maxs.z = p.z + dim.z + 2.0f * eps + move.z;
      }
      return t;
    }

    AABB operator + ( const Vec3 &v ) const
    {
      return AABB( p + v, dim );
    }

    AABB operator - ( const Vec3 &v ) const
    {
      return AABB( p - v, dim );
    }

    AABB &operator += ( const Vec3 &v )
    {
      p += v;
      return *this;
    }

    AABB &operator -= ( const Vec3 &v )
    {
      p -= v;
      return *this;
    }

    AABB operator * ( float k ) const
    {
      return AABB( p, dim * k );
    }

    AABB operator / ( float k ) const
    {
      return AABB( p, dim / k );
    }

    AABB &operator *= ( float k )
    {
      dim *= k;
      return *this;
    }

    AABB &operator /= ( float k )
    {
      dim /= k;
      return *this;
    }

    bool includes( const Vec3 &v, float eps = 0.0f ) const
    {
      Vec3 relPos = v - p;
      Vec3 d( dim.x + eps,
              dim.y + eps,
              dim.z + eps );

      return
          -d.x <= relPos.x && relPos.x <= d.x &&
          -d.y <= relPos.y && relPos.y <= d.y &&
          -d.z <= relPos.z && relPos.z <= d.z;
    }

    bool isInside( const AABB &a, float eps = 0.0f ) const
    {
      Vec3 relPos = p - a.p;
      Vec3 d( a.dim.x - dim.x + eps,
              a.dim.y - dim.y + eps,
              a.dim.z - dim.z + eps );

      return
          -d.x <= relPos.x && relPos.x <= d.x &&
          -d.y <= relPos.y && relPos.y <= d.y &&
          -d.z <= relPos.z && relPos.z <= d.z;
    }

    bool includes( const AABB &a, float eps = 0.0f ) const
    {
      return a.isInside( *this, eps );
    }

    bool overlaps( const AABB &a, float eps = 0.0f ) const
    {
      Vec3 relPos = a.p - p;
      Vec3 d( a.dim.x + dim.x + eps,
              a.dim.y + dim.y + eps,
              a.dim.z + dim.z + eps );

      return
          -d.x < relPos.x && relPos.x < d.x &&
          -d.y < relPos.y && relPos.y < d.y &&
          -d.z < relPos.z && relPos.z < d.z;
    }

    bool isInside( const Bounds &b, float eps = 0.0f ) const
    {
      return
          b.mins.x - eps <= p.x - dim.x && p.x + dim.x <= b.maxs.x + eps &&
          b.mins.y - eps <= p.y - dim.y && p.y + dim.y <= b.maxs.y + eps &&
          b.mins.z - eps <= p.z - dim.z && p.z + dim.z <= b.maxs.z + eps;
    }

    bool includes( const Bounds &b, float eps = 0.0f ) const
    {
      return
          b.mins.x + eps >= p.x - dim.x && p.x + dim.x >= b.maxs.x - eps &&
          b.mins.y + eps >= p.y - dim.y && p.y + dim.y >= b.maxs.y - eps &&
          b.mins.z + eps >= p.z - dim.z && p.z + dim.z >= b.maxs.z - eps;
    }

    bool overlaps( const Bounds &b, float eps = 0.0f ) const
    {
      return
          b.mins.x - eps <= p.x + dim.x && p.x - dim.x <= b.maxs.x + eps &&
          b.mins.y - eps <= p.y + dim.y && p.y - dim.y <= b.maxs.y + eps &&
          b.mins.z - eps <= p.z + dim.z && p.z - dim.z <= b.maxs.z + eps;
    }

  };

  inline bool Bounds::isInside( const AABB &a, float eps ) const
  {
    return a.includes( *this, eps );
  }

  inline bool Bounds::includes( const AABB &a, float eps ) const
  {
    return a.isInside( *this, eps );
  }

  inline bool Bounds::overlaps( const AABB &a, float eps ) const
  {
    return a.overlaps( *this, eps );
  }

}
