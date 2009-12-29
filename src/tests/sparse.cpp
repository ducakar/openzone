/*
 *  sparse.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "stdio.h"

using namespace oz;

struct A
{
  int nextSlot;
  int value;

  A() {}
  A( int value_ ) : value( value_ ) {}
};

int main()
{
  Sparse<A> sparse( 1 );

  sparse << A( 0 );
  sparse << A( 1 );
  sparse << A( 2 );
  sparse << A( 3 );
  sparse << A( 4 );

  sparse.remove( 2 );
  sparse.remove( 4 );

  foreach( i, sparse.iterator() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );
  foreach( i, iterator<A>( sparse, sparse + sparse.capacity() ) ) {
    printf( "%d,%d ", i->value, i->nextSlot );
  }
  printf( "\n" );

  sparse << A( 4 );
  sparse << A( 2 );

  foreach( i, sparse.iterator() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );
  foreach( i, iterator<A>( sparse, sparse + sparse.capacity() ) ) {
    printf( "%d,%d ", i->value, i->nextSlot );
  }
  printf( "\n" );

  return 0;
}
