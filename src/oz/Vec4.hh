/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Vec4.hh
 */

#pragma once

#include "Point3.hh"

namespace oz
{

/**
 * Generic four-component vector.
 *
 * @ingroup oz
 */
class Vec4
{
  public:

    /// Zero vector, [0, 0, 0, 0].
    static const Vec4 ZERO;

    /// %Vector of ones, [1, 1, 1, 1].
    static const Vec4 ONE;

    /// Equivalent to origin point or identity quaternion, [0, 0, 0, 1].
    static const Vec4 ID;

    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.
    float w; ///< W component.

    /**
     * Create an uninitialised instance.
     */
    Vec4() = default;

    /**
     * Create a vector with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( float x_, float y_, float z_, float w_ ) :
      x( x_ ), y( y_ ), z( z_ ), w( w_ )
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const float* v ) :
      x( v[0] ), y( v[1] ), z( v[2] ), w( v[3] )
    {}

    /**
     * Create vector from a 3D vector (the additional component is zero).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Vec3& v ) :
      x( v.x ), y( v.y ), z( v.z ), w( 0.0f )
    {}

    /**
     * Create vector from a 3D vector, specify the last component.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Vec3& v, float w_ ) :
      x( v.x ), y( v.y ), z( v.z ), w( w_ )
    {}

    /**
     * Create vector from a point (the additional component is one).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Point3& p ) :
      x( p.x ), y( p.y ), z( p.z ), w( 1.0f )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Vec4& v ) const
    {
      return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Vec4& v ) const
    {
      return x != v.x || y != v.y || z != v.z || w != v.w;
    }

    /**
     * Cast to 3D vector.
     */
    operator Vec3 () const
    {
      return Vec3( x, y, z );
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
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Reference to the i-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * %Vector with absolute components.
     */
    OZ_ALWAYS_INLINE
    Vec4 abs() const
    {
      return Vec4( Math::fabs( x ), Math::fabs( y ), Math::fabs( z ), Math::fabs( w ) );
    }

    /**
     * Length.
     */
    OZ_ALWAYS_INLINE
    float operator ! () const
    {
      return Math::sqrt( x*x + y*y + z*z + w*w );
    }

    /**
     * Approximate length.
     */
    OZ_ALWAYS_INLINE
    float fastL() const
    {
      return Math::fastSqrt( x*x + y*y + z*z + w*w );
    }

    /**
     * Square of length.
     */
    OZ_ALWAYS_INLINE
    float sqL() const
    {
      return x*x + y*y + z*z + w*w;
    }

    /**
     * Unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator ~ () const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
    }

    /**
     * Approximate unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 fastUnit() const
    {
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
    }

    /**
     * Original vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator + () const
    {
      return *this;
    }

    /**
     * Opposite vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator - () const
    {
      return Vec4( -x, -y, -z, -w );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator + ( const Vec4& v ) const
    {
      return Vec4( x + v.x, y + v.y, z + v.z, w + v.w );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator - ( const Vec4& v ) const
    {
      return Vec4( x - v.x, y - v.y, z - v.z, w - v.w );
    }

    /**
     * %Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator * ( float k ) const
    {
      return Vec4( x * k, y * k, z * k, w * k );
    }

    /**
     * %Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    friend Vec4 operator * ( float k, const Vec4& v )
    {
      return Vec4( k * v.x, k * v.y, k * v.z, k * v.w );
    }

    /**
     * %Vector divided by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator / ( float k ) const
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      return Vec4( x * k, y * k, z * k, w * k );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator += ( const Vec4& v )
    {
      x += v.x;
      y += v.y;
      z += v.z;
      w += v.w;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator -= ( const Vec4& v )
    {
      x -= v.x;
      y -= v.y;
      z -= v.z;
      w -= v.w;
      return *this;
    }

    /**
     * Multiplication by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Division by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator /= ( float k )
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Scalar product.
     */
    OZ_ALWAYS_INLINE
    float operator * ( const Vec4& v ) const
    {
      return x*v.x + y*v.y + z*v.z + w*v.w;
    }

};

}
