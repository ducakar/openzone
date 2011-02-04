/*
 *  sparse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <cstdio>
#include <SDL_main.h>

using namespace oz;

bool Alloc::isLocked = true;

struct A
{
  int nextSlot[1];
  int value;

  A() {}
  A( int value_ ) : value( value_ ) {}
};

int main( int, char** )
{
  Alloc::isLocked = false;
  onleave( []() {
    Alloc::isLocked = true;
    Alloc::printLeaks();
  } );

  Sparse<A> sparse( 1 );

  sparse.add( A( 0 ) );
  sparse.add( A( 1 ) );
  sparse.add( A( 2 ) );
  sparse.add( A( 3 ) );
  sparse.add( A( 4 ) );

  sparse.remove( 2 );
  sparse.remove( 4 );

  foreach( i, sparse.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );
  foreach( i, citer<A>( sparse, sparse.capacity() ) ) {
    printf( "%d,%d ", i->value, i->nextSlot[0] );
  }
  printf( "\n" );

  sparse.add( A( 4 ) );
  sparse.add( A( 2 ) );

  foreach( i, sparse.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );
  foreach( i, citer<A>( sparse, sparse.capacity() ) ) {
    printf( "%d,%d ", i->value, i->nextSlot[0] );
  }
  printf( "\n" );

  return 0;
}
