/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

using namespace oz;

int main( int, char** )
{
  Pair<int, int> p;
  Pair<int, int> r = p;

  Alloc::printLeaks();
  return 0;
}
