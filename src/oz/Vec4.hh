/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/Vec4.hh
 *
 * Vec3 class.
 */

#pragma once

#include "Point.hh"

namespace oz
{

/**
 * Generic four-component vector.
 */
class Vec4
{
  public:

    /// Zero vector, [0, 0, 0, 0].
    static const Vec4 ZERO;

    /// Vector of ones, [1, 1, 1, 1].
    static const Vec4 ONE;

    /// Equivalent to origin point or identity quaternion, [0, 0, 0, 1].
    static const Vec4 ID;

#ifdef OZ_SIMD_MATH
    union OZ_ALIGNED( 16 )
    {
      float4 f4;
      uint4  u4;
      struct
      {
        float x; ///< X component.
        float y; ///< Y component.
        float z; ///< Z component.
        float w; ///< W component.
      };
    };
#else
    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.
    float w; ///< W component.
#endif

  public:

    /**
     * Create an uninitialised instance.
     */
    Vec4() = default;

#ifdef OZ_SIMD_MATH

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( float4 f4_ ) :
      f4( f4_ )
    {}

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( uint4 u4_ ) :
      u4( u4_ )
    {}

#endif

    /**
     * Create a vector with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( float x_, float y_, float z_, float w_ ) :
#ifdef OZ_SIMD_MATH
      f4( float4( x_, y_, z_, w_ ) )
#else
      x( x_ ), y( y_ ), z( z_ ), w( w_ )
#endif
    {}

    /**
     * Create from an array of 4 floats.
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const float* v ) :
#ifdef OZ_SIMD_MATH
      f4( float4( v[0], v[1], v[2], v[3] ) )
#else
      x( v[0] ), y( v[1] ), z( v[2] ), w( v[3] )
#endif
    {}

    /**
     * Create vector from a 3D vector (the additional component is zero).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Vec3& v ) :
#ifdef OZ_SIMD_MATH
      f4( v.f4 )
#else
      x( v.x ), y( v.y ), z( v.z ), w( 0.0f )
#endif
    {}

    /**
     * Create vector from a point (the additional component is one).
     */
    OZ_ALWAYS_INLINE
    explicit Vec4( const Point& p ) :
#ifdef OZ_SIMD_MATH
      f4( p.f4 )
#else
      x( p.x ), y( p.y ), z( p.z ), w( 1.0f )
#endif
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
     * Constant reference to the `i`-th member.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Reference to the `i`-th member.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 4 );

      return ( &x )[i];
    }

    /**
     * Return the 3D vector this vector represents (last component should be 0).
     */
    Vec3 vec3() const
    {
#ifdef OZ_SIMD_MATH
      return Vec3( f4 );
#else
      hard_assert( w == 0.0f );

      return Vec3( x, y, z );
#endif
    }

    /**
     * Return the point vector represents (last component should be 1).
     */
    Point point() const
    {
#ifdef OZ_SIMD_MATH
      return Point( f4 );
#else
      hard_assert( w == 1.0f );

      return Point( x, y, z );
#endif
    }

    /**
     * Vector with absolute components.
     */
    OZ_ALWAYS_INLINE
    Vec4 abs() const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( vAbs( u4 ) );
#else
      return Vec4( Math::fabs( x ), Math::fabs( y ), Math::fabs( z ), Math::fabs( w ) );
#endif
    }

    /**
     * Norm.
     */
    OZ_ALWAYS_INLINE
    float operator ! () const
    {
#ifdef OZ_SIMD_MATH
      return Math::sqrt( vsDot( f4, f4 ) );
#else
      return Math::sqrt( x*x + y*y + z*z + w*w );
#endif
    }

    /**
     * Approximate norm.
     */
    OZ_ALWAYS_INLINE
    float fastN() const
    {
#ifdef OZ_SIMD_MATH
      return Math::fastSqrt( vsDot( f4, f4 ) );
#else
      return Math::fastSqrt( x*x + y*y + z*z + w*w );
#endif
    }

    /**
     * Squared norm.
     */
    OZ_ALWAYS_INLINE
    scalar sqN() const
    {
#ifdef OZ_SIMD_MATH
      return vDot( f4, f4 );
#else
      return x*x + y*y + z*z + w*w;
#endif
    }

    /**
     * Unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator ~ () const
    {
#ifdef OZ_SIMD_MATH
      scalar s = 1.0f / Math::sqrt( vsDot( f4, f4 ) );
      return Vec4( f4 * s.f4 );
#else
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
#endif
    }

    /**
     * Approximate unit vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 fastUnit() const
    {
#ifdef OZ_SIMD_MATH
      scalar s = Math::fastInvSqrt( vsDot( f4, f4 ) );
      return Vec4( f4 * s.f4 );
#else
      hard_assert( x*x + y*y + z*z + w*w > 0.0f );

      float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
      return Vec4( x * k, y * k, z * k, w * k );
#endif
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
#ifdef OZ_SIMD_MATH
      return Vec4( -f4 );
#else
      return Vec4( -x, -y, -z, -w );
#endif
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator + ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 + v.f4 );
#else
      return Vec4( x + v.x, y + v.y, z + v.z, w + v.w );
#endif
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator - ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 - v.f4 );
#else
      return Vec4( x - v.x, y - v.y, z - v.z, w - v.w );
#endif
    }

    /**
     * Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator * ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 * s.f4 );
#else
      return Vec4( x * s, y * s, z * s, w * s );
#endif
    }

    /**
     * Vector multiplied by a scalar.
     */
    OZ_ALWAYS_INLINE
    friend Vec4 operator * ( scalar s, const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      return Vec4( s.f4 * v.f4 );
#else
      return Vec4( s * v.x, s * v.y, s * v.z, s * v.w );
#endif
    }

    /**
     * Vector divided by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator / ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      return Vec4( f4 / s.f4 );
#else
      hard_assert( s != 0.0f );

      s = 1.0f / s;
      return Vec4( x * s, y * s, z * s, w * s );
#endif
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator += ( const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      f4 += v.f4;
#else
      x += v.x;
      y += v.y;
      z += v.z;
      w += v.w;
#endif
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator -= ( const Vec4& v )
    {
#ifdef OZ_SIMD_MATH
      f4 -= v.f4;
#else
      x -= v.x;
      y -= v.y;
      z -= v.z;
      w -= v.w;
#endif
      return *this;
    }

    /**
     * Multiplication by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator *= ( scalar s )
    {
#ifdef OZ_SIMD_MATH
      f4 *= s.f4;
#else
      x *= s;
      y *= s;
      z *= s;
      w *= s;
#endif
      return *this;
    }

    /**
     * Division by a scalar.
     */
    OZ_ALWAYS_INLINE
    Vec4& operator /= ( scalar s )
    {
#ifdef OZ_SIMD_MATH
      f4 /= s.f4;
#else
      hard_assert( s != 0.0f );

      s  = 1.0f / s;
      x *= s;
      y *= s;
      z *= s;
      w *= s;
#endif
      return *this;
    }

    /**
     * Scalar product.
     */
    OZ_ALWAYS_INLINE
    scalar operator * ( const Vec4& v ) const
    {
#ifdef OZ_SIMD_MATH
      return vDot( f4, v.f4 );
#else
      return x*v.x + y*v.y + z*v.z + w*v.w;
#endif
    }

};

}
