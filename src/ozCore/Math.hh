/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include "System.hh"

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
  static constexpr float NaN = __builtin_nanf("");

  /// \f$ +\infty \f$.
  static constexpr float INF = __builtin_inff();

  /// \f$ e \f$.
  static constexpr float E = 2.718281828459045f;

  /// \f$ 2\pi \f$.
  static constexpr float TAU = 6.283185307179586f;

private:

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
   * Static class.
   */
  Math() = delete;

  /**
   * Round to the nearest integer towards negative infinity.
   */
  OZ_ALWAYS_INLINE
  static float floor(float x)
  {
    return __builtin_floorf(x);
  }

  /**
   * Round to the nearest integer towards positive infinity.
   */
  OZ_ALWAYS_INLINE
  static float ceil(float x)
  {
    return __builtin_ceilf(x);
  }

  /**
   * Round to the nearest integer, away from zero.
   */
  OZ_ALWAYS_INLINE
  static float round(float x)
  {
    return __builtin_roundf(x);
  }

  /**
   * Round to the nearest integer, away from zero (return as int).
   */
  OZ_ALWAYS_INLINE
  static int lround(float x)
  {
    return int(__builtin_lroundf(x));
  }

  /**
   * Subtract fractional part.
   */
  OZ_ALWAYS_INLINE
  static float trunc(float x)
  {
    return __builtin_truncf(x);
  }

  /**
   * Reminder.
   */
  OZ_ALWAYS_INLINE
  static float fmod(float x, float y)
  {
    OZ_ASSERT(y != 0.0f);

    return __builtin_fmodf(x, y);
  }

  /**
   * Get integral and fractional parts.
   */
  OZ_ALWAYS_INLINE
  static float modf(float x, float* integral)
  {
    return __builtin_modff(x, integral);
  }

  /**
   * Square root.
   */
  OZ_ALWAYS_INLINE
  static float sqrt(float x)
  {
    OZ_ASSERT(x >= 0.0f);

    return __builtin_sqrtf(x);
  }

  /**
   * Exponent function (base e).
   */
  OZ_ALWAYS_INLINE
  static float exp(float x)
  {
    return __builtin_expf(x);
  }

  /**
   * Exponent function (base 2).
   */
  OZ_ALWAYS_INLINE
  static float exp2(float x)
  {
    return __builtin_exp2f(x);
  }

  /**
   * Exponent function (base 10).
   */
  OZ_ALWAYS_INLINE
  static float exp10(float x)
  {
    return __builtin_expf(__builtin_logf(10.0f) * x);
  }

  /**
   * Logarithm (base e).
   */
  OZ_ALWAYS_INLINE
  static float log(float x)
  {
    OZ_ASSERT(x > 0.0f);

    return __builtin_logf(x);
  }

  /**
   * Logarithm (base 2).
   */
  OZ_ALWAYS_INLINE
  static float log2(float x)
  {
    OZ_ASSERT(x > 0.0f);

    return __builtin_log2f(x);
  }

  /**
   * Logarithm (base 10).
   */
  OZ_ALWAYS_INLINE
  static float log10(float x)
  {
    OZ_ASSERT(x > 0.0f);

    return __builtin_log10f(x);
  }

  /**
   * Power.
   */
  OZ_ALWAYS_INLINE
  static float pow(float x, float y)
  {
    OZ_ASSERT(x > 0.0f || (x == 0.0f && y >= 0.0f));

    return __builtin_powf(x, y);
  }

  /**
   * Sine.
   */
  OZ_ALWAYS_INLINE
  static float sin(float x)
  {
    return __builtin_sinf(x);
  }

  /**
   * Cosine.
   */
  OZ_ALWAYS_INLINE
  static float cos(float x)
  {
    return __builtin_cosf(x);
  }

  /**
   * `sincos` function, calculates both sine and cosine at the same time.
   */
  OZ_ALWAYS_INLINE
  static void sincos(float x, float* s, float* c)
  {
    // No need to use sincosf(). GCC optimises the following calls into one sincosf() call and
    // LLVM/Clang is missing a built-in for sincosf().
    *s = __builtin_sinf(x);
    *c = __builtin_cosf(x);
  }

  /**
   * Tangent function.
   */
  OZ_ALWAYS_INLINE
  static float tan(float x)
  {
    return __builtin_tanf(x);
  }

  /**
   * Arc sine.
   */
  OZ_ALWAYS_INLINE
  static float asin(float x)
  {
    OZ_ASSERT(-1.0f <= x && x <= 1.0f);

    return __builtin_asinf(x);
  }

  /**
   * Arc cosine.
   */
  OZ_ALWAYS_INLINE
  static float acos(float x)
  {
    OZ_ASSERT(-1.0f <= x && x <= 1.0f);

    return __builtin_acosf(x);
  }

  /**
   * Arc tangent function.
   */
  OZ_ALWAYS_INLINE
  static float atan(float x)
  {
    return __builtin_atanf(x);
  }

  /**
   * Arc tangent of two variables.
   */
  OZ_ALWAYS_INLINE
  static float atan2(float x, float y)
  {
    return __builtin_atan2f(x, y);
  }

  /**
   * True iff the number is not NaN or infinity.
   */
  OZ_ALWAYS_INLINE
  static bool isFinite(float x)
  {
#if __FINITE_MATH_ONLY__
    return (toBits(x) << 1) < 0xff000000;
#else
    return __builtin_isfinite(x) != 0;
#endif
  }

  /**
   * True iff the number is (positive or negative) infinity.
   */
  OZ_ALWAYS_INLINE
  static bool isInf(float x)
  {
#if __FINITE_MATH_ONLY__
    return (toBits(x) << 1) == 0xff000000;
#else
    return __builtin_isinf(x) != 0;
#endif
  }

  /**
   * True iff the number is NaN.
   */
  OZ_ALWAYS_INLINE
  static bool isNaN(float x)
  {
#if __FINITE_MATH_ONLY__
    return (toBits(x) << 1) > 0xff000000;
#else
    return __builtin_isnan(x) != 0;
#endif
  }

  /**
   * True iff the number is not subnormal.
   */
  OZ_ALWAYS_INLINE
  static bool isNormal(float x)
  {
    return __builtin_isnormal(x) != 0;
  }

  /**
   * Sign, -1.0 for negative and 1.0 for non-negative.
   */
  OZ_ALWAYS_INLINE
  static float sgn(float x)
  {
    return x < 0.0f ? -1.0f : 1.0f;
  }

  /**
   * Remainder, always non-negative, on interval [0, y).
   */
  OZ_ALWAYS_INLINE
  static float mod(float x, float y)
  {
    OZ_ASSERT(y > 0.0f);

    return x - __builtin_floorf(x / y) * y;
  }

  /**
   * Convert degrees to radians.
   */
  OZ_ALWAYS_INLINE
  static constexpr float rad(float x)
  {
    return TAU / 360.0f * x;
  }

  /**
   * Convert radians to degrees.
   */
  OZ_ALWAYS_INLINE
  static constexpr float deg(float x)
  {
    return 360.0f / TAU * x;
  }

  /**
   * Get floating-point value as a bit-field.
   */
  OZ_ALWAYS_INLINE
  static uint toBits(float x)
  {
    return FloatToBits{x}.bits;
  }

  /**
   * Get floating-point value represented by a bit-field.
   */
  OZ_ALWAYS_INLINE
  static float fromBits(uint b)
  {
    return BitsToFloat{b}.value;
  }

  /**
   * Fast square root (using improved algorithm from Quake).
   */
  OZ_ALWAYS_INLINE
  static float fastSqrt(float x)
  {
    OZ_ASSERT(x >= 0.0f);

    FloatToBits fb = {x};

    fb.bits = 0x5f375a86 - (fb.bits >> 1);
    return x * fb.value * (1.5f - 0.5f * x * fb.value*fb.value);
  }

  /**
   * Fast reciprocal square root (using improved algorithm from Quake).
   */
  OZ_ALWAYS_INLINE
  static float fastInvSqrt(float x)
  {
    OZ_ASSERT(x > 0.0f);

    FloatToBits fb = {x};

    fb.bits = 0x5f375a86 - (fb.bits >> 1);
    return fb.value * (1.5f - 0.5f * x * fb.value*fb.value);
  }

  /**
   * Convex combination.
   */
  template <typename Value>
  OZ_ALWAYS_INLINE
  static constexpr Value mix(const Value& a, const Value& b, float t)
  {
    return a + t * (b - a);
  }

  /**
   * 0.0 if x < step, else 1.0.
   */
  OZ_ALWAYS_INLINE
  static constexpr float step(float edge, float t)
  {
    return t < edge ? 0 : 1;
  }

  /**
   * \f$ [0, 1] \to [0, 1]: t \mapsto 3 t^2 - 2 t^3\f$.
   */
  OZ_ALWAYS_INLINE
  static constexpr float smooth(float t)
  {
    return t*t * (3 - 2*t);
  }

  /**
   * Index of the first 1 bit (counting from the least significant bit) or -1 if `v` is 0.
   */
  OZ_ALWAYS_INLINE
  static constexpr int index1(int i)
  {
    return i == 0 ? -1 : sizeof(int) * 8 - 1 - __builtin_clz(i);
  }

  /**
   * True iff a given integer is a power of 2.
   */
  OZ_ALWAYS_INLINE
  static constexpr bool isPow2(int i)
  {
    return i > 0 && (i & (i - 1)) == 0;
  }

  /**
   * The first power of 2 greater than a given integer.
   */
  OZ_ALWAYS_INLINE
  static constexpr int nextPow2(int i)
  {
    return i <= 0 ? 1 : 1 << (sizeof(int) * 8 - __builtin_clz(i));
  }

  /**
   * Align to the previous boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type alignDown(Type size, size_t alignment = OZ_ALIGNMENT)
  {
    return Type(size_t(size) & ~(alignment - 1));
  }

  /**
   * Align to the next boundary.
   */
  template <typename Type>
  OZ_ALWAYS_INLINE
  static constexpr Type alignUp(Type size, size_t alignment = OZ_ALIGNMENT)
  {
    return Type((size_t(size) + alignment - 1) & ~(alignment - 1));
  }

  /**
   * %Set seed for random generator.
   */
  static void seed(int n);

  /**
   * Random integer from `[min, max]`.
   */
  static int rand(int min, int max);

  /**
   * Random float number from [min, max].
   */
  static float rand(float min, float max);

  /**
   * Random number with standard distribution.
   */
  static float normalRand(float mean = 0.0f, float deviation = 1.0f);

};

}
