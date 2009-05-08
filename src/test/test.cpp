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
  Vector<int> v = Vector<int>( 1 );

  Vector<int>::Iterator i = v.iterator();
  printf( "%d\n", i == v.iterator() );
  return 0;
}
