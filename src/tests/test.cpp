/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include <algorithm>
#include <iostream>

using namespace oz;

struct Foo
{
  Foo()
  {
    printf( "Foo()\n" );
  }

  Foo( const Foo& )
  {
    printf( "Foo( const Foo& )\n" );
  }

  Foo( Foo&& )
  {
    printf( "Foo( Foo&& )\n" );
  }

  Foo& operator = ( const Foo& )
  {
    printf( "Foo& operator = ( const Foo& )\n" );
    return *this;
  }

  Foo& operator = ( Foo&& )
  {
    printf( "Foo& operator = ( Foo&& )\n" );
    return *this;
  }

  ~Foo()
  {
    printf( "~Foo()\n" );
  }
};

void bar( const Foo& )
{
  printf( "void bar( const Foo& )\n" );
}

void bar( Foo&& )
{
  printf( "void bar( Foo&& )\n" );
}

#define move_cast( expr ) \
  static_cast< decltype( expr )&& >( expr )

template <class Type>
void cal( Type&& f )
{
  bar( move_cast( f ) );
}

int main( int, char** )
{
  Foo f;
  cal( move_cast( f ) );
  return 0;
}
