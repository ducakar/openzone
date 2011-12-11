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

String String::operator + ( const char* s ) const
{
  hard_assert( s != null );

  int    sLength = length( s );
  int    rCount  = count + sLength;
  String r       = String( rCount, 0 );

  aCopy( r.buffer, buffer, count );
  aCopy( r.buffer + count, s, sLength + 1 );

  return r;
}

String String::operator + ( const String& s ) const
{
  int    rCount = count + s.count;
  String r      = String( rCount, 0 );

  aCopy( r.buffer, buffer, count );
  aCopy( r.buffer + count, s.buffer, s.count + 1 );

  return r;
}

String operator + ( const char* s, const String& t )
{
  hard_assert( s != null );

  int    sLength = String::length( s );
  int    rCount  = t.count + sLength;
  String r       = String( rCount, 0 );

  aCopy( r.buffer, s, sLength );
  aCopy( r.buffer + sLength, t.buffer, t.count + 1 );

  return r;
}

String String::substring( int start ) const
{
  hard_assert( 0 <= start && start <= count );

  int    rCount = count - start;
  String r      = String( rCount, 0 );

  aCopy( r.buffer, buffer + start, rCount + 1 );

  return r;
}

String String::substring( int start, int end ) const
{
  hard_assert( 0 <= start && start <= count && start <= end && end <= count );

  int    rCount = end - start;
  String r      = String( rCount, 0 );

  aCopy( r.buffer, buffer + start, rCount );
  r.buffer[rCount] = '\0';

  return r;
}

String String::trim() const
{
  const char* start = buffer;
  const char* end = buffer + count;

  while( start < end && isBlank( *start ) ) {
    ++start;
  }
  while( start < end && isBlank( *( end - 1 ) ) ) {
    --end;
  }
  return String( int( end - start ), start );
}

String String::trim( const char* s )
{
  int count = length( s );
  const char* start = s;
  const char* end = s + count;

  while( start < end && isBlank( *start ) ) {
    ++start;
  }
  while( start < end && isBlank( *( end - 1 ) ) ) {
    --end;
  }
  return String( int( end - start ), start );
}

String String::replace( char whatChar, char withChar ) const
{
  String r = String( count, 0 );

  for( int i = 0; i < count; ++i ) {
    r.buffer[i] = buffer[i] == whatChar ? withChar : buffer[i];
  }
  r.buffer[count] = '\0';

  return r;
}

String String::replace( const char* s, char whatChar, char withChar )
{
  int    count = length( s );
  String r     = String( count, 0 );

  for( int i = 0; i < count; ++i ) {
    r.buffer[i] = s[i] == whatChar ? withChar : s[i];
  }
  r.buffer[count] = '\0';

  return r;
}

DArray<String> String::split( char ch ) const
{
  int p0    = 0;
  int p1    = index( ch );
  int i     = 0;
  int count = 1;

  // count substrings first
  while( p1 >= 0 ) {
    p0 = p1 + 1;
    p1 = index( ch, p0 );
    ++count;
  }

  DArray<String> array( count );

  p0 = 0;
  p1 = index( ch );

  while( p1 >= 0 ) {
    array[i] = substring( p0, p1 );
    p0 = p1 + 1;
    p1 = index( ch, p0 );
    ++i;
  }
  array[i] = substring( p0 );

  return array;
}

}
