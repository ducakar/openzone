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
 * @file ozCore/Mat4.hh
 *
 * `Mat4` class.
 */

#pragma once

#include "Mat3.hh"

namespace oz
{

/**
 * Column-major 4x4 matrix.
 *
 * @sa `oz::Mat3`
 */
class Mat4
{
public:

  /// Zero matrix.
  static const Mat4 ZERO;

  /// Identity.
  static const Mat4 ID;

  Vec4 x; ///< First column (image of i base vector).
  Vec4 y; ///< Second column (image of j base vector).
  Vec4 z; ///< Third column (image of k base vector).
  Vec4 w; ///< Fourth column (translation).

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Mat4() = default;

  /**
   * Create matrix with given columns.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4( const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d ) :
    x( a ), y( b ), z( c ), w( d )
  {}

  /**
   * Create matrix for base vector images `a`, `b`, `c` and translation `d`.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d ) :
    x( a ), y( b ), z( c ), w( d.x, d.y, d.z, 1.0f )
  {}

  /**
   * Create matrix with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4( float xx, float xy, float xz, float xw,
                 float yx, float yy, float yz, float yw,
                 float zx, float zy, float zz, float zw,
                 float wx, float wy, float wz, float ww ) :
    x( xx, xy, xz, xw ),
    y( yx, yy, yz, yw ),
    z( zx, zy, zz, zw ),
    w( wx, wy, wz, ww )
  {}

  /**
   * Create matrix from an array of 16 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4( const float* v ) :
    x( &v[0] ), y( &v[4] ), z( &v[8] ), w( &v[12] )
  {}

  /**
   * Create from a 3x3 matrix.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4( const Mat3& m ) :
    x( m.x ), y( m.y ), z( m.z ), w( 0.0f, 0.0f, 0.0f, 1.0f )
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == ( const Mat4& m ) const
  {
    return x == m.x && y == m.y && z == m.z && w == m.w;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != ( const Mat4& m ) const
  {
    return !operator == ( m );
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
  const Vec4& operator [] ( int i ) const
  {
    return ( &x )[i];
  }

  /**
   * Reference to the `i`-th column.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator [] ( int i )
  {
    return ( &x )[i];
  }

  /**
   * `i`-th row.
   */
  OZ_ALWAYS_INLINE
  Vec4 row( int i ) const
  {
    return Vec4( x[i], y[i], z[i], w[i] );
  }

  /**
   * First 3x3 submatrix.
   */
  OZ_ALWAYS_INLINE
  Mat3 mat3() const
  {
    return Mat3( x.x, x.y, x.z,
                 y.x, y.y, y.z,
                 z.x, z.y, z.z );
  }

  /**
   * Transposed matrix.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator ~ () const
  {
    return Mat4( x.x, y.x, z.x, w.x,
                 x.y, y.y, z.y, w.y,
                 x.z, y.z, z.z, w.z,
                 x.w, y.w, z.w, w.w );
  }

  /**
   * Determinant.
   */
  float det() const
  {
    float klop = z.z * w.w - w.z * z.w;
    float jlnp = y.z * w.w - w.z * y.w;
    float jkno = y.z * z.w - z.z * y.w;
    float ilmp = x.z * w.w - w.z * x.w;
    float ikmo = x.z * z.w - z.z * x.w;
    float ijmn = x.z * y.w - y.z * x.w;

    return x.x * y.y * klop -
           x.x * z.y * jlnp +
           x.x * w.y * jkno -
           y.x * x.y * klop +
           y.x * z.y * ilmp -
           y.x * w.y * ikmo +
           z.x * x.y * jlnp -
           z.x * y.y * ilmp +
           z.x * w.y * ijmn -
           w.x * x.y * jkno +
           w.x * y.y * ikmo -
           w.x * z.y * ijmn;
  }

  /**
   * Determinant of the upper-left 3x3 submatrix.
   */
  float det3() const
  {
    return x.x * ( y.y*z.z - y.z*z.y ) -
           y.x * ( x.y*z.z - x.z*z.y ) +
           z.x * ( x.y*y.z - x.z*y.y );
  }

  /**
   * Convert 3x3 rotation submatrix to a quaternion.
   */
  Quat toQuat() const;

