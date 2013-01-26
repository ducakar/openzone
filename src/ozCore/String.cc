/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozCore/String.cc
 */

#include "String.hh"

#include "Math.hh"
#include "System.hh"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined( __ANDROID__ ) || defined( _WIN32 )
# define exp10( x ) exp( log( 10.0 ) * ( x ) )
#endif

namespace oz
{

static const int LOCAL_BUFFER_SIZE = 4096;

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

String String::substring( const char* s, int start )
{
  hard_assert( 0 <= start && start <= length( s ) );

  int    rCount = length( s ) - start;
  String r      = String( rCount, 0 );

  mCopy( r.buffer, s + start, size_t( rCount + 1 ) );

  return r;
}

String String::substring( const char* s, int start, int end )
{
  hard_assert( 0 <= start && start <= end && end <= length( s ) );

  int    rCount = end - start;
  String r      = String( rCount, 0 );

  mCopy( r.buffer, s + start, size_t( rCount ) );
  r.buffer[rCount] = '\0';

  return r;
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
  return String( start, int( end - start ) );
}

DArray<String> String::split( const char* s, char delimiter )
{
  int count       = length( s );
  int nPartitions = 0;
  int begin       = 0;
  int end         = index( s, delimiter );

  // Count substrings first.
  while( end >= 0 ) {
    if( begin != end ) {
      ++nPartitions;
    }

    begin = end + 1;
    end   = index( s, delimiter, begin );
  }
  if( begin != count ) {
    ++nPartitions;
  }

  DArray<String> array( nPartitions );

  begin = 0;
  end   = index( s, delimiter );

  for( int i = 0; end >= 0; ) {
    if( begin != end ) {
      array[i] = substring( s, begin, end );
      ++i;
    }

    begin = end + 1;
    end   = index( s, delimiter, begin );
  }
  if( begin != count ) {
    array[nPartitions - 1] = substring( s, begin, count );
  }

  return array;
}

String String::fileName( const char* s )
{
  int slash = lastIndex( s, '/' );

  return slash < 0 ? String( s ) : substring( s, slash + 1 );
}

String String::fileBaseName( const char* s )
{
  int slash = lastIndex( s, '/' );
  int dot   = lastIndex( s, '.' );

  if( slash < dot ) {
    return substring( s, slash + 1, dot );
  }
  else {
    return substring( s, slash + 1 );
  }
}

String String::fileExtension( const char* s )
{
  int slash = lastIndex( s, '/' );
  int dot   = lastIndex( s, '.' );

  return slash < dot ? substring( s, dot + 1 ) : String();
}

bool String::fileHasExtension( const char* s, const char* ext )
{
  const char* slash = findLast( s, '/' );
  const char* dot   = findLast( s, '.' );

  if( slash < dot ) {
    return compare( dot + 1, ext ) == 0;
  }
  else {
    return isEmpty( ext );
  }
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

bool String::parseBool( const char* s, const char** end )
{
  if( s[0] == 't' && s[1] == 'r' && s[2] == 'u' && s[3] == 'e' ) {
    if( end != nullptr ) {
      *end = s + 4;
    }
    return true;
  }
  else {
    if( end != nullptr ) {
      if( s[0] == 'f' && s[1] == 'a' && s[2] == 'l' && s[3] == 's' && s[4] == 'e' ) {
        *end = s + 5;
      }
      else {
        *end = s;
      }
    }
    return false;
  }
}

int String::parseInt( const char* s, const char** end )
{
  const char* p = s;

  // Sign.
  int sign = 1;
  if( *p == '-' ) {
    sign = -1;
    ++p;
  }

  // Non-zero integer.
  if( '1' <= *p && *p <= '9' ) {
    int number = *p - '0';
    ++p;

    while( '0' <= *p && *p <= '9' ) {
      number *= 10;
      number += *p - '0';
      ++p;
    }

    if( end != nullptr ) {
      *end = p;
    }
    return sign * number;
  }
  // Zero or invalid.
  else {
    if( end != nullptr ) {
      *end = *p == '0' ? p + 1 : s;
    }
    return 0;
  }
}

float String::parseFloat( const char* s, const char** end )
{
  const char* p = s;
  double number;

  // Sign.
  double sign = 1.0;
  if( *p == '-' ) {
    sign = -1.0;
    ++p;
  }

  // Non-zero integer part.
  if( '1' <= *p && *p <= '9' ) {
    number = *p - '0';
    ++p;

    while( '0' <= *p && *p <= '9' ) {
      number *= 10.0;
      number += *p - '0';
      ++p;
    }
  }
  // Zero integer part.
  else if( *p == '0' ) {
    number = 0.0;
    ++p;
  }
  // Infinity.
  else if( p[0] == 'i' && p[1] == 'n' && p[2] == 'f' ) {
    if( end != nullptr ) {
      *end = p + 3;
    }
    return float( sign ) * Math::INF;
  }
  // Not-a-number.
  else if( p[0] == 'n' && p[1] == 'a' && p[2] == 'n' ) {
    if( end != nullptr ) {
      *end = p + 3;
    }
    return float( sign ) * Math::NaN;
  }
  // Invalid.
  else {
    invalidNumber:
    if( end != nullptr ) {
      *end = s;
    }
    return Math::NaN;
  }

  // Fractional part.
  if( *p == '.' ) {
    ++p;

    if( *p < '0' || '9' < *p ) {
      goto invalidNumber;
    }

    double fract = 1.0;
    do {
      fract  *= 0.1;
      number += ( *p - '0' ) * fract;
      ++p;
    }
    while( '0' <= *p && *p <= '9' );
  }

  // Exponent.
  if( *p == 'e' || *p == 'E' ) {
    ++p;

    double expSign = 1.0;
    if( *p == '-' ) {
      expSign = -1.0;
      ++p;
    }
    else if( *p == '+' ) {
      ++p;
    }

    if( *p < '0' || '9' < *p ) {
      goto invalidNumber;
    }

    double expNumber = 0.0;
    do {
      expNumber *= 10.0;
      expNumber += *p - '0';
      ++p;
    }
    while( '0' <= *p && *p <= '9' );

    number *= exp10( expSign * expNumber );
  }

  if( end != nullptr ) {
    *end = p;
  }
  return float( sign * number );
}

String::String( const char* s, int count_ ) :
  buffer( baseBuffer ), count( count_ )
{
  if( count != 0 ) {
    ensureCapacity();
    mCopy( buffer, s, size_t( count ) );
  }
  buffer[count] = '\0';
}

String::String( const char* s )
{
  if( s == nullptr ) {
    buffer    = baseBuffer;
    count     = 0;
    buffer[0] = '\0';
  }
  else {
    count = length( s );
    ensureCapacity();
    mCopy( buffer, s, size_t( count + 1 ) );
  }
}

String::String( const char* s, const char* t )
{
  int sCount = length( s );
  int tCount = length( t );

  count = sCount + tCount;
  ensureCapacity();
  mCopy( buffer, s, size_t( sCount ) );
  mCopy( buffer + sCount, t, size_t( tCount + 1) );
}

String::String( bool b ) :
  buffer( baseBuffer )
{
  static_assert( BUFFER_SIZE >= 6, "Too small String::baseBuffer for bool representation." );

  if( b ) {
    strcpy( baseBuffer, "true" );
    count = 4;
  }
  else {
    strcpy( baseBuffer, "false" );
    count = 5;
  }
}

String::String( int i ) :
  buffer( baseBuffer ), count( 0 )
{
  static_assert( BUFFER_SIZE >= 12, "Too small String::baseBuffer for int representation." );

  // First, count the digits (at least one digit is always counted).
  int n = i;
  do {
    n /= 10;
    ++count;
  }
  while( n != 0 );

  if( i < 0 ) {
    if( i == int( 0x80000000 ) ) {
      strcpy( baseBuffer, "-2147483648" );
      return;
    }
    else {
      i = -i;

      baseBuffer[0] = '-';
      ++count;
    }
  }

  for( int j = count - 1; ; j-- ) {
    baseBuffer[j] = char( '0' + ( i % 10 ) );
    i /= 10;

    if( i == 0 ) {
      break;
    }
  }
  baseBuffer[count] = '\0';
}

String::String( float f, int nDigits ) :
  buffer( baseBuffer ), count( 0 )
{
  static_assert( BUFFER_SIZE >= 16, "Too small String::baseBuffer for float representation." );
  hard_assert( 0 < nDigits && nDigits <= 9 );

  double d = f;

  union DoubleBits
  {
    double  value;
    ulong64 bits;
  }
  db = { d };

  // Sign.
  if( db.bits & 1ull << ( sizeof( d ) * 8 - 1 ) ) {
    baseBuffer[count++] = '-';
    d = -d;
  }

  // Check for zero, infinity and NaN.
  if( d == 0.0 ) {
    baseBuffer[count++] = '0';
    baseBuffer[count] = '\0';
    return;
  }
  else if( d + 1e38 == d || d != d ) {
    if( d * 0.0 == d || d != d ) {
      strcpy( baseBuffer + count, "nan" );
      count += 4;
    }
    else {
      strcpy( baseBuffer + count, "inf" );
      count += 4;
    }
    return;
  }

  // Check exponent.
  int e = int( log10( d ) );

  // Non-exponential form.
  if( -4 < e && e < nDigits ) {
    // Mantissa.
    double eps = d * exp10( 1 - nDigits );
    double n   = d;

    for( int i = min( 0, e ); ; ++i ) {
      double base = exp10( e - i );

      if( i == e + 1 ) {
        baseBuffer[count++] = '.';
      }

      baseBuffer[count++] = char( '0' + int( n / base ) );
      n = fmod( n, base );

      if( base - n < eps ) {
        n = 0.0;
        ++baseBuffer[count - 1];

        if( e - i <= 0 ) {
          break;
        }
      }
      else if( n < eps && e - i <= 0 ) {
        break;
      }
    }
  }
  // Exponential form.
  else {
    // Mantissa.
    double eps = d * exp10( 1 - nDigits );
    double n   = d;

    if( d < 1.0 ) {
      --e;
    }

    for( int i = 0; i < nDigits; ++i ) {
      double base = exp10( e - i );

      if( i == 1 ) {
        baseBuffer[count++] = '.';
      }

      baseBuffer[count++] = char( '0' + int( n / base ) );
      n = fmod( d, base );

      if( n < eps ) {
        break;
      }
      else if( base - n < eps ) {
        ++baseBuffer[count - 1];
        break;
      }
    }

    // Exponent.
    baseBuffer[count++] = 'e';

    if( e < 0 ) {
      baseBuffer[count++] = '-';
      e = -e;
    }
    else {
      baseBuffer[count++] = '+';
    }

    baseBuffer[count++] = char( '0' + e / 10 );
    baseBuffer[count++] = char( '0' + e % 10 );
  }
  baseBuffer[count] = '\0';
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
  mCopy( buffer, s.buffer, size_t( count + 1 ) );
}

String::String( String&& s ) :
  count( s.count )
{
  if( s.buffer != s.baseBuffer ) {
    buffer   = s.buffer;
    s.buffer = s.baseBuffer;
  }
  else {
    buffer = baseBuffer;
    mCopy( baseBuffer, s.baseBuffer, size_t( count + 1 ) );
  }

  s.count         = 0;
  s.baseBuffer[0] = '\0';
}

String& String::operator = ( const String& s )
{
  if( &s == this ) {
    return *this;
  }

  if( buffer != baseBuffer ) {
    free( buffer );
  }

  count = s.count;
  ensureCapacity();
  mCopy( buffer, s.buffer, size_t( count + 1 ) );

  return *this;
}

String& String::operator = ( String&& s )
{
  if( &s == this ) {
    return *this;
  }

  if( buffer != baseBuffer ) {
    free( buffer );
  }

  count = s.count;

  if( s.buffer != s.baseBuffer ) {
    buffer   = s.buffer;
    s.buffer = s.baseBuffer;
  }
  else {
    buffer = baseBuffer;
    mCopy( baseBuffer, s.baseBuffer, size_t( count + 1 ) );
  }

  s.count         = 0;
  s.baseBuffer[0] = '\0';

  return *this;
}

String& String::operator = ( const char* s )
{
  if( s == buffer ) {
    return *this;
  }

  if( buffer != baseBuffer ) {
    free( buffer );
  }

  if( s == nullptr ) {
    buffer    = baseBuffer;
    count     = 0;
    buffer[0] = '\0';
  }
  else {
    count = length( s );
    ensureCapacity();
    mCopy( buffer, s, size_t( count + 1 ) );
  }

  return *this;
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
  mCopy( r.buffer, localBuffer, size_t( r.count + 1 ) );

  return r;
}

String String::create( int length, char** buffer_ )
{
  String r( length, 0 );

  *buffer_ = r.buffer;
  return r;
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

  mCopy( r.buffer, buffer, size_t( count ) );
  mCopy( r.buffer + count, s.buffer, size_t( s.count + 1 ) );

  return r;
}

String String::operator + ( const char* s ) const
{
  int    sLength = length( s );
  int    rCount  = count + sLength;
  String r       = String( rCount, 0 );

  mCopy( r.buffer, buffer, size_t( count ) );
  mCopy( r.buffer + count, s, size_t( sLength + 1 ) );

  return r;
}

String operator + ( const char* s, const String& t )
{
  int    sLength = String::length( s );
  int    rCount  = t.count + sLength;
  String r       = String( rCount, 0 );

  mCopy( r.buffer, s, size_t( sLength ) );
  mCopy( r.buffer + sLength, t.buffer, size_t( t.count + 1 ) );

  return r;
}

String& String::operator += ( const String& s )
{
  char* oBuffer = buffer;
  int   oCount  = count;

  count += s.count;
  ensureCapacity();

  if( buffer != oBuffer ) {
    mCopy( buffer, oBuffer, size_t( oCount ) );

    if( oBuffer != baseBuffer ) {
      free( oBuffer );
    }
  }
  mCopy( buffer + oCount, s, size_t( s.count + 1 ) );

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
    mCopy( buffer, oBuffer, size_t( oCount ) );

    if( oBuffer != baseBuffer ) {
      free( oBuffer );
    }
  }
  mCopy( buffer + oCount, s, size_t( sLength + 1 ) );

  return *this;
}

String String::substring( int start ) const
{
  hard_assert( 0 <= start && start <= count );

  int    rCount = count - start;
  String r      = String( rCount, 0 );

  mCopy( r.buffer, buffer + start, size_t( rCount + 1 ) );

  return r;
}

String String::substring( int start, int end ) const
{
  hard_assert( 0 <= start && start <= count && start <= end && end <= count );

  int    rCount = end - start;
  String r      = String( rCount, 0 );

  mCopy( r.buffer, buffer + start, size_t( rCount ) );
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
  int nPartitions = 0;
  int begin       = 0;
  int end         = index( buffer, delimiter );

  // Count substrings first.
  while( end >= 0 ) {
    if( begin != end ) {
      ++nPartitions;
    }

    begin = end + 1;
    end   = index( delimiter, begin );
  }
  if( begin != count ) {
    ++nPartitions;
  }

  DArray<String> array( nPartitions );

  begin = 0;
  end   = index( buffer, delimiter );

  for( int i = 0; end >= 0; ) {
    if( begin != end ) {
      array[i] = substring( begin, end );
      ++i;
    }

    begin = end + 1;
    end   = index( delimiter, begin );
  }
  if( begin != count ) {
    array[nPartitions - 1] = substring( begin );
  }

  return array;
}

String String::fileName() const
{
  int slash = lastIndex( '/' );

  return slash < 0 ? *this : substring( slash + 1 );
}

String String::fileBaseName() const
{
  int slash = lastIndex( '/' );
  int dot   = lastIndex( '.' );

  if( slash < dot ) {
    return substring( slash + 1, dot );
  }
  else {
    return substring( slash + 1 );
  }
}

String String::fileExtension() const
{
  int slash = lastIndex( '/' );
  int dot   = lastIndex( '.' );

  return slash < dot ? substring( dot + 1 ) : String();
}

bool String::fileHasExtension( const char* ext ) const
{
  const char* slash = findLast( '/' );
  const char* dot   = findLast( '.' );

  if( slash < dot ) {
    return compare( dot + 1, ext ) == 0;
  }
  else {
    return isEmpty( ext );
  }
}

}
