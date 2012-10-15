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

#include <oz/oz.hh>

#include <limits.h>

using namespace oz;

class Foo
{
  public:

    int  value;
    Foo* next[1];
    Foo* prev[1];

    explicit Foo() :
      value( 0 )
    {
      Log() << "Foo()\n";
    }

    Foo( int value_ ) :
      value( value_ )
    {
      Log() << "Foo( " << value_ << " )\n";
    }

    ~Foo()
    {
      Log() << "~Foo()\n";
    }

    Foo( const Foo& f ) :
      value( f.value )
    {
      Log() << "Foo( const Foo& )\n";
    }

    Foo( Foo&& f ) :
      value( f.value )
    {
      Log() << "Foo( Foo&& )\n";

      f.value = 0;
    }

    Foo& operator = ( const Foo& f )
    {
      Log() << "Foo& operator = ( const Foo& )\n";

      if( &f == this ) {
        return *this;
      }

      value = f.value;
      return *this;
    }

    Foo& operator = ( Foo&& f )
    {
      Log() << "Foo& operator = ( Foo&& )\n";

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

    void foo() const;

};

int main()
{
  System::init();
  return 0;
}
