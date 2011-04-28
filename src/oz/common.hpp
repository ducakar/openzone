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
 * The most essential C/C++ definitions (std::nullptr_t, size_t, ptrdiff_t, NULL and offsetof)
 */
#include <cstddef>

/*
 * Platform-independent argument reading for variable-argument functions
 */
#include <cstdarg>

/*
 * Initialiser list (only works when declared as std::initializer_list)
 */
#include <initializer_list>

/*
 * Standard exception definitions (usually included via <new>, but just for sure)
 */
#include <exception>

/*
 * Standard new/delete operator
 */
#include <new>

namespace oz
{

  //***********************************
  //*          BASIC MACROS           *
  //***********************************

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
  struct nil_t
  {
    /**
     * Always return true as all instances of this type are the same.
     * @param
     * @return
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator == ( nil_t ) const
    {
      return true;
    }

    /**
     * Always return false as all instances of this type are the same.
     * @param
     * @return
     */
    OZ_ALWAYS_INLINE
    constexpr bool operator != ( nil_t ) const
    {
      return false;
    }
  };

  /**
   * Unit value
   */
  const nil_t nil = nil_t();

  /**
   * nullptr_t
   */
  typedef decltype( nullptr ) nullptr_t;

  /**
   * null
   * Nicer alias for nullptr.
   */
  const nullptr_t null = nullptr;

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
   * Import initialiser list
   */
  using std::initializer_list;

  // some assumptions about types
  static_assert( sizeof( short ) == 2, "sizeof( short ) should be 2" );
  static_assert( sizeof( int ) == 4, "sizeof( int ) should be 4" );
  static_assert( sizeof( long64 ) == 8, "sizeof( long64 ) should be 8" );

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
    Type t = static_cast<Type&&>( a );
    a = static_cast<Type&&>( b );
    b = static_cast<Type&&>( t );
  }

  /**
   * Minimum.
   * @param a
   * @param b
   * @return a if a <= b, b otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline constexpr const Type& min( const Type& a, const Type& b )
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
  inline constexpr const Type& max( const Type& a, const Type& b )
  {
    return a < b ? b : a;
  }

  /**
   * Clamp c between a and b. Equals to max( a, min( b, c ) ).
   * @param c
   * @param a
   * @param b
   * @return c, if a <= c <= b, respective boundary otherwise
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  inline constexpr const Type& clamp( const Type& c, const Type& a, const Type& b )
  {
    return c < a ? a : ( b < c ? b : c );
  }

}
