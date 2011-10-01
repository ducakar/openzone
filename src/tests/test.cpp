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
  File f( "/etc" );
  DArray<File> list;

  f.ls( &list );

  foreach( file, list.citer() ) {
    printf( "%s\n", file->path() );
  }

  list.dealloc();

  Alloc::printLeaks();
  return 0;
}
