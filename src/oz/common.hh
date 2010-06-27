/*
 *  common.hh
 *
 *  Common types and templates
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

/**
 *  \file base/common.hh
 *
 *  \brief Common types and templates
 *
 *  You may add <code>null</code>, <code>foreach</code> and the types to your
 *  <code>~/.kde/share/apps/katepart/syntax/cpp.xml</code> or global file
 *  <code>$KDEDIR/share/apps/katepart/syntax/cpp.xml</code> to look like reserved words in
 *  Katepart (Kate/KWrite/KDevelop).
 */

namespace oz
{

  /*
   * TYPES
   */

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

  /*
   * TYPE UTILITIES
   */

  /**
   * Type manipulations
   */
  struct Type
  {
    template <typename Type_>
    struct StripRef
    {
      typedef Type_ Type;
    };

    template <typename Type_>
    struct StripRef<Type_&>
    {
      typedef Type_ Type;
    };
  };

  /**
   * \def null
   * It is equivalent to nullptr/NULL macro but it looks prettier.
   */
#ifdef __GNUG__
# define null __null
#else
# define null nullptr
#endif

  /**
   * \def S
   * "Stringify" the given identifier/type/reserved word/...
   */
# define S( s ) #s

  /*
   * BASIC ALGORITHMS
   */

  /**
   * Swap values of a and b with move semantics.
   * @param a reference to first variable
   * @param b reference to second variable
   */
  template <typename Value>
  inline void swap( Value& a, Value& b )
  {
    Value t = static_cast<Value&&>( a );
    a = static_cast<Value&&>( b );
    b = static_cast<Value&&>( t );
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return minimum of a and b
   */
  template <typename Value>
  inline const Value& min( const Value& a, const Value& b )
  {
    return a < b ? a : b;
  }

  /**
   * Minimum
   * Non-const version, can be used as lvalue.
   * @param a
   * @param b
   * @return minimum of a and b
   */
  template <typename Value>
  inline Value& min( Value& a, Value& b )
  {
    return a < b ? a : b;
  }

  /**
   * Maximum.
   * @param a
   * @param b
   * @return maximum of a and b
   */
  template <typename Value>
  inline const Value& max( const Value& a, const Value& b )
  {
    return a > b ? a : b;
  }

  /**
   * Maximum
   * Non-const version, can be used as lvalue.
   * @param a
   * @param b
   * @return maximum of a and b
   */
  template <typename Value>
  inline Value& max( Value& a, Value& b )
  {
    return a > b ? a : b;
  }

  /**
   * Bound c between a and b. Equals to max( min( c, b ), a ).
   * @param c
   * @param a
   * @param b
   * @return clamped value of c
   */
  template <typename Value>
  inline const Value& bound( const Value& c, const Value& a, const Value& b )
  {
    assert( a <= b );

    return c < a ? a : ( c > b ? b : c );
  }

  /**
   * Bound c between a and b. Equals to max( min( c, b ), a ).
   * Non-const version, can be used as lvalue.
   * @param c
   * @param a
   * @param b
   * @return clamped value of c
   */
  template <typename Value>
  inline Value& bound( Value& c, Value& a, Value& b )
  {
    assert( a <= b );

    return c < a ? a : ( c > b ? b : c );
  }

  /**
   * Generic absolute value.
   * For floating-point use Math::abs.
   */
  template <typename Value>
  inline Value abs( const Value& a )
  {
    return a < 0 ? -a : a;
  }

}
