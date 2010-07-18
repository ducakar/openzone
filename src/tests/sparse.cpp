/*
 *  sparse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <cstdio>
#include <SDL_main.h>

OZ_IMPORT()

struct A
{
  int nextSlot[1];
  int value;

  A() {}
  A( int value_ ) : value( value_ ) {}
};

int main( int, char** )
{
  Sparse<A> sparse( 1 );

  sparse << A( 0 );
  sparse << A( 1 );
  sparse << A( 2 );
  sparse << A( 3 );
  sparse << A( 4 );

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

  sparse << A( 4 );
  sparse << A( 2 );

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
