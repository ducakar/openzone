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

#ifdef OZ_SIMD_MATH

/**
 * SIMD vector of four floats.
 */
typedef float __attribute__(( vector_size( 16 ) )) float4;

/**
 * SIMD vector of four unsigned integers.
 */
typedef uint __attribute__(( vector_size( 16 ) )) uint4;

OZ_ALWAYS_INLINE
inline float4 vFill( float x, float y, float z, float w )
{
  return (float4) { x, y, z, w };
}

OZ_ALWAYS_INLINE
inline float4 vFill( float x )
{
  return (float4) { x, x, x, x };
}

OZ_ALWAYS_INLINE
inline uint4 vFill( uint x, uint y, uint z, uint w )
{
  return (uint4) { x, y, z, w };
}

OZ_ALWAYS_INLINE
inline uint4 vFill( uint x )
{
  return (uint4) { x, x, x, x };
}

OZ_ALWAYS_INLINE
inline float vFirst( float4 a )
{
#ifdef OZ_CLANG
  return a[0];
#else
  return __builtin_ia32_vec_ext_v4sf( a, 0 );
#endif
}

/**
 * @def vShuffle
 * Compiler-dependent built-in function for SIMD vector shuffle.
 */
#if defined( OZ_CLANG )
# define vShuffle( a, b, i, j, k, l ) \
  __builtin_shufflevector( a, b, i, j, k, l )
#else
# define vShuffle( a, b, i, j, k, l ) \
  __builtin_ia32_shufps( a, b, i | ( j << 2 ) | ( k << 4 ) | ( l << 6 ) );
#endif

OZ_ALWAYS_INLINE
inline float4 vHAdd( float4 a, float4 b )
{
  return __builtin_ia32_haddps( a, b );
}

OZ_ALWAYS_INLINE
inline float4 vHSub( float4 a, float4 b )
{
  return __builtin_ia32_hsubps( a, b );
}

/**
 * Absolute value of a float SIMD vector.
 */
OZ_ALWAYS_INLINE
inline uint4 vAbs( uint4 a )
{
  return a & vFill( 0x7fffffffu );
}

/**
 * Scalar product for float SIMD vectors (returns float SIMD vector).
 */
OZ_ALWAYS_INLINE
inline float4 vDot( float4 a, float4 b )
{
  float4 p = a * b;
  float4 s = vHAdd( p, p );
  return vHAdd( s, s );
}

#endif // OZ_SIMD_MATH

/**
 * Base class for 3-component vector and similar algebra structures.
 */
class VectorBase3
{
  public:

#ifdef OZ_SIMD_MATH
    union OZ_ALIGNED( 16 )
    {
      float4 f4;
      uint4  u4;
      struct
      {
        float x; ///< X component.
        float y; ///< Y component.
        float z; ///< Z component.
      };
    };
#else
    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.
#endif

  protected:

    /**
     * Default constructor, creates uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    VectorBase3() = default;

#ifdef OZ_SIMD_MATH

    OZ_ALWAYS_INLINE
    explicit VectorBase3( float4 f4_ ) :
      f4( f4_ )
    {}

    OZ_ALWAYS_INLINE
    explicit VectorBase3( uint4 u4_ ) :
      u4( u4_ )
    {}

    OZ_ALWAYS_INLINE
    explicit VectorBase3( float x_, float y_, float z_, float w_ ) :
      f4( vFill( x_, y_, z_, w_ ) )
    {}

#else

    /**
     * Create a vector with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit VectorBase3( float x_, float y_, float z_, float ) :
      x( x_ ), y( y_ ), z( z_ )
    {}

#endif

};

/**
 * Base class for 4-component vector and similar algebra structures.
 */
class VectorBase4
{
  public:

#ifdef OZ_SIMD_MATH
    union OZ_ALIGNED( 16 )
    {
      float4 f4;
      uint4  u4;
      struct
      {
        float x; ///< X component.
        float y; ///< Y component.
        float z; ///< Z component.
        float w; ///< W component.
      };
    };
#else
    float x; ///< X component.
    float y; ///< Y component.
    float z; ///< Z component.
    float w; ///< W component.
#endif

  protected:

