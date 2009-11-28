/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

using namespace oz;

struct Elem
{
  int value;
  Elem *prev[1];
  Elem *next[1];

  Elem( int value_ ) : value( value_ ) {}
};

int main()
{
  DList<Elem, 0> a;
  DList<Elem, 0> b;

  a << new Elem( 1 );
  a << new Elem( 2 );
  a << new Elem( 3 );
  a << new Elem( 4 );

  b << new Elem( 5 );
  b << new Elem( 6 );
  b << new Elem( 7 );
  b << new Elem( 8 );

  a.transfer( b );
  a.transfer( b );

  foreach( i, a.iterator() ) {
    printf( "%d :: ", i->value );
  }
  printf( "\n" );

  printf( "%f %f", 1.0f / Math::sqrt( 2.0f ), Math::fInvSqrt( 2.0f ) );

  return 0;
}