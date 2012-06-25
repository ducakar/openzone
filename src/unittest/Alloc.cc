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
 * @file unittest/Alloc.cc
 */

#include "unittest/unittest.hh"

using namespace oz;

void test_Alloc()
{
  Log::out << "+ Alloc\n";

  static const size_t STAT_META_SIZE = Alloc::ALIGNMENT;

  int oCount    = Alloc::count;
  int oSumCount = Alloc::sumCount;

  Foo* array = new Foo[10];
  OZ_CHECK( Alloc::amount >= 10 * sizeof( Foo ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::sumAmount >= 10 * sizeof( Foo ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::count == oCount + 1 );
  OZ_CHECK( Alloc::sumCount == oSumCount + 1 );

  array = aRealloc( array, 5, 8 );
  OZ_CHECK( Alloc::amount >= 8 * sizeof( Foo ) + STAT_META_SIZE );
  OZ_CHECK( Alloc::sumAmount >= 18 * sizeof( Foo ) + 2 * STAT_META_SIZE );
  OZ_CHECK( Alloc::count == oCount + 1 );
  OZ_CHECK( Alloc::sumCount == oSumCount + 2 );

  delete[] array;
  OZ_CHECK( Alloc::amount == 0 );
  OZ_CHECK( Alloc::sumAmount >= 18 * sizeof( Foo ) + 2 * STAT_META_SIZE );
  OZ_CHECK( Alloc::count == oCount + 0 );
  OZ_CHECK( Alloc::sumCount == oSumCount + 2 );

  OZ_CHECK( Alloc::alignDown( 0 ) == 0 );
  OZ_CHECK( Alloc::alignDown( 1 ) == 0 );
  OZ_CHECK( Alloc::alignDown( Alloc::ALIGNMENT - 1 ) == 0 );
  OZ_CHECK( Alloc::alignDown( Alloc::ALIGNMENT ) == Alloc::ALIGNMENT );

  OZ_CHECK( Alloc::alignUp( 0 ) == 0 );
  OZ_CHECK( Alloc::alignUp( 1 ) == Alloc::ALIGNMENT );
  OZ_CHECK( Alloc::alignUp( Alloc::ALIGNMENT - 1 ) == Alloc::ALIGNMENT );
  OZ_CHECK( Alloc::alignUp( Alloc::ALIGNMENT ) == Alloc::ALIGNMENT );

  char* zeroptr = null;
  char* oneptr  = zeroptr + Alloc::ALIGNMENT;

  OZ_CHECK( Alloc::alignDown( zeroptr ) == zeroptr );
  OZ_CHECK( Alloc::alignDown( zeroptr + 1 ) == zeroptr );
  OZ_CHECK( Alloc::alignDown( oneptr - 1 ) == zeroptr );
  OZ_CHECK( Alloc::alignDown( oneptr ) == oneptr );

  OZ_CHECK( Alloc::alignUp( zeroptr ) == zeroptr );
  OZ_CHECK( Alloc::alignUp( zeroptr + 1 ) == oneptr );
  OZ_CHECK( Alloc::alignUp( oneptr - 1 ) == oneptr );
  OZ_CHECK( Alloc::alignUp( oneptr ) == oneptr );
}
