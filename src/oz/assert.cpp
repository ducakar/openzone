/*
 *  assert.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "assert.hpp"

#include <cstdio>

namespace oz
{

  void _softAssert( const char* message, const char* file, int line )
  {
    fprintf( stderr, "%s:%d: Soft assertion `%s' failed.", file, line, message );
  }

}
