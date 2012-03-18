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
 * @file oz/Math.hh
 *
 * Math class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Replacement for <tt>\<cmath\></tt> plus some other utility functions.
 *
 * @ingroup oz
 */
class Math
{
  public:

    /// Floating point epsilon.
    static constexpr float FLOAT_EPS = __FLT_EPSILON__;

    /// Not a number.
    static constexpr float NaN = __builtin_nanf( "" );

    /// \f$ +\infty \f$.
    static constexpr float INF = __builtin_inff();

    /// \f$ e \f$.
    static constexpr float E = 2.718281828459045f;

    /// \f$ 2\pi \f$.
    static constexpr float TAU = 6.283185307179586f;

    /// \f$ \sqrt{2} \f$.
    static constexpr float SQRT_2 = 1.414213562373095f;

    /// \f$ \sqrt{3} \f$.
    static constexpr float SQRT_3 = 1.732050807568877f;

    /**
     * Singleton.
     */
    Math() = delete;

    /**
     * Absolute value.
     */
    OZ_ALWAYS_INLINE
    static float fabs( float x )
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
      hard_assert( y != 0.0f );

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
      hard_assert( x >= 0.0f );

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
      hard_assert( x > 0.0f );

      return __builtin_logf( x );
    }

    /**
     * Power.
     */
    OZ_ALWAYS_INLINE
    static float pow( float x, float y )
    {
      hard_assert( x >= 0.0f );

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
      // No need to use sincosf(). GCC optimises the following calls into one sincosf() call and
      // LLVM/Clang is missing built-in for sincosf().
      *s = __builtin_sinf( x );
      *c = __builtin_cosf( x );
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
      hard_assert( -1.0f <= x && x <= 1.0f );

      return __builtin_asinf( x );
    }

    /**
     * Arc cosine.
     */
    OZ_ALWAYS_INLINE
    static float acos( float x )
    {
      hard_assert( -1.0f <= x && x <= 1.0f );

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
#if !defined( __clang__ ) && defined( __FAST_MATH__ )
      // GCC's isfinite() is broken with -ffast-math.
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
#if !defined( __clang__ ) && defined( __FAST_MATH__ )
      // GCC's isinf() is broken with -ffast-math.
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
#if !defined( __clang__ ) && defined( __FAST_MATH__ )
      // GCC's isnan() is broken with -ffast-math.
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
     * Get a float from its per-bit representation.
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
     * Fast square root (calculated via fast square root from Quake).
     */
    OZ_ALWAYS_INLINE
    static float fastSqrt( float x )
    {
      float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
      return x * y * ( 1.5f - 0.5f * x * y*y );
    }

    /**
     * Fast inverse square root, the one used in Quake.
     */
    OZ_ALWAYS_INLINE
    static float fastInvSqrt( float x )
    {
      hard_assert( x != 0.0f );

      float y = fromBits( 0x5f3759df - ( toBits( x ) >> 1 ) );
      return y * ( 1.5f - 0.5f * x * y*y );
    }

    /**
     * True iff the integer is either a power of 2 or zero.
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
      hard_assert( 0.0f <= t && t <= 1.0f );

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
