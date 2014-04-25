/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Mat3.hh
 *
 * `Mat3` class.
 */

#pragma once

#include "Plane.hh"
#include "Quat.hh"

namespace oz
{

/**
 * Column-major 3x3 matrix.
 *
 * @sa `oz::Mat4`
 */
class Mat3
{
public:

  /// Zero matrix.
  static const Mat3 ZERO;

  /// Identity.
  static const Mat3 ID;

  Vec3 x; ///< First column (image of i base vector).
  Vec3 y; ///< Second column (image of j base vector).
  Vec3 z; ///< Third column (image of k base vector).

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  explicit Mat3() = default;

  /**
   * Create matrix with given columns.
   */
  OZ_ALWAYS_INLINE
  explicit Mat3( const Vec3& a, const Vec3& b, const Vec3& c ) :
    x( a ), y( b ), z( c )
  {}

  /**
   * Create matrix with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Mat3( float xx, float xy, float xz,
                 float yx, float yy, float yz,
                 float zx, float zy, float zz ) :
    x( xx, xy, xz ),
    y( yx, yy, yz ),
    z( zx, zy, zz )
  {}

  /**
   * Create matrix from an array of 9 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Mat3( const float* v ) :
    x( &v[0] ), y( &v[3] ), z( &v[6] )
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Mat3& m ) const
  {
    return x == m.x && y == m.y && z == m.z;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Mat3& m ) const
  {
    return x != m.x || y != m.y || z != m.z;
  }

  /**
   * Constant pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator const float* () const
  {
    return &x.x;
  }

  /**
   * Pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator float* ()
  {
    return &x.x;
  }

  /**
   * Constant reference to the `i`-th column.
   */
  OZ_ALWAYS_INLINE
  const Vec3& operator [] ( int i ) const
  {
    return ( &x )[i];
  }

  /**
   * Reference to the `i`-th column.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator [] ( int i )
  {
    return ( &x )[i];
  }

  /**
   * `i`-th row.
   */
  OZ_ALWAYS_INLINE
  Vec3 row( int i ) const
  {
    return Vec3( x[i], y[i], z[i] );
  }

  /**
   * Transposed matrix.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator ~ () const
  {
    return Mat3( x.x, y.x, z.x,
                 x.y, y.y, z.y,
                 x.z, y.z, z.z );
  }

  /**
   * Determinant.
   */
  float det() const
  {
    return x.x * ( y.y*z.z - y.z*z.y ) -
           y.x * ( x.y*z.z - x.z*z.y ) +
           z.x * ( x.y*y.z - x.z*y.y );
  }

  /**
   * Convert rotation matrix to a quaternion.
   */
  Quat toQuat() const;

  /**
   * Original matrix.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator + () const
  {
    return *this;
  }

  /**
   * Matrix with negated elements.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator - () const
  {
    return Mat3( -x, -y, -z );
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator + ( const Mat3& a ) const
  {
    return Mat3( x + a.x, y + a.y, z + a.z );
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator - ( const Mat3& a ) const
  {
    return Mat3( x - a.x, y - a.y, z - a.z );
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator * ( scalar s ) const
  {
    return Mat3( x * s, y * s, z * s );
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  friend Mat3 operator * ( scalar s, const Mat3& m )
  {
    return Mat3( s * m.x, s * m.y, s * m.z );
  }

  /**
   * Product, compositum of linear transformations.
   */
  Mat3 operator * ( const Mat3& m ) const
  {
#ifdef OZ_SIMD_MATH
    return Mat3( Vec3( x.f4 * vFill( m.x.x ) + y.f4 * vFill( m.x.y ) + z.f4 * vFill( m.x.z ) ),
                 Vec3( x.f4 * vFill( m.y.x ) + y.f4 * vFill( m.y.y ) + z.f4 * vFill( m.y.z ) ),
                 Vec3( x.f4 * vFill( m.z.x ) + y.f4 * vFill( m.z.y ) + z.f4 * vFill( m.z.z ) ) );
#else
    return Mat3( x * m.x.x + y * m.x.y + z * m.x.z,
                 x * m.y.x + y * m.y.y + z * m.y.z,
                 x * m.z.x + y * m.z.y + z * m.z.z );
#endif
  }

