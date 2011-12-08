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
 * @file oz/Vec3.hh
 */

#pragma once

#include "Math.hh"

namespace oz
{

/**
 * 3D vector.
 *
 * @ingroup oz
 */
class Vec3
{
  public:

    /// Zero vector, [0, 0, 0] or [0, 0, 0, 0] for SIMD.
    static const Vec3 ZERO;

    /// %Vector of ones, [1, 1, 1] or [1, 1, 1, 0] for SIMD.
    static const Vec3 ONE;

    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.

    /**
     * Create an uninitialised instance.
     */
    Vec3() = default;

    /**
     * Create a vector with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Vec3( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
    {}

    /**
     * Create from an array of 3 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Vec3( const float* v ) : x( v[0] ), y( v[1] ), z( v[2] )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Vec3& v ) const
    {
      return x == v.x && y == v.y && z == v.z;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Vec3& v ) const
    {
      return x != v.x || y != v.y || z != v.z;
    }

    /**
     * Constant float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator const float* () const
    {
      return &x;
    }

    /**
     * Float pointer to the members.
     */
    OZ_ALWAYS_INLINE
    operator float* ()
    {
      return &x;
    }

    /**
     * Constant reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 3 );

      return ( &x )[i];
    }

    /**
     * Reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 3 );

      return ( &x )[i];
    }

    /**
     * %Vector with absolute components.
     */
    OZ_ALWAYS_INLINE
    Vec3 abs() const
    {
      return Vec3( Math::fabs( x ), Math::fabs( y ), Math::fabs( z ) );
    }

    /**
     * Length.
     */
    OZ_ALWAYS_INLINE
    float operator ! () const
    {
      return Math::sqrt( x*x + y*y + z*z );
    }

    /**
     * Approximate length.
     */
    OZ_ALWAYS_INLINE
    float fastL() const
    {
      return Math::fastSqrt( x*x + y*y + z*z );
    }

    /**
     * Square of length.
     */
    OZ_ALWAYS_INLINE
    float sqL() const
    {
      return x*x + y*y + z*z;
    }

    /**
     * Unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator ~ () const
    {
      hard_assert( x*x + y*y + z*z > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z );
      return Vec3( x * k, y * k, z * k );
    }

    /**
     * Approximate unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 fastUnit() const
    {
      hard_assert( x*x + y*y + z*z > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z );
      return Vec3( x * k, y * k, z * k );
    }

    /**
     * Original vector.
     */
    OZ_ALWAYS_INLINE
    const Vec3& operator + () const
    {
      return *this;
    }

    /**
     * Opposite vector.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - () const
    {
      return Vec3( -x, -y, -z );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator + ( const Vec3& v ) const
    {
      return Vec3( x + v.x, y + v.y, z + v.z );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator - ( const Vec3& v ) const
    {
      return Vec3( x - v.x, y - v.y, z - v.z );
    }

    /**
     * %Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator * ( float k ) const
    {
      return Vec3( x * k, y * k, z * k );
    }

    /**
     * %Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    friend Vec3 operator * ( float k, const Vec3& v )
    {
      return Vec3( k * v.x, k * v.y, k * v.z );
    }

    /**
     * %Vector divided by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator / ( float k ) const
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      return Vec3( x * k, y * k, z * k );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Vec3& operator += ( const Vec3& v )
    {
      x += v.x;
      y += v.y;
      z += v.z;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Vec3& operator -= ( const Vec3& v )
    {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      return *this;
    }

    /**
     * Multiplication by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec3& operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      return *this;
    }

    /**
     * Division by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec3& operator /= ( float k )
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      return *this;
    }

    /**
     * Scalar product.
     */
    OZ_ALWAYS_INLINE
    float operator * ( const Vec3& v ) const
    {
      return x*v.x + y*v.y + z*v.z;
    }

    /**
     * %Vector product.
     */
    OZ_ALWAYS_INLINE
    Vec3 operator ^ ( const Vec3& v ) const
    {
      return Vec3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
    }

    /**
     * Spherical linear interpolation.
     */
    OZ_ALWAYS_INLINE
    static Vec3 slerp( const Vec3& a, const Vec3& b, float t )
    {
      float angle = Math::acos( a.x*b.x + a.y*b.y + a.z*b.z );
      float sine  = Math::sin( angle );

      if( sine == 0.0f ) {
        hard_assert( a * b > 0.0f );

        return a;
      }
      else {
        float alpha = Math::sin( ( 1.0f - t ) * angle ) / sine;
        float beta  = Math::sin( t * angle ) / sine;

        return alpha * a + beta * b;
      }
    }

};

}
