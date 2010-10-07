/*
 *  unittest.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <iostream>
#include <SDL_main.h>

#ifdef NDEBUG
# error NDEBUG should not be defined. Unittest needs assert macro enabled to function properly.
#endif

#define OZ_UNITTEST( name ) \
  name##Unittest(); \
  assert( constructCount == 0 ); \
  cout << #name " Passed" << endl;

using namespace std;
using namespace oz;

static int constructCount = 0;

struct Test
{
  // on constructed objects, magic should be 42, otherwise some random number (hope not 42) or -1
  // on destructed objects
  int magic;
  int value;

  Test* prev[1];
  Test* next[1];
  int nextSlot;

  explicit Test()
  {
    assert( magic != 42 );

    magic = 42;
    ++constructCount;
  }

  explicit Test( int value_ )
  {
    assert( magic != 42 );

    magic = 42;
    value = value_;
    ++constructCount;
  }

  Test( const Test& t )
  {
    assert( magic != 42 );
    assert( t.magic == 42 );

    magic = 42;
    value = t.value;
    ++constructCount;
  }

  ~Test()
  {
    assert( magic == 42 );

    magic = -1;
    --constructCount;
  }

  Test& operator = ( const Test& t )
  {
    assert( magic == 42 );
    assert( t.magic == 42 );

    value = t.value;
    return *this;
  }

  bool operator == ( const Test& t ) const
  {
    assert( magic == 42 );
    assert( t.magic == 42 );

    return value == t.value;
  }

  bool operator < ( const Test& t ) const
  {
    assert( magic == 42 );
    assert( t.magic == 42 );

    return value < t.value;
  }
};

void ozCommonUnittest()
{
  Test a( 42 ), b( 43 ), c( 44 ), d( 43 );

  assert( &min( a, b ) == &a );
  assert( &min( b, d ) == &b );
  assert( &min( d, b ) == &d );
  assert( &min( b, a ) == &a );

  assert( &max( a, b ) == &b );
  assert( &max( b, a ) == &b );
  assert( &max( b, d ) == &b );
  assert( &max( d, b ) == &d );

  assert( &bound( a, b, c ) == &b );
  assert( &bound( b, a, c ) == &b );
  assert( &bound( c, a, b ) == &b );
  assert( &bound( c, b, d ) == &d );
  assert( &bound( a, b, d ) == &b );
}

void ozIterablesUnittest()
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

  assert( iContains( l.citer(), Test( -1 ) ) );
  assert( iContains( v.citer(), Test( -1 ) ) );
  assert( !iContains( l.citer(), Test( 0 ) ) );
  assert( !iContains( v.citer(), Test( 0 ) ) );
  assert( iContains( l.citer(), Test( 3 ) ) );
  assert( iContains( v.citer(), Test( 3 ) ) );
  assert( iContains( l.citer(), Test( 4 ) ) );
  assert( iContains( v.citer(), Test( 4 ) ) );
  assert( iContains( l.citer(), Test( 5 ) ) );
  assert( iContains( v.citer(), Test( 5 ) ) );

  auto li = l.citer();
  auto vi = v.citer();

  assert( iFind( l.citer(), Test( -1 ) ) == li );
  assert( iFind( v.citer(), Test( -1 ) ) == vi );

  ++li;
  ++vi;
  assert( iFind( l.citer(), Test( 3 ) ) == li );
  assert( iFind( v.citer(), Test( 3 ) ) == vi );

  ++li;
  ++vi;
  assert( iFind( l.citer(), Test( 4 ) ) == li );
  assert( iFind( v.citer(), Test( 4 ) ) == vi );

  ++li;
  ++vi;
  ++li;
  ++vi;
  assert( iFind( l.citer(), Test( 5 ) ) == li );
  assert( iFind( v.citer(), Test( 5 ) ) == vi );

  li = l.citer();
  vi = v.citer();

  assert( iFindLast( l.citer(), Test( -1 ) ) == li );
  assert( iFindLast( v.citer(), Test( -1 ) ) == vi );

  ++li;
  ++vi;
  ++li;
  ++vi;
  assert( iFindLast( l.citer(), Test( 4 ) ) == li );
  assert( iFindLast( v.citer(), Test( 4 ) ) == vi );

  ++li;
  ++vi;
  assert( iFindLast( l.citer(), Test( 3 ) ) == li );
  assert( iFindLast( v.citer(), Test( 3 ) ) == vi );

  ++li;
  ++vi;
  assert( iFindLast( l.citer(), Test( 5 ) ) == li );
  assert( iFindLast( v.citer(), Test( 5 ) ) == vi );

  li = decltype( li )();
  vi = decltype( vi )();
  assert( iFindLast( l.citer(), Test( 0 ) ) == li );
  assert( iFindLast( v.citer(), Test( 0 ) ) == vi );

  assert( iIndex( l.citer(), Test( -1 ) ) == 0 );
  assert( iIndex( v.citer(), Test( -1 ) ) == 0 );
  assert( iIndex( l.citer(), Test( 3 ) ) == 1 );
  assert( iIndex( v.citer(), Test( 3 ) ) == 1 );
  assert( iIndex( l.citer(), Test( 4 ) ) == 2 );
  assert( iIndex( v.citer(), Test( 4 ) ) == 2 );
  assert( iIndex( l.citer(), Test( 5 ) ) == 4 );
  assert( iIndex( v.citer(), Test( 5 ) ) == 4 );

  assert( iLastIndex( l.citer(), Test( -1 ) ) == 0 );
  assert( iLastIndex( v.citer(), Test( -1 ) ) == 0 );
  assert( iLastIndex( l.citer(), Test( 3 ) ) == 3 );
  assert( iLastIndex( v.citer(), Test( 3 ) ) == 3 );
  assert( iLastIndex( l.citer(), Test( 4 ) ) == 2 );
  assert( iLastIndex( v.citer(), Test( 4 ) ) == 2 );
  assert( iLastIndex( l.citer(), Test( 5 ) ) == 4 );
  assert( iLastIndex( v.citer(), Test( 5 ) ) == 4 );

  assert( iEquals( l.citer(), v.citer() ) );
  l.add( new Test( 0 ) );
  assert( !iEquals( l.citer(), v.citer() ) );
  v.pushFirst( Test( 0 ) );
  assert( iEquals( l.citer(), v.citer() ) );

  iDestruct( l.citer() );
  assert( constructCount == 6 );
  iConstruct( l.iter(), v.citer() );
  assert( constructCount == 12 );
  assert( iEquals( l.citer(), v.citer() ) );

  auto set12 = [] ( Test& t ) { t = Test( 12 ); };
  iMap( l.iter(), set12 );
  assert( !iEquals( v.citer(), l.citer() ) );
  iSet( v.iter(), Test( 12 ) );
  assert( iEquals( v.citer(), l.citer() ) );

  iDestruct( v.citer() );
  assert( constructCount == 6 );
  iConstruct( v.iter(), l.citer() );
  assert( constructCount == 12 );
  assert( iEquals( l.citer(), v.citer() ) );

  iCopy( l.iter(), v.citer() );
  assert( iEquals( l.citer(), v.citer() ) );

  iDestruct( l.citer() );
  iConstruct( l.iter() );

  auto destroy = [] ( const Test& t ) { delete &t; };
  iMap( l.citer(), destroy );

  Vector<Test*> pv;
  pv.add( new Test() );
  pv.add( new Test() );

  iFree( pv.iter() );
}

void ozArraysUnittest()
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

  assert( aContains( a, Test( -1 ), 5 ) );
  assert( aContains( b, Test( -1 ), 5 ) );
  assert( !aContains( a, Test( 0 ), 5 ) );
  assert( !aContains( b, Test( 0 ), 5 ) );
  assert( aContains( a, Test( 3 ), 5 ) );
  assert( aContains( b, Test( 3 ), 5 ) );
  assert( aContains( a, Test( 4 ), 5 ) );
  assert( aContains( b, Test( 4 ), 5 ) );
  assert( aContains( a, Test( 5 ), 5 ) );
  assert( aContains( b, Test( 5 ), 5 ) );

  assert( aIndex( a, Test( -1 ), 5 ) == 0 );
  assert( aIndex( a, Test( 3 ), 5 ) == 1 );
  assert( aIndex( a, Test( 4 ), 5 ) == 2 );
  assert( aIndex( a, Test( 5 ), 5 ) == 4 );

  assert( aLastIndex( a, Test( -1 ), 5 ) == 0 );
  assert( aLastIndex( a, Test( 3 ), 5 ) == 3 );
  assert( aLastIndex( a, Test( 4 ), 5 ) == 2 );
  assert( aLastIndex( a, Test( 5 ), 5 ) == 4 );

  assert( aEquals( a, b, 5 ) );
  a[0] = Test( 0 );
  assert( !aEquals( b, a, 5 ) );

  aDestruct( a, 5 );
  assert( constructCount == 5 );
  aConstruct( a, b, 5 );
  assert( constructCount == 10 );
  assert( aEquals( a, b, 5 ) );

  a[0] = Test( 0 );
  aCopy( a, b, 5 );
  assert( aEquals( a, b, 5 ) );
  a[0] = Test( 0 );
  aReverseCopy( a, b, 5 );
  assert( aEquals( a, b, 5 ) );
  aCopy( a, a + 1, 4 );
  assert( a[0] == Test( 3 ) && a[1] == Test( 4 ) && a[2] == Test( 3 ) && a[3] == Test( 5 ) );
  aReverseCopy( b + 1, b, 4 );
  assert( b[1] == Test( -1 ) && b[2] == Test( 3 ) && b[3] == Test( 4 ) && b[4] == Test( 3 ) );

  aRemove( a, 3, 4 );
  aRemove( a, 1, 3 );
  aRemove( b, 0, 5 );

  assert( a[0] == Test( 3 ) && a[1] == Test( 3 ) );
  assert( b[0] == Test( -1 ) && b[1] == Test( 3 ) && b[2] == Test( 4 ) && b[3] == Test( 3 ) );

  auto set12 = [] ( Test& t ) { t = Test( 12 ); };
  aMap( a, set12, 5 );
  assert( !aEquals( a, b, 5 ) );
  aSet( b, Test( 12 ), 5 );
  assert( aEquals( b, a, 5 ) );
  foreach( i, citer( a, 5 ) ) {
    assert( *i == Test( 12 ) );
  }

  Test** c = new Test*[5];
  foreach( i, iter( c, 5 ) ) {
    *i = new Test();
  }
  aFree( c, 5 );

  assert( aLength( a ) == 5 );

  Test* d = new Test[5];
  aCopy( d, b, 5 );
  d = aRealloc( d, 5, 10 );
  assert( aEquals( b, d, 5 ) );
  delete[] d;

  int r[1000];
  for( int i = 0; i < 1000; ++i ) {
    r[i] = Math::randn( 1000 );
  }
  aSort( r, 1000 );

  for( int i = -1; i <= 1000; ++i ) {
    int findIndex = aBisectFind( r, i, 1000 );
    int positionIndex = aBisectPosition( r, i, 1000 );

    if( 1 <= i && i < 1000 ) {
      assert( r[i - 1] <= r[i] );
    }

    assert( findIndex == -1 || r[findIndex] == i );
    assert( ( positionIndex == 0 && r[1] > i ) || positionIndex == 1000 ||
            ( r[positionIndex - 1] <= i && r[positionIndex] > i ) );
  }
}

void ozAllocUnittest()
{
  Test* array = Alloc::alloc<Test>( 10 );
  assert( Alloc::amount == 10 * sizeof( Test ) );
  assert( Alloc::sumAmount == 10 * sizeof( Test ) );
  assert( Alloc::count == 1 );
  assert( Alloc::sumCount == 1 );
  aConstruct( array, 5 );

  array = Alloc::realloc( array, 5, 8 );
  assert( Alloc::amount == 8 * sizeof( Test ) );
  assert( Alloc::sumAmount == 18 * sizeof( Test ) );
  assert( Alloc::count == 1 );
  assert( Alloc::sumCount == 2 );
  assert( constructCount == 5 );

  aDestruct( array, 5 );
  assert( constructCount == 0 );
  Alloc::dealloc( array );
  assert( Alloc::amount == 0 );
  assert( Alloc::sumAmount == 18 * sizeof( Test ) );
  assert( Alloc::count == 0 );
  assert( Alloc::sumCount == 2 );
}

int main( int, char** )
{
  // check first, before any memory allocations are made
  OZ_UNITTEST( ozAlloc );

  OZ_UNITTEST( ozCommon );
  OZ_UNITTEST( ozIterables );
  OZ_UNITTEST( ozArrays );

  return 0;
}
