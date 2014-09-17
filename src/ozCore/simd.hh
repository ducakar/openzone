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
 * @file ozCore/simd.hh
 *
 * SIMD type definitions and utility functions.
 */

#pragma once

#include "common.hh"

#ifdef OZ_SIMD_MATH
# if defined(__ARM_NEON__)
#  include <arm_neon.h>
# elif defined(__SSE__)
#  include <xmmintrin.h>
# else
#  error OZ_SIMD_MATH is only implemented for SSE1 and ARM NEON.
# endif
#endif

namespace oz
{

#ifdef OZ_SIMD_MATH

/**
 * SIMD vector of four floats.
 */
#ifdef __ARM_NEON__
typedef float32x4_t float4;
#else
typedef float __attribute__((vector_size(16))) float4;
#endif

/**
 * SIMD vector of four unsigned integers.
 */
#ifdef __ARM_NEON__
typedef uint32x4_t uint4;
#else
typedef uint __attribute__((vector_size(16))) uint4;
#endif

/**
 * Construct a float vector with given components.
 */
OZ_ALWAYS_INLINE
inline float4 vFill(float x, float y, float z, float w)
{
  return float4 { x, y, z, w };
}

/**
 * Construct an uniform (i.e. all components identical) float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vFill(float x)
{
  return float4 { x, x, x, x };
}

/**
 * Construct an uint vector with given components.
 */
OZ_ALWAYS_INLINE
inline uint4 vFill(uint x, uint y, uint z, uint w)
{
  return uint4 { x, y, z, w };
}

/**
 * Construct an uniform (i.e. all components identical) uint vector.
 */
OZ_ALWAYS_INLINE
inline uint4 vFill(uint x)
{
  return uint4 { x, x, x, x };
}

/**
 * First component of a vector.
 */
OZ_ALWAYS_INLINE
inline float vFirst(float4 a)
{
#ifdef __ARM_NEON__
  return vgetq_lane_f32(a, 0);
#else
  return _mm_cvtss_f32(a);
#endif
}

/**
 * @def vShuffle
 * Shuffle elements of a single vector.
 */
#if defined(OZ_CLANG)
# define vShuffle(a, i, j, k, l) __builtin_shufflevector(a, a, i, j, k, l)
#else
# define vShuffle(a, i, j, k, l) __builtin_shuffle(a, uint4 { i, j, k, l })
#endif

/**
 * Component-wise absolute value of a float vector (accessed as uint vector).
 */
OZ_ALWAYS_INLINE
inline uint4 vAbs(uint4 a)
{
  return a & vFill(0x7fffffffu);
}

/**
 * Component-wise minimum of float vectors.
 */
OZ_ALWAYS_INLINE
inline float4 vMin(float4 a, float4 b)
{
#if defined(__ARM_NEON__)
  return vminq_f32(a, b);
#else
  return _mm_min_ps(a, b);
#endif
}

/**
 * Component-wise maximum of float vectors.
 */
OZ_ALWAYS_INLINE
inline float4 vMax(float4 a, float4 b)
{
#if defined(__ARM_NEON__)
  return vmaxq_f32(a, b);
#else
  return _mm_max_ps(a, b);
#endif
}

/**
 * Component-wise square root of a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vSqrt(float4 a)
{
#if defined(__ARM_NEON__)
  return vFill(__builtin_sqrtf(vFirst(a)));
#else
  return _mm_sqrt_ps(a);
#endif
}

/**
 * Component-wise reciprocal square root of a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vInvSqrt(float4 a)
{
#if defined(__ARM_NEON__)
  return vFill(1.0f / __builtin_sqrtf(vFirst(a)));
#else
  return _mm_rsqrt_ps(a);
#endif
}

/**
 * Component-wise fast inverse square root of a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vFastInvSqrt(float4 a)
{
#ifdef __ARM_NEON__

  return vrsqrteq_f32(a);

#else

  union Float4Bits
  {
    float4 f4;
    uint4  u4;
  };
  Float4Bits s = { a };

  s.u4 = vFill(0x5f375a86u) - (s.u4 >> vFill(1u));
  return s.f4 * (vFill(1.5f) - vFill(0.5f) * a * s.f4*s.f4);

#endif
}

/**
 * Scalar product of float vectors returned as a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vDot(float4 a, float4 b)
{
  float4 p = a * b;

#ifdef __ARM_NEON__
  p += vrev64q_f32(p);
  p += vcombine_f32(vget_high_f32(p), vget_low_f32(p));
#else
  p += vShuffle(p, 1, 0, 3, 2);
  p += vShuffle(p, 2, 3, 0, 1);
#endif
  return p;
}

#endif // OZ_SIMD_MATH

/**
 * Base class for 3-component vector and similar algebra structures.
 */
class VectorBase3
{
public:

#ifdef OZ_SIMD_MATH
  __extension__ union
  {
    float4 f4;
    uint4  u4;
    __extension__ struct
    {
      float x;
      float y;
      float z;
    };
  };
#else
  float x; ///< X component.
  float y; ///< Y component.
  float z; ///< Z component.
#endif

protected:

#ifdef OZ_SIMD_MATH

  OZ_ALWAYS_INLINE
  explicit VectorBase3(float4 f4_) :
    f4(f4_)
  {}

  OZ_ALWAYS_INLINE
  explicit VectorBase3(uint4 u4_) :
    u4(u4_)
  {}

  OZ_ALWAYS_INLINE
  explicit VectorBase3(float x_, float y_, float z_, float w_) :
    f4(vFill(x_, y_, z_, w_))
  {}

#else

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit VectorBase3(float x_, float y_, float z_, float) :
    x(x_), y(y_), z(z_)
  {}

#endif

public:

  /**
   * Constant float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator const float* () const
  {
    return &x;
  }

  /**
   * Float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator float* ()
  {
    return &x;
  }

  /**
   * Constant reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  const float& operator [] (int i) const
  {
    return (&x)[i];
  }

  /**
   * Reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  float& operator [] (int i)
  {
    return (&x)[i];
  }

};

/**
 * Base class for 4-component vector and similar algebra structures.
 */
class VectorBase4
{
public:

#ifdef OZ_SIMD_MATH
  __extension__ union
  {
    float4 f4;
    uint4  u4;
    __extension__ struct
    {
      float x;
      float y;
      float z;
      float w;
    };
  };
#else
  float x; ///< X component.
  float y; ///< Y component.
  float z; ///< Z component.
  float w; ///< W component.
#endif

protected:

#ifdef OZ_SIMD_MATH

  OZ_ALWAYS_INLINE
  explicit VectorBase4(float4 f4_) :
    f4(f4_)
  {}

  OZ_ALWAYS_INLINE
  explicit VectorBase4(uint4 u4_) :
    u4(u4_)
  {}

  OZ_ALWAYS_INLINE
  explicit VectorBase4(float x_, float y_, float z_, float w_) :
    f4(vFill(x_, y_, z_, w_))
  {}

#else

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit VectorBase4(float x_, float y_, float z_, float w_) :
    x(x_), y(y_), z(z_), w(w_)
  {}

#endif

public:

  /**
   * Constant float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator const float* () const
  {
    return &x;
  }

  /**
   * Float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator float* ()
  {
    return &x;
  }

  /**
   * Constant reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  const float& operator [] (int i) const
  {
    return (&x)[i];
  }

  /**
   * Reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  float& operator [] (int i)
  {
    return (&x)[i];
  }

};

}
