/*
 *  String.h
 *
 *  Immutable string object (similar to Java String)
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

namespace oz
{

  class String
  {
    private:

      static const int BUFFER_SIZE = 64;

      char *buffer;
      int  count;
      char baseBuffer[BUFFER_SIZE];

      String( int count_, int ) : count( count_ )
      {
        ensureCapacity();
      }

      // if existing buffer is to small, allocate a new one
      void ensureCapacity()
      {
        buffer = count < BUFFER_SIZE ? baseBuffer : new char[count + 1];
      }

    public:

      String() : buffer( baseBuffer ), count( 0 )
      {
        buffer[0] = '\0';
      }

      String( const String &s ) : count( s.count )
      {
        ensureCapacity();
        aCopy( buffer, s.buffer, count + 1 );
      }

      String( const char *s )
      {
        assert( s != null );

        count = length( s );
        ensureCapacity();
        aCopy( buffer, s, count + 1 );
      }

      String( bool b ) : buffer( baseBuffer )
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
      }

      String( int n ) : buffer( baseBuffer ), count( 1 )
      {
        // that should assure enough space, since log10( 2^( 8*sizeof(int) ) ) <= 3*sizeof(int),
        // +2 for sign and terminating null char
        assert( BUFFER_SIZE >= 3 * (int) sizeof( int ) + 2 );

        // we have [sign +] first digit + remaining digits
        // since we always count first digit, we assure that we never get 0 digits (if n == 0)

        // first, we count first digit + remaining digits (count has been set to 1 in initialization)
        int nn = n / 10;
        while( nn != 0 ) {
          nn /= 10;
          count++;
        }

        // check if we have a negative sign
        if( n < 0 ) {
          n = -n;
          count++;
          buffer[0] = '-';
        }

        // terminating null character
        buffer[count] = '\0';

        // we always write first digit
        buffer[count - 1] = '0' + ( n % 10 );
        n /= 10;

        for( int i = count - 2; n != 0; i-- ) {
          buffer[i] = '0' + ( n % 10 );
          n /= 10;
        }
      }

      String( float f );
      String( double d );

      ~String()
      {
        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
      }

      operator const char* () const
      {
        return (const char*) buffer;
      }

      const char *cstr() const
      {
        return (const char*) buffer;
      }

      String &operator = ( const char *s )
      {
        count = length( s );

        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
        ensureCapacity();
        aCopy( buffer, s, count + 1 );

        return *this;
      }

      String &operator = ( const String &s )
      {
        count = s.count;

        if( buffer != baseBuffer ) {
          delete[] buffer;
        }
        ensureCapacity();
        aCopy( buffer, s.buffer, count + 1 );

        return *this;
      }

      bool operator == ( const char *s ) const
      {
        assert( s != null );

        for( int i = 0; buffer[i] == s[i]; i++ ) {
          if( buffer[i] == '\0' ) {
            return true;
          }
        }
        return false;
      }

      bool operator != ( const char *s ) const
      {
        assert( s != null );

        for( int i = 0; buffer[i] == s[i]; i++ ) {
          if( buffer[i] == '\0' ) {
            return false;
          }
        }
        return true;
      }

      bool operator == ( const String &s ) const
      {
        return this->operator == ( s.buffer );
      }

      bool operator != ( const String &s ) const
      {
        return this->operator != ( s.buffer );
      }

      friend bool operator == ( const char *a, const String &b )
      {
        return b.operator == ( a );
      }

      friend bool operator != ( const char *a, const String &b )
      {
        return b.operator != ( a );
      }

      static bool equals( const char *a, const char *b )
      {
        assert( a != null && b != null );

        for( int i = 0; a[i] == b[i]; i++ ) {
          if( a[i] == '\0' ) {
            return true;
          }
        }
        return false;
      }

      const char &operator [] ( int i ) const
      {
        assert( 0 <= i && i < count );

        return buffer[i];
      }

      const char &charAt( int i ) const
      {
        assert( 0 <= i && i < count );

        return buffer[i];
      }

      static int length( const char *s )
      {
        int i = 0;

        while( s[i] != '\0' ) {
          i++;
        }
        return i;
      }

      int length() const
      {
        return count;
      }

      static int compare( const char *a, const char *b )
      {
        assert( a != null && b != null );

        int diff = 0;
        for( int i = 0; ( diff = a[i] - b[i] ) == 0 && a[i] != 0; i++ ) {
        }

        return diff;
      }

      int compare( const char *s ) const
      {
        return compare( buffer, s );
      }

      int compare( const String &s ) const
      {
        return compare( buffer, s.buffer );
      }

      int index( char ch, int start = 0 ) const
      {
        int i = start;

        while( buffer[i] != ch && i < count ) {
          i++;
        }
        return i == count ? -1 : i;
      }

      int lastIndex( char ch ) const
      {
        int i = count - 1;

        while( buffer[i] != ch && i >= 0 ) {
          i--;
        }
        return i;
      }

      int lastIndex( char ch, int end ) const
      {
        int i = end;

        while( buffer[i] != ch && i >= 0 ) {
          i--;
        }
        return i;
      }

      /**
       * Bernstein's hash function.
       * @param s
       * @return absolute value of hash
       */
      static uint hash( const char *s )
      {
        uint hash = 5381;
        int count = length( s );

        for( int i = 0; i < count; i++ ) {
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

      String operator + ( const char *s ) const
      {
        assert( s != null );

        int    sLength = length( s );
        int    rCount  = count + sLength;
        String r       = String( rCount, 0 );

        aCopy( r.buffer, buffer, count );
        aCopy( r.buffer + count, s, sLength + 1 );

        return r;
      }

      String operator + ( const String &s ) const
      {
        int    rCount = count + s.count;
        String r      = String( rCount, 0 );

        aCopy( r.buffer, buffer, count );
        aCopy( r.buffer + count, s.buffer, s.count + 1 );

        return r;
      }

      friend String operator + ( const char *s, const String &t )
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

      Vector<String> split( char ch ) const
      {
        Vector<String> v;

        int p0 = 0;
        int p1 = index( ch );

        while( p1 >= 0 ) {
          v << substring( p0, p1 );
          p0 = p1 + 1;
          p1 = index( ch, p0 );
        }
        v << substring( p0 );
        return v;
      }

  };

}
