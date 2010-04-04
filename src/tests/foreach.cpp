/*
 *  foreach.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

using namespace oz;

struct Elem
{
  int value;

  Elem* prev[1];
  Elem* next[1];

  Elem( int value_ ) : value( value_ ) {}
};

struct SparseElem
{
  int value;
  int nextSlot[1];

  SparseElem() {}
  SparseElem( int value_ ) : value( value_ ) {}
};

int main()
{
  List<Elem, 0> l;
  DList<Elem, 0> dl;
  int a[5];
  Vector<int> v;
  SVector<int, 5> sv;
  Sparse<SparseElem> s;
  HashIndex<int, 4> hi;
  HashString<int, 4> hs;

  // 1
  l << new Elem( 1 );
  dl << new Elem( 1 );
  a[0] = 1;
  v << 1;
  sv << 1;
  s << SparseElem( 1 );
  hi.add( 101, 1 );
  hs.add( "101", 1 );

  // 2
  l << new Elem( 2 );
  dl << new Elem( 2 );
  a[1] = 2;
  v << 2;
  sv << 2;
  s << SparseElem( 2 );
  hi.add( 102, 2 );
  hs.add( "102", 2 );

  // 4
  l << new Elem( 4 );
  dl << new Elem( 4 );
  a[2] = 4;
  v << 4;
  sv << 4;
  s << SparseElem( 4 );
  hi.add( 104, 4 );
  hs.add( "104", 4 );

  // 3
  l << new Elem( 3 );
  dl << new Elem( 3 );
  a[3] = 3;
  v << 3;
  sv << 3;
  s << SparseElem( 3 );
  hi.add( 103, 3 );
  hs.add( "103", 3 );

  // 5
  l << new Elem( 5 );
  dl << new Elem( 5 );
  a[4] = 5;
  v << 5;
  sv << 5;
  s << SparseElem( 5 );
  hi.add( 105, 5 );
  hs.add( "105", 5 );

  iSet( v.iter(), 1 );

  foreach( i, l.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, dl.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, citer( a, 5 ) ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, v.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, sv.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, s.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, hi.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  foreach( i, hs.citer() ) {
    printf( "%d ", *i );
  }
  printf( "\n" );

  hi.clear();
  hs.clear();

  return 0;
}
