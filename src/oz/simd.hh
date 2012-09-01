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

namespace oz
{

/**
 * @def OZ_SHUFFLE_VECTOR
 * Compiler-dependent built-in function for SIMD register shuffle.
 */
#if defined( OZ_CLANG )
# define OZ_SHUFFLE_VECTOR( v1, v2, i, j, k, l ) \
  __builtin_shufflevector( v1, v2, i, j, k, l )
#else
# define OZ_SHUFFLE_VECTOR( v1, v2, i, j, k, l ) \
  __builtin_ia32_shufps( v1, v2, ( i << 6 ) | ( j << 4 ) | ( k << 2 ) | l )
#endif

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

OZ_ALWAYS_INLINE
inline uint4 vAbs( const uint4& a )
{
  return a & uint4( 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff );
}

OZ_ALWAYS_INLINE
inline float4 vDot3( const float4& a, const float4& b )
{
  float4 p = a * b;
  return OZ_SHUFFLE_VECTOR( p, p, 0, 0, 0, 0 ) + OZ_SHUFFLE_VECTOR( p, p, 1, 1, 1, 1 ) +
         OZ_SHUFFLE_VECTOR( p, p, 2, 2, 2, 2 );
}

OZ_ALWAYS_INLINE
inline float4 vDot4( const float4& a, const float4& b )
{
  float4 p = a * b;
  return OZ_SHUFFLE_VECTOR( p, p, 0, 0, 0, 0 ) + OZ_SHUFFLE_VECTOR( p, p, 1, 1, 1, 1 ) +
         OZ_SHUFFLE_VECTOR( p, p, 2, 2, 2, 2 ) + OZ_SHUFFLE_VECTOR( p, p, 3, 3, 3, 3 );
}

}
