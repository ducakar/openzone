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

bool Alloc::isLocked = true;

struct Foo
{
  int a;
  int b[];
};

int main( int, char** )
{
  log.println( "Build details {" );
  log.indent();

  log.println( "Time: %s", Build::TIME );
  log.println( "Compiler: %s", Build::COMPILER );

  log.unindent();
  log.println( "}" );
  return 0;
}
