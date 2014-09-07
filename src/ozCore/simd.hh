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

#if defined(OZ_SIMD_MATH) && !defined(__SSE3__)
# error OZ_SIMD_MATH is only implemented for SSE3.
#endif

namespace oz
{

#ifdef OZ_SIMD_MATH

/**
 * SIMD vector of four floats.
 */
typedef float __attribute__((vector_size(16))) float4;

/**
 * SIMD vector of four unsigned integers.
 */
typedef uint __attribute__((vector_size(16))) uint4;

/**
 * SIMD vector that can be accessed either as float4 or uint4.
 */
union simd4
{
  float4 f4;
  uint4  u4;
};

OZ_ALWAYS_INLINE
inline float4 vFill(float x, float y, float z, float w)
{
  return float4 { x, y, z, w };
}

OZ_ALWAYS_INLINE
inline float4 vFill(float x)
{
  return float4 { x, x, x, x };
}

OZ_ALWAYS_INLINE
inline uint4 vFill(uint x, uint y, uint z, uint w)
{
  return uint4 { x, y, z, w };
}

OZ_ALWAYS_INLINE
inline uint4 vFill(uint x)
{
  return uint4 { x, x, x, x };
}

OZ_ALWAYS_INLINE
inline float vFirst(float4 a)
{
#if defined(OZ_CLANG)
  return a[0];
#elif defined(__ARM_NEON__)
  return __builtin_neon_vget_lanev4sf(a, 0, 3);
#else
  return __builtin_ia32_vec_ext_v4sf(a, 0);
#endif
}

/**
 * @def vShuffle
 * Compiler- and platform-dependent built-in function for SIMD vector shuffle.
 */
#if defined(OZ_CLANG)
# define vShuffle(a, b, i, j, k, l) \
  __builtin_shufflevector(a, b, i, j, k, l)
#elif defined(__ARM_NEON__)
  // TODO Neon shuffle for GCC
#else
# define vShuffle(a, b, i, j, k, l) \
  __builtin_ia32_shufps(a, b, i | (j << 2) | (k << 4) | (l << 6));
#endif

/**
 * Component-wise absolute value of a SIMD vector.
 */
OZ_ALWAYS_INLINE
inline uint4 vAbs(uint4 a)
{
#if defined(__ARM_NEON__)
  return __builtin_neon_vabsv4sf(a, 3);
#else
  return a & vFill(0x7fffffffu);
#endif
}

/**
 * Component-wise minimum of float SIMD vectors.
 */
OZ_ALWAYS_INLINE
inline float4 vMin(float4 a, float4 b)
{
#if defined(__ARM_NEON__)
  return __builtin_neon_vminv4sf(a, b, 3);
#else
  return __builtin_ia32_minps(a, b);
#endif
}

/**
 * Component-wise maximum of float SIMD vectors.
 */
OZ_ALWAYS_INLINE
inline float4 vMax(float4 a, float4 b)
{
#if defined(__ARM_NEON__)
  return __builtin_neon_vmaxv4sf(a, b, 3);
#else
  return __builtin_ia32_maxps(a, b);
#endif
}

/**
 * Component-wise square root of a float SIMD vector.
 */
OZ_ALWAYS_INLINE
inline float4 vSqrt(float4 a)
{
#if defined(__ARM_NEON__)
  // TODO Neon sqrt
#else
  return __builtin_ia32_sqrtps(a);
#endif
}

/**
 * Component-wise fast inverse square root of a float SIMD vector.
 */
OZ_ALWAYS_INLINE
inline float4 vFastInvSqrt(float4 a)
{
  simd4 s = { a };

  s.u4 = vFill(0x5f375a86u) - (s.u4 >> vFill(1u));
  return s.f4 * (vFill(1.5f) - vFill(0.5f) * a * s.f4*s.f4);
}

/**
 * Component-wise fast inverse square root of a float SIMD vector.
 */
OZ_ALWAYS_INLINE
inline float4 vFastSqrt(float4 a)
{
  simd4 s = { a };

  s.u4 = vFill(0x5f375a86u) - (s.u4 >> vFill(1u));
  return a * s.f4 * (vFill(1.5f) - vFill(0.5f) * a * s.f4*s.f4);
}

/**
 * Scalar product for float SIMD vectors (returns float SIMD vector).
 */
OZ_ALWAYS_INLINE
inline float4 vDot(float4 a, float4 b)
{
  float4 p = a * b;
#if defined(__ARM_NEON__)
  // TODO Neon dot product
#else
  float4 s = __builtin_ia32_haddps(p, p);
  return __builtin_ia32_haddps(s, s);
#endif
}

#endif // OZ_SIMD_MATH

/**
 * Base class for 3-component vector and similar algebra structures.
 */
class VectorBase3
{
public:

#ifdef OZ_SIMD_MATH
  __extension__ union OZ_ALIGNED(16)
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
  __extension__ union OZ_ALIGNED(16)
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
