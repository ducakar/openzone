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

#include "windefs.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace oz
{

static const int LOCAL_BUFFER_SIZE = 4096;

const char* String::ParseException::what() const noexcept
{
  return "oz::String::ParseException";
}

OZ_HIDDEN
String::String( int count_, int ) :
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

    if( buffer == null ) {
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

bool String::parseBool( const char* s )
{
  if( s[0] == 'f' && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' && s[5] == '\0' ) {
    return false;
  }
  else if( s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e' && s[4] == '\0' ) {
    return true;
  }
  else {
    throw ParseException();
  }
}

int String::parseInt( const char* s )
{
  char* end;
  int i = int( strtol( s, &end, 0 ) );

  if( *end != '\0' || s[0] == '\0' ) {
    throw ParseException();
  }
  return i;
}

long String::parseLong( const char* s )
{
  char* end;
  long l = strtol( s, &end, 0 );

  if( *end != '\0' || s[0] == '\0' ) {
    throw ParseException();
  }
  return l;
}

float String::parseFloat( const char* s )
{
  char* end;
  float f = strtof( s, &end );

  if( *end != '\0' || s[0] == '\0' ) {
    throw ParseException();
  }
  return f;
}

double String::parseDouble( const char* s )
{
  char* end;
  double d = strtod( s, &end );

  if( *end != '\0' || s[0] == '\0' ) {
    throw ParseException();
  }
  return d;
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

String::String( int count_, const char* s ) :
  count( count_ )
{
  ensureCapacity();
  memcpy( buffer, s, size_t( count ) );
  buffer[count] = '\0';
}

String::String( const char* s )
{
  count = s == null ? 0 : length( s );
  ensureCapacity();

  memcpy( buffer, s, size_t( count ) );
  buffer[count] = '\0';
}

String::String( bool b ) :
  buffer( baseBuffer )
{
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

String::String( char c ) :
  buffer( baseBuffer ), count( 1 )
{
  baseBuffer[0] = c;
  baseBuffer[1] = '\0';
}

String::String( byte b ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( byte ) ) ) <= 3*sizeof( byte ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( byte ) ) + 2,
                 "String::BUFFER_SIZE too small for 'byte' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%d", b );
}

String::String( ubyte b ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( ubyte ) ) ) <= 3*sizeof( ubyte ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( ubyte ) ) + 2,
                 "String::BUFFER_SIZE too small for 'ubyte' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%u", b );
}

String::String( short s ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( short ) ) ) <= 3*sizeof( short ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( short ) ) + 2,
                 "String::BUFFER_SIZE too small for 'short' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%hd", s );
}

String::String( ushort s ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( ushort ) ) ) <= 3*sizeof( ushort ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( ushort ) ) + 2,
                 "String::BUFFER_SIZE too small for 'ushort' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%hu", s );
}

String::String( int i ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( int ) ) ) <= 3*sizeof( int ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( int ) ) + 2,
                 "String::BUFFER_SIZE too small for 'int' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%d", i );
}

String::String( uint i ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( uint ) ) ) <= 3*sizeof( uint ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( uint ) ) + 2,
                 "String::BUFFER_SIZE too small for 'uint' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%u", i );
}

String::String( long l ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( long ) ) ) <= 3*sizeof( long ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( long ) ) + 2,
                 "String::BUFFER_SIZE too small for 'long' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%ld", l );
}

String::String( ulong l ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( ulong ) ) ) <= 3*sizeof( ulong ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( ulong ) ) + 2,
                 "String::BUFFER_SIZE too small for 'ulong' representation" );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%lu", l );
}

String::String( long64 l ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( long64 ) ) ) <= 3*sizeof( long64 ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( long64 ) ) + 2,
                 "String::BUFFER_SIZE too small for 'long64' representation" );

#ifdef _WIN32
  count = snprintf( baseBuffer, BUFFER_SIZE, "%ld", long( l ) );
#else
  count = snprintf( baseBuffer, BUFFER_SIZE, "%lld", l );
#endif
}

String::String( ulong64 l ) :
  buffer( baseBuffer )
{
  // That should assure enough space as log10( 2^( 8*sizeof( ulong64 ) ) ) <= 3*sizeof( ulong64 ),
  // +2 for sign and terminating null char.
  static_assert( BUFFER_SIZE >= 3 * int( sizeof( ulong64 ) ) + 2,
                 "String::BUFFER_SIZE too small for 'ulong64' representation" );

#ifdef _WIN32
  count = snprintf( baseBuffer, BUFFER_SIZE, "%lu", ulong( l ) );
#else
  count = snprintf( baseBuffer, BUFFER_SIZE, "%llu", l );
#endif
}

String::String( float f ) :
  buffer( baseBuffer )
{
  // Worst case: sign + "0.000" + 8 digits + exponent (at most e-xx) + '\0'.
  static_assert( BUFFER_SIZE >= 19, "String::BUFFER_SIZE is too small for float representation." );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%.8g", f );
}

String::String( double d ) :
  buffer( baseBuffer )
{
  // Worst case: sign + "0.000" + 16 digits + exponent (at most e-xxx) + '\0'.
  static_assert( BUFFER_SIZE >= 28, "String::BUFFER_SIZE is too small for double representation." );

  count = snprintf( baseBuffer, BUFFER_SIZE, "%.16g", d );
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
  String r( length, 0 );

  *buffer_ = r.buffer;
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

String& String::operator = ( const char* s )
{
  if( s == buffer ) {
    return *this;
  }

  count = s == null ? 0 : length( s );

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
  String r      = String( rCount, 0 );

  memcpy( r.buffer, buffer, size_t( count ) );
  memcpy( r.buffer + count, s.buffer, size_t( s.count + 1 ) );

  return r;
}

String String::operator + ( const char* s ) const
{
  int    sLength = length( s );
  int    rCount  = count + sLength;
  String r       = String( rCount, 0 );

  memcpy( r.buffer, buffer, size_t( count ) );
  memcpy( r.buffer + count, s, size_t( sLength + 1 ) );

  return r;
}

String operator + ( const char* s, const String& t )
{
  int    sLength = String::length( s );
  int    rCount  = t.count + sLength;
  String r       = String( rCount, 0 );

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
  String r      = String( rCount, 0 );

  memcpy( r.buffer, buffer + start, size_t( rCount + 1 ) );

  return r;
}

String String::substring( int start, int end ) const
{
  hard_assert( 0 <= start && start <= count && start <= end && end <= count );

  int    rCount = end - start;
  String r      = String( rCount, 0 );

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
  return String( int( end - start ), start );
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
