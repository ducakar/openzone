/*
 * liboz - OpenZone Core Library.
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
 * Class has static storage of `BUFFER_SIZE` bytes, if string is larger it is stored in a
 * dynamically allocated storage. For storage allocation `std::malloc()` is used, so it bypasses
 * `Alloc` memory manager.
 *
 * To deallocate storage just assign an empty string.
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
    explicit String( int count );

    /**
     * If existing storage is too small, allocate a new one.
     */
    void ensureCapacity();

  public:

    /*
     * C string functions.
     */

    /**
     * Equality.
     */
    static bool equals( const char* a, const char* b )
    {
      return compare( a, b ) == 0;
    }

    /**
     * Compare two C strings per-byte (same as `strcmp()`).
     */
    static int compare( const char* a, const char* b )
    {
      hard_assert( a != nullptr && b != nullptr );

      int diff;

      while( ( diff = *a - *b ) == 0 && *a != '\0' ) {
        ++a;
        ++b;
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
      hard_assert( s != nullptr );

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
      return nullptr;
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
      return nullptr;
    }

    /**
     * Pointer to the last occurrence of the character.
     */
    static const char* findLast( const char* s, char ch )
    {
      const char* last = nullptr;

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
      return c == ' ' || c == '\t' || c == '\n' || c == '\r';
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
     * Create string form the given C string with a known length.
     *
     * @param s C string.
     * @param count length in bytes without the terminating null character.
     */
    String( const char* s, int count );

    /**
     * Create string form the given C string.
     */
    String( const char* s );

    /**
     * Destructor.
     */
    ~String();

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
     * Replace current string with the giver C string.
     *
     * Reuse existing storage only if it the size matches.
     */
    String& operator = ( const char* s );

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
     * Create a copy that has all instances of `whatChar` replaced by `withChar`.
     */
    static String replace( const char* s, char whatChar, char withChar );

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
      return compare( buffer, s ) == 0;
    }

    /**
     * Equality.
     */
    friend bool operator == ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) == 0;
    }

    /**
     * Inequality.
     */
    bool operator != ( const String& s ) const
    {
      return compare( buffer, s.buffer ) != 0;
    }

    /**
     * Inequality.
     */
    bool operator != ( const char* s ) const
    {
      return compare( buffer, s ) != 0;
    }

    /**
     * Inequality.
     */
    friend bool operator != ( const char* a, const String& b )
    {
      return compare( a, b.buffer ) != 0;
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
      return compare( buffer, s.buffer ) == 0;
    }

    /**
     * Equality.
     */
    bool equals( const char* s ) const
    {
      return compare( buffer, s ) == 0;
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
     * Constant refernece to the `i`-th byte.
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
    OZ_ALWAYS_INLINE
    char first() const
    {
      return buffer[0];
    }

    /**
     * Last character or null character if empty string.
     */
    OZ_ALWAYS_INLINE
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
     * Create concatenated string.
     */
    String operator + ( const String& s ) const;

    /**
     * Create concatenated string.
     */
    String operator + ( const char* s ) const;

    /**
     * Create concatenated string.
     */
    friend String operator + ( const char* s, const String& t );

    /**
     * Replace with concatenated string.
     */
    String& operator += ( const String& s );

    /**
     * Replace with concatenated string.
     */
    String& operator += ( const char* s );

    /**
     * Substring from `start` character (inclusively).
     */
    String substring( int start ) const;

    /**
     * Substring between `start` (inclusively) and `end` (not inclusively) character.
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
     * Create a copy that has all instances of `whatChar` replaced by `withChar`.
     */
    String replace( char whatChar, char withChar ) const;

    /**
     * Returns array of substrings between occurrences of the given character delimiter.
     */
    DArray<String> split( char delimiter ) const;

    /**
     * Extract file name from a path (substring after the last `/`).
     */
    String fileName() const;

    /**
     * Extract base file name from a path (substring after the last `/` till the last following
     * dot).
     */
    String fileBaseName() const;

    /**
     * Extract file extension from a path (substring after the last dot in file name extracted form
     * the path).
     */
    String fileExtension() const;

    /**
     * True iff file name extracted from a path has the given extension.
     */
    bool hasFileExtension( const char* ext ) const;

};

}
