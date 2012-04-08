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
 * @file oz/String.hh
 *
 * String class.
 */

#pragma once

#include "DArray.hh"

namespace oz
{

/**
 * Immutable string.
 *
 * Class has static storage of <tt>BUFFER_SIZE</tt> bytes, if string is larger it is stored in
 * a dynamically allocated storage. For storage allocation <tt>std::malloc()</tt> is used, so it
 * bypasses <tt>Alloc</tt> memory manager.
 *
 * To deallocate storage just assign an empty string.
 *
 * @ingroup oz
 */
class String
{
  public:

    /**
     * Thrown by parse functions if parsing fails or completes only partially.
     */
    class ParseException : public std::exception
    {};

  private:

    /// Size of static buffer.
    static const int BUFFER_SIZE = 36;

    char* buffer;                  ///< Pointer to the current buffer.
    int   count;                   ///< Length in bytes without the terminating null character.
    char  baseBuffer[BUFFER_SIZE]; ///< Static buffer.

    /**
     * Only allocate storage, do not initialise string.
     */
    explicit String( int count_, int );

    /**
     * If existing storage is too small, allocate a new one.
     */
    void ensureCapacity();

    /**
     * Deallocate storage (needed by destructor to avoid including <tt>\<cstdlib\></tt>).
     */
    void dealloc();

  public:

    /*
     * C string functions.
     */

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
     * Compare two C strings per-byte (same as <tt>strcmp()</tt>).
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
    OZ_ALWAYS_INLINE
    static bool isEmpty( const char* s )
    {
      hard_assert( s != null );

      return s[0] == '\0';
    }

    /**
     * Index of the first occurrence of the character from the given index (inclusive).
     */
    static int index( const char* s, char ch, int start = 0 )
    {
      for( int i = start; s[i] != '\0'; ++i ) {
        if( s[i] == ch ) {
          return i;
        }
      }
      return -1;
    }

    /**
     * Index of the last occurrence of the character before the given index (not inclusive).
     */
    static int lastIndex( const char* s, char ch, int end )
    {
      for( int i = end - 1; i >= 0; --i ) {
        if( s[i] == ch ) {
          return i;
        }
      }
      return -1;
    }

    /**
     * Index of the last occurrence of the character.
     */
    static int lastIndex( const char* s, char ch )
    {
      int last = -1;

      for( int i = 0; s[i] != '\0'; ++i ) {
        if( s[i] == ch ) {
          last = i;
        }
      }
      return last;
    }

    /**
     * Pointer to the first occurrence of the character from the given index (inclusive).
     */
    static const char* find( const char* s, char ch, int start = 0 )
    {
      for( const char* p = s + start; *p != '\0'; ++p ) {
        if( *p == ch ) {
          return p;
        }
      }
      return null;
    }

    /**
     * Pointer to the last occurrence of the character before the given index (not inclusive).
     */
    static const char* findLast( const char* s, char ch, int end )
    {
      for( const char* p = s + end - 1; p >= s; --p ) {
        if( *p == ch ) {
          return p;
        }
      }
      return null;
    }

    /**
     * Pointer to the last occurrence of the character.
     */
    static const char* findLast( const char* s, char ch )
    {
      const char* last = null;

      for( const char* p = s; *p != '\0'; ++p ) {
        if( *p == ch ) {
          last = p;
        }
      }
      return last;
    }

    /**
     * True iff string begins with the given characters.
     */
    static bool beginsWith( const char* s, const char* sub )
    {
      while( *sub != '\0' && *sub == *s ) {
        ++sub;
        ++s;
      }
      return *sub == '\0';
    }

    /**
     * True iff string ends with the given characters.
     */
    static bool endsWith( const char* s, const char* sub );

    /**
     * Bernstein's hash function.
     *
     * @return Absolute value of hash.
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
     * True iff character is an ASCII digit.
     */
    OZ_ALWAYS_INLINE
    static bool isDigit( char c )
    {
      return '0' <= c && c <= '9';
    }

    /**
     * True iff character is an ASCII letter.
     */
    OZ_ALWAYS_INLINE
    static bool isLetter( char c )
    {
      return ( 'A' <= c && c <= 'Z' ) || ( 'a' <= c && c <= 'z' );
    }

    /**
     * True iff character is a space.
     */
    OZ_ALWAYS_INLINE
    static bool isSpace( char c )
    {
      return c == ' ' || c == '\t';
    }

    /**
     * True iff character is a space, horizontal tab or newline.
     */
    OZ_ALWAYS_INLINE
    static bool isBlank( char c )
    {
      return c == ' ' || c == '\t' || c == '\n';
    }

