/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

using namespace oz;

int main( int, char** )
{
  config.load( "share/openzone/class/lord.rc" );

  printf( "%g\n", config.get( "life", 0.0f ) );

  Alloc::printLeaks();
  return 0;
}
