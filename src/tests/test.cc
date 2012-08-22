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
#include <fcntl.h>

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

using namespace oz;
using namespace std;

class Foo
{
  public:

    int value;

    Foo() :
      value( 0 )
    {
      Log::out << "Foo()\n";
    }

    ~Foo()
    {
      Log::out << "~Foo()\n";
    }

    Foo( const Foo& f ) :
      value( f.value )
    {
      Log::out << "Foo( const Foo& )\n";
    }

    Foo( Foo&& f ) :
      value( f.value )
    {
      Log::out << "Foo( Foo&& )\n";

      f.value = 0;
    }

    explicit Foo( int value_ ) :
      value( value_ )
    {
      Log::out << "Foo( " << value_ << " )\n";
    }

    Foo& operator = ( const Foo& f )
    {
      Log::out << "Foo& operator = ( const Foo& )\n";

      if( &f == this ) {
        return *this;
      }

      value = f.value;
      return *this;
    }

    Foo& operator = ( Foo&& f )
    {
      Log::out << "Foo& operator = ( Foo&& )\n";

      if( &f == this ) {
        return *this;
      }

      value = f.value;
      f.value = 0;
      return *this;
    }

    bool operator == ( const Foo& f ) const
    {
      return value == f.value;
    }

    bool operator != ( const Foo& f ) const
    {
      return value != f.value;
    }

    bool operator < ( const Foo& f ) const
    {
      return value < f.value;
    }

};

int main()
{
  System::init();

  int a[] = { 1, 2 };
  int b[] = { 3, 4 };

  iSwap( iter( a ), iter( b ) );
  aSwap( a, b );
  aSwap( a, b, 2 );

  for( auto& i : a ) {
    Log::out << i << " :: ";
  }
  Log::out << "\n";

  for( auto& i : b ) {
    Log::out << i << " :: ";
  }
  Log::out << "\n";
  return 0;
}
