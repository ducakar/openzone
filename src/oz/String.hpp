/*
 *  String.hpp
 *
 *  Immutable string object (similar to Java String)
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

namespace oz
{

  class String
  {
    private:

      static const int BUFFER_SIZE = 64;

      char* buffer;
      int   count;
      char  baseBuffer[BUFFER_SIZE];

      // no equality operators, String::equals functions should be used instead for verbosity
      bool operator == ( const String& ) const;
      bool operator != ( const String& ) const;
      bool operator == ( const char* ) const;
      bool operator != ( const char* ) const;
      friend bool operator == ( const char*, const String& );
      friend bool operator != ( const char*, const String& );

      explicit String( int count_, int ) : count( count_ )
      {
        ensureCapacity();
      }

      // if existing buffer is to small, allocate a new one
      void ensureCapacity()
      {
        buffer = count < BUFFER_SIZE ? baseBuffer : new char[count + 1];
      }

    public:

      explicit String() : buffer( baseBuffer ), count( 0 )
      {
        buffer[0] = '\0';
      }

      explicit String( const char* s, int count_ ) : count( count_ )
      {
        assert( s != null && s != baseBuffer );
        assert( length( s ) >= count );

        ensureCapacity();
        aCopy( buffer, s, count );
        buffer[count] = '\0';

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
      }

      String( const char* s )
      {
        assert( s != null && s != baseBuffer );

        count = length( s );
        ensureCapacity();
        aCopy( buffer, s, count + 1 );

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
      }

      explicit String( bool b ) : buffer( baseBuffer )
      {
        // some protection against too small buffers
        assert( BUFFER_SIZE >= 6 );

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

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
      }

      explicit String( int n ) : buffer( baseBuffer ), count( 1 )
      {
        // that should assure enough space, since log10( 2^( 8*sizeof( int ) ) ) <= 3*sizeof( int ),
        // +2 for sign and terminating null char
        assert( BUFFER_SIZE >= 3 * int( sizeof( int ) ) + 2 );

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

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
      }

      explicit String( float f );
      explicit String( double d );

      String( const String& s ) : count( s.count )
      {
        assert( &s != this );

        ensureCapacity();
        aCopy( buffer, s.buffer, count + 1 );

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );
      }

      ~String()
      {
        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
      }

      operator const char* () const
      {
        return buffer;
      }

      const char* cstr() const
      {
        return buffer;
      }

      String& operator = ( const char* s )
      {
        assert( s != buffer );

        count = length( s );

        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
        ensureCapacity();
        aCopy( buffer, s, count + 1 );

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

        return *this;
      }

      String& operator = ( const String& s )
      {
        assert( &s != this );

        count = s.count;

        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
        ensureCapacity();
        aCopy( buffer, s.buffer, count + 1 );

        assert( ( buffer == baseBuffer ) == ( count < BUFFER_SIZE ) );

        return *this;
      }

      bool equals( const char* s ) const
      {
        assert( s != null );

        for( int i = 0; buffer[i] == s[i]; ++i ) {
          if( buffer[i] == '\0' ) {
            return true;
          }
        }
        return false;
      }

      bool equals( const String& s ) const
      {
        return equals( s.buffer );
      }

      static bool equals( const char* a, const char* b )
      {
        assert( a != null && b != null );

        for( int i = 0; a[i] == b[i]; ++i ) {
          if( a[i] == '\0' ) {
            return true;
          }
        }
        return false;
      }

      const char& operator [] ( int i ) const
      {
        assert( 0 <= i && i < count );

        return buffer[i];
      }

      static bool isEmpty( const char* s )
      {
        assert( s != null );

        return s[0] == '\0';
      }

      static int length( const char* s )
      {
        int i = 0;

        while( s[i] != '\0' ) {
          ++i;
        }
        return i;
      }

      bool isEmpty() const
      {
        return count == 0;
      }

      int length() const
      {
        return count;
      }

      static int compare( const char* a, const char* b )
      {
        assert( a != null && b != null );

        int diff = 0;
        int i = 0;

        while( ( diff = a[i] - b[i] ) == 0 && a[i] != 0 ) {
          ++i;
        }
        return diff;
      }

      int compare( const char* s ) const
      {
        return compare( buffer, s );
      }

      int compare( const String& s ) const
      {
        return compare( buffer, s.buffer );
      }

      int index( char ch, int start = 0 ) const
      {
        int i = start;

        while( buffer[i] != ch && buffer[i] != '\0' ) {
          ++i;
        }
        return i == count ? -1 : i;
      }

      int lastIndex( char ch, int end ) const
      {
        int i = end;

        while( buffer[i] != ch && i >= 0 ) {
          --i;
        }
        return i;
      }

      int lastIndex( char ch ) const
      {
        return lastIndex( ch, count - 1 );
      }

      const char* find( char ch, int start = 0 ) const
      {
        return &buffer[index( ch, start )];
      }

      const char* findLast( char ch, int end ) const
      {
        return &buffer[lastIndex( ch, end )];
      }

      const char* findLast( char ch ) const
      {
        return findLast( ch, count - 1 );
      }

      static const char* find( const char* s, char ch )
      {
        while( *s != '\0' ) {
          if( *s == ch ) {
            return s;
          }
          ++s;
        }
        return null;
      }

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
       * Bernstein's hash function.
       * @param s
       * @return absolute value of hash
       */
      static uint hash( const char* s )
      {
        uint hash = 5381;
        int count = length( s );

        for( int i = 0; i < count; ++i ) {
          hash = hash * 33 + s[i];
        }
        // absolute value
        return hash;
      }

      /**
       * Bernstein's hash function.
       * @return absolute value of hash
       */
      uint hash() const
      {
        return hash( buffer );
      }

      String operator + ( const char* s ) const
      {
        assert( s != null );

        int    sLength = length( s );
        int    rCount  = count + sLength;
        String r       = String( rCount, 0 );

        aCopy( r.buffer, buffer, count );
        aCopy( r.buffer + count, s, sLength + 1 );

        return r;
      }

      String operator + ( const String& s ) const
      {
        int    rCount = count + s.count;
        String r      = String( rCount, 0 );

        aCopy( r.buffer, buffer, count );
        aCopy( r.buffer + count, s.buffer, s.count + 1 );

        return r;
      }

      friend String operator + ( const char* s, const String& t )
      {
        assert( s != null );

        int    sLength = String::length( s );
        int    rCount  = t.count + sLength;
        String r       = String( rCount, 0 );

        aCopy( r.buffer, s, sLength );
        aCopy( r.buffer + sLength, t.buffer, t.count + 1 );

        return r;
      }

      String substring( int start ) const
      {
        assert( 0 <= start && start <= count );

        int    rCount = count - start;
        String r      = String( rCount, 0 );

        aCopy( r.buffer, buffer + start, rCount + 1 );

        return r;
      }

      String substring( int start, int end ) const
      {
        assert( 0 <= start && start <= count && start <= end && end <= count );

        int    rCount = end - start;
        String r      = String( rCount, 0 );

        aCopy( r.buffer, buffer + start, rCount );
        r.buffer[rCount] = '\0';

        return r;
      }

      const String trim() const
      {
        const char* start = buffer;
        const char* end = buffer + count;

        while( start < end && isBlank( *start ) ) {
          ++start;
        }
        while( start < end && isBlank( *( end - 1 ) ) ) {
          --end;
        }
        return String( start, end - start );
      }

      static String trim( const char* s )
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
        return String( start, end - start );
      }

      /**
       * Returns array of substrings between occurrences of given character
       * @param ch split at that characket
       * @return
       */
      void split( char ch, DArray<String>& array ) const
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

        array( count );
        p0 = 0;
        p1 = index( ch );

        while( p1 >= 0 ) {
          array[i] = substring( p0, p1 );
          p0 = p1 + 1;
          p1 = index( ch, p0 );
          ++i;
        }
        array[i] = substring( p0 );
      }

      static bool isDigit( char c )
      {
        return '0' <= c && c <= '9';
      }

      static bool isLetter( char c )
      {
        return ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' );
      }

      static bool isSpace( char c )
      {
        return c == ' ' || c == '\t';
      }

      static bool isBlank( char c )
      {
        return c == ' ' || c == '\t' || c == '\n';
      }

  };

}