  /**
   * Transformed 3D vector.
   */
  Vec3 operator * ( const Vec3& v ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec3( x.f4 * vFill( v.x ) + y.f4 * vFill( v.y ) + z.f4 * vFill( v.z ) );
#else
    return Vec3( x.x * v.x + y.x * v.y + z.x * v.z,
                 x.y * v.x + y.y * v.y + z.y * v.z,
                 x.z * v.x + y.z * v.y + z.z * v.z );
#endif
  }

  /**
   * Rotated plane.
   */
  Plane operator * ( const Plane& p ) const
  {
    Plane tp;

#ifdef OZ_SIMD_MATH
    tp.n = Vec3( x.f4 * vFill( p.n.x ) + y.f4 * vFill( p.n.y ) + z.f4 * vFill( p.n.z ) );
    tp.d = p.d;
#else
    tp.n = Vec3( x.x * p.n.x + y.x * p.n.y + z.x * p.n.z,
                 x.y * p.n.x + y.y * p.n.y + z.y * p.n.z,
                 x.z * p.n.x + y.z * p.n.y + z.z * p.n.z );
    tp.d = p.d;
#endif

    return tp;
  }

  /**
   * Quotient.
   */
  OZ_ALWAYS_INLINE
  Mat3 operator / ( scalar s ) const
  {
    hard_assert( s != 0.0f );

#ifdef OZ_SIMD_MATH
    s = vFill( 1.0f ) / s.f4;
#else
    s = 1.0f / s;
#endif
    return Mat3( x * s, y * s, z * s );
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Mat3& operator += ( const Mat3& a )
  {
    x += a.x;
    y += a.y;
    z += a.z;
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Mat3& operator -= ( const Mat3& a )
  {
    x -= a.x;
    y -= a.y;
    z -= a.z;
    return *this;
  }

  /**
   * Multiplication.
   */
  OZ_ALWAYS_INLINE
  Mat3& operator *= ( scalar s )
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  /**
   * Division.
   */
  OZ_ALWAYS_INLINE
  Mat3& operator /= ( scalar s )
  {
#ifdef OZ_SIMD_MATH
    s  = vFill( 1.0f ) / s.f4;
#else
    hard_assert( s != 0.0f );

    s  = 1.0f / s;
#endif
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }

  /**
   * Compose with a rotation from the right.
   */
  OZ_ALWAYS_INLINE
  void rotate( const Quat& q )
  {
    *this = *this * rotation( q );
  }

  /**
   * Compose with a rotation from the right.
   */
  void rotateX( float theta );

  /**
   * Compose with a rotation from the right.
   */
  void rotateY( float theta );

  /**
   * Compose with a rotation from the right.
   */
  void rotateZ( float theta );

  /**
   * Compose with a scale from the right.
   */
  OZ_ALWAYS_INLINE
  void scale( const Vec3& v )
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }

  /**
   * Create matrix for rotation from a quaternion.
   */
  static Mat3 rotation( const Quat& q );

  /**
   * Create matrix for rotation around x axis.
   */
  static Mat3 rotationX( float theta );

  /**
   * Create matrix for rotation around y axis.
   */
  static Mat3 rotationY( float theta );

  /**
   * Create matrix for rotation around z axis.
   */
  static Mat3 rotationZ( float theta );

  /**
   * `rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )`.
   */
  static Mat3 rotationZXZ( float heading, float pitch, float roll );

  /**
   * Create matrix for scaling.
   */
  OZ_ALWAYS_INLINE
  static Mat3 scaling( const Vec3& v )
  {
    return Mat3(  v.x, 0.0f, 0.0f,
                 0.0f,  v.y, 0.0f,
                 0.0f, 0.0f,  v.z );
  }

};

/**
 * Per-component absolute value of a matrix.
 */
OZ_ALWAYS_INLINE
inline Mat3 abs( const Mat3& a )
{
  return Mat3( abs( a.x ), abs( a.y ), abs( a.z ) );
}

/**
 * Per-component minimum of two matrices.
 */
OZ_ALWAYS_INLINE
inline Mat3 min( const Mat3& a, const Mat3& b )
{
  return Mat3( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ) );
}

/**
 * Per-component maximum of two matrices.
 */
OZ_ALWAYS_INLINE
inline Mat3 max( const Mat3& a, const Mat3& b )
{
  return Mat3( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ) );
}

/**
 * Per-component clamped value of matrices.
 */
OZ_ALWAYS_INLINE
inline Mat3 clamp( const Mat3& c, const Mat3& a, const Mat3& b )
{
  return Mat3( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ) );
}

}
