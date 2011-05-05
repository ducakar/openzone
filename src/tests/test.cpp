/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <initializer_list>

using namespace oz;

struct Foo
{
  int data[2];

  Foo* prev[1];
  Foo* next[1];

  Foo()
  {
    printf( "Foo()\n" );
  }

  ~Foo()
  {
    printf( "~Foo()\n" );
  }

  Foo( const Foo& f )
  {
    printf( "Foo( const Foo& )\n" );

    data[0] = f.data[0];
    data[1] = f.data[1];
    prev[0] = f.prev[0];
    next[0] = f.next[0];
  }

//   Foo( Foo&& f )
//   {
//     printf( "Foo( Foo&& )\n" );
//
//     data[0] = f.data[0];
//     data[1] = f.data[1];
//     prev[0] = f.prev[0];
//     next[0] = f.next[0];
//   }

  Foo& operator = ( const Foo& f )
  {
    printf( "Foo& operator = ( const Foo& )\n" );

    data[0] = f.data[0];
    data[1] = f.data[1];
    prev[0] = f.prev[0];
    next[0] = f.next[0];
    return *this;
  }

//   Foo& operator = ( Foo&& f )
//   {
//     printf( "Foo& operator = ( Foo&& )\n" );
//
//     data[0] = f.data[0];
//     data[1] = f.data[1];
//     prev[0] = f.prev[0];
//     next[0] = f.next[0];
//     return *this;
//   }

};

struct Bar
{
  int i;
  Bar* next;
};

int main( int, char** )
{
  HashString<Foo> hs;
  HashString<Foo> hs1;
  hs1 = /*static_cast< HashString<Foo>&& >(*/ hs /*)*/;

  foreach( i, hs1.citer() ) {
    printf( "%s :: ", i.key().cstr() );
  }
  printf( "\n" );

  hs.add( "0" );

  return 0;
}
