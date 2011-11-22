/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Math.hpp
 */

#pragma once

#include "Pair.hpp"

namespace oz
{

/**
 * Replacement for \<cmath\> plus some other utility functions.
 *
 * @ingroup oz
 */
class Math
{
  public:

    /// Floating point epsilon.
    static const float EPSILON;

    /// Number e.
    static const float E;

    /// \f$ 2\pi \f$.
    static const float TAU;

    /// \f$ +\infty \f$.
    static const float INF;

    /// Not a number.
    static const float NaN;

    /**
     * Singleton.
     */
    Math() = delete;

    /**
     * Absolute value.
     */
    OZ_ALWAYS_INLINE
    static float abs( float x )
    {
      return __builtin_fabsf( x );
    }

    /**
     * Round to the closest integer towards negative infinity.
     */
    OZ_ALWAYS_INLINE
    static float floor( float x )
    {
      return __builtin_floorf( x );
    }

    /**
     * Round to the closest integer towards positive infinity.
     */
    OZ_ALWAYS_INLINE
    static float ceil( float x )
    {
      return __builtin_ceilf( x );
    }

    /**
     * Round to the closest integer.
     */
    OZ_ALWAYS_INLINE
    static float round( float x )
    {
      return __builtin_roundf( x );
    }

    /**
     * Subtract fractional part.
     */
    OZ_ALWAYS_INLINE
    static float trunc( float x )
    {
      return __builtin_truncf( x );
    }

    /**
     * Reminder.
     */
    OZ_ALWAYS_INLINE
    static float fmod( float x, float y )
    {
      return __builtin_fmodf( x, y );
    }

    /**
     * Get integral and fractional parts.
     */
    OZ_ALWAYS_INLINE
    static float modf( float x, float* integral )
    {
      return __builtin_modff( x, integral );
    }

    /**
     * Square root.
     */
    OZ_ALWAYS_INLINE
    static float sqrt( float x )
    {
      return __builtin_sqrtf( x );
    }

    /**
     * Base-e exponent function.
     */
    OZ_ALWAYS_INLINE
    static float exp( float x )
    {
      return __builtin_expf( x );
    }

    /**
     * Logarithm.
     */
    OZ_ALWAYS_INLINE
    static float log( float x )
    {
      return __builtin_logf( x );
    }

    /**
     * Power.
     */
    OZ_ALWAYS_INLINE
    static float pow( float x, float y )
    {
      return __builtin_powf( x, y );
    }

    /**
     * Sine.
     */
    OZ_ALWAYS_INLINE
    static float sin( float x )
    {
      return __builtin_sinf( x );
    }

    /**
     * Cosine.
     */
    OZ_ALWAYS_INLINE
    static float cos( float x )
    {
      return __builtin_cosf( x );
    }

    /**
     * sincos function, calculates both sine and cosine at a time.
     */
    OZ_ALWAYS_INLINE
    static void sincos( float x, float* s, float* c )
    {
#if defined( OZ_HAVE_SINCOSF ) && !defined( __clang__ )
      __builtin_sincosf( x, s, c );
#else
      *s = __builtin_sinf( x );
      *c = __builtin_cosf( x );
#endif
    }

    /**
     * Tangent function.
     */
    OZ_ALWAYS_INLINE
    static float tan( float x )
    {
      return __builtin_tanf( x );
    }

    /**
     * Arc sine.
     */
    OZ_ALWAYS_INLINE
    static float asin( float x )
    {
      return __builtin_asinf( x );
    }

    /**
     * Arc cosine.
     */
    OZ_ALWAYS_INLINE
    static float acos( float x )
    {
      return __builtin_acosf( x );
    }

    /**
     * Arc tangent function.
     */
    OZ_ALWAYS_INLINE
    static float atan( float x )
    {
      return __builtin_atanf( x );
    }

    /**
     * Arc tangent of two variables.
     */
    OZ_ALWAYS_INLINE
    static float atan2( float x, float y )
    {
      return __builtin_atan2f( x, y );
    }

