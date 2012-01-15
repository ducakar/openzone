/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file tests/simd.cc
 */

#include "oz/oz.hh"

#include <cstdio>

typedef float __attribute__(( vector_size( 16 ) )) float4;

using namespace oz;

#if 0
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
# define VecX Vec3
#endif

static const int MAX = 10000;

VecX a[MAX];
VecX b[MAX];
VecX c[MAX];
VecX d[MAX];
VecX e[MAX];

int main()
{
  for( int i = 0; i < MAX; ++i ) {
    a[i] = VecX( Math::rand(), Math::rand(), Math::rand() ) * 100.0f;
    b[i] = VecX( Math::rand(), Math::rand(), Math::rand() ) * 100.0f;
    c[i] = VecX( Math::rand(), Math::rand(), Math::rand() ) * 100.0f;
    d[i] = VecX( Math::rand(), Math::rand(), Math::rand() ) * 100.0f;
  }
  long64 t0 = Time::clock();
  for( int k = 0; k < 10000; ++k ) {
    for( int i = 0; i < MAX; ++i ) {
      d[i] += c[i] * ( a[i] * b[i] );
      e[i] = d[i] + c[i] - ( a[i] + b[i] );
    }
  }
  Quat q = Quat::ZERO;
  q *= 10;
  printf( "%g\n", float( Time::clock() - t0 ) / 1000.0f );

  Alloc::printLeaks();
  return 0;
}
