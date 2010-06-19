/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include <SDL_main.h>

using namespace oz;

struct Elem
{
  int value;
  Elem* prev[1];
  Elem* next[1];

  Elem( int value_ ) : value( value_ ) {}
};

int main( int, char** )
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

  a.clear();
  a.transfer( b );

  foreach( i, a.citer() ) {
    printf( "%d :: ", i->value );
  }
  printf( "\n" );

  int aa[4];
  int bb[4];
  auto ii = citer( aa, 4 );
  auto jj = iter( bb, 4 );
  iMove( jj, ii );

  printf( "%f %f", 1.0f / Math::sqrt( 2.0f ), Math::fastInvSqrt( 2.0f ) );
  return 0;
}
