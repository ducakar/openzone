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

#include "oz/Vector.hpp"

#include <cstdio>
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

int main( int, char** )
{
  std::locale l = std::locale( "sl_SI.UTF-8" );
//   l = l.combine< std::numpunct<char> >( std::locale() );
  std::locale::global( l );
  std::cout.imbue( l );
  printf( "%g\n", 1000.42 );
  return 0;
}
