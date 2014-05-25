/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file tests/containers.cc
 */

#include <ozCore/ozCore.hh>

using namespace oz;

struct Foo
{
  int number;

  Foo* next[1];
  Foo* prev[1];

  Foo( int n = 0 ) :
    number( n )
  {
    Log() << "Foo( int ) : " << number;
  }

  ~Foo()
  {
    Log() << "~Foo() : " << number;
  }

  Foo( const Foo& f ) :
    number( f.number )
  {
    Log() << "Foo( const Foo& ) : " << number;
  }

  Foo( Foo&& f ) :
    number( f.number )
  {
    Log() << "Foo( Foo&& ) : " << number;
  }

  Foo& operator = ( const Foo& f )
  {
    Log() << "Foo& operator = ( const Foo& ) : " << number << " := " << f.number;
    number = f.number;
    return *this;
  }

  Foo& operator = ( Foo&& f )
  {
    Log() << "Foo& operator = ( Foo&& ) : " << number << " := " << f.number;
    number = f.number;
    return *this;
  }

  bool operator == ( const Foo& f ) const
  {
    Log() << "bool operator == ( const Foo& ) : " << number << " == " << f.number;
    return number == f.number;
  }

  bool operator < ( const Foo& f ) const
  {
    Log() << "bool operator == ( const Foo& ) : " << number << " < " << f.number;
    return number < f.number;
  }

  friend int hash( const Foo& f )
  {
    return f.number;
  }
};

template <typename Type>
inline Type operator | ( const JSON& json, const Type& defaultValue )
{
  return json.get( defaultValue );
}

template <class Iterator, typename Predicate>
inline bool iAny( Iterator iter, Predicate predicate )
{
  while( iter.isValid() ) {
    if( predicate( *iter ) ) {
      return true;
    }
    ++iter;
  }
  return false;
}

template <class Iterator, typename Predicate>
inline bool iAll( Iterator iter, Predicate predicate )
{
  while( iter.isValid() ) {
    if( !predicate( *iter ) ) {
      return false;
    }
    ++iter;
  }
  return true;
}

template <class Iterator, typename Predicate>
inline int iCount( Iterator iter, Predicate predicate )
{
  int count = 0;

  while( iter.isValid() ) {
    count += predicate( *iter );
    ++iter;
  }
  return count;
}

template <class Iterator, typename Method>
inline void iMap( Iterator iter, Method method )
{
  while( iter.isValid() ) {
    method( *iter );
    ++iter;
  }
}

int main()
{
  System::init();
  return 0;
}
