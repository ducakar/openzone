/*
 * liboz - OpenZone core library.
 *
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file oz/Simd.hh
 */

#include "common.hh"

namespace oz
{

/**
 * SIMD vector of four integers.
 *
 * @ingroup oz
 */
typedef int int4 __attribute__(( vector_size( 16 ) ));

/**
 * SIMD vector of four unsigned integers.
 *
 * @ingroup oz
 */
typedef uint uint4 __attribute__(( vector_size( 16 ) )) ;

/**
 * SIMD vector of four floats.
 *
 * @ingroup oz
 */
typedef float float4 __attribute__(( vector_size( 16 ) ));

/**
 * @def int4
 * "Constructor" for <tt>int4</tt> type.
 *
 * @ingroup oz
 */
# define int4( x, y, z, w ) int4( { x, y, z, w } )

/**
 * @def uint4
 * "Constructor" for <tt>uint4</tt> type.
 *
 * @ingroup oz
 */
# define uint4( x, y, z, w ) uint4( { x, y, z, w } )

/**
 * @def float4
 * "Constructor" for <tt>float4</tt> type.
 *
 * @ingroup oz
 */
# define float4( x, y, z, w ) float4( { x, y, z, w } )

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
  };

  /**
   * Create an uninitialised instance.
   */
  Simd() = default;

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

}
