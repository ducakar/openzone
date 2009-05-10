/*
 *  common.h
 *
 *  Common types and templates
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

/**
 *  \file common.h
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

  /**
   * \def null
   * It is equivalent to NULL macro but it looks prettier.
   */
# define null 0

  /**
   * signed byte
   * It should be used where char must be signed (otherwise char may be either signed or unsigned
   * depeneding on the platform).
   */
  typedef signed   char  byte;

  /**
   * unsigned byte
   * It should be used where char must be unsigned (otherwise char may be either signed or unsigned
   * depeneding on the platform).
   */
  typedef unsigned char  ubyte;

  /**
   * unsigned short integer
   */
  typedef unsigned short ushort;

  /**
   * unsigned integer
   */
  typedef unsigned int   uint;

  /**
   * unsigned long integer
   */
  typedef unsigned long  ulong;

  //***********************************
  //*     MISCELLANEOUS TEMPLATES     *
  //***********************************

  /**
   * Swap values of a and b.
   * @param a reference to first variable
   * @param b reference to second variable
   */
  template <class ValueA, class ValueB>
  inline void swap( ValueA &a, ValueB &b )
  {
    ValueA temp = a;

    a = b;
    b = temp;
  }

  /**
   * Absolute value.
   * @param a
   * @return absolute value
   */
  template <class Value>
  inline Value abs( const Value &a )
  {
    return a < 0 ? -a : a;
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return minimum of a and b
   */
  template <class Value, class ValueB>
  inline const Value &min( const Value &a, const ValueB &b )
  {
    return a < b ? a : b;
  }

  /**
   * Maximum.
   * @param a
   * @param b
   * @return maximum of a and b
   */
  template <class Value, class ValueB>
  inline const Value &max( const Value &a, const ValueB &b )
  {
    return a > b ? a : b;
  }

  /**
   * c bounded between a and b. Equals to max( min( c, b ), a ).
   * @param c
   * @param a
   * @param b
   * @return clamped value of c
   */
  template <class ValueC, class ValueA, class ValueB>
  inline const ValueC &bound( const ValueC &c, const ValueA &a, const ValueB &b )
  {
    assert( a < b );

    if( a > c ) {
      return a;
    }
    else if( b < c ) {
      return b;
    }
    else {
      return c;
    }
  }

}
