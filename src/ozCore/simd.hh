/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#ifdef OZ_SIMD
# if defined(__ARM_NEON__)
#  include <arm_neon.h>
# elif defined(__SSE__)
#  include <xmmintrin.h>
# endif
#endif

namespace oz
{

#ifdef OZ_SIMD

/**
 * SIMD vector of four floats.
 */
#ifdef __ARM_NEON__
using float4 = float32x4_t;
#else
using float4 = float __attribute__((vector_size(16)));
#endif

/**
 * SIMD vector of four unsigned integers.
 */
#ifdef __ARM_NEON__
using uint4 = uint32x4_t;
#else
using uint4 = uint __attribute__((vector_size(16)));
#endif

static_assert(sizeof(float4) >= sizeof(std::max_align_t),
              "Allocation alignment is not sufficent for SIMD types.");

/**
 * Create a float vector with all components set to a given value.
 */
OZ_ALWAYS_INLINE
inline constexpr float4 vFill(float x)
{
  return float4{x, x, x, x};
}

/**
 * Create an uint vector with all components set to a given value.
 */
OZ_ALWAYS_INLINE
inline constexpr uint4 vFill(uint x)
{
  return uint4{x, x, x, x};
}

/**
 * @def vShuffle
 * Shuffle elements of a single vector.
 */
#ifdef __clang__
# define vShuffle(a, i, j, k, l) __builtin_shufflevector(a, a, i, j, k, l)
#else
# define vShuffle(a, i, j, k, l) __builtin_shuffle(a, uint4{i, j, k, l})
#endif

/**
 * Component-wise absolute value of a float vector (accessed as uint vector).
 */
OZ_ALWAYS_INLINE
inline float4 vAbs(float4 a)
{
  return float4(uint4(a) & vFill(0x7fffffffu));
}

/**
 * Component-wise minimum of float vectors.
 */
OZ_ALWAYS_INLINE
inline float4 vMin(float4 a, float4 b)
{
#if defined(__ARM_NEON__)
  return vminq_f32(a, b);
#elif defined(__SSE__)
  return _mm_min_ps(a, b);
#else
  uint4 mask = a < b;
  return float4((mask & uint4(a)) | (~mask & uint4(b)));
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
#elif defined(__SSE__)
  return _mm_max_ps(a, b);
#else
  uint4 mask = a > b;
  return float4((mask & uint4(a)) | (~mask & uint4(b)));
#endif
}

/**
 * Component-wise square root of a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vSqrt(float4 a)
{
#ifdef __SSE__
  return _mm_sqrt_ps(a);
#else
  return vFill(__builtin_sqrtf(a[0]));
#endif
}

/**
 * Component-wise reciprocal square root of a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vInvSqrt(float4 a)
{
#ifdef __SSE__
  return _mm_rsqrt_ps(a);
#else
  return vFill(1.0f / __builtin_sqrtf(a[0]));
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
  float4 s = float4(vFill(0x5f375a86u) - (uint4(a) >> vFill(1u)));
  return s * (vFill(1.5f) - vFill(0.5f) * a * s*s);
#endif
}

/**
 * Scalar product of float vectors returned as a uniform float vector.
 */
OZ_ALWAYS_INLINE
inline float4 vDot(float4 a, float4 b)
{
  float4 p = a * b;

  p += vShuffle(p, 1, 0, 3, 2);
  p += vShuffle(p, 2, 3, 0, 1);
  return p;
}

#endif // OZ_SIMD

/**
 * Base class for 3-component vector and similar algebra structures.
 */
class VectorBase3
{
public:

#ifdef OZ_SIMD
  __extension__ union
  {
    float4 f4 = {0.0f, 0.0f, 0.0f, 0.0f};
    __extension__ struct
    {
      float x;
      float y;
      float z;
      float w;
    };
  };
#else
  float x = 0.0f; ///< X component.
  float y = 0.0f; ///< Y component.
  float z = 0.0f; ///< Z component.
#endif

protected:

  /**
   * Zero vector.
   */
  OZ_ALWAYS_INLINE
  constexpr VectorBase3() = default;

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
#ifdef OZ_SIMD
  explicit constexpr VectorBase3(float x_, float y_, float z_, float w_)
    : f4{x_, y_, z_, w_}
#else
  explicit constexpr VectorBase3(float x_, float y_, float z_, float)
    : x(x_), y(y_), z(z_)
#endif
  {}

public:

#ifdef OZ_SIMD

  OZ_ALWAYS_INLINE
  explicit constexpr VectorBase3(float4 f4_)
    : f4(f4_)
  {}

#endif

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const VectorBase3& v) const
  {
#if defined(OZ_SIMD) && defined(__SSE__)
    return (_mm_movemask_ps(f4 == v.f4) & 0x7) == 0x7;
#elif defined(OZ_SIMD)
    uint4 mask = uint4(f4 == v.f4);

    return (mask & vFill(mask[1]) & vFill(mask[2]))[0] != 0;
#else
    return x == v.x && y == v.y && z == v.z;
#endif
  }

  /**
   * Constant float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator const float*() const
  {
    return &x;
  }

  /**
   * Float pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator float*()
  {
    return &x;
  }

  /**
   * Constant reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  const float& operator[](int i) const
  {
    return (&x)[i];
  }

  /**
   * Reference to the `i`-th member.
   */
  OZ_ALWAYS_INLINE
  float& operator[](int i)
  {
    return (&x)[i];
  }

};

/**
 * Base class for 4-component vector and similar algebra structures.
 */
class VectorBase4 : public VectorBase3
{
public:

#ifndef OZ_SIMD
  float w = 0.0f; ///< W component.
#endif

protected:

  /**
   * Zero vector.
   */
  OZ_ALWAYS_INLINE
  constexpr VectorBase4() = default;

#ifndef OZ_SIMD

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr VectorBase4(float x_, float y_, float z_, float w_)
    : VectorBase3(x_, y_, z_, 0.0f), w(w_)
  {}

#endif

public:

  using VectorBase3::VectorBase3;

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator==(const VectorBase4& v) const
  {
#if defined(OZ_SIMD) && defined(__SSE__)
    return _mm_movemask_ps(f4 == v.f4) == 0xf;
#elif defined(OZ_SIMD)
    uint4 mask = uint4(f4 == v.f4);

    mask &= vShuffle(mask, 1, 0, 3, 2);
    mask &= vShuffle(mask, 2, 3, 0, 1);

    return mask[0] != 0;
#else
    return x == v.x && y == v.y && z == v.z && w == v.w;
#endif
  }

};

}
