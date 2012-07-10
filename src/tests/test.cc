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

#include <cstdio>

using namespace oz;

class Foo
{
  private:

    int p;

  public:

    struct Bar
    {
      void bar( Foo f )
      {
        printf( "%d\n", f.p );
      }
    };

    const char* s = "";

    explicit Foo( const char* s_ ) :
      s( s_ )
    {
      Log::out << "Foo(" << s << ")\n";
    }

    Foo()
    {
      Log::out << "Foo()\n";
    }

    ~Foo()
    {
      Log::out << "~Foo(" << s << ")\n";
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

};

int main()
{
  System::init();

  SList<int, 8> l;
  l.add();
  Log::out << l[0] << "\n";
  l.clear();
  l.add();
  Log::out << l[0] << "\n";

  Alloc::printLeaks();
  return 0;
}
