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
#include <cstdlib>
#include <cstring>

namespace oz
{

const int LOCAL_BUFFER_SIZE = 4096;

static thread_local char localBuffer[LOCAL_BUFFER_SIZE];

String::String( bool b ) : buffer( baseBuffer )
{
  // some protection against too small buffers
  static_assert( BUFFER_SIZE >= 6, "String::BUFFER_SIZE too small for bool representation" );

  strncpy( buffer, b ? "true" : "false", 6 );
}

String::String( int i ) : buffer( baseBuffer )
{
  // that should assure enough space, since log10( 2^( 8*sizeof( int ) ) ) <= 3*sizeof( int ),
  // +2 for sign and terminating null char
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( int ) ) + 2,
                 "String::BUFFER_SIZE too small for int representation" );

  count = snprintf( buffer, BUFFER_SIZE, "%d", i );
}

String::String( float f ) : buffer( baseBuffer )
{
  // worst case: sign + 8 digits + dot + exponent (at most e-xx) + optional '.0' + '\0'
  static_assert( BUFFER_SIZE >= 17,
                 "String::BUFFER_SIZE too small for float representation." );

  count = snprintf( buffer, BUFFER_SIZE, "%#.8g", f );
}

String::String( double d ) : buffer( baseBuffer )
{
  // worst case: sign + 16 digits + dot + exponent (at most e-xxx) + optional '.0' + '\0'
  static_assert( BUFFER_SIZE >= 26,
                 "String::BUFFER_SIZE too small for double representation." );

  count = snprintf( buffer, BUFFER_SIZE, "%#.16g", d );
}

String String::str( const char* s, ... )
{
  String string;

  va_list ap;
  va_start( ap, s );

  string.count = vsnprintf( localBuffer, LOCAL_BUFFER_SIZE, s, ap );

  va_end( ap );

  string.buffer = string.count < BUFFER_SIZE ? string.baseBuffer : new char[string.count + 1];
  memcpy( string.buffer, localBuffer, size_t( string.count ) + 1 );

  return string;
}

}