  /**
   * Original matrix.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator + () const
  {
    return *this;
  }

  /**
   * Matrix with negated elements.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator - () const
  {
    return Mat4( -x, -y, -z, -w );
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator + ( const Mat4& a ) const
  {
    return Mat4( x + a.x, y + a.y, z + a.z, w + a.w );
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator - ( const Mat4& a ) const
  {
    return Mat4( x - a.x, y - a.y, z - a.z, w - a.w );
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator * ( scalar s ) const
  {
    return Mat4( x * s, y * s, z * s, w * s );
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  friend Mat4 operator * ( scalar s, const Mat4& m )
  {
    return Mat4( s * m.x, s * m.y, s * m.z, s * m.w );
  }

  /**
   * Product, composite of linear transformations.
   */
  Mat4 operator * ( const Mat4& m ) const
  {
#ifdef OZ_SIMD_MATH
    return Mat4( Vec4( x.f4 * vFill( m.x.x ) + y.f4 * vFill( m.x.y ) +
                       z.f4 * vFill( m.x.z ) + w.f4 * vFill( m.x.w ) ),
                 Vec4( x.f4 * vFill( m.y.x ) + y.f4 * vFill( m.y.y ) +
                       z.f4 * vFill( m.y.z ) + w.f4 * vFill( m.y.w ) ),
                 Vec4( x.f4 * vFill( m.z.x ) + y.f4 * vFill( m.z.y ) +
                       z.f4 * vFill( m.z.z ) + w.f4 * vFill( m.z.w ) ),
                 Vec4( x.f4 * vFill( m.w.x ) + y.f4 * vFill( m.w.y ) +
                       z.f4 * vFill( m.w.z ) + w.f4 * vFill( m.w.w ) ) );
#else
    return Mat4( x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                 x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                 x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                 x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w );
#endif
  }

  /**
   * Transformed 3D vector (no translation is applied).
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
   * Transformed point (translation is applied).
   */
  Point operator * ( const Point& p ) const
  {
#ifdef OZ_SIMD_MATH
    return Point( x.f4 * vFill( p.x ) + y.f4 * vFill( p.y ) + z.f4 * vFill( p.z ) + w.f4 );
#else
    return Point( x.x * p.x + y.x * p.y + z.x * p.z + w.x,
                  x.y * p.x + y.y * p.y + z.y * p.z + w.y,
                  x.z * p.x + y.z * p.y + z.z * p.z + w.z );
#endif
  }

  /**
   * Transformed plane.
   */
  Plane operator * ( const Plane& p ) const
  {
    Plane tp;

#ifdef OZ_SIMD_MATH
    tp.n = Vec3( x.f4 * vFill( p.n.x ) + y.f4 * vFill( p.n.y ) + z.f4 * vFill( p.n.z ) );
    tp.d = p.d + vFirst( vDot( tp.n.f4, w.f4 ) );
#else
    tp.n = Vec3( x.x * p.n.x + y.x * p.n.y + z.x * p.n.z,
                 x.y * p.n.x + y.y * p.n.y + z.y * p.n.z,
                 x.z * p.n.x + y.z * p.n.y + z.z * p.n.z );
    tp.d = p.d + tp.n.x * w.x + tp.n.y * w.y + tp.n.z * w.z;
#endif

    return tp;
  }

  /**
   * Product with a four-component vector.
   */
  Vec4 operator * ( const Vec4& v ) const
  {
#ifdef OZ_SIMD_MATH
    return Vec4( x.f4 * vFill( v.x ) + y.f4 * vFill( v.y ) +
                 z.f4 * vFill( v.z ) + w.f4 * vFill( v.w ) );
#else
    return Vec4( x.x * v.x + y.x * v.y + z.x * v.z + w.x * v.w,
                 x.y * v.x + y.y * v.y + z.y * v.z + w.y * v.w,
                 x.z * v.x + y.z * v.y + z.z * v.z + w.z * v.w,
                 x.w * v.x + y.w * v.y + z.w * v.z + w.w * v.w );
#endif
  }

