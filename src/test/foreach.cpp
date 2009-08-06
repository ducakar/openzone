/*
 *  foreach.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

using namespace oz;

struct Elem
{
  int value;

  Elem *prev[1];
  Elem *next[1];

  Elem( int value_ ) : value( value_ )
  {}
};

int main()
{
//   int a[5];
//   Vector<int> v;
//   SVector<int, 5> sv;
//   List<Elem, 0> l;
//   DList<Elem, 0> dl;
//   HashIndex<int, 4> hi;
//   HashString<int, 4> hs;
//
//   // 1
//   a[0] = 1;
//   v << 1;
//   sv << 1;
//   l << new Elem( 1 );
//   dl << new Elem( 1 );
//   hi.add( 101, 1 );
//   hs.add( "101", 1 );
//
//   // 2
//   a[1] = 2;
//   v << 2;
//   sv << 2;
//   l << new Elem( 2 );
//   dl << new Elem( 2 );
//   hi.add( 102, 2 );
//   hs.add( "102", 2 );
//
//   // 4
//   a[2] = 4;
//   v << 4;
//   sv << 4;
//   l << new Elem( 4 );
//   dl << new Elem( 4 );
//   hi.add( 104, 4 );
//   hs.add( "104", 4 );
//
//   // 3
//   a[3] = 3;
//   v << 3;
//   sv << 3;
//   l << new Elem( 3 );
//   dl << new Elem( 3 );
//   hi.add( 103, 3 );
//   hs.add( "103", 3 );
//
//   // 5
//   a[4] = 5;
//   v << 5;
//   sv << 5;
//   l << new Elem( 5 );
//   dl << new Elem( 5 );
//   hi.add( 105, 5 );
//   hs.add( "105", 5 );
//
//   foreach( i, iterator( a, 5 ) ) {
//     printf( "%d ", *i );
//   }
//   printf( "\n" );
//
//   foreach( i, v.iterator() ) {
//     printf( "%d ", *i );
//   }
//   printf( "\n" );
//
//   foreach( i, sv.iterator() ) {
//     printf( "%d ", *i );
//   }
//   printf( "\n" );
//
// //   List<Elem, 0> l1 = l.clone();
//   foreach( i, l.iterator() ) {
//     printf( "%d ", i->value );
//   }
//   printf( "\n" );
//
//   foreach( i, dl.iterator() ) {
//     printf( "%d ", i->value );
//   }
//   printf( "\n" );
//
//   foreach( i, hi.iterator() ) {
//     printf( "%d ", *i );
//   }
//   printf( "\n" );
//
//   foreach( i, hs.iterator() ) {
//     printf( "%d ", *i );
//   }
//   printf( "\n" );

  Vector<int> v;
  foreach( i, v.iterator() ) {
    printf( "%d ", *i );
  }
  Vector<int> v1;
  foreach( i, Iterator<int>( v1, v1 + v1.length() ) ) {
    printf( "%d ", *i );
  }
  Vector<int> v2;
  Vector<int>::Iterator iv( v2 );
  foreach( i, iv ) {
    printf( "%d ", *i );
  }

  return 0;
}
