/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file tests/unittest.cpp
 */

#include "stable.hpp"

#include <iostream>
#include <SDL/SDL_main.h>

#ifdef NDEBUG
# warning NDEBUG should not be defined. Unittest needs assert macro enabled to function properly.
#endif

#define OZ_UNITTEST( name ) \
  oz::name##Unittest(); \
  hard_assert( oz::constructCount == 0 ); \
  cout << #name " Passed" << endl;

using namespace std;

namespace oz
{

  bool Alloc::isLocked = true;

  static int constructCount = 0;

  struct Test
  {
    // on constructed objects, magic should be 42, otherwise some random number (hope not 42) or -1
    // on destructed objects
    int magic;
    int value;

    Test* prev[1];
    Test* next[1];

    Test()
    {
      hard_assert( magic != 42 );

      magic = 42;
      ++constructCount;
    }

    Test( const Test& t )
    {
      hard_assert( magic != 42 );
      hard_assert( t.magic == 42 );

      magic = 42;
      value = t.value;
      ++constructCount;
    }

    Test& operator = ( const Test& t )
    {
      hard_assert( magic == 42 );
      hard_assert( t.magic == 42 );

      value = t.value;
      return *this;
    }

    ~Test()
    {
      hard_assert( magic == 42 );

      magic = -1;
      --constructCount;
    }

    explicit Test( int value_ )
    {
      hard_assert( magic != 42 );

      magic = 42;
      value = value_;
      ++constructCount;
    }

    bool operator == ( const Test& t ) const
    {
      hard_assert( magic == 42 );
      hard_assert( t.magic == 42 );

      return value == t.value;
    }

    bool operator < ( const Test& t ) const
    {
      hard_assert( magic == 42 );
      hard_assert( t.magic == 42 );

      return value < t.value;
    }
  };

  static void ozCommonUnittest()
  {
    Test a( 42 ), b( 43 ), c( 44 ), d( 43 );

    hard_assert( &min( a, b ) == &a );
    hard_assert( &min( b, d ) == &b );
    hard_assert( &min( d, b ) == &d );
    hard_assert( &min( b, a ) == &a );

    hard_assert( &max( a, b ) == &b );
    hard_assert( &max( b, a ) == &b );
    hard_assert( &max( b, d ) == &b );
    hard_assert( &max( d, b ) == &d );

    hard_assert( &clamp( a, b, c ) == &b );
    hard_assert( &clamp( b, a, c ) == &b );
    hard_assert( &clamp( c, a, b ) == &b );
    hard_assert( &clamp( c, b, d ) == &d );
    hard_assert( &clamp( a, b, d ) == &b );
  }

