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

  OutputStream os( 0 );

  SBitset<43> bs;
  bs.clearAll();

  Log() << bs.length();
  Log() << bs.unitLength();

  bs.set( 42 );
  os.writeBitset( bs, bs.length() );
  bs.clearAll();

  os.seek( 0 );
  os.readBitset( bs, 43 );

  for( int i = 0; i < bs.length(); ++i ) {
    if( bs.get( i ) ) {
      Log() << i;
    }
  }
  return 0;
}