    /**
     * True iff the number is not NaN or infinity.
     */
    OZ_ALWAYS_INLINE
    static bool isfinite( float x )
    {
#if defined( __GNUC__ ) && !defined( __clang__ )
      return x + 1.0f != x;
#else
      return __builtin_isfinite( x );
#endif
    }

    /**
     * True iff the number (positive or negative) infinity.
     */
    OZ_ALWAYS_INLINE
    static int isinf( float x )
    {
#if defined( __GNUC__ ) && !defined( __clang__ )
      return x + 1.0f == x && x * 0.0f != x;
#else
      return __builtin_isinf( x );
#endif
    }

    /**
     * True iff the number is NaN.
     */
    OZ_ALWAYS_INLINE
    static bool isnan( float x )
    {
#if defined( __GNUC__ ) && !defined( __clang__ )
      return x + 1.0f == x && x * 0.0f == x;
#else
      return __builtin_isnan( x );
#endif
    }

    /**
     * True iff the number is not subnormal (i.e. too close to zero for the whole mantissa to be
     * used).
     */
    OZ_ALWAYS_INLINE
    static bool isnormal( float x )
    {
      return __builtin_isnormal( x );
    }

    /**
     * Sign, -1.0 for negative and 1.0 for non-negative.
     */
    OZ_ALWAYS_INLINE
    static float sgn( float x )
    {
      return x < 0.0f ? -1.0f : 1.0f;
    }

    /**
     * Convert degrees to radians.
     */
    OZ_ALWAYS_INLINE
    static float rad( float x )
    {
      return x * ( TAU / 360.0f );
    }

    /**
     * Convert radians to degrees.
     */
    OZ_ALWAYS_INLINE
    static float deg( float x )
    {
      return x * ( 360.0f / TAU );
    }

    /**
     * Get per-bit float representation in an integer.
     */
    OZ_ALWAYS_INLINE
    static int toBits( float x )
    {
      union FloatToBits
      {
        float f;
        int   b;
      };
      FloatToBits fb = { x };
      return fb.b;
    }

    /**
     * Get a float from its per.bit representation.
     */
    OZ_ALWAYS_INLINE
    static float fromBits( int i )
    {
      union BitsToFloat
      {
        int   b;
        float f;
      };
      BitsToFloat bf = { i };
      return bf.f;
    }

    /**
     * Fast square root.
     */
    OZ_ALWAYS_INLINE
    static float fastSqrt( float x )
    {
      return x * fastInvSqrt( x );
    }

    /**
     * Fast inverse square root, the on used in Quake.
     */
    OZ_ALWAYS_INLINE
    static float fastInvSqrt( float x )
    {
      float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
      return y * ( 1.5f - 0.5f * x * y*y );
    }

    /**
     * True iff the integer is a power of 2.
     */
    template <typename Value>
    OZ_ALWAYS_INLINE
    static bool isPow2( const Value& v )
    {
      return ( v & ( v - 1 ) ) == 0;
    }

    /**
     * Linear interpolation.
     */
    template <typename Value>
    OZ_ALWAYS_INLINE
    static Value mix( const Value& a, const Value& b, float t )
    {
      return a + t * ( b - a );
    }

    /**
     * Set seed for random generator.
     */
    static void seed( int n );

    /**
     * Random integer between from [0, <tt>max</tt>).
     */
    static int rand( int max );

    /**
     * Random float number from [0, 1].
     */
    static float rand();

    /**
     * Random float number from [-1, 1].
     */
    static float centralRand();

    /**
     * Random number from [-1, 1], numbers close to zero are more likely.
     *
     * It tries to generate something "relatively" close to Gaussian distribution.
     * It actually returns \f$ x^3 \f$, for an \f$ x \in [-1, +1] \f$ returned by <tt>rand()</tt>.
     */
    static float normalRand();

};

}
