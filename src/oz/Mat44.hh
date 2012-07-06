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
 * @file oz/Mat44.hh
 *
 * Mat44 class.
 */

#pragma once

#include "Plane.hh"
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

    Vec4 x; ///< First column (image of i base vector).
    Vec4 y; ///< Second column (image of j base vector).
    Vec4 z; ///< Third column (image of k base vector).
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
     * Create matrix for base vector images <tt>a</tt>, <tt>b</tt>, <tt>c</tt> and translation
     * <tt>d</tt>.
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
    explicit Mat44( const float* v ) :
      x( &v[0] ), y( &v[4] ), z( &v[8] ), w( &v[12] )
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
    OZ_ALWAYS_INLINE
    float det3() const
    {
      return x.x * ( y.y*z.z - y.z*z.y ) -
             y.x * ( x.y*z.z - x.z*z.y ) +
             z.x * ( x.y*y.z - x.z*y.y );
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
     * Original matrix.
     */
    OZ_ALWAYS_INLINE
    Mat44 operator + () const
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
     * Transformed 3D vector (no translation is applied).
     */
    OZ_ALWAYS_INLINE
    Vec3 operator * ( const Vec3& v ) const
    {
      return Vec3( x.x * v.x + y.x * v.y + z.x * v.z,
                   x.y * v.x + y.y * v.y + z.y * v.z,
                   x.z * v.x + y.z * v.y + z.z * v.z );
    }

    /**
     * Transformed point (translation is applied).
     */
    OZ_ALWAYS_INLINE
    Point operator * ( const Point& p ) const
    {
      return Point( x.x * p.x + y.x * p.y + z.x * p.z + w.x,
                    x.y * p.x + y.y * p.y + z.y * p.z + w.y,
                    x.z * p.x + y.z * p.y + z.z * p.z + w.z );
    }

    /**
     * Transformed plane.
     */
    OZ_ALWAYS_INLINE
    Plane operator * ( const Plane& p ) const
    {
      Plane tp;

      tp.n = Vec3( x.x * p.n.x + y.x * p.n.y + z.x * p.n.z,
                   x.y * p.n.x + y.y * p.n.y + z.y * p.n.z,
                   x.z * p.n.x + y.z * p.n.y + z.z * p.n.z );
      tp.d = p.d + tp.n.x * w.x + tp.n.y * w.y + tp.n.z * w.z;

      return tp;
    }

    /**
     * Product with a four-component vector.
     */
    OZ_ALWAYS_INLINE
    Vec4 operator * ( const Vec4& v ) const
    {
      return Vec4( x.x * v.x + y.x * v.y + z.x * v.z + w.x * v.w,
                   x.y * v.x + y.y * v.y + z.y * v.z + w.y * v.w,
                   x.z * v.x + y.z * v.y + z.z * v.z + w.z * v.w,
                   x.w * v.x + y.w * v.y + z.w * v.z + w.w * v.w );
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

      k  = 1.0f / k;
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
      w.x = x.x * v.x + y.x * v.y + z.x * v.z + w.x;
      w.y = x.y * v.x + y.y * v.y + z.y * v.z + w.y;
      w.z = x.z * v.x + y.z * v.y + z.z * v.z + w.z;
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
      Vec4 j = y;
      Vec4 k = z;

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
      Vec4 i = x;
      Vec4 k = z;

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
      Vec4 i = x;
      Vec4 j = y;

      float s, c;
      Math::sincos( theta, &s, &c );

      x = i * c + j * s;
      y = j * c - i * s;
    }

    /**
     * Compose with a scale from the right (fourth vector component is assumed 1.0).
     */
    OZ_ALWAYS_INLINE
    void scale( const Vec3& v )
    {
      x.x *= v.x;
      x.y *= v.x;
      x.z *= v.x;

      y.x *= v.y;
      y.y *= v.y;
      y.z *= v.y;

      z.x *= v.z;
      z.y *= v.z;
      z.z *= v.z;
    }

    /**
     * Compose with a scale from the right.
     */
    OZ_ALWAYS_INLINE
    void scale( const Vec4& v )
    {
      x.x *= v.x;
      x.y *= v.x;
      x.z *= v.x;
      x.w *= v.x;

      y.x *= v.y;
      y.y *= v.y;
      y.z *= v.y;
      y.w *= v.y;

      z.x *= v.z;
      z.y *= v.z;
      z.z *= v.z;
      z.w *= v.z;

      w.x *= v.w;
      w.y *= v.w;
      w.z *= v.w;
      w.w *= v.w;
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

      return Mat44( yy1 - zz2, xy2 + zw2, xz2 - yw2, 0.0f,
                    xy2 - zw2, xx1 - zz2, yz2 + xw2, 0.0f,
                    xz2 + yw2, yz2 - xw2, xx1 - yy2, 0.0f,
                         0.0f,      0.0f,      0.0f, 1.0f );
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
     * <tt>rotationZ( heading ) * rotationX( pitch ) * rotationZ( roll )</tt>.
     */
    OZ_ALWAYS_INLINE
    static Mat44 rotationZXZ( float heading, float pitch, float roll )
    {
      float hs, hc, ps, pc, rs, rc;

      Math::sincos( heading, &hs, &hc );
      Math::sincos( pitch, &ps, &pc );
      Math::sincos( roll, &rs, &rc );

      float hspc = hs*pc;
      float hcpc = hc*pc;

      return Mat44(  hc*rc - hspc*rs,  hs*rc + hcpc*rs, ps*rs, 0.0f,
                    -hc*rs - hspc*rc, -hs*rs + hcpc*rc, ps*rc, 0.0f,
                               hs*ps,           -hc*ps,    pc, 0.0f,
                                0.0f,             0.0f,  0.0f, 1.0f );
    }

    /**
     * Create matrix for scaling (fourth vector component is assumed 1.0).
     */
    OZ_ALWAYS_INLINE
    static Mat44 scaling( const Vec3& v )
    {
      return Mat44(  v.x, 0.0f, 0.0f, 0.0f,
                    0.0f,  v.y, 0.0f, 0.0f,
                    0.0f, 0.0f,  v.z, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f );
    }

    /**
     * Create matrix for scaling.
     */
    OZ_ALWAYS_INLINE
    static Mat44 scaling( const Vec4& v )
    {
      return Mat44(  v.x, 0.0f, 0.0f, 0.0f,
                    0.0f,  v.y, 0.0f, 0.0f,
                    0.0f, 0.0f,  v.z, 0.0f,
                    0.0f, 0.0f, 0.0f,  v.w );
    }

};

}
