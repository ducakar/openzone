/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/Mat33.hh
 *
 * Mat33 class.
 */

#pragma once

#include "Plane.hh"
#include "Quat.hh"

namespace oz
{

/**
 * Column-major 3x3 matrix.
 */
class Mat33
{
  public:

    /// Zero matrix.
    static const Mat33 ZERO;

    /// Identity.
    static const Mat33 ID;

    Vec3 x; ///< First column (image of i base vector).
    Vec3 y; ///< Second column (image of j base vector).
    Vec3 z; ///< Third column (image of k base vector).

  public:

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    explicit Mat33() = default;

    /**
     * Create matrix with the given columns.
     */
    OZ_ALWAYS_INLINE
    explicit Mat33( const Vec3& a, const Vec3& b, const Vec3& c ) :
      x( a ), y( b ), z( c )
    {}

    /**
     * Create matrix with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Mat33( float xx, float xy, float xz,
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
    explicit Mat33( const float* v ) :
      x( &v[0] ), y( &v[3] ), z( &v[6] )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Mat33& m ) const
    {
      return x == m.x && y == m.y && z == m.z;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Mat33& m ) const
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
    Mat33 operator ~ () const
    {
      return Mat33( x.x, y.x, z.x,
                    x.y, y.y, z.y,
                    x.z, y.z, z.z );
    }

    /**
     * Determinant.
     */
    OZ_ALWAYS_INLINE
    float det() const
    {
      return x.x * ( y.y*z.z - y.z*z.y ) -
             y.x * ( x.y*z.z - x.z*z.y ) +
             z.x * ( x.y*y.z - x.z*y.y );
    }

    /**
     * Convert rotation matrix to a quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat toQuat() const
    {
      float w2 = Math::sqrt( 1.0f + x.x + y.y + z.z );
      float w4 = 2.0f * w2;

      return ~Quat( ( y.z - z.y ) / w4, ( z.x - x.z ) / w4, ( x.y - y.x ) / w4, 0.5f * w2 );
    }

    /**
     * Original matrix.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator + () const
    {
      return *this;
    }

    /**
     * Matrix with negated elements.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator - () const
    {
      return Mat33( -x, -y, -z );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator + ( const Mat33& a ) const
    {
      return Mat33( x + a.x, y + a.y, z + a.z );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator - ( const Mat33& a ) const
    {
      return Mat33( x - a.x, y - a.y, z - a.z );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator * ( scalar s ) const
    {
      return Mat33( x * s, y * s, z * s );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend Mat33 operator * ( scalar s, const Mat33& m )
    {
      return Mat33( s * m.x, s * m.y, s * m.z );
    }

    /**
     * Product, compositum of linear transformations.
     */
    OZ_ALWAYS_INLINE
    Mat33 operator * ( const Mat33& m ) const
    {
#ifdef OZ_SIMD_MATH
      return Mat33( Vec3( x.f4 * vFill( m.x.x ) + y.f4 * vFill( m.x.y ) + z.f4 * vFill( m.x.z ) ),
                    Vec3( x.f4 * vFill( m.y.x ) + y.f4 * vFill( m.y.y ) + z.f4 * vFill( m.y.z ) ),
                    Vec3( x.f4 * vFill( m.z.x ) + y.f4 * vFill( m.z.y ) + z.f4 * vFill( m.z.z ) ) );
#else
      return Mat33( x * m.x.x + y * m.x.y + z * m.x.z,
                    x * m.y.x + y * m.y.y + z * m.y.z,
                    x * m.z.x + y * m.z.y + z * m.z.z );
#endif
    }

