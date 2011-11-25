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
 * @file oz/String.hpp
 */

#pragma once

#include "DArray.hpp"

namespace oz
{

/**
 * Immutable string.
 *
 * Class has static storage of <tt>BUFFER_SIZE</tt> bytes, if string is larger it is stored in
 * a dynamically allocated storage.
 *
 * To deallocate storage just assign an empty string.
 *
 * @ingroup oz
 */
class String
{
  private:

    /// Size of static buffer.
    static const int BUFFER_SIZE = 36;

    char* buffer;                  ///< Pointer to the current buffer.
    int   count;                   ///< Length in bytes without the terminating null character.
    char  baseBuffer[BUFFER_SIZE]; ///< Static buffer.

    /**
     * Only allocate storage, do not initialise string.
     */
    explicit String( int count_, int ) : count( count_ )
    {
      ensureCapacity();
    }

    /**
     * If existing storage is too small, allocate a new one.
     */
    void ensureCapacity()
    {
      buffer = count < BUFFER_SIZE ? baseBuffer : new char[count + 1];
    }

  public:

    /**
     * Create an empty string.
     */
    String() : buffer( baseBuffer ), count( 0 )
    {
      buffer[0] = '\0';
    }

    /**
     * Destructor.
     */
    ~String()
    {
      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

      if( buffer != baseBuffer ) {
        delete[] buffer;
      }
    }

