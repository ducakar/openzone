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
 * Davorin Učakar <davorin.ucakar@gmail.com>
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

    /// 2π.
    static const float TAU;

    /// +∞.
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
    static float mod( float x, float y )
    {
      return __builtin_fmodf( x, y );
    }

    /**
     * Get integral and fractional parts.
     */
    OZ_ALWAYS_INLINE
    static Pair<float> fract( float x )
    {
      float integral;
      float fractional = __builtin_modff( x, &integral );
      return Pair<float>( integral, fractional );
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
     * True iff the number is "Not a Number".
     *
     * Function does not work with <tt>-ffast-math</tt> turned on.
     */
    OZ_ALWAYS_INLINE
    static bool isNaN( float x )
    {
      return __builtin_isnan( x );
    }

    /**
     * True iff the number is not "Not a Number" or infinity.
     *
     * Function does not work with <tt>-ffast-math</tt> turned on.
     */
    OZ_ALWAYS_INLINE
    static bool isFinite( float x )
    {
      return __builtin_isfinite( x );
    }

    /**
     * True iff the number (positive or negative) infinity.
     *
     * Function does not work with <tt>-ffast-math</tt> turned on.
     */
    OZ_ALWAYS_INLINE
    static int isInf( float x )
    {
      return __builtin_isinf( x );
    }

    /**
     * True iff the number is not subnormal (i.e. too close to zero for the whole mantissa to be
     * used).
     *
     * Function does not work with <tt>-ffast-math</tt> turned on.
     */
    OZ_ALWAYS_INLINE
    static bool isNormal( float x )
    {
      return __builtin_isnormal( x );
    }

    /**
     * True iff the number is (positive or negative) infinity.
     *
     * Function works with <tt>-ffast-math</tt>.
     */
    OZ_ALWAYS_INLINE
    static int isInfFM( float x )
    {
      return x == 2.0f * x;
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
     * Set seed for random generator.
     */
    static void seed( int n );

    /**
     * Random integer between 0 and <tt>max</tt> non-inclusively.
     */
    static int rand( int max );

    /**
     * Random float number from [0, 1].
     */
    static float rand();

};

}
