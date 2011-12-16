/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file common/AABB.hh
 */

#pragma once

#include "common/Bounds.hh"

namespace oz
{

class AABB
{
  public:

    // max allowed dimension for an object plus 1 cm for epsilon
    static constexpr float MAX_DIM      = 4.00f;
    static constexpr float REAL_MAX_DIM = 3.99f;

    /// Position.
    Point3 p;

    /// Extents.
    Vec3 dim;

    AABB() = default;

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

OZ_ALWAYS_INLINE
inline Bounds& Bounds::fromAABBMove( const AABB& a, const Vec3& move, float eps )
{
  mins = a.p - a.dim - 2.0f * Vec3( eps, eps, eps );
  maxs = a.p + a.dim + 2.0f * Vec3( eps, eps, eps );

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
