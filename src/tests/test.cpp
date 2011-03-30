/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "../oz/oz.hpp"

#include <cstdio>
#include <cmath>

using namespace oz;

bool Alloc::isLocked = true;

struct A
{
  A() = default;

  A( const A& ) = default;

  ~A() = default;
};

int main( int, char** )
{
  return 0;
}
