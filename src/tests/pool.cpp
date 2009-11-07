/*
 *  pool.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

using namespace oz;

class A : public PoolAlloc<A, 0>
{
  public:

    int value;
    A   *next[1];

};

int main()
{
  int max = 10000;

  A *array[max];
  for( int i = 0; i < max; i++ ) {
    array[i] = new A();
    array[i]->value = 10;
  }
  for( int i = 0; i < max; i++ ) {
    delete array[i];
  }

  return 0;
}
