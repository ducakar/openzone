/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>
#include <ozFactory/ozFactory.hh>

using namespace oz;

struct Foo
{
  Foo()
  {
    Log() << "Foo()";
  }

  ~Foo()
  {
    Log() << "~Foo()";
  }

  Foo( const Foo& )
  {
    Log() << "Foo( const Foo& )";
  }

  Foo( Foo&& )
  {
    Log() << "Foo( Foo&& )";
  }

  Foo& operator = ( const Foo& )
  {
    Log() << "Foo& operator = ( const Foo& )";
    return *this;
  }

  Foo& operator = ( Foo&& )
  {
    Log() << "Foo& operator = ( Foo&& )";
    return *this;
  }
};

int main()
{
  System::init();

  HashMap<String, int> hm = {
    { "Drekec", 1 },
    { "Pekec", 2 },
    { "Pizda", 42 }
  };

  hm.add( "Jajc", 0 );
  hm.add( "Pizda", 314 );

  for( HashMap<String, int>::Pair e : hm ) {
    Log() << e.key << " :: " << e.value;
  }
  return 0;
}
