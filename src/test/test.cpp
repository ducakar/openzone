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
  DArray<int> a( 10 );

  a[0] = 0;
  a[1] = 1;
  a[2] = 2;
  a[3] = 3;
  a[4] = 4;
  a[5] = 5;
  a[6] = 6;
  a[7] = 7;
  a[8] = 8;
  a[9] = 9;
//   a[10] = 10;

  DArray<int> b;
  b = a;

  foreach( i, b.iterator() ) {
    printf( "%d :: ", *i );
  }
  printf( "\n" );

  return 0;
}