    /**
     * Default constructor, creates uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    VectorBase4() = default;

#ifdef OZ_SIMD_MATH

    OZ_ALWAYS_INLINE
    explicit VectorBase4( float4 f4_ ) :
      f4( f4_ )
    {}

    OZ_ALWAYS_INLINE
    explicit VectorBase4( uint4 u4_ ) :
      u4( u4_ )
    {}

    OZ_ALWAYS_INLINE
    explicit VectorBase4( float x_, float y_, float z_, float w_ ) :
      f4( vFill( x_, y_, z_, w_ ) )
    {}

#else

    /**
     * Create a vector with the given components.
     */
    OZ_ALWAYS_INLINE
    explicit VectorBase4( float x_, float y_, float z_, float w_ ) :
      x( x_ ), y( y_ ), z( z_ ), w( w_ )
    {}

#endif

};

#ifdef OZ_SIMD_MATH

/**
 * SIMD vector containing 4 equal values.
 *
 * For many operations like multiplication of a vector and scalar, dot product, normalisation etc.
 * we need to convert between float scalars and a SIMD vectors which is an expensive operation. In
 * order to minimise such conversions we introduce scalar class that represents a SIMD register
 * holding 4 identical values. It is used to replace float temporary values.
 */
class scalar
{
  public:

    union OZ_ALIGNED( 16 )
    {
      float4 f4; ///< Float SIMD vector.
      uint4  u4; ///< Unsigned integer SIMD vector.
    };

  public:

    /**
     * Create an uninitialised instance.
     */
    OZ_ALWAYS_INLINE
    scalar() = default;

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    scalar( float4 f4_ ) :
      f4( f4_ )
    {}

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    scalar( uint4 u4_ ) :
      u4( u4_ )
    {}

    /**
     * Create from a float value.
     */
    OZ_ALWAYS_INLINE
    scalar( float f ) :
      f4( vFill( f ) )
    {}

    /**
     * %Set to the given float value.
     */
    scalar& operator = ( float f )
    {
      f4 = vFill( f );
      return *this;
    }

    /**
     * Cast to float.
     */
    OZ_ALWAYS_INLINE
    operator float () const
    {
      return vFirst( f4 );
    }

    /**
     * Absolute value.
     */
    OZ_ALWAYS_INLINE
    scalar abs() const
    {
      return vAbs( u4 );
    }

    /**
     * Original value.
     */
    OZ_ALWAYS_INLINE
    scalar operator + () const
    {
      return *this;
    }

    /**
     * Opposite value.
     */
    OZ_ALWAYS_INLINE
    scalar operator - () const
    {
      return -f4;
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    scalar operator + ( scalar s ) const
    {
      return f4 + s.f4;
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    scalar operator + ( float f ) const
    {
      return f4 + vFill( f );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    friend scalar operator + ( float f, scalar s )
    {
      return vFill( f ) + s.f4;
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    scalar operator - ( scalar s ) const
    {
      return f4 - s.f4;
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    scalar operator - ( float f ) const
    {
      return f4 - vFill( f );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    friend scalar operator - ( float f, scalar s )
    {
      return vFill( f ) - s.f4;
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    scalar operator * ( scalar s ) const
    {
      return f4 * s.f4;
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    scalar operator * ( float f ) const
    {
      return f4 * vFill( f );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend scalar operator * ( float f, scalar s )
    {
      return vFill( f ) * s.f4;
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    scalar operator / ( scalar s ) const
    {
      return f4 / s.f4;
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    scalar operator / ( float f ) const
    {
      return f4 / vFill( f );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    friend scalar operator / ( float f, scalar s )
    {
      return vFill( f ) / s.f4;
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    scalar& operator += ( scalar s )
    {
      f4 += s.f4;
      return *this;
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    scalar& operator += ( float f )
    {
      f4 += vFill( f );
      return *this;
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    friend float& operator += ( float& f, scalar s )
    {
      f += vFirst( s.f4 );
      return f;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    scalar& operator -= ( scalar s )
    {
      f4 -= s.f4;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    scalar& operator -= ( float f )
    {
      f4 -= vFill( f );
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    friend float& operator -= ( float& f, scalar s )
    {
      f -= vFirst( s.f4 );
      return f;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    scalar& operator *= ( scalar s )
    {
      f4 *= s.f4;
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    scalar& operator *= ( float f )
    {
      f4 *= vFill( f );
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    friend float& operator *= ( float& f, scalar s )
    {
      f *= vFirst( s.f4 );
      return f;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    scalar& operator /= ( scalar s )
    {
      f4 /= s.f4;
      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    scalar& operator /= ( float f )
    {
      f4 /= vFill( f );
      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    friend float& operator /= ( float& f, scalar s )
    {
      f /= vFirst( s.f4 );
      return f;
    }

};

#else

/**
 * Alias for float unless OZ_SIMD_MATH is enabled.
 */
typedef float scalar;

#endif

}
