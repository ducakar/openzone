/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz/oz.hpp"

#include <cstdio>

using namespace oz;

struct T
{
  int i;

  T() { printf( "default ctor\n" ); }
  T( const T& ) { printf( "copy ctor\n" ); }
  T( T&& ) { printf( "move ctor\n" ); }

  T& operator = ( const T& ) { printf( "copy op\n" ); return *this; }
  T& operator = ( T&& ) { printf( "move op\n" ); return *this; }
};

int main( int, char** )
{
  int a = 1, b = 2;
  tie( a, b ) = pair( b, a );

  printf( "%d %d\n", a, b );
  return 0;
}
