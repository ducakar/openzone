/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file unittest/arrays.cc
 */

#include "unittest/unittest.hh"

using namespace oz;

void test_arrays()
{
  Log::out << "+ arrays\n";

  CIterator<Foo> ici;
  Iterator<Foo>  ii;

  Foo a[4] = { 1, 2, 3, 2 };
  Foo b[4];

  aMove( b, a, 4 );
  aMove( b, b, 4 );
  aMove( b, a, 0 );
  OZ_CHECK_CONTENTS( a, -1, -1, -1, -1 );
  OZ_CHECK_CONTENTS( b, 1, 2, 3, 2 );
  OZ_CHECK( !aEquals( a, b, 4 ) );
  OZ_CHECK( aEquals( a, b, 0 ) );

  aReverseMove( a, b, 4 );
  aReverseMove( a, a, 4 );
  aReverseMove( a, b, 0 );
  aMove( b, a, 4 );
  OZ_CHECK_CONTENTS( a, -1, -1, -1, -1 );
  OZ_CHECK_CONTENTS( b, 1, 2, 3, 2 );
  OZ_CHECK( !aEquals( a, b, 4 ) );
  OZ_CHECK( aEquals( a, b, 0 ) );

  aSet( a, 0, 4 );
  aSet( a, -1, 0 );
  OZ_CHECK_CONTENTS( a, 0, 0, 0, 0 );

  iSet( iter( a ), 1 );
  OZ_CHECK_CONTENTS( a, 1, 1, 1, 1 );

  aCopy( a, b, 4 );
  aCopy( a, b, 0 );
  OZ_CHECK( aEquals( a, b, 4 ) );
  OZ_CHECK( iEquals( citer( a ), citer( b ) ) );

  aReverseCopy( b, a, 4 );
  aReverseCopy( b, a, 0 );
  OZ_CHECK( aEquals( a, b, 4 ) );

  aCopy( a, a, 4 );
  OZ_CHECK( aEquals( a, b, 4 ) );

  aReverseCopy( a, a, 4 );
  OZ_CHECK( aEquals( a, b, 4 ) );

  OZ_CHECK( !aContains( a, 0, 4 ) );
  OZ_CHECK( !aContains( b, 0, 4 ) );
  OZ_CHECK( aContains( a, 1, 4 ) );
  OZ_CHECK( aContains( b, 1, 4 ) );
  OZ_CHECK( aContains( a, 2, 4 ) );
  OZ_CHECK( aContains( b, 2, 4 ) );
  OZ_CHECK( aContains( a, 3, 4 ) );
  OZ_CHECK( aContains( b, 3, 4 ) );
  OZ_CHECK( !aContains( b, 3, 0 ) );

  OZ_CHECK( aFind( a, 0, 4 ) == null );
  OZ_CHECK( aFind( a, 1, 4 ) == &a[0] );
  OZ_CHECK( aFind( a, 2, 4 ) == &a[1] );
  OZ_CHECK( aFind( a, 3, 4 ) == &a[2] );
  OZ_CHECK( aFind( a, 3, 0 ) == null );

  OZ_CHECK( aFindLast( a, 0, 4 ) == null );
  OZ_CHECK( aFindLast( a, 1, 4 ) == &a[0] );
  OZ_CHECK( aFindLast( a, 2, 4 ) == &a[3] );
  OZ_CHECK( aFindLast( a, 3, 4 ) == &a[2] );
  OZ_CHECK( aFindLast( a, 3, 0 ) == null );

  const Foo ( & ca )[4] = a;
  OZ_CHECK( aFind( ca, 0, 4 ) == null );
  OZ_CHECK( aFindLast( ca, 0, 4 ) == null );
  OZ_CHECK( aFindLast( ca, 3, 0 ) == null );

  OZ_CHECK( aIndex( a, 0, 4 ) == -1 );
  OZ_CHECK( aIndex( a, 1, 4 ) == 0 );
  OZ_CHECK( aIndex( a, 2, 4 ) == 1 );
  OZ_CHECK( aIndex( a, 3, 4 ) == 2 );
  OZ_CHECK( aIndex( a, 3, 0 ) == -1 );

  OZ_CHECK( aLastIndex( a, 0, 4 ) == -1 );
  OZ_CHECK( aLastIndex( a, 1, 4 ) == 0 );
  OZ_CHECK( aLastIndex( a, 2, 4 ) == 3 );
  OZ_CHECK( aLastIndex( a, 3, 4 ) == 2 );
  OZ_CHECK( aLastIndex( a, 3, 0 ) == -1 );

  aRemove( a, 3, 4 );
  aRemove( a, 1, 3 );
  aRemove( b, 0, 4 );
  aRemove( b, 0, 3 );
  aRemove( b, 0, 1 );

  OZ_CHECK_CONTENTS( a, 1, 3, -1, 2 );
  OZ_CHECK_CONTENTS( b, 3, 2, -1, -1 );

  aInsert( a, 2, 1, 3 );
  aInsert( a, 2, 3, 4 );
  aInsert( b, 2, 0, 3 );
  aInsert( b, 1, 0, 4 );
  aInsert( b, 2, 3, 4 );

  OZ_CHECK_CONTENTS( a, 1, 2, 3, 2 );
  OZ_CHECK_CONTENTS( b, 1, 2, 3, 2 );

  Foo** c = new Foo*[5];
  foreach( i, iter( c, 5 ) ) {
    *i = new Foo();
  }
  aFree( c, 5 );
  delete[] c;

  OZ_CHECK( aLength( a ) == 4 );

  Foo* d = new Foo[4];
  aCopy( d, b, 4 );
  d = aRealloc( d, 4, 10 );
  OZ_CHECK( aEquals( b, d, 4 ) );
  delete[] d;

  for( int j = 0; j < 100; ++j ) {
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
      OZ_CHECK( ( positionIndex == 0 && r[0] > i ) ||
                ( positionIndex == 1000 && r[999] <= i ) ||
                ( r[positionIndex - 1] <= i && r[positionIndex] > i ) );
    }
  }
}
