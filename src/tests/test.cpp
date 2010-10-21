/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <vector>

using namespace oz;

int main( int, char** )
{
  HashIndex<int> hi;

//  hi.add( 0, 42 );

  printf( "%d\n", Alloc::count );
  printf( "%ld\n", Alloc::amount );
  return 0;
}
