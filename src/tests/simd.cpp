/*
 *  simd.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <stdio.h>
#include <time.h>

using namespace oz;

#if 1
struct __attribute__(( aligned( 16 ) )) VecX
{
  union {
    float4 f4;
    struct {
      float x, y, z, w;
    };
  };

  VecX() {}
  VecX( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ ) {}
  VecX( float x_, float y_, float z_, float w_ ) : x( x_ ), y( y_ ), z( z_ ), w( w_ ) {}

  VecX operator + ( const VecX& v ) const
  {
    return VecX( x + v.x, y + v.y, z + v.z, w + v.w );
//     return VecX( x + v.x, y + v.y, z + v.z );
  }

  VecX& operator += ( const VecX& v )
  {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }

  VecX operator - ( const VecX& v ) const
  {
    return VecX( x - v.x, y - v.y, z - v.z, w - v.w );
//     return VecX( x - v.x, y - v.y, z - v.z );
  }

  VecX operator * ( float k ) const
  {
    return VecX( x * k, y * k, z * k, w * k );
//     return VecX( x * k, y * k, z * k );
  }

  float operator * ( const VecX& v ) const
  {
//     return x*v.x + y*v.y + z*v.z + w*v.w;
    return x*v.x + y*v.y + z*v.z;
  }
};
#elif 0
struct __attribute__(( aligned( 16 ) )) VecX
{
  union {
    float4 f4;
    struct {
      float x, y, z, w;
    };
  };

  VecX() {}
  VecX( float x_, float y_, float z_ ) : f4( (float4) { x_, y_, z_, 0.0f } ) {}
  VecX( float4 f4_ ) : f4( f4_ ) {}

  VecX operator + ( const VecX& v ) const
  {
    return VecX( f4 + v.f4 );
  }

  VecX& operator += ( const VecX& v )
  {
    f4 += v.f4;
    return *this;
  }

  VecX operator - ( const VecX& v ) const
  {
    return VecX( f4 - v.f4 );
  }

  VecX operator * ( float k ) const
  {
    return VecX( f4 * (float4) { k, k, k, k } );
  }

  float operator * ( const VecX& v ) const
  {
//     VecX u = f4 * v.f4;
//     return u.x + u.y + u.z;
    return x*v.x + y*v.y + z*v.z;
  }
};
#else
# define VecX Vec4
#endif

static const int MAX = 10000;

VecX a[MAX];
VecX b[MAX];
VecX c[MAX];
VecX d[MAX];
VecX e[MAX];

int main( int, char** )
{
  for( int i = 0; i < MAX; ++i ) {
    a[i] = VecX( Math::frand(), Math::frand(), Math::frand() ) * 100.0f;
    b[i] = VecX( Math::frand(), Math::frand(), Math::frand() ) * 100.0f;
    c[i] = VecX( Math::frand(), Math::frand(), Math::frand() ) * 100.0f;
    d[i] = VecX( Math::frand(), Math::frand(), Math::frand() ) * 100.0f;
  }
  long t0 = clock();
  for( int k = 0; k < 10000; ++k ) {
    for( int i = 0; i < MAX; ++i ) {
      d[i] += c[i] * ( a[i] * b[i] );
      e[i] = d[i] + c[i] - ( a[i] + b[i] );
    }
  }
  Quat q = Quat::ZERO;
  q *= 10;
  printf( "%g\n", float( clock() - t0 ) / float( CLOCKS_PER_SEC ) );
  return 0;
}