  static void ozIterablesUnittest()
  {
    DList<Test> l;
    Vector<Test> v;

    l.add( new Test( 5 ) );
    l.add( new Test( 3 ) );
    l.add( new Test( 4 ) );
    l.add( new Test( 3 ) );
    l.add( new Test( -1 ) );

    v.add( Test( -1 ) );
    v.add( Test( 3 ) );
    v.add( Test( 4 ) );
    v.add( Test( 3 ) );
    v.add( Test( 5 ) );

    hard_assert( iContains( l.citer(), Test( -1 ) ) );
    hard_assert( iContains( v.citer(), Test( -1 ) ) );
    hard_assert( !iContains( l.citer(), Test( 0 ) ) );
    hard_assert( !iContains( v.citer(), Test( 0 ) ) );
    hard_assert( iContains( l.citer(), Test( 3 ) ) );
    hard_assert( iContains( v.citer(), Test( 3 ) ) );
    hard_assert( iContains( l.citer(), Test( 4 ) ) );
    hard_assert( iContains( v.citer(), Test( 4 ) ) );
    hard_assert( iContains( l.citer(), Test( 5 ) ) );
    hard_assert( iContains( v.citer(), Test( 5 ) ) );

    auto li = l.citer();
    auto vi = v.citer();

    hard_assert( iFind( l.citer(), Test( -1 ) ) == li );
    hard_assert( iFind( v.citer(), Test( -1 ) ) == vi );

    ++li;
    ++vi;
    hard_assert( iFind( l.citer(), Test( 3 ) ) == li );
    hard_assert( iFind( v.citer(), Test( 3 ) ) == vi );

    ++li;
    ++vi;
    hard_assert( iFind( l.citer(), Test( 4 ) ) == li );
    hard_assert( iFind( v.citer(), Test( 4 ) ) == vi );

    ++li;
    ++vi;
    ++li;
    ++vi;
    hard_assert( iFind( l.citer(), Test( 5 ) ) == li );
    hard_assert( iFind( v.citer(), Test( 5 ) ) == vi );

    li = l.citer();
    vi = v.citer();

    hard_assert( iFindLast( l.citer(), Test( -1 ) ) == li );
    hard_assert( iFindLast( v.citer(), Test( -1 ) ) == vi );

    ++li;
    ++vi;
    ++li;
    ++vi;
    hard_assert( iFindLast( l.citer(), Test( 4 ) ) == li );
    hard_assert( iFindLast( v.citer(), Test( 4 ) ) == vi );

    ++li;
    ++vi;
    hard_assert( iFindLast( l.citer(), Test( 3 ) ) == li );
    hard_assert( iFindLast( v.citer(), Test( 3 ) ) == vi );

    ++li;
    ++vi;
    hard_assert( iFindLast( l.citer(), Test( 5 ) ) == li );
    hard_assert( iFindLast( v.citer(), Test( 5 ) ) == vi );

    typedef decltype( li ) LI;
    typedef decltype( vi ) VI;

    li = LI();
    vi = VI();

    hard_assert( iFindLast( l.citer(), Test( 0 ) ) == li );
    hard_assert( iFindLast( v.citer(), Test( 0 ) ) == vi );

    hard_assert( iIndex( l.citer(), Test( -1 ) ) == 0 );
    hard_assert( iIndex( v.citer(), Test( -1 ) ) == 0 );
    hard_assert( iIndex( l.citer(), Test( 3 ) ) == 1 );
    hard_assert( iIndex( v.citer(), Test( 3 ) ) == 1 );
    hard_assert( iIndex( l.citer(), Test( 4 ) ) == 2 );
    hard_assert( iIndex( v.citer(), Test( 4 ) ) == 2 );
    hard_assert( iIndex( l.citer(), Test( 5 ) ) == 4 );
    hard_assert( iIndex( v.citer(), Test( 5 ) ) == 4 );

    hard_assert( iLastIndex( l.citer(), Test( -1 ) ) == 0 );
    hard_assert( iLastIndex( v.citer(), Test( -1 ) ) == 0 );
    hard_assert( iLastIndex( l.citer(), Test( 3 ) ) == 3 );
    hard_assert( iLastIndex( v.citer(), Test( 3 ) ) == 3 );
    hard_assert( iLastIndex( l.citer(), Test( 4 ) ) == 2 );
    hard_assert( iLastIndex( v.citer(), Test( 4 ) ) == 2 );
    hard_assert( iLastIndex( l.citer(), Test( 5 ) ) == 4 );
    hard_assert( iLastIndex( v.citer(), Test( 5 ) ) == 4 );

    hard_assert( iEquals( l.citer(), v.citer() ) );
    l.add( new Test( 0 ) );
    hard_assert( !iEquals( l.citer(), v.citer() ) );
    v.pushFirst( Test( 0 ) );
    hard_assert( iEquals( l.citer(), v.citer() ) );

    iSet( v.iter(), Test( 12 ) );
    hard_assert( !iEquals( v.citer(), l.citer() ) );

    iCopy( l.iter(), v.citer() );
    hard_assert( iEquals( l.citer(), v.citer() ) );

    Vector<Test*> pv;
    pv.add( new Test() );
    pv.add( new Test() );

    iFree( pv.iter() );
    l.free();
  }

