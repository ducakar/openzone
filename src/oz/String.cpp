/*
 *  String.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "String.hpp"

#include <cstdio>

namespace oz
{

  String::String( float f ) : buffer( baseBuffer )
  {
    // worst case: sign + 8 digits + dot + exponent (at most e-xx) + '\0'
    static_assert( BUFFER_SIZE >= 15,
                   "String::BUFFER_SIZE too small for float representation." );

    count = snprintf( buffer, BUFFER_SIZE, "%#.8g", f );
  }

  String::String( double d ) : buffer( baseBuffer )
  {
    // worst case: sign + 16 digits + dot + exponent (at most e-xxx) + '\0'
    static_assert( BUFFER_SIZE >= 24,
                   "String::BUFFER_SIZE too small for double representation." );

    count = snprintf( buffer, BUFFER_SIZE, "%#.16g", d );
  }

}
