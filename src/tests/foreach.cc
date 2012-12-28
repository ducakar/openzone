/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file tests/foreach.cc
 */

#include <ozCore/ozCore.hh>

#include <cstdio>

using namespace oz;

struct Elem
{
  int value;

  Elem* prev[1];
  Elem* next[1];

  explicit Elem( int value_ ) :
    value( value_ )
  {}
};

int main()
{
  Chain<Elem, 0> l;
  DChain<Elem, 0> dl;
  Array<int, 5> a;
  List<int> v;
  SList<int, 5> sv;
  Set<int> s;
  Map<int, int> m;
  HashMap<int, int, 7> hi;
  HashMap<String, int, 7> hs;

  Chain<Elem, 0> l1;
  DChain<Elem, 0> dl1;
  Array<int, 5> a1;
  List<int> v1;
  SList<int, 5> sv1;
  Set<int> s1;
  Map<int, int> m1;
  HashMap<int, int, 7> hi1;
  HashMap<String, int, 7> hs1;

  // 1
  l.add( new Elem( 1 ) );
  dl.add( new Elem( 1 ) );
  a[0] = 1;
  v.add( 1 );
  sv.add( 1 );
  s.include( 1 );
  m.include( 1, 1 );
  hi.add( 101, 1 );
  hs.add( "101", 1 );

  // 2
  l.add( new Elem( 2 ) );
  dl.add( new Elem( 2 ) );
  a[1] = 2;
  v.add( 2 );
  sv.add( 2 );
  s.include( 2 );
  m.include( 2, 2 );
  hi.add( 102, 2 );
  hs.add( "102", 2 );

  // 4
  l.add( new Elem( 4 ) );
  dl.add( new Elem( 4 ) );
  a[2] = 4;
  v.add( 4 );
  sv.add( 4 );
  s.include( 4 );
  m.include( 4, 4 );
  hi.add( 104, 4 );
  hs.add( "104", 4 );

  // 3
  l.add( new Elem( 3 ) );
  dl.add( new Elem( 3 ) );
  a[3] = 3;
  v.add( 3 );
  sv.add( 3 );
  s.include( 3 );
  m.include( 3, 3 );
  hi.add( 103, 3 );
  hs.add( "103", 3 );

  // 5
  l.add( new Elem( 5 ) );
  dl.add( new Elem( 5 ) );
  a[4] = 5;
  v.add( 5 );
  sv.add( 5 );
  s.include( 5 );
  m.include( 5, 5 );
  hi.add( 105, 5 );
  hs.add( "105", 5 );

  swap( l, l1 );
  swap( dl, dl1 );
  swap( a, a1 );
  swap( v, v1 );
  swap( sv, sv1 );
  swap( s, s1 );
  swap( m, m1 );
  swap( hi, hi1 );
  swap( hs, hs1 );

  swap( l, l1 );
  swap( dl, dl1 );
  swap( a, a1 );
  swap( v, v1 );
  swap( sv, sv1 );
  swap( s, s1 );
  swap( m, m1 );
  swap( hi, hi1 );
  swap( hs, hs1 );

  v1.deallocate();
  s1.deallocate();
  m1.deallocate();
  hi1.deallocate();
  hs1.deallocate();

  foreach( i, l.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, dl.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  foreach( i, a.citer() ) {
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
    printf( "%d ", *i );
  }
  printf( "\n" );
  foreach( i, m.citer() ) {
    printf( "%d ", i->key );
  }
  printf( "\n" );

  foreach( i, hi.citer() ) {
    printf( "%d ", i->key );
  }
  printf( "\n" );

  foreach( i, hs.citer() ) {
    printf( "%d ", i->value );
  }
  printf( "\n" );

  l.free();
  dl.free();
  v.clear();
  v.deallocate();
  s.clear();
  s.deallocate();
  m.clear();
  m.deallocate();
  hi.clear();
  hi.deallocate();
  hs.clear();
  hs.deallocate();

  Alloc::printLeaks();
  return 0;
}
