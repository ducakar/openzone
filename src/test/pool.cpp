/*
 *  pool.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.hpp"

using namespace Dark;

class A : public PoolAlloc<A>
{
  public:

    int value;

};

int main()
{
  Pool<A> pool;
  A::setPool( &pool );

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
