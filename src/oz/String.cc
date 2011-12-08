/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/String.cc
 */

#include "String.hh"

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
