/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file oz/String.cc
 */

#include "String.hh"

#include "System.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace oz
{

static const int LOCAL_BUFFER_SIZE = 4096;

OZ_HIDDEN
String::String( int count_ ) :
  count( count_ )
{
  ensureCapacity();
}

OZ_HIDDEN
void String::ensureCapacity()
{
  if( count < BUFFER_SIZE ) {
    buffer = baseBuffer;
  }
  else {
    buffer = static_cast<char*>( malloc( size_t( count + 1 ) ) );

    if( buffer == nullptr ) {
      OZ_ERROR( "String allocation failed" );
    }
  }
}

bool String::endsWith( const char* s, const char* sub )
{
  int len    = length( s );
  int subLen = length( sub );

  if( subLen > len ) {
    return false;
  }

  const char* end    = s   + len    - 1;
  const char* subEnd = sub + subLen - 1;

  while( subEnd >= sub && *subEnd == *end ) {
    --subEnd;
    --end;
  }
  return subEnd < sub;
}

String::~String()
{
  if( buffer != baseBuffer ) {
    free( buffer );
  }
}

String::String( const String& s ) :
  count( s.count )
{
  ensureCapacity();
  memcpy( buffer, s.buffer, size_t( count + 1 ) );
}

String::String( String&& s ) :
  count( s.count )
{
  if( s.buffer != s.baseBuffer ) {
    buffer = s.buffer;
    s.buffer = s.baseBuffer;
  }
  else {
    buffer = baseBuffer;
    memcpy( baseBuffer, s.baseBuffer, size_t( count + 1 ) );
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
  memcpy( buffer, s.buffer, size_t( count + 1 ) );

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
    memcpy( baseBuffer, s.baseBuffer, size_t( count + 1 ) );
  }

  s.count = 0;
  s.baseBuffer[0] = '\0';

  return *this;
}

String::String( const char* s, int count_ ) :
  count( count_ )
{
  ensureCapacity();
  memcpy( buffer, s, size_t( count ) );
  buffer[count] = '\0';
}

String::String( const char* s )
{
  count = s == nullptr ? 0 : length( s );
  ensureCapacity();

  memcpy( buffer, s, size_t( count ) );
  buffer[count] = '\0';
}

String String::str( const char* s, ... )
{
  String r;

  va_list ap;
  va_start( ap, s );

  char localBuffer[LOCAL_BUFFER_SIZE];
  r.count = vsnprintf( localBuffer, LOCAL_BUFFER_SIZE, s, ap );

  va_end( ap );

  r.ensureCapacity();
  memcpy( r.buffer, localBuffer, size_t( r.count + 1 ) );

  return r;
}

String String::create( int length, char** buffer_ )
{
  String r( length );

  *buffer_ = r.buffer;
  return r;
}

String String::replace( const char* s, char whatChar, char withChar )
{
  int    count = length( s );
  String r     = String( count );

  for( int i = 0; i < count; ++i ) {
    r.buffer[i] = s[i] == whatChar ? withChar : s[i];
  }
  r.buffer[count] = '\0';

  return r;
}

String& String::operator = ( const char* s )
{
  if( s == buffer ) {
    return *this;
  }

  count = s == nullptr ? 0 : length( s );

  if( buffer != baseBuffer ) {
    free( buffer );
  }
  ensureCapacity();

  memcpy( buffer, s, size_t( count ) );
  buffer[count] = '\0';

  return *this;
}

bool String::endsWith( const char* sub ) const
{
  int subLen = length( sub );

  if( subLen > count ) {
    return false;
  }

  const char* end    = buffer + count  - 1;
  const char* subEnd = sub    + subLen - 1;

  while( subEnd >= sub && *subEnd == *end ) {
    --subEnd;
    --end;
  }
  return subEnd < sub;
}

String String::operator + ( const String& s ) const
{
  int    rCount = count + s.count;
  String r      = String( rCount );

  memcpy( r.buffer, buffer, size_t( count ) );
  memcpy( r.buffer + count, s.buffer, size_t( s.count + 1 ) );

  return r;
}

String String::operator + ( const char* s ) const
{
  int    sLength = length( s );
  int    rCount  = count + sLength;
  String r       = String( rCount );

  memcpy( r.buffer, buffer, size_t( count ) );
  memcpy( r.buffer + count, s, size_t( sLength + 1 ) );

  return r;
}

String operator + ( const char* s, const String& t )
{
  int    sLength = String::length( s );
  int    rCount  = t.count + sLength;
  String r       = String( rCount );

  memcpy( r.buffer, s, size_t( sLength ) );
  memcpy( r.buffer + sLength, t.buffer, size_t( t.count + 1 ) );

  return r;
}

String& String::operator += ( const String& s )
{
  char* oBuffer = buffer;
  int   oCount  = count;

  count += s.count;
  ensureCapacity();

  if( buffer != oBuffer ) {
    memcpy( buffer, oBuffer, size_t( oCount ) );

    if( oBuffer != baseBuffer ) {
      free( oBuffer );
    }
  }
  memcpy( buffer + oCount, s, size_t( s.count + 1 ) );

  return *this;
}

String& String::operator += ( const char* s )
{
  char* oBuffer = buffer;
  int   oCount  = count;
  int   sLength = length( s );

  count += sLength;
  ensureCapacity();

  if( buffer != oBuffer ) {
    memcpy( buffer, oBuffer, size_t( oCount ) );

    if( oBuffer != baseBuffer ) {
      free( oBuffer );
    }
  }
  memcpy( buffer + oCount, s, size_t( sLength + 1 ) );

  return *this;
}

String String::substring( int start ) const
{
  hard_assert( 0 <= start && start <= count );

  int    rCount = count - start;
  String r      = String( rCount );

  memcpy( r.buffer, buffer + start, size_t( rCount + 1 ) );

  return r;
}

String String::substring( int start, int end ) const
{
  hard_assert( 0 <= start && start <= count && start <= end && end <= count );

  int    rCount = end - start;
  String r      = String( rCount );

  memcpy( r.buffer, buffer + start, size_t( rCount ) );
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
  return String( start, int( end - start ) );
}

String String::trim( const char* s )
{
  int sCount = length( s );
  const char* start = s;
  const char* end = s + sCount;

  while( start < end && isBlank( *start ) ) {
    ++start;
  }
  while( start < end && isBlank( *( end - 1 ) ) ) {
    --end;
  }
  return String( start, int( end - start ) );
}

String String::replace( char whatChar, char withChar ) const
{
  String r = String( count );

  for( int i = 0; i < count; ++i ) {
    r.buffer[i] = buffer[i] == whatChar ? withChar : buffer[i];
  }
  r.buffer[count] = '\0';

  return r;
}

DArray<String> String::split( char delimiter ) const
{
  int firstDelimiter = index( delimiter );
  int begin          = 0;
  int end            = firstDelimiter;
  int nPartitions    = 1;

  // Count substrings first.
  while( end >= 0 ) {
    begin = end + 1;
    end   = index( delimiter, begin );

    ++nPartitions;
  }

  DArray<String> array( nPartitions );

  begin = 0;
  end   = firstDelimiter;

  for( int i = 0; end >= 0; ++i ) {
    array[i] = substring( begin, end );

    begin = end + 1;
    end   = index( delimiter, begin );
  }
  array[nPartitions - 1] = substring( begin );

  return array;
}

}
