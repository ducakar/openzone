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
 * @file unittest/Alloc.cc
 */

#include "unittest.hh"

#include <climits>

using namespace oz;

static void test_String_parsing()
{
  for( int i = 0; i < 100000; ++i ) {
    int x = Math::rand( INT_MAX );
    if( i == 0 ) {
      x = int( 0xffffffff );
    }
    else if( i == 1 ) {
      x = int( 0x7fffffff );
    }
    else if( i == 2 ) {
      x = int( 0x80000000 );
    }

    String s = String( x );
    int    y = String::parseInt( s );
    String t = String::str( "%d", x );
    int    z = String::parseInt( t );

    if( x != y ) {
      Log::println( "%d -> %s -> %d  delta: %d", x, s.cstr(), y, y - x );
    }
    OZ_CHECK( x == y );

    if( x != z ) {
      Log::println( "%d -> printf -> %d  delta: %d", x, z, z - x );
    }
    OZ_CHECK( x == z );
  }

  for( int i = 0; i < 100000; ++i ) {
    float x = Math::rand();

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
    OZ_CHECK( x == y );

    if( x != z ) {
      Log::println( "%.9g -> printf -> %.9g  delta: %.9g", x, z, z - x );
    }
    OZ_CHECK( x == z );
  }
  for( int i = 0; i < 100000; ++i ) {
    float x = Math::rand() * 3.4e38f;

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
    OZ_CHECK( x == y );

    if( x != z ) {
      Log::println( "%.9g -> printf -> %.9g  delta: %.9g", x, z, z - x );
    }
    OZ_CHECK( x == z );
  }
  for( int i = 0; i < 10000; ++i ) {
    float x = Math::rand() * 3.4e-38f;

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
    OZ_CHECK( x == y );

    if( x != z ) {
      Log::println( "%.9g -> printf -> %.9g  delta: %.9g", x, z, z - x );
    }
    OZ_CHECK( x == z );
  }
}

void test_String()
{
  Log() << "+ String\n";

  test_String_parsing();
}
