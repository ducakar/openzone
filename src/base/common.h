/*
 *  common.h
 *
 *  Common types and templates
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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

  /**
   * Dummy class.
   * Can be useful for some advanced templates e.g. Tuple.
   */
  struct Nil
  {
    bool operator == ( const Nil& ) const { return true; }
    bool operator != ( const Nil& ) const { return true; }
  };

  /**
   * Type modify templates (add or remove pointer/reference/constness)
   */
  template <typename Type>
  struct Mod
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  template <typename Type>
  struct Mod<const Type>
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  template <typename Type>
  struct Mod<Type&>
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  template <typename Type>
  struct Mod<const Type&>
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  template <typename Type>
  struct Mod<Type*>
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  template <typename Type>
  struct Mod<const Type*>
  {
    typedef Type        Plain;
    typedef const Type  Const;
    typedef       Type& Ref;
    typedef const Type& ConstRef;
    typedef       Type* Ptr;
    typedef const Type* ConstPtr;
  };

  //***********************************
  //*     MISCELLANEOUS TEMPLATES     *
  //***********************************

  /**
   * Swap values of a and b.
   * @param a reference to first variable
   * @param b reference to second variable
   */
  template <typename Value>
  inline void swap( Value& a, Value& b )
  {
    Value temp = a;

    a = b;
    b = temp;
  }

  /**
   * Absolute value.
   * @param a
   * @return absolute value
   */
  template <typename Value>
  inline Value abs( const Value& a )
  {
    return a < 0 ? -a : a;
  }

  /**
   * Minimum.
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
   * Maximum.
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
   * Bound c between a and b. Equals to max( min( c, b ), a ).
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
   * \def $
   * "Stringify" the given identifier/type/reserved word/...
   */
# define $( s ) #s

}
