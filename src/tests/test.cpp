/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include <stable.hpp>

#include <unistd.h>
#include <csignal>

using namespace oz;

bool Alloc::isLocked = true;

void foo();

void foo()
{
  printf("a");
  soft_assert( 0 );
  int* a = new int;
  printf("a");
  delete[] a;
}

int main( int, char** )
{
  System::catchSignals();

  Alloc::isLocked = false;
  onleave( []() {
    Alloc::isLocked = true;
    Alloc::printLeaks();
  } );

  foo();

  log.println( "Hello, world!" );
  return 0;
}
