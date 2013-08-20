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
 * @file ozCore/String.hh
 *
 * `String` class.
 */

#pragma once

#include "DArray.hh"

namespace oz
{

/**
 * Immutable string.
 *
 * Class has static storage of `BUFFER_SIZE` bytes, if string is larger it is stored in a
 * dynamically allocated storage. To deallocate storage just assign an empty string.
 *
 * For storage allocation `malloc()` is used, so it bypasses `Alloc` memory manager. However, it
 * still updates `Alloc::amount` & other members but those allocations are not tracked when built
 * with `OZ_TRACK_ALLOCS`.
 */
class String
{
  private:

    /// Size of static buffer.
    static const int BUFFER_SIZE = 48 - sizeof( char* ) - sizeof( int );

    char* buffer;                  ///< Pointer to the current buffer.
    int   count;                   ///< Length in bytes without the terminating null character.
    char  baseBuffer[BUFFER_SIZE]; ///< Static buffer.

    /**
     * Only allocate storage, do not initialise string.
     */
    explicit String( int count, int );

    /**
     * Resize storage if necessary and set `count` to `newCount`.
     */
    void ensureCapacity( int newCount );

  public:

    /*
     * C string functions.
     */

    /**
     * Compare two C strings per-byte (same as `strcmp()`).
     */
    static int compare( const char* a, const char* b )
    {
      int diff;

      while( ( diff = *a - *b ) == 0 && *a != '\0' ) {
        ++a;
        ++b;
      }
      return diff;
    }

    /**
     * Equality.
     */
    static bool equals( const char* a, const char* b )
    {
      return compare( a, b ) == 0;
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
      return s[0] == '\0';
    }

    /**
     * FNV hash function, slower but has better distribution than Bernstein's `oz::hash()`.
     */
    static int strongHash( const char* s )
    {
      int value = int( 2166136261 );

      while( *s != '\0' ) {
        value = ( value * 16777619 ) ^ *s;
        ++s;
      }
      return value;
    }

    /**
     * First character or null character if an empty string.
     */
    OZ_ALWAYS_INLINE
    static char first( const char* s )
    {
      return s[0];
    }

    /**
     * Last character or null character if an empty string.
     */
    static char last( const char* s )
    {
      int sCount = length( s );
      return sCount == 0 ? '\0' : s[sCount - 1];
    }

    /**
     * Index of the first occurrence of a character from a given index (inclusive).
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
     * Index of the last occurrence of a character before a given index (not inclusive).
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
     * Index of the last occurrence of a character.
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
     * Pointer to the first occurrence of a character from a given index (inclusive).
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
     * Pointer to the last occurrence of a character before a given index (not inclusive).
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
     * Pointer to the last occurrence of a character.
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
     * True iff string begins with given characters.
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
     * True iff string ends with given characters.
     */
    static bool endsWith( const char* s, const char* sub );

    /**
     * Substring from `start` character (inclusively).
     */
    static String substring( const char* s, int start );

    /**
     * Substring between `start` (inclusively) and `end` (not inclusively) character.
     */
    static String substring( const char* s, int start, int end );

    /**
     * Create string with stripped leading and trailing blanks.
     */
    static String trim( const char* s );

    /**
     * Create a copy that has all instances of `whatChar` replaced by `withChar`.
     */
    static String replace( const char* s, char whatChar, char withChar );

    /**
     * Return array of substrings between occurrences of a given delimiter.
     *
     * Empty strings between two immediate delimiter occurrences or between a delimiter and
     * beginning/end of the original string are included.
     */
    static DArray<String> split( const char* s, char delimiter );

    /**
     * True iff file path is empty (i.e. an empty string or "@").
     */
    OZ_ALWAYS_INLINE
    static bool fileIsEmpty( const char* s )
    {
      return s[0] == '\0' || ( s[1] == '\0' && s[0] == '@' );
    }

    /**
     * True iff file path is a VFS file path.
     */
    OZ_ALWAYS_INLINE
    static bool fileIsVirtual( const char* s )
    {
      return s[0] == '@';
    }

