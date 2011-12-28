/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2011 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/String.cc
 */

#include "String.hh"

#include <cstdio>
#include <cstdlib>

namespace oz
{

static const int LOCAL_BUFFER_SIZE = 4096;

static OZ_THREAD_LOCAL char localBuffer[LOCAL_BUFFER_SIZE];

inline String::String( int count_, int ) : count( count_ )
{
  ensureCapacity();
}

void String::ensureCapacity()
{
  buffer = count < BUFFER_SIZE ?
      baseBuffer : reinterpret_cast<char*>( malloc( size_t( count + 1 ) ) );
}

void String::dealloc()
{
  free( buffer );
}

String::String( const String& s ) : count( s.count )
{
  ensureCapacity();
  aCopy( buffer, s.buffer, count + 1 );
}

String::String( String&& s ) : count( s.count )
{
  if( s.buffer != s.baseBuffer ) {
    buffer = s.buffer;
    s.buffer = s.baseBuffer;
  }
  else {
    buffer = baseBuffer;
    aCopy( baseBuffer, s.baseBuffer, count + 1 );
  }

  s.count = 0;
  s.baseBuffer[0] = '\0';
}

String& String::operator = ( const String& s )
{
  if( &s == this ) {
    return *this;
  }

  count = s.count;

  if( buffer != baseBuffer ) {
    free( buffer );
  }

  ensureCapacity();
  aCopy( buffer, s.buffer, count + 1 );

  return *this;
}

String& String::operator = ( String&& s )
{
  if( &s == this ) {
    return *this;
  }

  count = s.count;

  if( buffer != baseBuffer ) {
    free( buffer );
  }

  if( s.buffer != s.baseBuffer ) {
    buffer = s.buffer;
    s.buffer = s.baseBuffer;

  }
  else {
    buffer = baseBuffer;
    aCopy( baseBuffer, s.baseBuffer, count + 1 );
  }

  s.count = 0;
  s.baseBuffer[0] = '\0';

  return *this;
}

String::String( int count_, const char* s ) : count( count_ )
{
  hard_assert( s != null && count >= 0 && length( s ) >= count );

  ensureCapacity();
  aCopy( buffer, s, count );
  buffer[count] = '\0';

  hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
}

String::String( const char* s )
{
  if( s == null ) {
    buffer = baseBuffer;
    count = 0;
    baseBuffer[0] = '\0';
  }
  else {
    count = length( s );
    ensureCapacity();
    aCopy( buffer, s, count + 1 );
  }

  hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
}

String::String( bool b ) : buffer( baseBuffer )
{
  // some protection against too small buffers
  static_assert( BUFFER_SIZE >= 6, "String::BUFFER_SIZE too small for bool representation" );

  if( b ) {
    count = 4;

    baseBuffer[0] = 't';
    baseBuffer[1] = 'r';
    baseBuffer[2] = 'u';
    baseBuffer[3] = 'e';
    baseBuffer[4] = '\0';
  }
  else {
    count = 5;

    baseBuffer[0] = 'f';
    baseBuffer[1] = 'a';
    baseBuffer[2] = 'l';
    baseBuffer[3] = 's';
    baseBuffer[4] = 'e';
    baseBuffer[5] = '\0';
  }
}

String::String( int i ) : buffer( baseBuffer )
{
  // that should assure enough space, since log10( 2^( 8*sizeof( int ) ) ) <= 3*sizeof( int ),
  // +2 for sign and terminating null char
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( int ) ) + 2,
                 "String::BUFFER_SIZE too small for int representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%d", i );
}

String::String( float f ) : buffer( baseBuffer )
{
  // worst case: sign + 8 digits + dot + exponent (at most e-xx) + optional '.0' + '\0'
  static_assert( BUFFER_SIZE >= 17,
                 "String::BUFFER_SIZE too small for float representation." );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%#.8g", f );
}

String::String( double d ) : buffer( baseBuffer )
{
  // worst case: sign + 16 digits + dot + exponent (at most e-xxx) + optional '.0' + '\0'
  static_assert( BUFFER_SIZE >= 26,
                 "String::BUFFER_SIZE too small for double representation." );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%#.16g", d );
}

String String::str( const char* s, ... )
{
  String string;

  va_list ap;
  va_start( ap, s );

  string.count = vsnprintf( localBuffer, LOCAL_BUFFER_SIZE, s, ap );

  va_end( ap );

  string.ensureCapacity();
  aCopy( string.buffer, localBuffer, string.count + 1 );

  return string;
}

String& String::operator = ( const char* s )
{
  if( s == buffer ) {
    return *this;
  }

  count = length( s );

  if( buffer != baseBuffer ) {
    free( buffer );
  }
  ensureCapacity();
  aCopy( buffer, s, count + 1 );

  hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

  return *this;
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
