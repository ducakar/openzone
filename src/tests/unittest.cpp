/*
 *  unittest.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

using namespace oz;

struct Test
{
  static const int DEFAULT_CTOR_BIT = 0x00000001;
  static const int COPY_CTOR_BIT    = 0x00000010;
  static const int MOVE_CTOR_BIT    = 0x00000020;
  static const int COPY_OP_BIT      = 0x00000040;
  static const int MOVE_OP_BIT      = 0x00000080;

  int flags;

  Test() : flags( DEFAULT_CTOR_BIT )
  {}

  Test( const Test& ) : flags( COPY_CTOR_BIT )
  {
    printf( "copy ctor\n" );
  }

  Test( Test&& ) : flags( MOVE_CTOR_BIT )
  {
    printf( "move ctor\n" );
  }

  Test& operator = ( const Test& )
  {
    printf( "copy op\n" );

    flags |= COPY_OP_BIT;
    return *this;
  }

  Test& operator = ( Test&& )
  {
    printf( "move op\n" );

    flags |= MOVE_OP_BIT;
    return *this;
  }
};

int main()
{
  Test a, b;
  Pair<Test&, int> p( a, 0 );
  Pair<Test, int> r = p;
  return 0;
}
