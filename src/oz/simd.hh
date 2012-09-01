/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/simd.hh
 *
 * SIMD type definitions and utility functions.
 */

#pragma once

#include "common.hh"

/**
 * @def OZ_SIMD_SHUFFLE
 * Compiler-dependent built-in function for SIMD vector shuffle.
 */
#if defined( OZ_CLANG )
# define OZ_SIMD_SHUFFLE( a, b, i, j, k, l ) \
  __builtin_shufflevector( a, b, i, j, k, l )
#else
# define OZ_SIMD_SHUFFLE( a, b, i, j, k, l ) \
  __builtin_ia32_shufps( a, b, ( i << 6 ) | ( j << 4 ) | ( k << 2 ) | l )
#endif

/**
 * @def OZ_SIMD_FIRST
 * Compiler-dependent built-in function for extraction first float from a SIMD vector.
 */
#if defined( OZ_CLANG )
# define OZ_SIMD_FIRST( a ) \
  ( a[0] )
#else
# define OZ_SIMD_FIRST( a ) \
  __builtin_ia32_vec_ext_v4sf( a, 0 )
#endif

/**
 * @def OZ_SIMD_HADD
 * Compiler-dependent built-in function for SSE3 horizontal addition.
 */
#define OZ_SIMD_HADD( a, b ) \
  __builtin_ia32_haddps( a, b )

/**
 * @def OZ_SIMD_HSUB
 * Compiler-dependent built-in function for SSE3 horizontal subtraction.
 */
#define OZ_SIMD_HSUB( a, b ) \
  __builtin_ia32_hsubps( a, b )

namespace oz
{

/**
 * SIMD vector of four floats.
 */
typedef float __attribute__(( vector_size( 16 ) )) float4;

/**
 * @def float4
 * "Constructor" for `float4` type.
 */
#ifndef __LINE__
# define float4( x, y, z, w ) float4( { x, y, z, w } )
#elif defined( OZ_CLANG ) && OZ_CLANG == 300
# define float4( x, y, z, w ) float4( { x, y, z, w } )
#else
# define float4( x, y, z, w ) float4{ x, y, z, w }
#endif

/**
 * SIMD vector of four unsigned integers.
 */
typedef uint __attribute__(( vector_size( 16 ) )) uint4;

/**
 * @def uint4
 * "Constructor" for `uint4` type.
 */
#ifndef __LINE__
# define uint4( x, y, z, w ) uint4( { x, y, z, w } )
#elif defined( OZ_CLANG ) && OZ_CLANG == 300
# define uint4( x, y, z, w ) uint4( { x, y, z, w } )
#else
# define uint4( x, y, z, w ) uint4{ x, y, z, w }
#endif

/**
 * Absolute value of a float SIMD vector.
 */
OZ_ALWAYS_INLINE
inline uint4 vAbs( uint4 a )
{
  return a & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff );
}

/**
 * Scalar product for float SIMD vectors (returns float SIMD vector).
 */
OZ_ALWAYS_INLINE
inline float4 vDot( float4 a, float4 b )
{
  float4 p = a * b;
  float4 s = OZ_SIMD_HADD( p, p );
  return OZ_SIMD_HADD( s, s );
}

/**
 * Scalar product for float SIMD vectors (returns float value).
 */
OZ_ALWAYS_INLINE
inline float vsDot( float4 a, float4 b )
{
  float4 p = a * b;
  float4 s = OZ_SIMD_HADD( p, p );
  float4 t = OZ_SIMD_HADD( s, s );
  return OZ_SIMD_FIRST( t );
}

}
