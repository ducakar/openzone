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
#include <ozDynamics/ozDynamics.hh>

#include <clocale>
#include <climits>

using namespace oz;

void bar();

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
      bar();
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

static void testParseFloat()
{
  for( int i = 0; i < 1000000; ++i ) {
    float x = Math::rand() * 1.0e38f;

    if( i == 0 ) {
      x = 0.0f;
    }
    else if( i == 1 ) {
      x = Math::INF;
    }
    else if( i == 2 ) {
      x = -Math::INF;
    }
    else if( i == 3 ) {
      x = Math::NaN;
    }

    String s = String( x, 9 );
    float  y = String::parseFloat( s );
    String t = String::str( "%.9g", x ).replace( ',', '.' ); // Replace decimal ',' with '.'.
    float  z = String::parseFloat( t );

    if( Math::isNaN( x ) && Math::isNaN( y ) && Math::isNaN( z ) ) {
      continue;
    }
    if( x != y ) {
      Log::println( "%.9g -> %s -> %.9g  delta: %.9g", x, s.cstr(), y, y - x );
    }
    if( x != z ) {
      Log::println( "%.9g -> printf -> %.9g  delta: %.9g", x, z, z - x );
    }
  }
#if 0
  for( int i = 0; i < 100; ++i ) {
    float  x = Math::rand() * 3.4e38f;
    String s = String( x, 9 );
    float  y = String::parseFloat( s );
    String t = String::str( "%.9g", x ).replace( ',', '.' ); // Replace decimal ',' with '.'.
    float  z = String::parseFloat( t );

    if( x != y ) {
      Log::println( "%.9g -> %s -> %.9g  delta: %.9g", x, s.cstr(), y, y - x );
    }
    if( x != z ) {
      Log::println( "%.9g -> printf -> %.9g  delta: %.9g", x, z, z - x );
    }
  }
#endif
}

int main()
{
  System::init();
  testParseFloat();
  return 0;
}
