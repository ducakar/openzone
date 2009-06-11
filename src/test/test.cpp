/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

using namespace oz;

struct T
{
  int value;
  T   *prev[1];
  T   *next[1];

  T( int i ) : value( i )
  {}

  T( const T &t ) : value( t.value )
  {}

  bool operator == ( const T &t ) const
  {
    return value == t.value;
  }

  bool operator != ( const T &t ) const
  {
    return value != t.value;
  }
};

int main()
{
  HashIndex<int, 47> h1;
  HashIndex<int, 47> h2;
  HashIndex<int, 47> h3;

  h1.add( 10, 1 );
  h1.add( 20, 2 );
  h1.add( 20+47, 3 );

  h2 = h1;
  h3 = h1;

  assert( h2 == h3 );

  foreach( i, h1.iterator() ) {
    printf( "%d :: ", i.value() );
  }
  printf( "\n" );

  h1.clear();
  h2.clear();
  h3.clear();

  return 0;
}
