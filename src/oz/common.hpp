/*
 *  common.hpp
 *
 *  Common macros, types and templates
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 *  \file base/common.hpp
 *
 *  \brief Common types and templates
 *
 *  You may add <code>null</code>, <code>foreach</code> and the types to your
 *  <code>~/.kde/share/apps/katepart/syntax/cpp.xml</code> or global file
 *  <code>$KDEDIR/share/apps/katepart/syntax/cpp.xml</code> to look like reserved words in
 *  Katepart (Kate/KWrite/KDevelop).
 *  For Eclipse I use the same syntax highlighting for macros as for reserved words hence the macro
 *  definitions like #define uint uint.
 */

namespace oz
{

  //***********************************
  //*          BASIC MACROS           *
  //***********************************

  /**
   * \def null
   * It is equivalent to nullptr/NULL but it looks prettier.
   */
# define null nullptr

  /**
   * \def S
   * "Stringify" the given identifier.
   */
# define S( s ) #s

  /**
   * \def soft_assert
   * Like assert, but raises SIGTRAP with a dummy handler (like DebugBreak in MSVC).
   */
#ifdef NDEBUG

# define soft_assert( cond ) \
  static_cast<void>( 0 )

#else

# define soft_assert( cond ) \
  ( ( cond ) ? \
      static_cast<void>( 0 ) : \
      oz::_softAssert( #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

  void _softAssert( const char* message, const char* file, int line, const char* function );

#endif

  //***********************************
  //*             TYPES               *
  //***********************************

  /**
   * Unit type
   */
# define nil nil
  struct nil
  {
    /**
     * Always return true as all instances of this type are the same.
     * @param
     * @return
     */
    bool operator == ( const nil& ) const
    {
      return true;
    }

    /**
     * Always return false as all instances of this type are the same.
     * @param
     * @return
     */
    bool operator != ( const nil& ) const
    {
      return false;
    }
  };

  /**
   * signed byte
   * It should be used where char must be signed (otherwise char may be either signed or unsigned
   * depending on the platform).
   */
# define byte byte
  typedef signed   char  byte;

  /**
   * unsigned byte
   * It should be used where char must be unsigned (otherwise char may be either signed or unsigned
   * depending on the platform).
   */
# define ubyte ubyte
  typedef unsigned char  ubyte;

  /**
   * unsigned short integer
   */
# define ushort ushort
  typedef unsigned short ushort;

  /**
   * unsigned integer
   */
# define uint uint
  typedef unsigned int   uint;

  /**
   * unsigned long integer
   */
# define ulong ulong
  typedef unsigned long  ulong;

  // just make syntax highlighting work
# define size_t size_t

  //***********************************
  //*        BASIC ALGORITHMS         *
  //***********************************

  /**
   * Swap values of a and b with move semantics.
   * @param a reference to the first variable
   * @param b reference to the second variable
   */
  template <typename Type>
  inline void swap( Type& a, Type& b )
  {
    Type t( a );
    a = b;
    b = t;
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return a if a < b, b otherwise
   */
  template <typename Type>
  inline const Type& min( const Type& a, const Type& b )
  {
    return a < b ? a : b;
  }

  /**
   * Minimum
   * Non-const version, can be used as lvalue.
   * @param a
   * @param b
   * @return a if a < b, b otherwise
   */
  template <typename Type>
  inline Type& min( Type& a, Type& b )
  {
    return a < b ? a : b;
  }

  /**
   * Maximum.
   * @param a
   * @param b
   * @return a if a > b, b otherwise
   */
  template <typename Type>
  inline const Type& max( const Type& a, const Type& b )
  {
    return b < a ? a : b;
  }

  /**
   * Maximum
   * Non-const version, can be used as lvalue.
   * @param a
   * @param b
   * @return a if a > b, b otherwise
   */
  template <typename Type>
  inline Type& max( Type& a, Type& b )
  {
    return b < a ? a : b;
  }

  /**
   * Bound c between a and b. Equals to max( a, min( b, c ) ).
   * @param c
   * @param a
   * @param b
   * @return c, if a <= c <= b, respective boundary otherwise
   */
  template <typename Type>
  inline const Type& bound( const Type& c, const Type& a, const Type& b )
  {
    assert( a <= b );

    return c < a ? a : ( b < c ? b : c );
  }

  /**
   * Bound c between a and b. Equals to max( a, min( b, c ) ).
   * Non-const version, can be used as lvalue.
   * @param c
   * @param a
   * @param b
   * @return c, if a <= c <= b, respective boundary otherwise
   */
  template <typename Type>
  inline Type& bound( Type& c, Type& a, Type& b )
  {
    assert( a <= b );

    return c < a ? a : ( b < c ? b : c );
  }

}
