/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

using namespace oz;

int main()
{
  int a[] = { 1, 2, 3, 4 };

  foreach( i, iterator( a, 3 ) ) {
    printf( "%d :: ", *i );
  }
  printf( "\n" );
  getchar();

  return 0;
}