    /**
     * Transformed 3D vector.
     */
    OZ_ALWAYS_INLINE
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
    OZ_ALWAYS_INLINE
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
    Mat33 operator / ( scalar s ) const
    {
#ifdef OZ_SIMD_MATH
      s = vFill( 1.0f ) / s.f4;
#else
      hard_assert( s != 0.0f );

      s = 1.0f / s;
#endif
      return Mat33( x * s, y * s, z * s );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Mat33& operator += ( const Mat33& a )
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
    Mat33& operator -= ( const Mat33& a )
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
    Mat33& operator *= ( scalar s )
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
    Mat33& operator /= ( scalar s )
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
    OZ_ALWAYS_INLINE
    void rotateX( float theta )
    {
      Vec3 j = y;
      Vec3 k = z;

      float s, c;
      Math::sincos( theta, &s, &c );

      y = j * c + k * s;
      z = k * c - j * s;
    }

    /**
     * Compose with a rotation from the right.
     */
    OZ_ALWAYS_INLINE
    void rotateY( float theta )
    {
      Vec3 i = x;
      Vec3 k = z;

      float s, c;
      Math::sincos( theta, &s, &c );

      x = i * c - k * s;
      z = k * c + i * s;
    }

    /**
     * Compose with a rotation from the right.
     */
    OZ_ALWAYS_INLINE
    void rotateZ( float theta )
    {
      Vec3 i = x;
      Vec3 j = y;

      float s, c;
      Math::sincos( theta, &s, &c );

      x = i * c + j * s;
      y = j * c - i * s;
    }

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
    OZ_ALWAYS_INLINE
    static Mat33 rotation( const Quat& q )
    {
      //
      // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy   ]
      // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx   ]
      // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy ]
      //

      float x2 = q.x + q.x;
      float y2 = q.y + q.y;
      float z2 = q.z + q.z;
      float xx2 = x2 * q.x;
      float yy2 = y2 * q.y;
      float zz2 = z2 * q.z;
      float xy2 = x2 * q.y;
      float xz2 = x2 * q.z;
      float xw2 = x2 * q.w;
      float yz2 = y2 * q.z;
      float yw2 = y2 * q.w;
      float zw2 = z2 * q.w;
      float xx1 = 1.0f - xx2;
      float yy1 = 1.0f - yy2;

      return Mat33( yy1 - zz2, xy2 + zw2, xz2 - yw2,
                    xy2 - zw2, xx1 - zz2, yz2 + xw2,
                    xz2 + yw2, yz2 - xw2, xx1 - yy2 );
    }

    /**
     * Create matrix for rotation around x axis.
     */
    OZ_ALWAYS_INLINE
    static Mat33 rotationX( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat33( 1.0f, 0.0f, 0.0f,
                    0.0f,    c,    s,
                    0.0f,   -s,    c );
    }

    /**
     * Create matrix for rotation around y axis.
     */
    OZ_ALWAYS_INLINE
    static Mat33 rotationY( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat33(    c, 0.0f,   -s,
                    0.0f, 1.0f, 0.0f,
                       s, 0.0f,    c );
    }

    /**
     * Create matrix for rotation around z axis.
     */
    OZ_ALWAYS_INLINE
    static Mat33 rotationZ( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat33(    c,    s, 0.0f,
                      -s,    c, 0.0f,
                    0.0f, 0.0f, 1.0f );
    }

    /**
     * `rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )`.
     */
    OZ_ALWAYS_INLINE
    static Mat33 rotationZXZ( float heading, float pitch, float roll )
    {
      float hs, hc, ps, pc, rs, rc;

      Math::sincos( heading, &hs, &hc );
      Math::sincos( pitch, &ps, &pc );
      Math::sincos( roll, &rs, &rc );

      float hspc = hs*pc;
      float hcpc = hc*pc;

      return Mat33(  hc*rc - hspc*rs,  hs*rc + hcpc*rs, ps*rs,
                    -hc*rs - hspc*rc, -hs*rs + hcpc*rc, ps*rc,
                               hs*ps,           -hc*ps,    pc );
    }

    /**
     * Create matrix for scaling.
     */
    OZ_ALWAYS_INLINE
    static Mat33 scaling( const Vec3& v )
    {
      return Mat33(  v.x, 0.0f, 0.0f,
                    0.0f,  v.y, 0.0f,
                    0.0f, 0.0f,  v.z );
    }

};

/**
 * Per-component absolute value of a matrix.
 */
OZ_ALWAYS_INLINE
inline Mat33 abs( const Mat33& a )
{
  return Mat33( abs( a.x ), abs( a.y ), abs( a.z ) );
}

/**
 * Per-component minimum of two matrices.
 */
OZ_ALWAYS_INLINE
inline Mat33 min( const Mat33& a, const Mat33& b )
{
  return Mat33( min( a.x, b.x ), min( a.y, b.y ), min( a.z, b.z ) );
}

/**
 * Per-component maximum of two matrices.
 */
OZ_ALWAYS_INLINE
inline Mat33 max( const Mat33& a, const Mat33& b )
{
  return Mat33( max( a.x, b.x ), max( a.y, b.y ), max( a.z, b.z ) );
}

/**
 * Per-component clamped value of matrices.
 */
OZ_ALWAYS_INLINE
inline Mat33 clamp( const Mat33& c, const Mat33& a, const Mat33& b )
{
  return Mat33( clamp( c.x, a.x, b.x ), clamp( c.y, a.y, b.y ), clamp( c.z, a.z, b.z ) );
}

}
