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
 * @file tests/test.cc
 */

#include "oz/oz.hh"
#include "oz/windefs.h"

#include <cstdio>

using namespace oz;

struct Foo
{
  Foo()
  {
    printf( "Foo()\n" );
  }

  virtual ~Foo()
  {
    printf( "~Foo()\n" );
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

  virtual void bar()
  {}
};

struct Bar : Foo
{
  void bar() override;
};

int main()
{
  Pair<int> p;

  printf( "%d\n", noexcept( p.x = p.y ) );
  return 0;
}
