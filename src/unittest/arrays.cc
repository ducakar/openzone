/*
 * liboz - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include "unittest.hh"

using namespace oz;

void test_arrays()
{
  Log() << "+ arrays";

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

  aMoveBackward( a, b, 4 );
  aMoveBackward( a, a, 4 );
  aMoveBackward( a, b, 0 );
  aMove( b, a, 4 );
  OZ_CHECK_CONTENTS( a, -1, -1, -1, -1 );
  OZ_CHECK_CONTENTS( b, 1, 2, 3, 2 );
  OZ_CHECK( !aEquals( a, b, 4 ) );
  OZ_CHECK( aEquals( a, b, 0 ) );

  aFill( a, 0, 4 );
  aFill( a, -1, 0 );
  OZ_CHECK_CONTENTS( a, 0, 0, 0, 0 );

  iFill( iter( a ), 1 );
  OZ_CHECK_CONTENTS( a, 1, 1, 1, 1 );

  aSwap( a, b, 4 );
  OZ_CHECK_CONTENTS( a, 1, 2, 3, 2 );
  OZ_CHECK_CONTENTS( b, 1, 1, 1, 1 );
  aSwap( a + 1, b + 1, 3 );
  aSwap( a, b, 1 );

  aCopy( a, b, 4 );
  aCopy( a, b, 0 );
  OZ_CHECK( aEquals( a, b, 4 ) );
  OZ_CHECK( iEquals( citer( a ), citer( b ) ) );

  aCopyBackward( b, a, 4 );
  aCopyBackward( b, a, 0 );
  OZ_CHECK( aEquals( a, b, 4 ) );

  aCopy( a, a, 4 );
  OZ_CHECK( aEquals( a, b, 4 ) );

  aCopyBackward( a, a, 4 );
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

  OZ_CHECK( aFind( a, 0, 4 ) == nullptr );
  OZ_CHECK( aFind( a, 1, 4 ) == &a[0] );
  OZ_CHECK( aFind( a, 2, 4 ) == &a[1] );
  OZ_CHECK( aFind( a, 3, 4 ) == &a[2] );
  OZ_CHECK( aFind( a, 3, 0 ) == nullptr );

  OZ_CHECK( aFindLast( a, 0, 4 ) == nullptr );
  OZ_CHECK( aFindLast( a, 1, 4 ) == &a[0] );
  OZ_CHECK( aFindLast( a, 2, 4 ) == &a[3] );
  OZ_CHECK( aFindLast( a, 3, 4 ) == &a[2] );
  OZ_CHECK( aFindLast( a, 3, 0 ) == nullptr );

  const Foo ( & ca )[4] = a;
  OZ_CHECK( aFind( ca, 0, 4 ) == nullptr );
  OZ_CHECK( aFindLast( ca, 0, 4 ) == nullptr );
  OZ_CHECK( aFindLast( ca, 3, 0 ) == nullptr );

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

  aReverse( a, 4 );
  OZ_CHECK_CONTENTS( a, 2, 3, 2, 1 );

  Foo** c = new Foo*[5];
  foreach( i, iter( c, 5 ) ) {
    *i = new Foo();
  }
  aFree( c, 5 );
  delete[] c;

  OZ_CHECK( aLength( a ) == 4 );

  Foo* d = new Foo[4];
  aCopy( d, b, 4 );
  d = aReallocate( d, 4, 10 );
  OZ_CHECK( aEquals( b, d, 4 ) );
  delete[] d;

  for( int j = 0; j < 100; ++j ) {
    int r[1000];
    for( int i = 0; i < 1000; ++i ) {
      r[i] = Math::rand( 1000 );
    }
    aSort( r, 1000 );

    for( int i = -1; i <= 1000; ++i ) {
      int index = aBisection( r, i, 1000 );

      if( 1 <= i && i < 1000 ) {
        OZ_CHECK( r[i - 1] <= r[i] );
      }

      OZ_CHECK( ( index == -1 && r[0] > i ) ||
                ( index == 999 && r[999] <= i ) ||
                ( r[index] <= i && r[index + 1] > i ) );
    }
  }
}
