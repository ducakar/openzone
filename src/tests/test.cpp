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

int main( int, char** )
{
  Directory dir( "data/textures/oz" );

  foreach( file, dir.citer() ) {
    printf( "%s %d\n", &*file, file.hasExtension( "png" ) );
  }
  return 0;
}
