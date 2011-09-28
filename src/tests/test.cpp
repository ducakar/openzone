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
  float a = 0.0f;
  float b = 1e10;

  log.println( "%d", a == 2.0f * a );

  Alloc::printLeaks();
  return 0;
}
