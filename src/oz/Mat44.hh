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
 * @file oz/Mat44.hh
 */

#pragma once

#include "Quat.hh"

namespace oz
{

/**
 * Column-major 4x4 matrix.
 *
 * @ingroup oz
 */
class Mat44
{
  public:

    /// Zero matrix.
    static const Mat44 ZERO;

    /// Identity.
    static const Mat44 ID;

    Vec4 x; ///< First column (i base vector).
    Vec4 y; ///< Second column (j base vector).
    Vec4 z; ///< Third column (k base vector).
    Vec4 w; ///< Fourth column (translation).

    /**
     * Create an uninitialised instance.
     */
    Mat44() = default;

    /**
     * Create matrix with the given columns.
     */
    OZ_ALWAYS_INLINE
    explicit Mat44( const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d ) :
        x( a ), y( b ), z( c ), w( d )
    {}

    /**
     * Create matrix for base vector images <tt>a</tt>, <tt>b</tt>, <tt>c</tt> and
     * translation <tt>d</tt>.
     */
    OZ_ALWAYS_INLINE
    explicit Mat44( const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d ) :
        x( a, 0.0f ), y( b, 0.0f ), z( c, 0.0f ), w( d, 1.0f )
    {}

    /**
     * Create matrix with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit Mat44( float xx, float xy, float xz, float xw,
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
    explicit Mat44( const float* v ) : x( &v[0] ), y( &v[4] ), z( &v[8] ), w( &v[12] )
    {}

    /**
     * Equality.
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const Mat44& m ) const
    {
      return x == m.x && y == m.y && z == m.z && w == m.w;
    }

    /**
     * Inequality.
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const Mat44& m ) const
    {
      return x != m.x || y != m.y || z != m.z || w != m.w;
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
     * Constant reference to the i-th element.
     */
    OZ_ALWAYS_INLINE
    const float& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < 16 );

