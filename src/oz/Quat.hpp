/*
 *  Quat.hpp
 *
 *  Quaternion
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "Vec3.hpp"

namespace oz
{

  class Mat33;
  class Mat44;

#ifdef OZ_SIMD

  class Quat : public Vec3
  {
    public:

      static const Quat ZERO;
      static const Quat ID;

      OZ_ALWAYS_INLINE
      explicit Quat()
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( float4 f4 ) : Vec3( f4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( int4 i4 ) : Vec3( i4 )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( float x, float y, float z, float w ) : Vec3( float4( x, y, z, w ) )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const float* q ) : Vec3( float4( q[0], q[1], q[2], q[3] ) )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Quat& q ) const
      {
        return x == q.x && y == q.y && z == q.z && w == q.w;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Quat& q ) const
      {
        return x != q.x || y != q.y || z != q.z || w != q.w;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return f;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return f;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return f[i];
      }

      OZ_ALWAYS_INLINE
      Quat abs() const
      {
        return Quat( i4 & int4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator * () const
      {
        return Quat( i4 ^ int4( 0x80000000, 0x80000000, 0x80000000, 0x00000000 ) );
      }

      OZ_ALWAYS_INLINE
      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z + w*w );
      }

      OZ_ALWAYS_INLINE
      float sqL() const
      {
        return x*x + y*y + z*z + w*w;
      }

      OZ_ALWAYS_INLINE
      bool isUnit() const
      {
        return x*x + y*y + z*z + w*w == 1.0f;
      }

      OZ_ALWAYS_INLINE
      Quat operator ~ () const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Quat( f4 / float4( k, k, k, k ) );
      }

      OZ_ALWAYS_INLINE
      Quat fastUnit() const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Quat( f4 * float4( k, k, k, k ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat operator - () const
      {
        return Quat( -f4 );
      }

      OZ_ALWAYS_INLINE
      Quat operator + ( const Quat& q ) const
      {
        return Quat( f4 + q.f4 );
      }

      OZ_ALWAYS_INLINE
      Quat operator - ( const Quat& q ) const
      {
        return Quat( f4 - q.f4 );
      }

      OZ_ALWAYS_INLINE
      Quat operator * ( float k ) const
      {
        return Quat( f4 * float4( k, k, k, k ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator * ( float4 k ) const
      {
        return Quat( f4 * k );
      }

      OZ_ALWAYS_INLINE
      friend Quat operator * ( float k, const Quat& q )
      {
        return Quat( q.f4 * float4( k, k, k, k ) );
      }

      OZ_ALWAYS_INLINE
      friend Quat operator * ( float4 k, const Quat& q )
      {
        return Quat( q.f4 * k );
      }

      OZ_ALWAYS_INLINE
      Quat operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        return Quat( f4 / float4( k, k, k, k ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator / ( float4 k ) const
      {
        return Quat( f4 / k );
      }

      OZ_ALWAYS_INLINE
      Quat& operator += ( const Quat& q )
      {
        f4 += q.f4;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator -= ( const Quat& q )
      {
        f4 -= q.f4;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator *= ( float k )
      {
        f4 *= float4( k, k, k, k );
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator *= ( float4 k )
      {
        f4 *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        f4 /= float4( k, k, k, k );
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator /= ( float4 k )
      {
        f4 /= k;
        return *this;
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat operator ^ ( const Quat& a ) const
      {

        return Quat( w*a.x + x*a.w + y*a.z - z*a.y,
                     w*a.y + y*a.w + z*a.x - x*a.z,
                     w*a.z + z*a.w + x*a.y - y*a.x,
                     w*a.w - x*a.x - y*a.y - z*a.z );
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat& operator ^= ( const Quat& a )
      {
        float tx = x, ty = y, tz = z;

        x = w*a.x + tx*a.w + ty*a.z - tz*a.y;
        y = w*a.y + ty*a.w + tz*a.x - tx*a.z;
        z = w*a.z + tz*a.w + tx*a.y - ty*a.x;
        w = w*a.w - tx*a.x - ty*a.y - tz*a.z;

        return *this;
      }

      // make rotation matrix (implemented in Mat33.hpp)
      Mat33 rotMat33() const;

      // make transposed (inverse) rotation matrix (implemented in Mat33.hpp)
      Mat33 invRotMat33() const;

      // make rotation matrix (implemented in Mat44.hpp)
      Mat44 rotMat44() const;

      // make transposed (inverse) rotation matrix (implemented in Mat44.hpp)
      Mat44 invRotMat44() const;

      // make quaternion for rotation around given axis
      static Quat rotAxis( const Vec3& axis, float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( float4( s, s, s, s ) * axis.f4 + float4( 0.0f, 0.0f, 0.0f, c ) );
      }

      // make quaternion for rotation around x axis
      static Quat rotX( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( float4( s, 0.0f, 0.0f, c ) );
      }

      // make quaternion for rotation around y axis
      static Quat rotY( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( float4( 0.0f, s, 0.0f, c ) );
      }

      // make quaternion for rotation around z axis
      static Quat rotZ( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( float4( 0.0f, 0.0f, s, c ) );
      }

      // rotZ ^ rotX ^ rotY
      static Quat rotZYX( float z, float y, float x )
      {
        float sx, cx, sy, cy, sz, cz;

        Math::sincos( x * 0.5f, &sx, &cx );
        Math::sincos( y * 0.5f, &sy, &cy );
        Math::sincos( z * 0.5f, &sz, &cz );

        float cxcy = cx * cy;
        float cxsy = cx * sy;
        float sxcy = sx * cy;
        float sxsy = sx * sy;

        return Quat( sxcy * cz - cxsy * sz,
                     cxsy * cz + sxcy * sz,
                     cxcy * sz + sxsy * cz,
                     cxcy * cz - sxsy * sz );
      }

  };

#else

  class Quat : public Vec3
  {
    public:

      static const Quat ZERO;
      static const Quat ID;

      float w;

      OZ_ALWAYS_INLINE
      explicit Quat()
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( float x_, float y_, float z_, float w_ ) : Vec3( x_, y_, z_ ), w( w_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const Vec3& v, float w_ ) : Vec3( v ), w( w_ )
      {}

      OZ_ALWAYS_INLINE
      explicit Quat( const float* q ) : Vec3( q[0], q[1], q[2] ), w( q[3] )
      {}

      OZ_ALWAYS_INLINE
      bool operator == ( const Quat& q ) const
      {
        return x == q.x && y == q.y && z == q.z && w == q.w;
      }

      OZ_ALWAYS_INLINE
      bool operator != ( const Quat& q ) const
      {
        return x != q.x || y != q.y || z != q.z || w != q.w;
      }

      OZ_ALWAYS_INLINE
      operator const float* () const
      {
        return &x;
      }

      OZ_ALWAYS_INLINE
      operator float* ()
      {
        return &x;
      }

      OZ_ALWAYS_INLINE
      const float& operator [] ( int i ) const
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      float& operator [] ( int i )
      {
        hard_assert( 0 <= i && i < 4 );

        return ( &x )[i];
      }

      OZ_ALWAYS_INLINE
      Quat abs() const
      {
        return Quat( Math::abs( x ), Math::abs( y ), Math::abs( z ), Math::abs( w ) );
      }

      OZ_ALWAYS_INLINE
      Quat operator * () const
      {
        return Quat( -x, -y, -z, w );
      }

      OZ_ALWAYS_INLINE
      float operator ! () const
      {
        return Math::sqrt( x*x + y*y + z*z + w*w );
      }

      OZ_ALWAYS_INLINE
      float sqL() const
      {
        return x*x + y*y + z*z + w*w;
      }

      OZ_ALWAYS_INLINE
      bool isUnit() const
      {
        return x*x + y*y + z*z + w*w == 1.0f;
      }

      OZ_ALWAYS_INLINE
      Quat operator ~ () const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat fastUnit() const
      {
        hard_assert( x*x + y*y + z*z + w*w > 0.0f );

        float k = Math::fastInvSqrt( x*x + y*y + z*z + w*w );
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat operator + () const
      {
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat operator - () const
      {
        return Quat( -x, -y, -z, -w );
      }

      OZ_ALWAYS_INLINE
      Quat operator + ( const Quat& q ) const
      {
        return Quat( x + q.x, y + q.y, z + q.z, w + q.w );
      }

      OZ_ALWAYS_INLINE
      Quat operator - ( const Quat& q ) const
      {
        return Quat( x - q.x, y - q.y, z - q.z, w - q.w );
      }

      OZ_ALWAYS_INLINE
      Quat operator * ( float k ) const
      {
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      friend Quat operator * ( float k, const Quat& q )
      {
        return Quat( q.x * k, q.y * k, q.z * k, q.w * k );
      }

      OZ_ALWAYS_INLINE
      Quat operator / ( float k ) const
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        return Quat( x * k, y * k, z * k, w * k );
      }

      OZ_ALWAYS_INLINE
      Quat& operator += ( const Quat& q )
      {
        x += q.x;
        y += q.y;
        z += q.z;
        w += q.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator -= ( const Quat& q )
      {
        x -= q.x;
        y -= q.y;
        z -= q.z;
        w -= q.w;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator *= ( float k )
      {
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      OZ_ALWAYS_INLINE
      Quat& operator /= ( float k )
      {
        hard_assert( k != 0.0f );

        k = 1.0f / k;
        x *= k;
        y *= k;
        z *= k;
        w *= k;
        return *this;
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat operator ^ ( const Quat& a ) const
      {

        return Quat( w*a.x + x*a.w + y*a.z - z*a.y,
                     w*a.y + y*a.w + z*a.x - x*a.z,
                     w*a.z + z*a.w + x*a.y - y*a.x,
                     w*a.w - x*a.x - y*a.y - z*a.z );
      }

      // quaternion multiplication
      OZ_ALWAYS_INLINE
      Quat& operator ^= ( const Quat& a )
      {
        float tx = x, ty = y, tz = z;

        x = w*a.x + tx*a.w + ty*a.z - tz*a.y;
        y = w*a.y + ty*a.w + tz*a.x - tx*a.z;
        z = w*a.z + tz*a.w + tx*a.y - ty*a.x;
        w = w*a.w - tx*a.x - ty*a.y - tz*a.z;

        return *this;
      }

      // make rotation matrix (implemented in Mat33.hpp)
      Mat33 rotMat33() const;

      // make transposed (inverse) rotation matrix (implemented in Mat33.hpp)
      Mat33 invRotMat33() const;

      // make rotation matrix (implemented in Mat44.hpp)
      Mat44 rotMat44() const;

      // make transposed (inverse) rotation matrix (implemented in Mat44.hpp)
      Mat44 invRotMat44() const;

      // make quaternion for rotation around given axis
      static Quat rotAxis( const Vec3& axis, float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( s * axis.x, s * axis.y, s * axis.z, c );
      }

      // make quaternion for rotation around x axis
      static Quat rotX( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( s, 0.0f, 0.0f, c );
      }

      // make quaternion for rotation around y axis
      static Quat rotY( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( 0.0f, s, 0.0f, c );
      }

      // make quaternion for rotation around z axis
      static Quat rotZ( float theta )
      {
        float s, c;
        Math::sincos( theta * 0.5f, &s, &c );
        return Quat( 0.0f, 0.0f, s, c );
      }

      // rotZ ^ rotX ^ rotY
      static Quat rotZYX( float z, float y, float x )
      {
        float sx, cx, sy, cy, sz, cz;

        Math::sincos( x * 0.5f, &sx, &cx );
        Math::sincos( y * 0.5f, &sy, &cy );
        Math::sincos( z * 0.5f, &sz, &cz );

        float cxcy = cx * cy;
        float cxsy = cx * sy;
        float sxcy = sx * cy;
        float sxsy = sx * sy;

        return Quat( sxcy * cz - cxsy * sz,
                     cxsy * cz + sxcy * sz,
                     cxcy * sz + sxsy * cz,
                     cxcy * cz - sxsy * sz );
      }

  };

#endif

}
