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

void test1( const Vector<int>& v )
{
  for( auto i = v.citer(); i.isValid(); ++i ) {
    printf( "%d\n", *i );
  }
}

void test2( const Vector<int>& v )
{
  for( const auto& i : v.citer() ) {
    printf( "%d\n", *i );
  }
}

void test3( const Vector<int>& v )
{
  auto container = v.citer();
  for( auto __begin = begin( container ), __end = end( container );
      __begin != __end;
      ++__begin )
  {
    auto i = *__begin;
    {
      printf( "%d\n", *i );
    }
  }
}

int main( int, char** )
{
  Vector<int> v;
  v.add( 10 );
  v.add( 12 );

  test1( v );
  test2( v );

//   Alloc::printLeaks();
  return 0;
}
