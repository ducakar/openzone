/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/common.hh
 *
 * Common types and templates.
 *
 * You may want to add <tt>null</tt>, <tt>foreach</tt>, <tt>soft_assert</tt>, <tt>hard_assert</tt>,
 * <tt>byte</tt>, <tt>ubyte</tt>, <tt>ushort</tt>, <tt>ulong</tt>, <tt>long64</tt> and
 * <tt>ulong64</tt> to your <tt>~/.kde/share/apps/katepart/syntax/cpp.xml</tt> or global file
 * <tt>/usr/share/apps/katepart/syntax/cpp.xml</tt> to look like reserved words in KatePart
 * (KWrite/Kate/KDevelop).
 *
 * For Eclipse I use the same syntax highlighting for macro invocations and reserved words.
 * Import etc/eclipse-defines.xml file to define custom reserved words as macros.
 */

#pragma once

/*
 * Configuration.
 */
#include "ozconfig.hh"

/*
 * The most essential C/C++ definitions (std::nullptr_t, size_t, ptrdiff_t and offsetof).
 */
#include <cstddef>

/*
 * Platform-independent argument reading for variable-argument functions.
 */
#include <cstdarg>

/*
 * Standard exception definitions (usually included via <new>, but just to be sure).
 */
#include <exception>

/*
 * Standard declarations of new/delete & overloads.
 */
#include <new>

/**
 * Top-level OpenZone namespace.
 */
namespace oz
{

//***********************************
//*           ASSERTIONS            *
//***********************************

/**
 * @def soft_assert
 * If condition fails, raise SIGTRAP and print error using global log.
 *
 * @ingroup oz
 */

/**
 * @def hard_assert
 * If condition fails, raise SIGTRAP, print error using global log and abort program.
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
    oz::_hardAssertHelper( #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

# define soft_assert( cond ) \
  ( ( cond ) ? \
    static_cast<void>( 0 ) : \
    oz::_softAssertHelper( #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__ ) )

#endif

/**
 * Helper method for <tt>hard_assert</tt> macro.
 *
 * @ingroup oz
 */
OZ_NORETURN
void _hardAssertHelper( const char* message, const char* file, int line, const char* function );

/**
 * Helper method for <tt>soft_assert</tt> macro.
 *
 * @ingroup oz
 */
void _softAssertHelper( const char* message, const char* file, int line, const char* function );

//***********************************
//*             TYPES               *
//***********************************

// Import core C++ types from <cstddef>.
using std::nullptr_t;
using std::size_t;
using std::ptrdiff_t;

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

// Some assumptions type sizes.
static_assert( sizeof( short  ) == 2, "sizeof( short ) should be 2" );
static_assert( sizeof( int    ) == 4, "sizeof( int ) should be 4" );
static_assert( sizeof( long64 ) == 8, "sizeof( long64 ) should be 8" );
static_assert( sizeof( float  ) == 4, "sizeof( float ) should be 4" );
static_assert( sizeof( double ) == 8, "sizeof( double ) should be 8" );

//***********************************
//*         BASIC TEMPLATES         *
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
 * <tt>a</tt> if <tt>a <= b</tt>, <tt>b</tt> otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline const Value& min( const Value& a, const Value& b )
{
  return b < a ? b : a;
}

/**
 * <tt>a</tt> if <tt>a >= b</tt>, <tt>b</tt> otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline const Value& max( const Value& a, const Value& b )
{
  return a < b ? b : a;
}

/**
 * <tt>c</tt> if <tt>a <= c <= b</tt>, respective boundary otherwise.
 *
 * @ingroup oz
 */
template <typename Value>
OZ_ALWAYS_INLINE
inline const Value& clamp( const Value& c, const Value& a, const Value& b )
{
  hard_assert( !( b < a ) );

  return c < a ? a : ( b < c ? b : c );
}

}
