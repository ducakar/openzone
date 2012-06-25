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

using namespace oz;

struct Foo
{
  Foo()
  {
    Log::out << "Foo()\n";
  }

  ~Foo()
  {
    Log::out << "~Foo()\n";
  }

  Foo( const Foo& )
  {
    Log::out << "Foo( const Foo& )\n";
  }

  Foo( Foo&& )
  {
    Log::out << "Foo( Foo&& )\n";
  }

  Foo& operator = ( const Foo& )
  {
    Log::out << "Foo& operator = ( const Foo& )\n";
    return *this;
  }

  Foo& operator = ( Foo&& )
  {
    Log::out << "Foo& operator = ( Foo&& )\n";
    return *this;
  }

  void foo() const
  {
    Log::out << "Foo::foo()\n";
  };

  static Foo bar( bool b )
  {
    Foo f;

    if( b ) {
      return f;
    }
    else {
      return f;
    }
  }
};

int main()
{
  return 0;
}