    /**
     * Extract directory from a path (substring before the last `/`).
     */
    static String fileDirectory( const char* s );

    /**
     * Extract file name from a path (substring after the last `/`).
     */
    static String fileName( const char* s );

    /**
     * Extract base file name from a path (substring after the last `/` till the last dot following
     * it).
     */
    static String fileBaseName( const char* s );

    /**
     * Extract file extension from the path (substring after the last dot in file name or "" if no
     * extension).
     */
    static String fileExtension( const char* s );

    /**
     * True iff file name has a given extension.
     *
     * Empty string matches both no extension and files names ending with dot.
     */
    static bool fileHasExtension( const char* s, const char* ext );

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
     * Parse boolean value.
     *
     * @param s string to parse.
     * @param end if not nullptr, it is set to the first character after the successfully parsed
     * string. Thus, if parsing fails `end` value equals `s`.
     * @return true if "true", false if "false" or anything else (parsing fails).
     */
    static bool parseBool( const char* s, const char** end = nullptr );

    /**
     * Parse integer value.
     *
     * Signed integer string the should match regular expression `-?(0|[1-9][0-9]*)`.
     *
     * @param s string to parse.
     * @param end if not nullptr, it is set to the first character after the successfully parsed
     * string. Thus, if parsing fails `end` value equals `s`.
     * @return parsed integer value, 0 if parsing fails.
     */
    static int parseInt( const char* s, const char** end = nullptr );

    /**
     * Parse floating-point value.
     *
     * Double-precision floating-point value should match regular expression `-?inf|nan` or
     * `-?(0|(1-9][0-9]*)(\\.[0-9]+)?((e|E)(-?[0-9]+))` (JSON number format).
     *
     * @note
     * Only single-precision accuracy is guaranteed. Rounding errors as big as 3.33e-16 are known to
     * occur.
     *
     * @param s string to parse.
     * @param end if not nullptr, it is set to the first character after the successfully parsed
     * string. Thus, if parsing fails `end` value equals `s`.
     * @return parsed value, NaN if parsing fails.
     */
    static double parseDouble( const char* s, const char** end = nullptr );

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
     * Create string form a given C string with a known length.
     *
     * @param s C string.
     * @param count length in bytes without the terminating null character.
     */
    String( const char* s, int count );

    /**
     * Create string form a given C string (nullptr is permitted, equals "").
     */
    String( const char* s );

    /**
     * Create string by concatenating given two C strings.
     */
    String( const char* s, const char* t );

    /**
     * Create either "true" or "false" string.
     */
    explicit String( bool b );

    /**
     * Create a string representing a given int value.
     */
    explicit String( int i );

    /**
     * Create a string representing a given double value.
     *
     * Number is formatted the same as `%.9g` in `printf()` using C locale.
     *
     * Converting a 6-digit decimal representation to a single-precision float value and back to
     * decimal representation should be an identity. The same for converting a float to a 9-digit
     * decimal representation and back to a float.
     * For double-precision values these digit counts are 15 and 17 respectively. However, due to
     * `parseDouble()` accuracy limitation, accuracy of this function is limited to 1e-15 to avoid
     * producing numbers like 6665.999... instead of 6666 on all compilers.
     *
     * @param d number.
     * @param nDigits number of digits to show (must be 1...17).
     */
    explicit String( double d, int nDigits = 9 );

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
     * Existing storage is reused if its size matches.
     */
    String& operator = ( const String& s );

    /**
     * Move operator.
     */
    String& operator = ( String&& s );

    /**
     * Replace current string with a given C string (nullptr is permitted, equals "").
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
     * initialised by the caller.
     *
     * @param length length of string (without the terminating null character).
     * @param buffer where to pass non-constant pointer to the internal buffer.
     */
    static String create( int length, char** buffer );

