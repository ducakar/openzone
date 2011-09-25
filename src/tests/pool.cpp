/*
 *  pool.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <SDL/SDL_main.h>

using namespace oz;

bool Alloc::isLocked = true;

class A
{
  public:

    int value;
    A*  next[1];

    static Pool<A, 2> pool;

  OZ_STATIC_POOL_ALLOC( pool )

};

Pool<A, 2> A::pool;

int main( int, char** )
{
  Alloc::isLocked = false;

  const int max = 10000;

  A* array[max];
  for( int i = 0; i < max; ++i ) {
    array[i] = new A();
    array[i]->value = 10;
  }
  for( int i = 0; i < max; ++i ) {
    delete array[i];
  }

  A::pool.free();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}
