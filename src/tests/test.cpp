/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file tests/test.cpp
 */

#include "stable.hpp"

#include <cstdio>
#include <cstring>
#include <locale>
#include <iostream>

using namespace oz;

struct Foo
{
  Foo()
  {
    printf( "Foo()\n" );
  }

  Foo( const Foo& )
  {
    printf( "Foo( const Foo& )\n" );
  }

  Foo( Foo&& )
  {
    printf( "Foo( Foo&& )\n" );
  }

  Foo& operator = ( const Foo& )
  {
    printf( "Foo& operator = ( const Foo& )\n" );
    return *this;
  }

  Foo& operator = ( Foo&& )
  {
    printf( "Foo& operator = ( Foo&& )\n" );
    return *this;
  }
};

struct VecX
{
  float x;
  float y;
  float z;

  VecX() = default;

  explicit VecX( float x_, float y_, float z_ ) : x( x_ ), y( y_ ), z( z_ )
  {}

  VecX operator + ( const VecX& v )
  {
    return VecX( x + v.x, y + v.y, z + v.z );
  }
};

int main( int, char** )
{
  VecX* v1 = new VecX[10000];
  VecX* v2 = new VecX[10000];

  for( int i = 0; i < 10000; ++i ) {
    v1[i] = v1[i] + v2[i];
  }
  return 0;
}
