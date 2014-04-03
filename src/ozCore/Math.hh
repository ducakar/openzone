/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file ozCore/Math.hh
 *
 * `Math` class.
 */

#pragma once

#include "common.hh"

namespace oz
{

/**
 * Replacement for `\<cmath\>` plus some other utility functions.
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

  /**
   * Union for accessing a bitwise representation of a float value.
   */
  union FloatToBits
  {
    float value; ///< Float value.
    uint  bits;  ///< Bits.
  };

  /**
   * Union for obtaining a float value represented by a bit stream.
   */
  union BitsToFloat
  {
    uint  bits;  ///< Bits.
    float value; ///< Float value.
  };

public:

  /**
   * Forbid instances.
   */
  explicit Math() = delete;

  /**
   * Round to the nearest integer towards negative infinity.
   */
  OZ_ALWAYS_INLINE
  static float floor( float x )
  {
    return __builtin_floorf( x );
  }

  /**
   * Round to the nearest integer towards positive infinity.
   */
  OZ_ALWAYS_INLINE
  static float ceil( float x )
  {
    return __builtin_ceilf( x );
  }

  /**
   * Round to the nearest integer, away from zero.
   */
  OZ_ALWAYS_INLINE
  static float round( float x )
  {
    return __builtin_roundf( x );
  }

  /**
   * Round to the nearest integer, away from zero (return as int).
   */
  OZ_ALWAYS_INLINE
  static int lround( float x )
  {
    return int( __builtin_lroundf( x ) );
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
    return hard_assert( y != 0.0f ), __builtin_fmodf( x, y );
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
    return hard_assert( x >= 0.0f ), __builtin_sqrtf( x );
  }

  /**
   * Exponent function (base e).
   */
  OZ_ALWAYS_INLINE
  static float exp( float x )
  {
    return __builtin_expf( x );
  }

  /**
   * Exponent function (base 2).
   */
  OZ_ALWAYS_INLINE
  static float exp2( float x )
  {
    return __builtin_exp2f( x );
  }

  /**
   * Exponent function (base 10).
   */
  OZ_ALWAYS_INLINE
  static float exp10( float x )
  {
#ifdef OZ_CLANG
    return __builtin_expf( __builtin_logf( 10.0f ) * x );
#else
    return __builtin_exp10f( x );
#endif
  }

  /**
   * Logarithm (base e).
   */
  OZ_ALWAYS_INLINE
  static float log( float x )
  {
    return hard_assert( x > 0.0f ), __builtin_logf( x );
  }

  /**
   * Logarithm (base 2).
   */
  OZ_ALWAYS_INLINE
  static float log2( float x )
  {
    return hard_assert( x > 0.0f ), __builtin_log2f( x );
  }

  /**
   * Logarithm (base 10).
   */
  OZ_ALWAYS_INLINE
  static float log10( float x )
  {
    return hard_assert( x > 0.0f ), __builtin_log10f( x );
  }

