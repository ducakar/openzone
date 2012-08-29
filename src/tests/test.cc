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

#include <fcntl.h>

#include <clocale>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <vector>

using namespace oz;
using namespace std;

class Foo
{
  public:

    int value;
    Foo* next[1];
    Foo* prev[1];

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

    Foo( int value_ ) :
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

    void foo() const;

};

#undef foreach
/*
#define foreach( decl, container ) \
  for( auto _iter = container.begin(), _endIter = container.end(); _iter != _endIter; ++_iter ) \
    for( decl = *_iter; ; __extension__ ({ break; }) )
*/
#define foreach( decl, iterator ) \
  for( auto _iter = iterator; _iter.isValid(); ++_iter ) \
    for( decl = *_iter; ; __extension__({ break; }) )

int main()
{
  System::init();

  Map<Foo, nil_t> c;
  c.add( 1 );
  c.add( 2 );
  c.add( 0 );

  std::vector<Foo> v;

  foreach( auto& j, c.citer() ) {
    Log::out << j.key.value << "\n";
  }
//  foreach( const Foo& i, c ) {
//    Log::out << i.value << "\n";
//  }
  return 0;
}