    /**
     * Equality.
     */
    bool operator == ( const String& s ) const
    {
      return compare( buffer, s.buffer ) == 0;
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
     * Equality.
     */
    bool equals( const char* s ) const
    {
      return compare( buffer, s ) == 0;
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
     * FNV hash function, slower but has better distribution than Bernstein's `oz::hash()`.
     */
    int strongHash() const
    {
      return strongHash( buffer );
    }

    /**
     * Constant reference to the `i`-th byte.
     */
    OZ_ALWAYS_INLINE
    const char& operator [] ( int i ) const
    {
      hard_assert( 0 <= i && i <= count );

      return buffer[i];
    }

    /**
     * First character or null character if an empty string.
     */
    OZ_ALWAYS_INLINE
    char first() const
    {
      return buffer[0];
    }

    /**
     * Last character or null character if an empty string.
     */
    OZ_ALWAYS_INLINE
    char last() const
    {
      return count == 0 ? '\0' : buffer[count - 1];
    }

    /**
     * Index of the first occurrence of a character from a given index (inclusive).
     */
    int index( char ch, int start = 0 ) const
    {
      return index( buffer, ch, start );
    }

    /**
     * Index of the last occurrence of a character before a given index (not inclusive).
     */
    int lastIndex( char ch, int end ) const
    {
      return lastIndex( buffer, ch, end );
    }

    /**
     * Index of the last occurrence of a character.
     */
    int lastIndex( char ch ) const
    {
      return lastIndex( buffer, ch, count );
    }

    /**
     * Pointer to the first occurrence of a character from a given index (inclusive).
     */
    const char* find( char ch, int start = 0 ) const
    {
      return find( buffer, ch, start );
    }

    /**
     * Pointer to the last occurrence of a character before a given index (not inclusive).
     */
    const char* findLast( char ch, int end ) const
    {
      return findLast( buffer, ch, end );
    }

    /**
     * Pointer to the last occurrence of a character.
     */
    const char* findLast( char ch ) const
    {
      return findLast( buffer, ch, count );
    }

    /**
     * True iff string begins with given characters.
     */
    bool beginsWith( const char* sub ) const
    {
      return beginsWith( buffer, sub );
    }

    /**
     * True iff string ends with given characters.
     */
    bool endsWith( const char* sub ) const;

    /**
     * Parse bool value, wraps `parseBool( const char* s, const char** end )`.
     */
    OZ_ALWAYS_INLINE
    bool parseBool( const char** end = nullptr ) const
    {
      return parseBool( buffer, end );
    }

    /**
     * Parse int value, wraps `parseInt( const char* s, const char** end )`.
     */
    OZ_ALWAYS_INLINE
    int parseInt( const char** end = nullptr ) const
    {
      return parseInt( buffer, end );
    }

    /**
     * Parse double value, wraps `parseDouble( const char* s, const char** end )`.
     */
    OZ_ALWAYS_INLINE
    double parseDouble( const char** end = nullptr ) const
    {
      return parseDouble( buffer, end );
    }

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
     * Create a copy that has all instances of `whatChar` replaced by `withChar`.
     */
    String replace( char whatChar, char withChar ) const;

    /**
     * Return array of substrings between occurrences of a given delimiter.
     *
     * Empty strings between two immediate delimiter occurrences or between a delimiter and
     * beginning/end of the original string are included.
     */
    DArray<String> split( char delimiter ) const;

    /**
     * True iff file path is empty (i.e. an empty string or "@").
     */
    OZ_ALWAYS_INLINE
    bool fileIsEmpty() const
    {
      return fileIsEmpty( buffer );
    }

    /**
     * True iff file path is a VFS file path.
     */
    OZ_ALWAYS_INLINE
    bool fileIsVirtual() const
    {
      return fileIsVirtual( buffer );
    }

    /**
     * Extract directory from a path (substring before the last `/`).
     */
    String fileDirectory() const;

    /**
     * Extract file name from a path (substring after the last `/`).
     */
    String fileName() const;

    /**
     * Extract base file name from a path (substring after the last `/` till the last dot following
     * it).
     */
    String fileBaseName() const;

    /**
     * Extract file extension from the path (substring after the last dot in file name or "" if no
     * extension).
     */
    String fileExtension() const;

    /**
     * True iff file name has a given extension.
     *
     * Empty string matches both no extension and files names ending with dot.
     */
    bool fileHasExtension( const char* ext ) const;

};

}
