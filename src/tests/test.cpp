/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include <stable.hpp>

#include <execinfo.h>

using namespace oz;

bool Alloc::isLocked = true;

void foo();

void foo()
{
  int* a;
  int* b;
  int* c;

  a = new int;
  b = new int;
  c = new int;

  onleave( [&]() { delete a; delete b; } );

  throw Exception( "drek" );
}

int main( int, char** )
{
  Alloc::isLocked = false;

  try {
    foo();
  }
  catch( const Exception& e ) {
    log.printException( e );
  }

  Alloc::isLocked = true;
  Alloc::dumpLeaks();
  return 0;
}
