/*
 *  test.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

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
};

// DArray<Foo> bar()
// {
//   DArray<Foo> foos( 10 );
//   return foos;
// }

int main( int, char** )
{
  Vector<Foo> v;
  v.add( Foo() );
  v.add( Foo() );

//   for( decltype( v.citer() ) i : v.citer() ) {
//   }

//   Alloc::printLeaks();
  return 0;
}