  static void ozArraysUnittest()
  {
    Test a[5];
    Test b[5];

    a[0] = Test( -1 );
    a[1] = Test( 3 );
    a[2] = Test( 4 );
    a[3] = Test( 3 );
    a[4] = Test( 5 );

    b[0] = Test( -1 );
    b[1] = Test( 3 );
    b[2] = Test( 4 );
    b[3] = Test( 3 );
    b[4] = Test( 5 );

    hard_assert( aContains( a, Test( -1 ), 5 ) );
    hard_assert( aContains( b, Test( -1 ), 5 ) );
    hard_assert( !aContains( a, Test( 0 ), 5 ) );
    hard_assert( !aContains( b, Test( 0 ), 5 ) );
    hard_assert( aContains( a, Test( 3 ), 5 ) );
    hard_assert( aContains( b, Test( 3 ), 5 ) );
    hard_assert( aContains( a, Test( 4 ), 5 ) );
    hard_assert( aContains( b, Test( 4 ), 5 ) );
    hard_assert( aContains( a, Test( 5 ), 5 ) );
    hard_assert( aContains( b, Test( 5 ), 5 ) );

    hard_assert( aIndex( a, Test( -1 ), 5 ) == 0 );
    hard_assert( aIndex( a, Test( 3 ), 5 ) == 1 );
    hard_assert( aIndex( a, Test( 4 ), 5 ) == 2 );
    hard_assert( aIndex( a, Test( 5 ), 5 ) == 4 );

    hard_assert( aLastIndex( a, Test( -1 ), 5 ) == 0 );
    hard_assert( aLastIndex( a, Test( 3 ), 5 ) == 3 );
    hard_assert( aLastIndex( a, Test( 4 ), 5 ) == 2 );
    hard_assert( aLastIndex( a, Test( 5 ), 5 ) == 4 );

    hard_assert( aEquals( a, b, 5 ) );
    a[0] = Test( 0 );
    hard_assert( !aEquals( b, a, 5 ) );

    aCopy( a, b, 5 );
    hard_assert( aEquals( a, b, 5 ) );
    a[0] = Test( 0 );
    aReverseCopy( a, b, 5 );
    hard_assert( aEquals( a, b, 5 ) );
    aCopy( a, a + 1, 4 );
    hard_assert( a[0] == Test( 3 ) && a[1] == Test( 4 ) && a[2] == Test( 3 ) && a[3] == Test( 5 ) );
    aReverseCopy( b + 1, b, 4 );
    hard_assert( b[1] == Test( -1 ) && b[2] == Test( 3 ) && b[3] == Test( 4 ) && b[4] == Test( 3 ) );

    aRemove( a, 3, 4 );
    aRemove( a, 1, 3 );
    aRemove( b, 0, 5 );

    hard_assert( a[0] == Test( 3 ) && a[1] == Test( 3 ) );
    hard_assert( b[0] == Test( -1 ) && b[1] == Test( 3 ) && b[2] == Test( 4 ) && b[3] == Test( 3 ) );

    aSet( a, Test( 12 ), 2 );
    aSet( b, Test( 12 ), 5 );
    hard_assert( aEquals( b, a, 2 ) );
    foreach( i, citer( b, 5 ) ) {
      hard_assert( *i == Test( 12 ) );
    }

    Test** c = new Test*[5];
    foreach( i, iter( c, 5 ) ) {
      *i = new Test();
    }
    aFree( c, 5 );
    delete[] c;

    hard_assert( aLength( a ) == 5 );

    Test* d = new Test[5];
    aCopy( d, b, 5 );
    d = aRealloc( d, 5, 10 );
    hard_assert( aEquals( b, d, 5 ) );
    delete[] d;

    int r[1000];
    for( int i = 0; i < 1000; ++i ) {
      r[i] = Math::rand( 1000 );
    }
    aSort( r, 1000 );

    for( int i = -1; i <= 1000; ++i ) {
      int findIndex = aBisectFind( r, i, 1000 );
      int positionIndex = aBisectPosition( r, i, 1000 );

      if( 1 <= i && i < 1000 ) {
        hard_assert( r[i - 1] <= r[i] );
      }

      hard_assert( findIndex == -1 || r[findIndex] == i );
      hard_assert( ( positionIndex == 0 && r[1] > i ) || positionIndex == 1000 ||
              ( r[positionIndex - 1] <= i && r[positionIndex] > i ) );
    }
  }

  static void ozAllocUnittest()
  {
    static const size_t STAT_META_SIZE = Alloc::ALIGNMENT;

    hard_assert( Alloc::amount == 0 );
    Test* array = new Test[10];
    hard_assert( Alloc::amount >= 10 * sizeof( Test ) + STAT_META_SIZE );
    hard_assert( Alloc::sumAmount >= 10 * sizeof( Test ) + STAT_META_SIZE );
    hard_assert( Alloc::count == 1 );
    hard_assert( Alloc::sumCount == 1 );

    array = aRealloc( array, 5, 8 );
    hard_assert( Alloc::amount >= 8 * sizeof( Test ) + STAT_META_SIZE );
    hard_assert( Alloc::sumAmount >= 18 * sizeof( Test ) + 2 * STAT_META_SIZE );
    hard_assert( Alloc::count == 1 );
    hard_assert( Alloc::sumCount == 2 );
    hard_assert( constructCount == 8 );

    delete[] array;
    hard_assert( Alloc::amount == 0 );
    hard_assert( Alloc::sumAmount >= 18 * sizeof( Test ) + 2 * STAT_META_SIZE );
    hard_assert( Alloc::count == 0 );
    hard_assert( Alloc::sumCount == 2 );
  }
}

int main( int, char** )
{
  oz::Alloc::isLocked = false;

  // check first, before any memory allocations are made
  OZ_UNITTEST( ozAlloc );

  OZ_UNITTEST( ozCommon );
  OZ_UNITTEST( ozIterables );
  OZ_UNITTEST( ozArrays );

  oz::Alloc::isLocked = true;
  oz::Alloc::printLeaks();
  return 0;
}
