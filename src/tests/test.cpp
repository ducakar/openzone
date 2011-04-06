/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "Build.hpp"

using namespace oz;

bool Alloc::isLocked = false;

struct Foo
{
  int a;
  int b[];
};

int main( int, char** )
{
  HashString<int> h;
  h.add( "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 1 );
  h.clear();
  h.dealloc();

  Alloc::printLeaks();
  return 0;
}