    /**
     * Cast signed byte string to C string.
     */
    OZ_ALWAYS_INLINE
    static const char* cstr( const byte* s )
    {
      return reinterpret_cast<const char*>( s );
    }

    /**
     * Cast unsigned byte string to C string.
     */
    OZ_ALWAYS_INLINE
    static const char* cstr( const ubyte* s )
    {
      return reinterpret_cast<const char*>( s );
    }

    /**
     * Cast C string to signed byte string.
     */
    OZ_ALWAYS_INLINE
    static const byte* bytestr( const char* s )
    {
      return reinterpret_cast<const byte*>( s );
    }

    /**
     * Cast C string to unsigned byte string.
     */
    OZ_ALWAYS_INLINE
    static const ubyte* ubytestr( const char* s )
    {
      return reinterpret_cast<const ubyte*>( s );
    }

    /**
     * Parse boolean value (must exactly match either "true" or "false").
     *
     * If parsing fails, a ParseException is thrown.
     */
    static bool parseBool( const char* s );

    /**
     * Parse integer value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    static int parseInt( const char* s );

    /**
     * Parse float value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    static float parseFloat( const char* s );

    /**
     * Parse double value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    static double parseDouble( const char* s );

    /*
     * Functions that operate on a String object.
     */

    /**
     * Create an empty string.
     */
    String() :
      buffer( baseBuffer ), count( 0 )
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
        dealloc();
      }
    }

    /**
     * Copy constructor.
     */
    String( const String& s );

    /**
     * Move constructor.
     */
    String( String&& s );

    /**
     * Copy operator.
     *
     * Reuse existing storage only if the size matches.
     */
    String& operator = ( const String& s );

    /**
     * Move operator.
     */
    String& operator = ( String&& s );

    /**
     * Create string form the given C string with a known length.
     *
     * @param s
     * @param count_ length in bytes without the terminating null character.
     */
    explicit String( int count_, const char* s );

    /**
     * Create string form the given C string.
     */
    String( const char* s );

    /**
     * Create string form a boolean value, yields "true" or "false".
     */
    explicit String( bool b );

    /**
     * Create string from an integer value.
     */
    explicit String( int i );

    /**
     * Create string from a float value.
     */
    explicit String( float f );

    /**
     * Create string from a double value.
     */
    explicit String( double d );

    /**
     * Create a string in sprintf-like way.
     */
    OZ_PRINTF_FORMAT( 1, 2 )
    static String str( const char* s, ... );

    /**
     * Create uninitialised string.
     *
     * Terminating null character is written, so it represents a valid string even if not
     * initialised manually by the caller.
     *
     * @param length length of string (without the terminating null character).
     * @param buffer where to pass non-constant pointer to the internal buffer.
     */
    static String create( int length, char** buffer );

    /**
     * Create a copy that has all instances of <tt>whatChar</tt> replaced by <tt>withChar</tt>.
     */
    static String replace( const char* s, char whatChar, char withChar );

    /**
     * Replace current string with the giver C string.
     *
     * Reuse existing storage only if it the size matches.
     */
    String& operator = ( const char* s );

    /**
     * Equality.
     */
    bool operator == ( const String& s ) const
    {
      return equals( buffer, s.buffer );
    }

    /**
     * Equality.
     */
    bool operator == ( const char* s ) const
    {
      return equals( buffer, s );
    }

    /**
     * Equality.
     */
    friend bool operator == ( const char* a, const String& b )
    {
      return equals( a, b.buffer );
    }

    /**
     * Inequality.
     */
    bool operator != ( const String& s ) const
    {
      return !equals( buffer, s.buffer );
    }

    /**
     * Inequality.
     */
    bool operator != ( const char* s ) const
    {
      return !equals( buffer, s );
    }

    /**
     * Inequality.
     */
    friend bool operator != ( const char* a, const String& b )
    {
      return !equals( a, b.buffer );
    }

    /**
     * Operator <=.
     */
    bool operator <= ( const String& s ) const
    {
      return compare( buffer, s.buffer ) <= 0;
    }

    /**
     * Operator <=.
     */
    bool operator <= ( const char* s ) const
    {
      return compare( buffer, s ) <= 0;
    }

    /**
     * Operator <=.
     */
    friend bool operator <= ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) <= 0;
    }

    /**
     * Operator >=.
     */
    bool operator >= ( const String& s ) const
    {
      return compare( buffer, s.buffer ) >= 0;
    }

    /**
     * Operator >=.
     */
    bool operator >= ( const char* s ) const
    {
      return compare( buffer, s ) >= 0;
    }

    /**
     * Operator >=.
     */
    friend bool operator >= ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) >= 0;
    }

    /**
     * Operator <.
     */
    bool operator < ( const String& s ) const
    {
      return compare( buffer, s.buffer ) < 0;
    }

    /**
     * Operator <.
     */
    bool operator < ( const char* s ) const
    {
      return compare( buffer, s ) < 0;
    }

    /**
     * Operator <.
     */
    friend bool operator < ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) < 0;
    }

    /**
     * Operator >.
     */
    bool operator > ( const String& s ) const
    {
      return compare( buffer, s.buffer ) > 0;
    }

    /**
     * Operator >.
     */
    bool operator > ( const char* s ) const
    {
      return compare( buffer, s ) > 0;
    }

    /**
     * Operator >.
     */
    friend bool operator > ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) > 0;
    }

    /**
     * Equality.
     */
    bool equals( const String& s ) const
    {
      return equals( buffer, s.buffer );
    }

    /**
     * Equality.
     */
    bool equals( const char* s ) const
    {
      return equals( buffer, s );
    }

    /**
     * Compare strings per-byte.
     */
    int compare( const String& s ) const
    {
      return compare( buffer, s.buffer );
    }

    /**
     * Compare strings per-byte.
     */
    int compare( const char* s ) const
    {
      return compare( buffer, s );
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
     * Constant refernece to the i-th byte.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i < count );

      return buffer[i];
    }

    /**
     * First character or null character if empty string.
     */
    char first() const
    {
      return buffer[0];
    }

    /**
     * Last character or null character if empty string.
     */
    char last() const
    {
      return count == 0 ? '\0' : buffer[count - 1];
    }

    /**
     * Index of the first occurrence of the character from the given index (inclusive).
     */
    int index( char ch, int start = 0 ) const
    {
      return index( buffer, ch, start );
    }

    /**
     * Index of the last occurrence of the character before the given index (not inclusive).
     */
    int lastIndex( char ch, int end ) const
    {
      return lastIndex( buffer, ch, end );
    }

    /**
     * Index of the last occurrence of the character.
     */
    int lastIndex( char ch ) const
    {
      return lastIndex( buffer, ch, count );
    }

    /**
     * Pointer to the first occurrence of the character from the given index (inclusive).
     */
    const char* find( char ch, int start = 0 ) const
    {
      return find( buffer, ch, start );
    }

    /**
     * Pointer to the last occurrence of the character before the given index (not inclusive).
     */
    const char* findLast( char ch, int end ) const
    {
      return findLast( buffer, ch, end );
    }

    /**
     * Pointer to the last occurrence of the character.
     */
    const char* findLast( char ch ) const
    {
      return findLast( buffer, ch, count );
    }

    /**
     * True iff string begins with the given characters.
     */
    bool beginsWith( const char* sub ) const
    {
      return beginsWith( buffer, sub );
    }

    /**
     * True iff string ends with the given characters.
     */
    bool endsWith( const char* sub ) const;

    /**
     * Bernstein's hash function.
     *
     * @return Absolute value of hash.
     */
    int hash() const
    {
      return hash( buffer );
    }

    /**
     * Parse boolean value (must exactly match either "true" or "false").
     *
     * If parsing fails, a ParseException is thrown.
     */
    bool parseBool() const
    {
      return parseBool( buffer );
    }

    /**
     * Parse integer value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    int parseInt() const
    {
      return parseInt( buffer );
    }

    /**
     * Parse float value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    float parseFloat() const
    {
      return parseFloat( buffer );
    }

    /**
     * Parse double value.
     *
     * If parsing fails, a ParseException is thrown.
     */
    double parseDouble() const
    {
      return parseDouble( buffer );
    }

    /**
     * %String concatenation.
     */
    String operator + ( const char* s ) const;

    /**
     * %String concatenation.
     */
    String operator + ( const String& s ) const;

    /**
     * %String concatenation.
     */
    friend String operator + ( const char* s, const String& t );

    /**
     * Substring from <tt>start</tt> character (inclusively).
     */
    String substring( int start ) const;

    /**
     * Substring between <tt>start</tt> (inclusively) and <tt>end</tt> (not inclusively) character.
     */
    String substring( int start, int end ) const;

    /**
     * Create string with stripped leading and trailing blanks.
     */
    String trim() const;

    /**
     * Create string with stripped leading and trailing blanks.
     */
    String trim( const char* s );

    /**
     * Create a copy that has all instances of <tt>whatChar</tt> replaced by <tt>withChar</tt>.
     */
    String replace( char whatChar, char withChar ) const;

    /**
     * Returns array of substrings between occurrences of the given character token.
     */
    DArray<String> split( char ch ) const;

};

}
