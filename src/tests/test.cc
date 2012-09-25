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

using namespace oz;

class Foo
{
  public:

    int  value;
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

struct Elem
{
  int   value;
  Elem* next[1];
};

template <typename IteratorA, typename IteratorB = IteratorA>
inline IteratorB copy( IteratorA first, IteratorA past, IteratorB dest )
{
  for( ; first != past; ++first, ++dest ) {
    *dest = *first;
  }
}

template <typename IteratorA, typename IteratorB = IteratorA>
inline IteratorB move( IteratorA first, IteratorA past, IteratorB dest )
{
  for( ; first != past; ++first, ++dest ) {
    *dest = static_cast<typename IteratorA::ElemType&&>( *first );
  }
}

template <typename ElemA, typename ElemB = ElemA>
inline ElemB* move( ElemA* first, ElemA* past, ElemB* dest )
{
  for( ; first != past; ++first, ++dest ) {
    *dest = static_cast<ElemB&&>( *first );
  }
}

int main()
{
  System::init();

  Map<int> m;
  m.add( 1 );
  m.add( 2 );

  Time::sleep( 3000 );
  m.dealloc();

  return 0;
}
