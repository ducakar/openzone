/*
 *  common.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file common.hpp
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

/// @def soft_assert
/// If condition fails, prints error to log raises SIGTRAP.

/// @def hard_assert
/// If condition fails, prints error to log and aborts program.

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
 */
void _hardAssert( const char* message, const char* file, int line, const char* function );

/**
 * Helper function for <tt>hard_assert</tt>.
 */
void _softAssert( const char* message, const char* file, int line, const char* function );

#endif

//***********************************
//*             TYPES               *
//***********************************

using std::nullptr_t;

/// @def null
/// Nicer alias for nullptr.
# define null nullptr

/// Signed byte.
typedef signed char byte;

/// Unsigned byte.
typedef unsigned char ubyte;

/// Unsigned short integer.
typedef unsigned short ushort;

/// Unsigned integer.
typedef unsigned int uint;

/// Unsigned long integer.
typedef unsigned long ulong;

/// Signed 64-bit integer.
typedef long long long64;

/// Unsigned 64-bit integer.
typedef unsigned long long ulong64;

// Some assumptions about types
static_assert( sizeof( short )  == 2, "sizeof( short ) should be 2" );
static_assert( sizeof( int )    == 4, "sizeof( int ) should be 4" );
static_assert( sizeof( long64 ) == 8, "sizeof( long64 ) should be 8" );
static_assert( sizeof( float )  == 4, "sizeof( float ) should be 4" );
static_assert( sizeof( double ) == 8, "sizeof( double ) should be 8" );

#ifdef OZ_SIMD

/// SIMD vector of four integers.
typedef int   __attribute__(( vector_size( 16 ) )) int4;

/// SIMD vector of four unsigned integers.
typedef uint  __attribute__(( vector_size( 16 ) )) uint4;

/// SIMD vector of four floats.
typedef float __attribute__(( vector_size( 16 ) )) float4;

/// @def int4
/// "constructor" for <tt>int4</tt> type.
# define   int4( x, y, z, w )   (int4) { x, y, z, w }

/// @def uint4
/// "constructor" for <tt>uint4</tt> type.
# define  uint4( x, y, z, w )  (uint4) { x, y, z, w }

/// @def float4
/// "constructor" for <tt>float4</tt> type.
# define float4( x, y, z, w ) (float4) { x, y, z, w }

struct Simd
{
  union
  {
    int4   i4;
    int    i[4];

    uint4  u4;
    uint   u[4];

    float4 f4;
    float  f[4];

    // vector members
    struct
    {
      float x;
      float y;
      float z;
      float w;
    };

    // plane members
    struct
    {
      float nx;
      float ny;
      float nz;
      float d;
    };
  };

  OZ_ALWAYS_INLINE
  Simd()
  {}

  OZ_ALWAYS_INLINE
  Simd( int4 i4_ ) : i4( i4_ )
  {}

  OZ_ALWAYS_INLINE
  Simd( uint4 u4_ ) : u4( u4_ )
  {}

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
 *
 * @return a if a <= b, b otherwise.
 */
template <typename Type>
OZ_ALWAYS_INLINE
inline const Type& min( const Type& a, const Type& b )
{
  return b < a ? b : a;
}

/**
 * Maximum.
 *
 * @return a if a >= b, b otherwise.
 */
template <typename Type>
OZ_ALWAYS_INLINE
inline const Type& max( const Type& a, const Type& b )
{
  return a < b ? b : a;
}

/**
 * Clamp c between a and b.
 *
 * @return c, if a <= c <= b, respective boundary otherwise.
 */
template <typename Type>
OZ_ALWAYS_INLINE
inline const Type& clamp( const Type& c, const Type& a, const Type& b )
{
  return c < a ? a : ( b < c ? b : c );
}

}
