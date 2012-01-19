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
 * @file oz/Simd.hh
 */

#include "Plane.hh"
#include "Quat.hh"

namespace oz
{

/**
 * SIMD vector of four floats.
 *
 * @ingroup oz
 */
typedef float float4 __attribute__(( vector_size( 16 ) ));

/**
 * @def float4
 * "Constructor" for <tt>float4</tt> type.
 *
 * @ingroup oz
 */
#ifndef __LINE__
# define float4( x, y, z, w ) float4( { x, y, z, w } )
#elif defined( __clang__ )
# define float4( x, y, z, w ) float4( { x, y, z, w } )
#else
# define float4( x, y, z, w ) float4{ x, y, z, w }
#endif

/**
 * SIMD vector of four unsigned integers.
 *
 * @ingroup oz
 */
typedef uint uint4 __attribute__(( vector_size( 16 ) )) ;

/**
 * @def uint4
 * "Constructor" for <tt>uint4</tt> type.
 *
 * @ingroup oz
 */
#ifndef __LINE__
# define uint4( x, y, z, w ) uint4( { x, y, z, w } )
#elif defined( __clang__ )
# define uint4( x, y, z, w ) uint4( { x, y, z, w } )
#else
# define uint4( x, y, z, w ) uint4{ x, y, z, w }
#endif

/**
 * SIMD register representation.
 *
 * @ingroup oz
 */
struct Simd
{
  union
  {
    uint4  u4;   ///< Unsigned integer SIMD vector.
    uint   u[4]; ///< Unsigned integer component of SIMD vector.

    float4 f4;   ///< Float SIMD vector.
    float  f[4]; ///< Float components of SIMD vector.
  };

  /**
   * Create an uninitialised instance.
   */
  Simd() = default;

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  Simd( float4 f4_ ) :
    f4( f4_ )
  {}

  /**
   * Initialise all SIMD components to the given float value.
   */
  OZ_ALWAYS_INLINE
  Simd( float f ) :
    f4( float4( f, f, f, f ) )
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  Simd( uint4 u4_ ) :
    u4( u4_ )
  {}

  /**
   * Initialise all SIMD components to the given unsigned integer value.
   */
  OZ_ALWAYS_INLINE
  Simd( uint u ) :
    u4( uint4( u, u, u, u ) )
  {}

  /**
   * Create from <tt>Vec3</tt>.
   */
  OZ_ALWAYS_INLINE
  Simd( const Vec3& v ) :
    f4( float4( v.x, v.y, v.z, 0.0f ) )
  {}

  /**
   * Create from <tt>Point3</tt>.
   */
  OZ_ALWAYS_INLINE
  Simd( const Point3& p ) :
    f4( float4( p.x, p.y, p.z, 1.0f ) )
  {}

  /**
   * Create from <tt>Plane</tt>.
   */
  OZ_ALWAYS_INLINE
  Simd( const Plane& p ) :
    f4( float4( p.n.x, p.n.y, p.n.z, p.d ) )
  {}

  /**
   * Create from <tt>Vec4</tt>.
   */
  OZ_ALWAYS_INLINE
  Simd( const Vec4& v ) :
    f4( float4( v.x, v.y, v.z, v.w ) )
  {}

  /**
   * Create from <tt>Quat</tt>.
   */
  OZ_ALWAYS_INLINE
  Simd( const Quat& q ) :
    f4( float4( q.x, q.y, q.z, q.w ) )
  {}

  /**
   * Equality (compared per floats).
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Simd& s ) const
  {
    return f[0] == s.f[0] && f[1] == s.f[1] && f[2] == s.f[2] && f[3] == s.f[3];
  }

  /**
   * Inequality (compared per floats).
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Simd& s ) const
  {
    return f[0] != s.f[0] && f[1] != s.f[1] && f[2] != s.f[2] && f[3] != s.f[3];
  }

  /**
   * Absolute value.
   */
  OZ_ALWAYS_INLINE
  Simd abs() const
  {
    return Simd( u4 & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff ) );
  }

  /**
   * Construct <tt>Vec3</tt> from SIMD components.
   */
  OZ_ALWAYS_INLINE
  Vec3 vec3() const
  {
    return Vec3( f[0], f[1], f[2] );
  }

  /**
   * Construct <tt>Point3</tt> from SIMD components.
   */
  OZ_ALWAYS_INLINE
  Point3 point3() const
  {
    return Point3( f[0], f[1], f[2] );
  }

  /**
   * Construct <tt>Plane</tt> from SIMD components.
   */
  OZ_ALWAYS_INLINE
  Plane plane() const
  {
    return Plane( f[0], f[1], f[2], f[3] );
  }

  /**
   * Construct <tt>Vec4</tt> from SIMD components.
   */
  OZ_ALWAYS_INLINE
  Vec4 vec4() const
  {
    return Vec4( f[0], f[1], f[2], f[3] );
  }

  /**
   * Construct <tt>Quat</tt> from SIMD components.
   */
  OZ_ALWAYS_INLINE
  Quat quat() const
  {
    return Quat( f[0], f[1], f[2], f[3] );
  }

  /**
   * Dot product (only first three components).
   */
  OZ_ALWAYS_INLINE
  float dot3( const Simd& s ) const
  {
    return f[0]*s.f[0] + f[1]*s.f[1] + f[2]*s.f[2];
  }

  /**
   * Dot product (all four components).
   */
  OZ_ALWAYS_INLINE
  float dot4( const Simd& s ) const
  {
    return f[0]*s.f[0] + f[1]*s.f[1] + f[2]*s.f[2] + f[3]*s.f[3];
  }
};

}
