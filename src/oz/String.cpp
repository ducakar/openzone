/*
 *  String.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "oz.hpp"

#include <cstdio>

#ifdef OZ_MSVC
# define snprintf sprintf_s
#endif

namespace oz
{

  String::String( float f ) : buffer( baseBuffer )
  {
    // %g gives 6-digit precision + sign + dot + exponent (worst case e-xx) + terminating null char
    // 13 chars should be enough
    static_assert( BUFFER_SIZE >= 13, "Too small String::BUFFER_SIZE for float representation." );

    count = snprintf( buffer, BUFFER_SIZE, "%#g", f );
  }

  String::String( double d ) : buffer( baseBuffer )
  {
    // %g gives 6-digit precision + sign + dot + exponent (worst case e-xxx) + terminating null char
    // 14 chars should be enough
    static_assert( BUFFER_SIZE >= 14, "Too small String::BUFFER_SIZE for double representation." );

    count = snprintf( buffer, BUFFER_SIZE, "%#g", d );
  }

}
