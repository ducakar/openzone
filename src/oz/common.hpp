/*
 *  common.hpp
 *
 *  Common macros, types and templates
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * \file base/common.hpp
 *
 * \brief Common types and templates
 *
 * You may add <code>null</code>, <code>soft_assert</code>, <code>foreach</code>,
 * <code>onleave</code> and the types to your <code>~/.kde/share/apps/katepart/syntax/cpp.xml</code>
 * or global file <code>$KDEDIR/share/apps/katepart/syntax/cpp.xml</code> to look like reserved
 * words in Katepart (Kate/KWrite/KDevelop).
 * For Eclipse I use the same syntax highlighting for macro invocations and reserved words hence
 * the macro definitions like #define uint uint to highlight uint as the reserved word.
 */

/*
 * Configuration
 */
#include "ozconfig.hpp"

/*
 * Base C/C++ definitions (size_t, ptrdiff_t, NULL and offsetof)
 */
#include <cstddef>

/*
 * Platform-independent argument reading for variable-argument functions
 */
#include <cstdarg>

/*
 * Standard exception definitions (usually included via <new>, but just for sure)
 */
#include <exception>

/*
 * Standard new/delete operator and exception definitions
 */
#include <new>

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
   * \def const
   * Add constness to the variable type.
   */
# define const( a ) static_cast< const decltype( a ) >( a )

  /**
   * \def local
   * Thread-local modifier for a type.
   */
# define thread_local __thread

  /**
   * \def soft_assert
   * Like assert, but doesn't abort, it only raises SIGTRAP, prints stack trace and resumes
   * execution.
   */

  /**
   * \def hard_assert
   * Like assert, but also prints stack trace and waits for a debugger to attach.
   */
#ifdef NDEBUG

# define hard_assert( cond ) \
  static_cast<void>( 0 )

# define soft_assert( cond ) \
  static_cast<void>( 0 )

#else

# define hard_assert( cond ) \
  ( ( cond ) ? \
      static_cast<void>( 0 ) : \
      oz::_hardAssert( #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

# define soft_assert( cond ) \
  ( ( cond ) ? \
      static_cast<void>( 0 ) : \
      oz::_softAssert( #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

  void _hardAssert( const char* message, const char* file, int line, const char* function );
  void _softAssert( const char* message, const char* file, int line, const char* function );

#endif

  //***********************************
  //*             TYPES               *
  //***********************************

  /**
   * Unit type
   */
  struct nil
  {
    /**
     * Always return true as all instances of this type are the same.
     * @param
     * @return
     */
    OZ_ALWAYS_INLINE
    bool operator == ( const nil& ) const
    {
      return true;
    }

    /**
     * Always return false as all instances of this type are the same.
     * @param
     * @return
     */
    OZ_ALWAYS_INLINE
    bool operator != ( const nil& ) const
    {
      return false;
    }
  };

  /**
   * signed byte
   * It should be used where char must be signed (char may be either signed or unsigned depending
   * on the platform).
   */
  typedef signed char byte;

  /**
   * unsigned byte
   * It should be used where char must be unsigned (char may be either signed or unsigned depending
   * on the platform).
   */
  typedef unsigned char ubyte;

  /**
   * unsigned short integer
   */
  typedef unsigned short ushort;

  /**
   * unsigned integer
   */
  typedef unsigned int uint;

  /**
   * unsigned long integer
   */
  typedef unsigned long ulong;

  /**
   * signed 64-bit integer
   */
  typedef long long long64;

  /**
   * unsigned 64-bit integer
   */
  typedef unsigned long long ulong64;

  /**
   * SIMD vector of four ints
   */
  typedef int __attribute__(( vector_size( 16 ) )) int4;

  /**
   * \def int4
   * int4 "constructor"
   */
# define int4( x, y, z, w ) \
  (int4) { x, y, z, w }

  /**
   * SIMD vector of four floats
   */
  typedef float __attribute__(( vector_size( 16 ) )) float4;

  /**
   * \def float4
   * float4 "constructor"
   */
# define float4( x, y, z, w ) \
  (float4) { x, y, z, w }

  /**
   * Simd register structure
   */
  struct Simd
  {
    union
    {
      int4   i4;
      float4 f4;

      struct { int   i[4]; };
      struct { float f[4]; };

      struct
      {
        float x;
        float y;
        float z;
        float w;
      };

      struct
      {
        float nx;
        float ny;
        float nz;
        float d;
      };
    };

    OZ_ALWAYS_INLINE Simd()                          {}
    OZ_ALWAYS_INLINE Simd( int4    i4_ ) : i4( i4_ ) {}
    OZ_ALWAYS_INLINE Simd( float4  f4_ ) : f4( f4_ ) {}
  };

  //***********************************
  //*        BASIC ALGORITHMS         *
  //***********************************

  /**
   * Swap values of a and b.
   * @param a reference to the first variable
   * @param b reference to the second variable
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline void swap( Type& a, Type& b )
  {
    Type t = a;
    a = b;
    b = t;
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return a if a <= b, b otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline const Type& min( const Type& a, const Type& b )
  {
    return b < a ? b : a;
  }

  /**
   * Minimum
   * Non-const version, can be used as a lvalue.
   * @param a
   * @param b
   * @return a if a <= b, b otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline Type& min( Type& a, Type& b )
  {
    return b < a ? b : a;
  }

  /**
   * Maximum.
   * @param a
   * @param b
   * @return a if a >= b, b otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline const Type& max( const Type& a, const Type& b )
  {
    return a < b ? b : a;
  }

  /**
   * Maximum
   * Non-const version, can be used as a lvalue.
   * @param a
   * @param b
   * @return a if a >= b, b otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline Type& max( Type& a, Type& b )
  {
    return a < b ? b : a;
  }

  /**
   * Bound c between a and b. Equals to max( a, min( b, c ) ).
   * @param c
   * @param a
   * @param b
   * @return c, if a <= c <= b, respective boundary otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline const Type& bound( const Type& c, const Type& a, const Type& b )
  {
    hard_assert( !( b < a ) );

    return c < a ? a : ( b < c ? b : c );
  }

  /**
   * Bound c between a and b. Equals to max( a, min( b, c ) ).
   * Non-const version, can be used as a lvalue.
   * @param c
   * @param a
   * @param b
   * @return c, if a <= c <= b, respective boundary otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline Type& bound( Type& c, Type& a, Type& b )
  {
    hard_assert( !( b < a ) );

    return c < a ? a : ( b < c ? b : c );
  }

}