    /**
     * Copy constructor.
     */
    String( const String& s ) : count( s.count )
    {
      ensureCapacity();
      aCopy( buffer, s.buffer, count + 1 );

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Move constructor.
     */
    String( String&& s ) : count( s.count )
    {
      if( s.buffer != s.baseBuffer ) {
        buffer = s.buffer;

        s.buffer = s.baseBuffer;
        s.count = 0;
        s.baseBuffer[0] = '\0';
      }
      else {
        buffer = baseBuffer;
        aCopy( baseBuffer, s.baseBuffer, count + 1 );
      }

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Copy operator.
     *
     * Reuse existing storage only if the size matches.
     */
    String& operator = ( const String& s )
    {
      if( &s == this ) {
        soft_assert( &s != this );
        return *this;
      }

      count = s.count;

      if( buffer != baseBuffer ) {
        delete[] buffer;
      }

      ensureCapacity();
      aCopy( buffer, s.buffer, count + 1 );

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

      return *this;
    }

    /**
     * Move operator.
     */
    String& operator = ( String&& s )
    {
      if( &s == this ) {
        soft_assert( &s != this );
        return *this;
      }

      count = s.count;

      if( buffer != baseBuffer ) {
        delete[] buffer;
      }

      if( s.buffer != s.baseBuffer ) {
        buffer = s.buffer;

        s.buffer = s.baseBuffer;
        s.count = 0;
        s.baseBuffer[0] = '\0';
      }
      else {
        buffer = baseBuffer;
        aCopy( baseBuffer, s.baseBuffer, count + 1 );
      }

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

      return *this;
    }

    /**
     * Create string form the given C string with a known length.
     *
     * @param s
     * @param count_ length in bytes without the terminating null character.
     */
    explicit String( const char* s, int count_ ) : count( count_ )
    {
      hard_assert( s != null && s != baseBuffer );
      hard_assert( length( s ) >= count );

      ensureCapacity();
      aCopy( buffer, s, count );
      buffer[count] = '\0';

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Create string form the given C string.
     */
    String( const char* s )
    {
      hard_assert( s != null && s != baseBuffer );

      count = length( s );
      ensureCapacity();
      aCopy( buffer, s, count + 1 );

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Create string form a boolean value, yields "true" or "false".
     */
    explicit String( bool b ) : buffer( baseBuffer )
    {
      // some protection against too small buffers
      static_assert( BUFFER_SIZE >= 6, "String::BUFFER_SIZE too small for bool representation" );

      if( b ) {
        count = 4;

        buffer[0] = 't';
        buffer[1] = 'r';
        buffer[2] = 'u';
        buffer[3] = 'e';
        buffer[4] = '\0';
      }
      else {
        count = 5;

        buffer[0] = 'f';
        buffer[1] = 'a';
        buffer[2] = 'l';
        buffer[3] = 's';
        buffer[4] = 'e';
        buffer[5] = '\0';
      }

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Create string from an integer value.
     */
    explicit String( int n ) : buffer( baseBuffer ), count( 1 )
    {
      // that should assure enough space, since log10( 2^( 8*sizeof( int ) ) ) <= 3*sizeof( int ),
      // +2 for sign and terminating null char
      static_assert( BUFFER_SIZE >= 3 * int( sizeof( int ) ) + 2,
                     "String::BUFFER_SIZE too small for int representation" );

      // we have [sign +] first digit + remaining digits
      // since we always count first digit, we assure that we never get 0 digits (if n == 0)

      // first, we count first digit + remaining digits (count has been set to 1 in initialisation)
      int nn = n / 10;
      while( nn != 0 ) {
        nn /= 10;
        ++count;
      }

      // check if we have a negative sign
      if( n < 0 ) {
        n = -n;
        ++count;
        buffer[0] = '-';
      }

      // terminating null character
      buffer[count] = '\0';

      // we always write first digit
      buffer[count - 1] = char( '0' + ( n % 10 ) );
      n /= 10;

      for( int i = count - 2; n != 0; --i ) {
        buffer[i] = char( '0' + ( n % 10 ) );
        n /= 10;
      }

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
    }

    /**
     * Create sting from a float value.
     */
    explicit String( float f );

    /**
     * Create sting from a double value.
     */
    explicit String( double d );

    /**
     * Replace current string with the giver C string.
     *
     * Reuse existing storage only if it the size matches.
     */
    String& operator = ( const char* s )
    {
      if( s == buffer ) {
        soft_assert( s != buffer );
        return *this;
      }

      count = length( s );

      if( buffer != baseBuffer ) {
        delete[] buffer;
      }
      ensureCapacity();
      aCopy( buffer, s, count + 1 );

      hard_assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

      return *this;
    }

    /**
     * Equality.
     */
    bool operator == ( const String& s ) const
    {
      return equals( s );
    }

    /**
     * Inequality.
     */
    bool operator != ( const String& s ) const
    {
      return !equals( s );
    }

    /**
     * Equality.
     */
    bool equals( const char* s ) const
    {
      hard_assert( s != null );

      for( int i = 0; buffer[i] == s[i]; ++i ) {
        if( buffer[i] == '\0' ) {
          return true;
        }
      }
      return false;
    }

    /**
     * Equality.
     */
    bool equals( const String& s ) const
    {
      return equals( s.buffer );
    }

    /**
     * Equality.
     */
    static bool equals( const char* a, const char* b )
    {
      hard_assert( a != null && b != null );

      for( int i = 0; a[i] == b[i]; ++i ) {
        if( a[i] == '\0' ) {
          return true;
        }
      }
      return false;
    }

    /**
     * Cast to a C string.
     */
    OZ_ALWAYS_INLINE
    operator const char* () const
    {
      return buffer;
    }

    /**
     * Cast to a C string.
     */
    OZ_ALWAYS_INLINE
    const char* cstr() const
    {
      return buffer;
    }

    /**
     * Constant refernece to the i-th byte.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < count );

      return buffer[i];
    }

    /**
     * Length of a C string.
     */
    static int length( const char* s )
    {
      int i = 0;

      while( s[i] != '\0' ) {
        ++i;
      }
      return i;
    }

    /**
     * True iff the C string is empty.
     */
    static bool isEmpty( const char* s )
    {
      hard_assert( s != null );

      return s[0] == '\0';
    }

    /**
     * Length.
     */
    OZ_ALWAYS_INLINE
    int length() const
    {
      return count;
    }

    /**
     * True iff the string is empty.
     */
    OZ_ALWAYS_INLINE
    bool isEmpty() const
    {
      return count == 0;
    }

    /**
     * True iff character is an ASCII digit.
     */
    static bool isDigit( char c )
    {
      return '0' <= c && c <= '9';
    }

    /**
     * True iff character is an ASCII letter.
     */
    static bool isLetter( char c )
    {
      return ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' );
    }

    /**
     * True iff character is a space.
     */
    static bool isSpace( char c )
    {
      return c == ' ' || c == '\t';
    }

    /**
     * True iff character is a space, horizontal tab or newline.
     */
    static bool isBlank( char c )
    {
      return c == ' ' || c == '\t' || c == '\n';
    }

    /**
     * Compare two C strings per-byte (same as strcmp in \<cstring\>).
     */
    static int compare( const char* a, const char* b )
    {
      hard_assert( a != null && b != null );

      int diff = 0;
      int i = 0;

      while( ( diff = a[i] - b[i] ) == 0 && a[i] != 0 ) {
        ++i;
      }
      return diff;
    }

    /**
     * Compare with a C string per-byte.
     */
    int compare( const char* s ) const
    {
      return compare( buffer, s );
    }

    /**
     * Compare strings per-byte.
     */
    int compare( const String& s ) const
    {
      return compare( buffer, s.buffer );
    }

    /**
     * Index of the first occurrence of the character from the given index (inclusive).
     */
    int index( char ch, int start = 0 ) const
    {
      int i = start;

      while( buffer[i] != ch && buffer[i] != '\0' ) {
        ++i;
      }
      return i == count ? -1 : i;
    }

    /**
     * Index of the last occurrence of the character before the given index (inclusive).
     */
    int lastIndex( char ch, int end ) const
    {
      int i = end;

      while( buffer[i] != ch && i >= 0 ) {
        --i;
      }
      return i;
    }

    /**
     * Index of the last occurrence of the character.
     */
    int lastIndex( char ch ) const
    {
      return lastIndex( ch, count - 1 );
    }

    /**
     * Pointer to the first occurrence of the character from the given index (inclusive).
     */
    static const char* find( const char* s, char ch )
    {
      while( *s != ch && *s != '\0' ) {
        ++s;
      }
      return *s == ch ? s : null;
    }

    /**
     * Pointer to the last occurrence of the character.
     */
    static const char* findLast( const char* s, char ch )
    {
      const char* last = null;

      while( *s != '\0' ) {
        if( *s == ch ) {
          last = s;
        }
        ++s;
      }
      return last;
    }

    /**
     * Pointer to the first occurrence of the character from the given index (inclusive).
     */
    const char* find( char ch, int start = 0 ) const
    {
      const char* p = buffer + start;

      while( *p != ch && *p != '\0' ) {
        ++p;
      }
      return *p == ch ? p : null;
    }

    /**
     * Pointer to the last occurrence of the character.
     */
    const char* findLast( char ch ) const
    {
      const char* p = buffer + count - 1;

      while( *p != ch && p != buffer ) {
        --p;
      }
      return *p == ch ? p : null;
    }

    /**
     * Pointer to the first occurrence of the substring from the given index (inclusive).
     */
    const char* find( const char* str, int start ) const
    {
      const char* p = buffer + start;
      const char* begin = null;
      const char* end = str;

      while( *p != '\0' && *end != '\0' ) {
        if( *p == *end ) {
          begin = p;
          ++end;
        }
        ++p;
      }
      return *end == '\0' ? begin : null;
    }

    /**
     * Bernstein's hash function.
     *
     * @return absolute value of hash.
     */
    static int hash( const char* s )
    {
      uint hash = 5381;

      while( *s != '\0' ) {
        hash = hash * 33 + uint( *s );
        ++s;
      }
      return int( hash );
    }

    /**
     * Bernstein's hash function.
     *
     * @return absolute value of hash.
     */
    int hash() const
    {
      return hash( buffer );
    }

    /**
     * %String concatenation.
     */
    String operator + ( const char* s ) const
    {
      hard_assert( s != null );

      int    sLength = length( s );
      int    rCount  = count + sLength;
      String r       = String( rCount, 0 );

      aCopy( r.buffer, buffer, count );
      aCopy( r.buffer + count, s, sLength + 1 );

      return r;
    }

    /**
     * %String concatenation.
     */
    String operator + ( const String& s ) const
    {
      int    rCount = count + s.count;
      String r      = String( rCount, 0 );

      aCopy( r.buffer, buffer, count );
      aCopy( r.buffer + count, s.buffer, s.count + 1 );

      return r;
    }

    /**
     * %String concatenation.
     */
    friend String operator + ( const char* s, const String& t )
    {
      hard_assert( s != null );

      int    sLength = length( s );
      int    rCount  = t.count + sLength;
      String r       = String( rCount, 0 );

      aCopy( r.buffer, s, sLength );
      aCopy( r.buffer + sLength, t.buffer, t.count + 1 );

      return r;
    }

    /**
     * Substring.
     */
    String substring( int start ) const
    {
      hard_assert( 0 <= start && start <= count );

      int    rCount = count - start;
      String r      = String( rCount, 0 );

      aCopy( r.buffer, buffer + start, rCount + 1 );

      return r;
    }

    /**
     * Substring.
     */
    String substring( int start, int end ) const
    {
      hard_assert( 0 <= start && start <= count && start <= end && end <= count );

      int    rCount = end - start;
      String r      = String( rCount, 0 );

      aCopy( r.buffer, buffer + start, rCount );
      r.buffer[rCount] = '\0';

      return r;
    }

    /**
     * Create string with stripped leading and trailing blanks.
     */
    String trim() const
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

    /**
     * Create string with stripped leading and trailing blanks.
     */
    String trim( const char* s )
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

    /**
     * Create a copy that has all instances of <tt>whatChar</tt> replaced by <tt>withChar</tt>.
     */
    String replace( char whatChar, char withChar ) const
    {
      String r = String( count, 0 );

      for( int i = 0; i < count; ++i ) {
        r.buffer[i] = buffer[i] == whatChar ? withChar : buffer[i];
      }
      r.buffer[count] = '\0';

      return r;
    }

    /**
     * Create a copy that has all instances of <tt>whatChar</tt> replaced by <tt>withChar</tt>.
     */
    static String replace( const char* s, char whatChar, char withChar )
    {
      int    count = length( s );
      String r    = String( count, 0 );

      for( int i = 0; i < count; ++i ) {
        r.buffer[i] = s[i] == whatChar ? withChar : s[i];
      }
      r.buffer[count] = '\0';

      return r;
    }

    /**
     * Returns array of substrings between occurrences of the given character token.
     */
    DArray<String> split( char ch ) const
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

    /**
     * Cast signed byte string to C string.
     */
    static const char* cstr( const byte* s )
    {
      return reinterpret_cast<const char*>( s );
    }

    /**
     * Cast unsigned byte string to C string.
     */
    static const char* cstr( const ubyte* s )
    {
      return reinterpret_cast<const char*>( s );
    }

    /**
     * Cast C string to signed byte string.
     */
    static const byte* bytestr( const char* s )
    {
      return reinterpret_cast<const byte*>( s );
    }

    /**
     * Cast C string to unsigned byte string.
     */
    static const ubyte* ubytestr( const char* s )
    {
      return reinterpret_cast<const ubyte*>( s );
    }

};

}
