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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file tests/test.cpp
 */

#include "stable.hpp"

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

// DArray<Foo> bar()
// {
//   DArray<Foo> foos( 10 );
//   return foos;
// }

void test1( const Vector<int>& v )
{
  for( auto i = v.citer(); i.isValid(); ++i ) {
    printf( "%d\n", *i );
  }
}

void test2( const Vector<int>& v )
{
  for( const auto& i : v.citer() ) {
    printf( "%d\n", *i );
  }
}

void test3( const Vector<int>& v )
{
  auto container = v.citer();
  for( auto __begin = begin( container ), __end = end( container );
      __begin != __end;
      ++__begin )
  {
    auto i = *__begin;
    {
      printf( "%d\n", *i );
    }
  }
}

int main( int, char** )
{
  Vector<int> v;
  v.add( 10 );
  v.add( 12 );

  test1( v );
  test2( v );

//   Alloc::printLeaks();
  return 0;
}
