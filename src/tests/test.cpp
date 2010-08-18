/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz/oz.hpp"

#include <cstdio>
#include <initializer_list>
#include <vector>

using namespace oz;

/*namespace std {
template <class T>
struct initializer_list
{
  const T* first;
  size_t size;

  initializer_list() : first( nullptr ), size( 0 ) {}
  initializer_list( const T* first_, const T* last ) : first( first_ ), size( last - first ) {}
};
}*/

struct T
{
  int i;

  T() { printf( "default ctor\n" ); }
  T( const T& ) { printf( "copy ctor\n" ); }
  T( T&& ) { printf( "move ctor\n" ); }
  T( std::initializer_list<int> il ) { printf( "%d %d\n", *il.begin(), *il.end() ); }

  T& operator = ( const T& ) { printf( "copy op\n" ); return *this; }
  T& operator = ( T&& ) { printf( "move op\n" ); return *this; }
};

int main( int, char** )
{
  int array[] = { 0, 1, 2, 3, 4, 5, 6 };

  auto add42 = [] ( int* elem ) { *elem += 42; };

  iMap( iter( array, 7 ), add42 );
  iMap( iter( array, 7 ), [] ( int* elem ) { printf( "%d\n", *elem ); } );

  return 0;
}