  /**
   * Power.
   */
  OZ_ALWAYS_INLINE
  static float pow( float x, float y )
  {
    return hard_assert( x > 0.0f || ( x == 0.0f && y >= 0.0f ) ), __builtin_powf( x, y );
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
   * `sincos` function, calculates both sine and cosine at a time.
   */
  OZ_ALWAYS_INLINE
  static void sincos( float x, float* s, float* c )
  {
    // No need to use sincosf(). GCC optimises the following calls into one sincosf() call and
    // LLVM/Clang is missing a built-in for sincosf().
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
    return hard_assert( -1.0f <= x && x <= 1.0f ), __builtin_asinf( x );
  }

  /**
   * Arc cosine.
   */
  OZ_ALWAYS_INLINE
  static float acos( float x )
  {
    return hard_assert( -1.0f <= x && x <= 1.0f ), __builtin_acosf( x );
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
  static constexpr bool isFinite( float x )
  {
    // isfinite() is broken and NaN = NaN in GCC with -ffinite-math-only (implied by -ffast-math).
    // Furthermore, those expressions are faster than __builtin_isfinite().
#if defined( OZ_GCC ) && defined( __FINITE_MATH_ONLY__ )
    return x + 1.0e38f != x;
#else
    return x + 1.0e38f != x && x == x;
#endif
  }

  /**
   * True iff the number (positive or negative) infinity.
   */
  OZ_ALWAYS_INLINE
  static constexpr bool isInf( float x )
  {
    // isinf() is broken and NaN = NaN in GCC with -ffinite-math-only (implied by -ffast-math).
    // Furthermore, those expressions are faster than __builtin_isinf().
#if defined( OZ_GCC ) && defined( __FINITE_MATH_ONLY__ )
    return x + 1.0e38f == x && x * 0.0f != x;
#else
    return x + 1.0e38f == x;
#endif
  }

  /**
   * True iff the number is NaN.
   */
  OZ_ALWAYS_INLINE
  static constexpr bool isNaN( float x )
  {
    // isnan() is broken and NaN = NaN in GCC with -ffinite-math-only (implied by -ffast-math).
    // Furthermore, those expressions are faster than __builtin_isnan().
#if defined( OZ_GCC ) && defined( __FINITE_MATH_ONLY__ )
    return x + 1.0e38f == x && x * 0.0f == x;
#else
    return x != x;
#endif
  }

  /**
   * True iff the number is not subnormal.
   */
  OZ_ALWAYS_INLINE
  static constexpr bool isNormal( float x )
  {
    return __builtin_isnormal( x );
  }

  /**
   * Sign, -1.0 for negative and 1.0 for non-negative.
   */
  OZ_ALWAYS_INLINE
  static constexpr float sgn( float x )
  {
    return x < 0.0f ? -1.0f : 1.0f;
  }

  /**
   * Remainder, always non-negative, on interval [0, y).
   */
  OZ_ALWAYS_INLINE
  static float mod( float x, float y )
  {
    return hard_assert( y > 0.0f ), x - __builtin_floorf( x / y ) * y;
  }

  /**
   * Convert degrees to radians.
   */
  OZ_ALWAYS_INLINE
  static constexpr float rad( float x )
  {
    return TAU / 360.0f * x;
  }

  /**
   * Convert radians to degrees.
   */
  OZ_ALWAYS_INLINE
  static constexpr float deg( float x )
  {
    return 360.0f / TAU * x;
  }

  /**
   * Get floating-point value as a bit-field.
   */
  OZ_ALWAYS_INLINE
  static uint toBits( float x )
  {
    FloatToBits fb = { x };
    return fb.bits;
  }

  /**
   * Get floating-point value represented by a bit-field.
   */
  OZ_ALWAYS_INLINE
  static float fromBits( uint b )
  {
    BitsToFloat bf = { b };
    return bf.value;
  }

  /**
   * Fast square root (using improved algorithm from Quake).
   */
  OZ_ALWAYS_INLINE
  static float fastSqrt( float x )
  {
    hard_assert( x >= 0.0f );

    FloatToBits fb = { x };

    fb.bits = 0x5f375a86 - ( fb.bits >> 1 );
    return x * fb.value * ( 1.5f - 0.5f * x * fb.value*fb.value );
  }

  /**
   * Fast inverse square root (using improved algorithm from Quake).
   */
  OZ_ALWAYS_INLINE
  static float fastInvSqrt( float x )
  {
    hard_assert( x > 0.0f );

    FloatToBits fb = { x };

    fb.bits = 0x5f375a86 - ( fb.bits >> 1 );
    return fb.value * ( 1.5f - 0.5f * x * fb.value*fb.value );
  }

  /**
   * Index of the first 1 bit (counting from the least significant bit) or -1 if `v` is 0.
   */
  template <typename Value = int>
  OZ_ALWAYS_INLINE
  static constexpr int index1( Value v )
  {
    return v == 0 ? -1 : int( sizeof( ulong64 ) * 8 - 1 ) - __builtin_clzll( ulong64( v ) );
  }

  /**
   * True iff a given positive integer is a power of 2.
   */
  template <typename Value = int>
  OZ_ALWAYS_INLINE
  static constexpr bool isPow2( Value v )
  {
    return 0 < v && ( v & ( v - 1 ) ) == 0;
  }

  /**
   * Convex combination.
   */
  template <typename Value>
  OZ_ALWAYS_INLINE
  static constexpr Value mix( const Value& a, const Value& b, float t )
  {
    return hard_assert( 0.0f <= t && t <= 1.0f ), a + t * ( b - a );
  }

  /**
   * %Set seed for random generator.
   */
  static void seed( int n );

  /**
   * Random integer between from [0, `max`).
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
   * It actually returns \f$ x^3 \f$, for an \f$ x \in [-1, +1] \f$ returned by `rand()`.
   */
  static float normalRand();

};

}