  /**
   * Quotient.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator / ( scalar s ) const
  {
    hard_assert( s != 0.0f );

#ifdef OZ_SIMD_MATH
    s = vFill( 1.0f ) / s.f4;
#else
    s = 1.0f / s;
#endif
    return Mat4( x * s, y * s, z * s, w * s );
  }

  /**
   * Composite of affine transformations in 3D.
   *
   * This is a faster version of multiplication where the last row of both matrices is assumed to
   * be [0, 0, 0, 1].
   */
  Mat4 operator ^ ( const Mat4& m ) const
  {
#ifdef OZ_SIMD_MATH
    return Mat4( Vec4( x.f4 * vFill( m.x.x ) + y.f4 * vFill( m.x.y ) + z.f4 * vFill( m.x.z ) ),
                 Vec4( x.f4 * vFill( m.y.x ) + y.f4 * vFill( m.y.y ) + z.f4 * vFill( m.y.z ) ),
                 Vec4( x.f4 * vFill( m.z.x ) + y.f4 * vFill( m.z.y ) + z.f4 * vFill( m.z.z ) ),
                 Vec4( x.f4 * vFill( m.w.x ) + y.f4 * vFill( m.w.y ) +
                       z.f4 * vFill( m.w.z ) + w.f4 ) );
#else
    return Mat4( Vec4( x.x * m.x.x + y.x * m.x.y + z.x * m.x.z,
                       x.y * m.x.x + y.y * m.x.y + z.y * m.x.z,
                       x.z * m.x.x + y.z * m.x.y + z.z * m.x.z,
                       0.0f ),
                 Vec4( x.x * m.y.x + y.x * m.y.y + z.x * m.y.z,
                       x.y * m.y.x + y.y * m.y.y + z.y * m.y.z,
                       x.z * m.y.x + y.z * m.y.y + z.z * m.y.z,
                       0.0f ),
                 Vec4( x.x * m.z.x + y.x * m.z.y + z.x * m.z.z,
                       x.y * m.z.x + y.y * m.z.y + z.y * m.z.z,
                       x.z * m.z.x + y.z * m.z.y + z.z * m.z.z,
                       0.0f ),
                 Vec4( x.x * m.w.x + y.x * m.w.y + z.x * m.w.z + w.x,
                       x.y * m.w.x + y.y * m.w.y + z.y * m.w.z + w.y,
                       x.z * m.w.x + y.z * m.w.y + z.z * m.w.z + w.z,
                       1.0f ) );
#endif
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Mat4& operator += ( const Mat4& a )
  {
    x += a.x;
    y += a.y;
    z += a.z;
    w += a.w;
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Mat4& operator -= ( const Mat4& a )
  {
    x -= a.x;
    y -= a.y;
    z -= a.z;
    w -= a.w;
    return *this;
  }

  /**
   * Multiplication.
   */
  OZ_ALWAYS_INLINE
  Mat4& operator *= ( scalar s )
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /**
   * Division.
   */
  OZ_ALWAYS_INLINE
  Mat4& operator /= ( scalar s )
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
    w *= s;
    return *this;
  }

  /**
   * Compose with a translation from the right.
   */
  void translate( const Vec3& v )
  {
#ifdef OZ_SIMD_MATH
    w.f4 = x.f4 * vFill( v.x ) + y.f4 * vFill( v.y ) + z.f4 * vFill( v.z ) + w.f4;
#else
    w.x = x.x * v.x + y.x * v.y + z.x * v.z + w.x;
    w.y = x.y * v.x + y.y * v.y + z.y * v.z + w.y;
    w.z = x.z * v.x + y.z * v.y + z.z * v.z + w.z;
#endif
  }

  /**
   * Compose with a rotation from the right.
   */
  void rotate( const Quat& q );

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
   * Compose with a scale from the right (fourth vector component is assumed 1.0).
   */
  OZ_ALWAYS_INLINE
  void scale( const Vec3& v )
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }

  /**
   * Compose with a scale from the right.
   */
  OZ_ALWAYS_INLINE
  void scale( const Vec4& v )
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
  }

  /**
   * Create matrix for translation.
   */
  OZ_ALWAYS_INLINE
  static Mat4 translation( const Vec3& v )
  {
    return Mat4( 1.0f, 0.0f, 0.0f, 0.0f,
                 0.0f, 1.0f, 0.0f, 0.0f,
                 0.0f, 0.0f, 1.0f, 0.0f,
                  v.x,  v.y,  v.z, 1.0f );
  }

  /**
   * Create matrix for rotation from a quaternion.
   */
  static Mat4 rotation( const Quat& q );

  /**
   * Create matrix for rotation around x axis.
   */
  static Mat4 rotationX( float theta );

  /**
   * Create matrix for rotation around y axis.
   */
  static Mat4 rotationY( float theta );

  /**
   * Create matrix for rotation around z axis.
   */
  static Mat4 rotationZ( float theta );

  /**
   * `rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )`.
   */
  static Mat4 rotationZXZ( float heading, float pitch, float roll );

  /**
   * Create matrix for scaling (fourth vector component is assumed 1.0).
   */
  OZ_ALWAYS_INLINE
  static Mat4 scaling( const Vec3& v )
  {
    return Mat4(  v.x, 0.0f, 0.0f, 0.0f,
                 0.0f,  v.y, 0.0f, 0.0f,
                 0.0f, 0.0f,  v.z, 0.0f,
                 0.0f, 0.0f, 0.0f, 1.0f );
  }

  /**
   * Create matrix for scaling.
   */
  OZ_ALWAYS_INLINE
  static Mat4 scaling( const Vec4& v )
  {
    return Mat4(  v.x, 0.0f, 0.0f, 0.0f,
                 0.0f,  v.y, 0.0f, 0.0f,
                 0.0f, 0.0f,  v.z, 0.0f,
                 0.0f, 0.0f, 0.0f,  v.w );
  }

  /**
   * Per-component absolute value of a matrix.
   */
  OZ_ALWAYS_INLINE
  friend Mat4 abs( const Mat4& a )
  {
    return Mat4( abs( a.x ), abs( a.y ), abs( a.z ), abs( a.w ) );
  }

  /**
   * Per-component minimum of two matrices.
   */
  OZ_ALWAYS_INLINE
  friend Mat4 min( const Mat4& a, const Mat4& b )
  {
    return Mat4( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ), min( a.w, b.w ) );
  }

  /**
   * Per-component maximum of two matrices.
   */
  OZ_ALWAYS_INLINE
  friend Mat4 max( const Mat4& a, const Mat4& b )
  {
    return Mat4( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ), max( a.w, b.w ) );
  }

  /**
   * Per-component clamped value of matrices.
   */
  OZ_ALWAYS_INLINE
  friend Mat4 clamp( const Mat4& c, const Mat4& a, const Mat4& b )
  {
    return Mat4( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ),
                 clamp( c.w, a.w, b.w ) );
  }

};

}
