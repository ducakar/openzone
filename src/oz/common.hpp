/*
 *  common.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file oz/common.hpp
 *
 * Common types and templates.
 *
 * You may want to add <tt>null</tt>, <tt>foreach</tt>, <tt>soft_assert</tt>,
 * <tt>hard_assert</tt>, <tt>byte</tt>, <tt>ubyte</tt>, <tt>ushort</tt>,
 * <tt>ulong</tt>, <tt>long64</tt> and <tt>ulong64</tt> to your
 * <tt>~/.kde/share/apps/katepart/syntax/cpp.xml</tt> or global file
 * <tt>/usr/share/apps/katepart/syntax/cpp.xml</tt> to look like reserved words in
 * Katepart (Kate/KWrite/KDevelop).
 *
 * For Eclipse I use the same syntax highlighting for macro invocations and reserved words.
 * Import etc/eclipse-defines.xml file to define custom reserver words as macros.
 */

/*
 * Configuration.
 */
#include "ozconfig.hpp"

/*
 * The most essential C/C++ definitions (std::nullptr_t, size_t, ptrdiff_t, NULL and offsetof).
 */
#include <cstddef>

/*
 * Platform-independent argument reading for variable-argument functions.
 */
#include <cstdarg>

/*
 * Standard exception definitions (usually included via <new>, but just for sure).
 */
#include <exception>

/*
 * Standard new/delete operator.
 */
#include <new>

/**
 * Core namespace.
 */
namespace oz
{

//***********************************
//*          BASIC MACROS           *
//***********************************

/**
 * @def soft_assert
 * If condition fails, prints error to log raises SIGTRAP.
 *
 * @ingroup oz
 */

/**
 * @def hard_assert
 * If condition fails, prints error to log and aborts program.
 *
 * @ingroup oz
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

/**
 * Helper function for <tt>soft_assert</tt>.
 *
 * @ingroup oz
 */
void _hardAssert( const char* message, const char* file, int line, const char* function );

/**
 * Helper function for <tt>hard_assert</tt>.
 *
 * @ingroup oz
 */
void _softAssert( const char* message, const char* file, int line, const char* function );

#endif

//***********************************
//*             TYPES               *
//***********************************

// Import nullptr_t type into oz namespace.
using std::nullptr_t;

/**
 * Null constant.
 *
 * @ingroup oz
 */
const nullptr_t null = nullptr;

/**
 * Signed byte.
 *
 * @ingroup oz
 */
typedef signed char byte;

/**
 * Unsigned byte.
 *
 * @ingroup oz
 */
typedef unsigned char ubyte;

/**
 * Unsigned short integer.
 *
 * @ingroup oz
 */
typedef unsigned short ushort;

/**
 * Unsigned integer.
 *
 * @ingroup oz
 */
typedef unsigned int uint;

/**
 * Unsigned long integer.
 *
 * @ingroup oz
 */
typedef unsigned long ulong;

/**
 * Signed 64-bit integer.
 *
 * @ingroup oz
 */
typedef long long long64;

/**
 * Unsigned 64-bit integer.
 *
 * @ingroup oz
 */
typedef unsigned long long ulong64;

// Some assumptions about types
static_assert( sizeof( short )  == 2, "sizeof( short ) should be 2" );
static_assert( sizeof( int )    == 4, "sizeof( int ) should be 4" );
static_assert( sizeof( long64 ) == 8, "sizeof( long64 ) should be 8" );
static_assert( sizeof( float )  == 4, "sizeof( float ) should be 4" );
static_assert( sizeof( double ) == 8, "sizeof( double ) should be 8" );

#ifdef OZ_SIMD

/**
 * SIMD vector of four integers.
 *
 * @ingroup oz
 */
typedef int __attribute__(( vector_size( 16 ) )) int4;

/**
 * SIMD vector of four unsigned integers.
 *
 * @ingroup oz
 */
typedef uint __attribute__(( vector_size( 16 ) )) uint4;

/**
 * SIMD vector of four floats.
 *
 * @ingroup oz
 */
typedef float __attribute__(( vector_size( 16 ) )) float4;

/**
 * @def int4
 * "Constructor" for <tt>int4</tt> type.
 *
 * @ingroup oz
 */
# define int4( x, y, z, w ) (int4) { x, y, z, w }

/**
 * @def uint4
 * "Constructor" for <tt>uint4</tt> type.
 *
 * @ingroup oz
 */
# define uint4( x, y, z, w ) (uint4) { x, y, z, w }

/**
 * @def float4
 * "Constructor" for <tt>float4</tt> type.
 *
 * @ingroup oz
 */
# define float4( x, y, z, w ) (float4) { x, y, z, w }

/**
 * Base class for classes representing a SIMD register.
 *
 * @ingroup oz
 */
struct Simd
{
  union
  {
    int4   i4;   ///< Integer SIMD vector.
    int    i[4]; ///< Integer components of SIMD vector.

    uint4  u4;   ///< Unsigned integer SIMD vector.
    uint   u[4]; ///< Unsigned integer component of SIMD vector.

    float4 f4;   ///< Float SIMD vector.
    float  f[4]; ///< Float components of SIMD vector.

    /**
     * %Vector components.
     */
    struct
    {
      float x;   ///< X component.
      float y;   ///< Y component.
      float z;   ///< Z component.
      float w;   ///< W component.
    };

    /**
     * Plane components.
     */
    struct
    {
      float nx;  ///< X component of the normal.
      float ny;  ///< Y component of the normal.
      float nz;  ///< Z component of the normal.
      float d;   ///< Distance from origin.
    };
  };

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Simd()
  {}

  /**
   * Create from an int SIMD vector.
   */
  OZ_ALWAYS_INLINE
  Simd( int4 i4_ ) : i4( i4_ )
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  Simd( uint4 u4_ ) : u4( u4_ )
  {}

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  Simd( float4 f4_ ) : f4( f4_ )
  {}
};
#endif

//***********************************
//*        BASIC ALGORITHMS         *
//***********************************

/**
 * Swap values of variables.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline void swap( Value& a, Value& b )
{
  Value t = static_cast<Value&&>( a );
  a = static_cast<Value&&>( b );
  b = static_cast<Value&&>( t );
}

/**
 * Minimum.
 *
 * @return a if a <= b, b otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& min( const Value& a, const Value& b )
{
  return b < a ? b : a;
}

/**
 * Maximum.
 *
 * @return a if a >= b, b otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& max( const Value& a, const Value& b )
{
  return a < b ? b : a;
}

/**
 * Clamp c between a and b.
 *
 * @return c, if a <= c <= b, respective boundary otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline constexpr const Value& clamp( const Value& c, const Value& a, const Value& b )
{
  return c < a ? a : ( b < c ? b : c );
}

}
