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
 * @file tests/unittest.cc
 */

#include "oz/oz.hh"

#include <cstdio>

#define OZ_CHECK( cond ) \
  if( !( cond ) ) { throw Exception( "Check %s failed", #cond ); }

#define OZ_UNITTEST( name ) \
  oz::name##Unittest(); \
  OZ_CHECK( oz::constructCount == 0 ); \
  printf( #name " Passed\n" );

using namespace std;

namespace oz
{

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
    magic = 42;
    ++constructCount;
  }

  Test( const Test& t )
  {
    OZ_CHECK( t.magic == 42 );

    magic = 42;
    value = t.value;
    ++constructCount;
  }

  Test& operator = ( const Test& t )
  {
    OZ_CHECK( magic == 42 );
    OZ_CHECK( t.magic == 42 );

    value = t.value;
    return *this;
  }

  ~Test()
  {
    OZ_CHECK( magic == 42 );

    magic = -1;
    --constructCount;
  }

  explicit Test( int value_ )
  {
    magic = 42;
    value = value_;
    ++constructCount;
  }

  bool operator == ( const Test& t ) const
  {
    OZ_CHECK( magic == 42 );
    OZ_CHECK( t.magic == 42 );

    return value == t.value;
  }

  bool operator < ( const Test& t ) const
  {
    OZ_CHECK( magic == 42 );
    OZ_CHECK( t.magic == 42 );

    return value < t.value;
  }
};

static void ozCommonUnittest()
{
  Test a( 42 ), b( 43 ), c( 44 ), d( 43 );

  OZ_CHECK( &min( a, b ) == &a );
  OZ_CHECK( &min( b, d ) == &b );
  OZ_CHECK( &min( d, b ) == &d );
  OZ_CHECK( &min( b, a ) == &a );

  OZ_CHECK( &max( a, b ) == &b );
  OZ_CHECK( &max( b, a ) == &b );
  OZ_CHECK( &max( b, d ) == &b );
  OZ_CHECK( &max( d, b ) == &d );

  OZ_CHECK( &clamp( a, b, c ) == &b );
  OZ_CHECK( &clamp( b, a, c ) == &b );
  OZ_CHECK( &clamp( c, a, b ) == &b );
  OZ_CHECK( &clamp( c, b, d ) == &d );
  OZ_CHECK( &clamp( a, b, d ) == &b );
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

  OZ_CHECK( iContains( l.citer(), Test( -1 ) ) );
  OZ_CHECK( iContains( v.citer(), Test( -1 ) ) );
  OZ_CHECK( !iContains( l.citer(), Test( 0 ) ) );
  OZ_CHECK( !iContains( v.citer(), Test( 0 ) ) );
  OZ_CHECK( iContains( l.citer(), Test( 3 ) ) );
  OZ_CHECK( iContains( v.citer(), Test( 3 ) ) );
  OZ_CHECK( iContains( l.citer(), Test( 4 ) ) );
  OZ_CHECK( iContains( v.citer(), Test( 4 ) ) );
  OZ_CHECK( iContains( l.citer(), Test( 5 ) ) );
  OZ_CHECK( iContains( v.citer(), Test( 5 ) ) );

  auto li = l.citer();
  auto vi = v.citer();

  OZ_CHECK( iFind( l.citer(), Test( -1 ) ) == li );
  OZ_CHECK( iFind( v.citer(), Test( -1 ) ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFind( l.citer(), Test( 3 ) ) == li );
  OZ_CHECK( iFind( v.citer(), Test( 3 ) ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFind( l.citer(), Test( 4 ) ) == li );
  OZ_CHECK( iFind( v.citer(), Test( 4 ) ) == vi );

  ++li;
  ++vi;
  ++li;
  ++vi;
  OZ_CHECK( iFind( l.citer(), Test( 5 ) ) == li );
  OZ_CHECK( iFind( v.citer(), Test( 5 ) ) == vi );

  li = l.citer();
  vi = v.citer();

  OZ_CHECK( iFindLast( l.citer(), Test( -1 ) ) == li );
  OZ_CHECK( iFindLast( v.citer(), Test( -1 ) ) == vi );

  ++li;
  ++vi;
  ++li;
  ++vi;
  OZ_CHECK( iFindLast( l.citer(), Test( 4 ) ) == li );
  OZ_CHECK( iFindLast( v.citer(), Test( 4 ) ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFindLast( l.citer(), Test( 3 ) ) == li );
  OZ_CHECK( iFindLast( v.citer(), Test( 3 ) ) == vi );

  ++li;
  ++vi;
  OZ_CHECK( iFindLast( l.citer(), Test( 5 ) ) == li );
  OZ_CHECK( iFindLast( v.citer(), Test( 5 ) ) == vi );

  typedef decltype( li ) LI;
  typedef decltype( vi ) VI;

  li = LI();
  vi = VI();

  OZ_CHECK( iFindLast( l.citer(), Test( 0 ) ) == li );
  OZ_CHECK( iFindLast( v.citer(), Test( 0 ) ) == vi );

  OZ_CHECK( iIndex( l.citer(), Test( -1 ) ) == 0 );
  OZ_CHECK( iIndex( v.citer(), Test( -1 ) ) == 0 );
  OZ_CHECK( iIndex( l.citer(), Test( 3 ) ) == 1 );
  OZ_CHECK( iIndex( v.citer(), Test( 3 ) ) == 1 );
  OZ_CHECK( iIndex( l.citer(), Test( 4 ) ) == 2 );
  OZ_CHECK( iIndex( v.citer(), Test( 4 ) ) == 2 );
  OZ_CHECK( iIndex( l.citer(), Test( 5 ) ) == 4 );
  OZ_CHECK( iIndex( v.citer(), Test( 5 ) ) == 4 );

  OZ_CHECK( iLastIndex( l.citer(), Test( -1 ) ) == 0 );
  OZ_CHECK( iLastIndex( v.citer(), Test( -1 ) ) == 0 );
  OZ_CHECK( iLastIndex( l.citer(), Test( 3 ) ) == 3 );
  OZ_CHECK( iLastIndex( v.citer(), Test( 3 ) ) == 3 );
  OZ_CHECK( iLastIndex( l.citer(), Test( 4 ) ) == 2 );
  OZ_CHECK( iLastIndex( v.citer(), Test( 4 ) ) == 2 );
  OZ_CHECK( iLastIndex( l.citer(), Test( 5 ) ) == 4 );
  OZ_CHECK( iLastIndex( v.citer(), Test( 5 ) ) == 4 );

  OZ_CHECK( iEquals( l.citer(), v.citer() ) );
  l.add( new Test( 0 ) );
  OZ_CHECK( !iEquals( l.citer(), v.citer() ) );
  v.pushFirst( Test( 0 ) );
  OZ_CHECK( iEquals( l.citer(), v.citer() ) );

  iSet( v.iter(), Test( 12 ) );
  OZ_CHECK( !iEquals( v.citer(), l.citer() ) );

  iCopy( l.iter(), v.citer() );
  OZ_CHECK( iEquals( l.citer(), v.citer() ) );

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

  OZ_CHECK( aContains( a, Test( -1 ), 5 ) );
  OZ_CHECK( aContains( b, Test( -1 ), 5 ) );
  OZ_CHECK( !aContains( a, Test( 0 ), 5 ) );
  OZ_CHECK( !aContains( b, Test( 0 ), 5 ) );
  OZ_CHECK( aContains( a, Test( 3 ), 5 ) );
  OZ_CHECK( aContains( b, Test( 3 ), 5 ) );
  OZ_CHECK( aContains( a, Test( 4 ), 5 ) );
  OZ_CHECK( aContains( b, Test( 4 ), 5 ) );
  OZ_CHECK( aContains( a, Test( 5 ), 5 ) );
  OZ_CHECK( aContains( b, Test( 5 ), 5 ) );

  OZ_CHECK( aIndex( a, Test( -1 ), 5 ) == 0 );
  OZ_CHECK( aIndex( a, Test( 3 ), 5 ) == 1 );
  OZ_CHECK( aIndex( a, Test( 4 ), 5 ) == 2 );
  OZ_CHECK( aIndex( a, Test( 5 ), 5 ) == 4 );

  OZ_CHECK( aLastIndex( a, Test( -1 ), 5 ) == 0 );
  OZ_CHECK( aLastIndex( a, Test( 3 ), 5 ) == 3 );
  OZ_CHECK( aLastIndex( a, Test( 4 ), 5 ) == 2 );
  OZ_CHECK( aLastIndex( a, Test( 5 ), 5 ) == 4 );

  OZ_CHECK( aEquals( a, b, 5 ) );
  a[0] = Test( 0 );
  OZ_CHECK( !aEquals( b, a, 5 ) );

  aCopy( a, b, 5 );
  OZ_CHECK( aEquals( a, b, 5 ) );
  a[0] = Test( 0 );
  aReverseCopy( a, b, 5 );
  OZ_CHECK( aEquals( a, b, 5 ) );
  aCopy( a, a + 1, 4 );
  OZ_CHECK( a[0] == Test( 3 ) && a[1] == Test( 4 ) && a[2] == Test( 3 ) && a[3] == Test( 5 ) );
  aReverseCopy( b + 1, b, 4 );
  OZ_CHECK( b[1] == Test( -1 ) && b[2] == Test( 3 ) && b[3] == Test( 4 ) && b[4] == Test( 3 ) );

  aRemove( a, 3, 4 );
  aRemove( a, 1, 3 );
  aRemove( b, 0, 5 );

  OZ_CHECK( a[0] == Test( 3 ) && a[1] == Test( 3 ) );
  OZ_CHECK( b[0] == Test( -1 ) && b[1] == Test( 3 ) && b[2] == Test( 4 ) && b[3] == Test( 3 ) );

  aSet( a, Test( 12 ), 2 );
  aSet( b, Test( 12 ), 5 );
  OZ_CHECK( aEquals( b, a, 2 ) );
  foreach( i, citer( b ) ) {
    OZ_CHECK( *i == Test( 12 ) );
  }

  Test** c = new Test*[5];
  foreach( i, iter( c, 5 ) ) {
    *i = new Test();
  }
  aFree( c, 5 );
  delete[] c;

  OZ_CHECK( aLength( a ) == 5 );

  Test* d = new Test[5];
  aCopy( d, b, 5 );
  d = aRealloc( d, 5, 10 );
  OZ_CHECK( aEquals( b, d, 5 ) );
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
      OZ_CHECK( r[i - 1] <= r[i] );
    }

    OZ_CHECK( findIndex == -1 || r[findIndex] == i );
    OZ_CHECK( ( positionIndex == 0 && r[1] > i ) || positionIndex == 1000 ||
              ( r[positionIndex - 1] <= i && r[positionIndex] > i ) );
  }
}

static void ozAllocUnittest()
{
  static const size_t STAT_META_SIZE = Alloc::ALIGNMENT;

  OZ_CHECK( Alloc::amount == 0 );
  Test* array = new Test[10];
  OZ_CHECK( Alloc::amount >= 10 * sizeof( Test ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::sumAmount >= 10 * sizeof( Test ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::count == 1 );
  OZ_CHECK( Alloc::sumCount == 1 );

  array = aRealloc( array, 5, 8 );
  OZ_CHECK( Alloc::amount >= 8 * sizeof( Test ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::sumAmount >= 18 * sizeof( Test ) + 2 * STAT_META_SIZE );
  OZ_CHECK( Alloc::count == 1 );
  OZ_CHECK( Alloc::sumCount == 2 );
  OZ_CHECK( constructCount == 8 );

  delete[] array;
  OZ_CHECK( Alloc::amount == 0 );
  OZ_CHECK( Alloc::sumAmount >= 18 * sizeof( Test ) + 2 * STAT_META_SIZE );
  OZ_CHECK( Alloc::count == 0 );
  OZ_CHECK( Alloc::sumCount == 2 );
}

}

int main()
{
  // check first, before any memory allocations are made
  OZ_UNITTEST( ozAlloc );

  OZ_UNITTEST( ozCommon );
  OZ_UNITTEST( ozIterables );
  OZ_UNITTEST( ozArrays );

  oz::Alloc::printLeaks();
  return 0;
}
