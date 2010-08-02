/*
 *  pool.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <SDL_main.h>

using namespace oz;

class A
{
  public:

    int value;
    A*  next[1];

    static Pool<A, 0> pool;

  OZ_STATIC_POOL_ALLOC( pool )

};

Pool<A, 0> A::pool;

int main( int, char** )
{
  const int max = 10000;

  A* array[max];
  for( int i = 0; i < max; ++i ) {
    array[i] = new A();
    array[i]->value = 10;
  }
  for( int i = 0; i < max; ++i ) {
    delete array[i];
  }

  return 0;
}
