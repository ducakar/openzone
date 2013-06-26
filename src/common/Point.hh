/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file common/Point.hh
 *
 * `Point` class.
 */

#pragma once

#include <common/common.hh>

namespace oz
{
namespace common
{

/**
 * 3D point (toroidal version).
 */
class Point : public oz::Point
{
  public:

    /// Origin, [0, 0, 0].
    static const Point ORIGIN;

  private:

    /**
     * Wrap coordinate to the interval \f$ [-WORLD_DIM, +WORLD_DIM) \f$.
     *
     * It is assumed the original coordinate lies on \f$ [-3 WORLD_DIM, \infty) \f$.
     */
    OZ_ALWAYS_INLINE
    static float wrap( float x )
    {
      return Math::fmod( x + 3.0f*WORLD_DIM, 2.0f*WORLD_DIM ) - WORLD_DIM;
    }

  public:

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    explicit Point() = default;

    /**
     * Create a point with given coordinates.
     */
    OZ_ALWAYS_INLINE
    explicit Point( float x, float y, float z ) :
      oz::Point( wrap( x ), wrap( y ), wrap( z ) )
    {}

    /**
     * Create from an array of 3 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Point( const float* v ) :
      oz::Point( v[0], v[1], v[2] )
    {}

    /**
     * Create from an `oz::Point`.
     */
    OZ_ALWAYS_INLINE
    Point( const oz::Point& p ) :
      oz::Point( wrap( p.x ), wrap( p.y ), wrap( p.z ) )
    {}

    /**
     * Copy from an `oz::Point`.
     */
    OZ_ALWAYS_INLINE
    Point& operator = ( const oz::Point& p )
    {
      x = wrap( p.x );
      y = wrap( p.y );
      z = wrap( p.z );
      return *this;
    }

    /**
     * Point translated for `v`.
     */
    OZ_ALWAYS_INLINE
    Point operator + ( const Vec3& v ) const
    {
      return Point( wrap( x + v.x ), wrap( y + v.y ), wrap( z + v.z ) );
    }

    /**
     * Point translated for `-v`.
     */
    OZ_ALWAYS_INLINE
    Point operator - ( const Vec3& v ) const
    {
      return Point( wrap( x - v.x ), wrap( y - v.y ), wrap( z - v.z ) );
    }

    /**
     * Difference of two points.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - ( const Point& p ) const
    {
      return Vec3( wrap( x - p.x ), wrap( y - p.y ), wrap( z - p.z ) );
    }

    /**
     * Translate for `v`.
     */
    OZ_ALWAYS_INLINE
    Point& operator += ( const Vec3& v )
    {
      x = wrap( x + v.x );
      y = wrap( y + v.y );
      z = wrap( z + v.z );
      return *this;
    }

    /**
     * Translate for `-v`.
     */
    OZ_ALWAYS_INLINE
    Point& operator -= ( const Vec3& v )
    {
      x = wrap( x - v.x );
      y = wrap( y - v.y );
      z = wrap( z - v.z );
      return *this;
    }

};

/**
 * Transformed point (translation is applied).
 */
OZ_ALWAYS_INLINE
inline Point operator * ( const Mat44& m, const Point& p )
{
  return Point( m.x.x * p.x + m.y.x * p.y + m.z.x * p.z + m.w.x,
                m.x.y * p.x + m.y.y * p.y + m.z.y * p.z + m.w.y,
                m.x.z * p.x + m.y.z * p.y + m.z.z * p.z + m.w.z );
}

}
}
