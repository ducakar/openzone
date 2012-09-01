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
 * @file oz/Scalar.hh
 *
 * Scalar class.
 */

#pragma once

#include "simd.hh"

namespace oz
{

/**
 * SIMD vector containing 4 equal values.
 *
 * For many operations like multiplication of a vector and scalar, dot product, normalisation etc.
 * we need to convert between float scalars and a SIMD vectors which is an expensive operation. In
 * order to minimise such conversions we introduce Scalar class that represents a SIMD register
 * holding 4 identical values. It is used to replace float temporary values.
 */
union Scalar
{
  float4 f4; ///< Float SIMD vector.
  uint4  u4; ///< Unsigned integer SIMD vector.

  struct
  {
    float x; ///< X vector component.
    float y; ///< Y vector component.
    float z; ///< Z vector component.
    float w; ///< W vector component.
  };

  public:

    /**
     * Create an uninitialised instance.
     */
    Scalar() = default;

    /**
     * Create from a float SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Scalar( const float4& f4_ ) :
      f4( f4_ )
    {}

    /**
     * Create from an uint SIMD vector.
     */
    OZ_ALWAYS_INLINE
    explicit Scalar( const uint4& u4_ ) :
      u4( u4_ )
    {}

    /**
     * Create from a float value.
     */
    OZ_ALWAYS_INLINE
    Scalar( float f ) :
      f4( float4( f, f, f, f ) )
    {}

    /**
     * Set to the given float value.
     */
    Scalar& operator = ( float f )
    {
      f4 = float4( f, f, f, f );
      return *this;
    }

    /**
     * Case to float.
     */
    OZ_ALWAYS_INLINE
    operator float () const
    {
      return x;
    }

    /**
     * Absolute value.
     */
    OZ_ALWAYS_INLINE
    Scalar abs() const
    {
      return Scalar( vAbs( u4 ) );
    }

    /**
     * Original value.
     */
    OZ_ALWAYS_INLINE
    Scalar operator + () const
    {
      return *this;
    }

    /**
     * Opposite value.
     */
    OZ_ALWAYS_INLINE
    Scalar operator - () const
    {
      return Scalar( -f4 );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Scalar operator + ( const Scalar& s ) const
    {
      return Scalar( f4 + s.f4 );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    Scalar operator + ( float f ) const
    {
      return Scalar( f4 + float4( f, f, f, f ) );
    }

    /**
     * Sum.
     */
    OZ_ALWAYS_INLINE
    friend Scalar operator + ( float f, const Scalar& s )
    {
      return Scalar( float4( f, f, f, f ) + s.f4 );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Scalar operator - ( const Scalar& s ) const
    {
      return Scalar( f4 - s.f4 );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    Scalar operator - ( float f ) const
    {
      return Scalar( f4 - float4( f, f, f, f ) );
    }

    /**
     * Difference.
     */
    OZ_ALWAYS_INLINE
    friend Scalar operator - ( float f, const Scalar& s )
    {
      return Scalar( float4( f, f, f, f ) - s.f4 );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Scalar operator * ( const Scalar& s ) const
    {
      return Scalar( f4 * s.f4 );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    Scalar operator * ( float f ) const
    {
      return Scalar( f4 * float4( f, f, f, f ) );
    }

    /**
     * Product.
     */
    OZ_ALWAYS_INLINE
    friend Scalar operator * ( float f, const Scalar& s )
    {
      return Scalar( float4( f, f, f, f ) * s.f4 );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    Scalar operator / ( const Scalar& s ) const
    {
      return Scalar( f4 / s.f4 );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    Scalar operator / ( float f ) const
    {
      return Scalar( f4 / float4( f, f, f, f ) );
    }

    /**
     * Quotient.
     */
    OZ_ALWAYS_INLINE
    friend Scalar operator / ( float f, const Scalar& s )
    {
      return Scalar( float4( f, f, f, f ) / s.f4 );
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator += ( const Scalar& s )
    {
      f4 += s.f4;
      return *this;
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator += ( float f )
    {
      f4 += float4( f, f, f, f );
      return *this;
    }

    /**
     * Addition.
     */
    OZ_ALWAYS_INLINE
    friend float& operator += ( float& f, const Scalar& s )
    {
      f += s.x;
      return f;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator -= ( const Scalar& s )
    {
      f4 -= s.f4;
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator -= ( float f )
    {
      f4 -= float4( f, f, f, f );
      return *this;
    }

    /**
     * Subtraction.
     */
    OZ_ALWAYS_INLINE
    friend float& operator -= ( float& f, const Scalar& s )
    {
      f -= s.x;
      return f;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator *= ( const Scalar& s )
    {
      f4 *= s.f4;
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator *= ( float f )
    {
      f4 *= float4( f, f, f, f );
      return *this;
    }

    /**
     * Multiplication.
     */
    OZ_ALWAYS_INLINE
    friend float& operator *= ( float& f, const Scalar& s )
    {
      f *= s.x;
      return f;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator /= ( const Scalar& s )
    {
      f4 /= s.f4;
      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    Scalar& operator /= ( float f )
    {
      f4 /= float4( f, f, f, f );
      return *this;
    }

    /**
     * Division.
     */
    OZ_ALWAYS_INLINE
    friend float& operator /= ( float& f, const Scalar& s )
    {
      f /= s.x;
      return f;
    }

};

}