      return ( &x.x )[i];
    }

    /**
     * Reference to the i-th element.
     */
    OZ_ALWAYS_INLINE
    float& operator [] ( int i )
    {
      hard_assert( 0 <= i && i < 16 );

      return ( &x.x )[i];
    }

    /**
     * Transposed matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator ~ () const
    {
      return Mat44( x.x, y.x, z.x, w.x,
                    x.y, y.y, z.y, w.y,
                    x.z, y.z, z.z, w.z,
                    x.w, y.w, z.w, w.w );
    }

    /**
     * Determinant.
     */
    OZ_ALWAYS_INLINE
    float det() const
    {
      float klop = z.z * w.w - w.z * z.w;
      float jlnp = y.z * w.w - w.z * y.w;
      float jkno = y.z * z.w - z.z * y.w;
      float ilmp = x.z * w.w - w.z * x.w;
      float ikmo = x.z * z.w - z.z * x.w;
      float ijmn = x.z * y.w - y.z * x.w;
      return
          x.x * y.y * klop -
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
    OZ_ALWAYS_INLINE
    float det3() const
    {
      return
          x.x * ( y.y*z.z - y.z*z.y ) -
          y.x * ( x.y*z.z - x.z*z.y ) +
          z.x * ( x.y*y.z - x.z*y.y );
    }

    /**
     * Original matrix.
     */
    OZ_ALWAYS_INLINE
    const Mat44& operator + () const
    {
      return *this;
    }

    /**
     * Matrix with negated elements.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator - () const
    {
      return Mat44( -x, -y, -z, -w );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator + ( const Mat44& a ) const
    {
      return Mat44( x + a.x, y + a.y, z + a.z, w + a.w );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator - ( const Mat44& a ) const
    {
      return Mat44( x - a.x, y - a.y, z - a.z, w - a.w );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator * ( float k ) const
    {
      return Mat44( x * k, y * k, z * k, w * k );
    }

    /**
     * Product, compositum of linear transformations.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator * ( const Mat44& m ) const
    {
      return Mat44( x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                    x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                    x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                    x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w );
    }

    /**
     * Product, transformation of a vector (no translation is applied).
     */
    OZ_ALWAYS_INLINE
    Vec3 operator * ( const Vec3& v ) const
    {
      const Vec3& i = x;
      const Vec3& j = y;
      const Vec3& k = z;

      return i * v.x + j * v.y + k * v.z;
    }

    /**
     * Product, transformation of a point (translation is applied).
     */
    OZ_ALWAYS_INLINE
    Point3 operator * ( const Point3& p ) const
    {
      const Vec3& i = x;
      const Vec3& j = y;
      const Vec3& k = z;
      const Vec3& t = w;

      return Point3::ORIGIN + i * p.x + j * p.y + k * p.z + t;
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator / ( float k ) const
    {
      hard_assert( k != 0.0f );

      k = 1.0f / k;
      return Mat44( x * k, y * k, z * k, w * k );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Mat44& operator += ( const Mat44& a )
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
    Mat44& operator -= ( const Mat44& a )
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
    Mat44& operator *= ( float k )
    {
      x *= k;
      y *= k;
      z *= k;
      w *= k;
      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    Mat44& operator /= ( float k )
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
     * Compose with a translation from the right.
     */
    OZ_ALWAYS_INLINE
    void translate( const Vec3& v )
    {
      const Vec3& i = x;
      const Vec3& j = y;
      const Vec3& k = z;
      const Vec3& t = w;

      w = Vec4( i * v.x + j * v.y + k * v.z + t, 1.0f );
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

      y = Vec4( j * c + k * s, 0.0f );
      z = Vec4( k * c - j * s, 0.0f );
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

      x = Vec4( i * c - k * s, 0.0f );
      z = Vec4( k * c + i * s, 0.0f );
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

      x = Vec4( i * c + j * s, 0.0f );
      y = Vec4( j * c - i * s, 0.0f );
    }

    /**
     * Compose with a scale from the right.
     */
    OZ_ALWAYS_INLINE
    void scale( const Vec3& v )
    {
      Vec3& i = x;
      Vec3& j = y;
      Vec3& k = z;

      i *= v.x;
      j *= v.y;
      k *= v.z;
    }

    /**
     * Create matrix for translation.
     */
    OZ_ALWAYS_INLINE
    static Mat44 translation( const Vec3& v )
    {
      return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    v.x,  v.y,  v.z, 1.0f );
    }

    /**
     * Create matrix for rotation from a quaternion.
     */
    OZ_ALWAYS_INLINE
    static Mat44 rotation( const Quat& q )
    {
      //
      // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy    0 ]
      // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx    0 ]
      // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy  0 ]
      // [       0              0              0        1 ]
      //

      float x2 = q.x + q.x;
      float y2 = q.y + q.y;
      float z2 = q.z + q.z;
      float xx1 = 1.0f - x2 * q.x;
      float yy = y2 * q.y;
      float zz = z2 * q.z;
      float xy = x2 * q.y;
      float xz = x2 * q.z;
      float xw = x2 * q.w;
      float yz = y2 * q.z;
      float yw = y2 * q.w;
      float zw = z2 * q.w;

      return Mat44( 1.0f - yy - zz, xy + zw,  xz - yw,  0.0f,
                    xy - zw,        xx1 - zz, yz + xw,  0.0f,
                    xz + yw,        yz - xw,  xx1 - yy, 0.0f,
                    0.0f,           0.0f,     0.0f,     1.0f );
    }

    /**
     * Convert 3x3 rotation submatrix to a quaternion.
     */
    OZ_ALWAYS_INLINE
    Quat toQuat() const
    {
      float w2 = Math::sqrt( 1.0f + x.x + y.y + z.z );
      float w4 = 2.0f * w2;

      return ~Quat( ( y.z - z.y ) / w4, ( z.x - x.z ) / w4, ( x.y - y.x ) / w4, 0.5f * w2 );
    }

    /**
     * Create matrix for rotation around x axis.
     */
    OZ_ALWAYS_INLINE
    static Mat44 rotationX( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat44( 1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f,    c,    s, 0.0f,
                    0.0f,   -s,    c, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
    }

    /**
     * Create matrix for rotation around y axis.
     */
    OZ_ALWAYS_INLINE
    static Mat44 rotationY( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat44(    c, 0.0f,   -s, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                       s, 0.0f,    c, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
    }

    /**
     * Create matrix for rotation around z axis.
     */
    OZ_ALWAYS_INLINE
    static Mat44 rotationZ( float theta )
    {
      float s, c;
      Math::sincos( theta, &s, &c );

      return Mat44(    c,    s, 0.0f, 0.0f,
                      -s,    c, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
    }

    /**
     * Create matrix for scaling.
     */
    OZ_ALWAYS_INLINE
    static Mat44 scaling( const Vec3& v )
    {
      return Mat44(  v.x, 0.0f, 0.0f, 0.0f,
                    0.0f,  v.y, 0.0f, 0.0f,
                    0.0f, 0.0f,  v.z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
    }

};

}
