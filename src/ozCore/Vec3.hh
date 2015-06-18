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
 * @file ozCore/Vec3.hh
 *
 * `Vec3` class.
 */

#pragma once

#include "simd.hh"
#include "Math.hh"

namespace oz
{

/**
 * 3D vector.
 *
 * @sa `oz::Vec4`, `oz::Point`
 */
class Vec3 : public VectorBase3
{
public:

  /// Zero vector, [0, 0, 0].
  static const Vec3 ZERO;

  /// Vector of ones, [1, 1, 1].
  static const Vec3 ONE;

public:

  // Import SIMD constructors.
  using VectorBase3::VectorBase3;

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Vec3() = default;

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3(float x, float y, float z) :
    VectorBase3(x, y, z, 0.0f)
  {}

  /**
   * Create from an array of 3 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Vec3(const float* v) :
    VectorBase3(v[0], v[1], v[2], 0.0f)
  {}

  /**
   * Norm.
   */
  float operator ! () const
  {
#ifdef OZ_SIMD
    return Math::sqrt(vDot(f4, f4)[0]);
#else
    return Math::sqrt(x*x + y*y + z*z);
#endif
  }

  /**
   * Approximate norm.
   */
  float fastN() const
  {
#ifdef OZ_SIMD
    return Math::fastSqrt(vDot(f4, f4)[0]);
#else
    return Math::fastSqrt(x*x + y*y + z*z);
#endif
  }

  /**
   * Squared norm.
   */
  OZ_ALWAYS_INLINE
  float sqN() const
  {
#ifdef OZ_SIMD
    return vDot(f4, f4)[0];
#else
    return x*x + y*y + z*z;
#endif
  }

  /**
   * Unit vector.
   */
  Vec3 operator ~ () const
  {
    hard_assert(x*x + y*y + z*z > 0.0f);

#ifdef OZ_SIMD
    float4 k = vInvSqrt(vDot(f4, f4));
    return Vec3(f4 * k);
#else
    float k = 1.0f / Math::sqrt(x*x + y*y + z*z);
    return Vec3(x * k, y * k, z * k);
#endif
  }

  /**
   * Approximate unit vector.
   */
  Vec3 fastUnit() const
  {
    hard_assert(x*x + y*y + z*z > 0.0f);

#ifdef OZ_SIMD
    float4 k = vFastInvSqrt(vDot(f4, f4));
    return Vec3(f4 * k);
#else
    float k = Math::fastInvSqrt(x*x + y*y + z*z);
    return Vec3(x * k, y * k, z * k);
#endif
  }

  /**
   * Original vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator + () const
  {
    return *this;
  }

  /**
   * Opposite vector.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator - () const
  {
#ifdef OZ_SIMD
    return Vec3(-f4);
#else
    return Vec3(-x, -y, -z);
#endif
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator + (const Vec3& v) const
  {
#ifdef OZ_SIMD
    return Vec3(f4 + v.f4);
#else
    return Vec3(x + v.x, y + v.y, z + v.z);
#endif
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator - (const Vec3& v) const
  {
#ifdef OZ_SIMD
    return Vec3(f4 - v.f4);
#else
    return Vec3(x - v.x, y - v.y, z - v.z);
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator * (float s) const
  {
#ifdef OZ_SIMD
    return Vec3(f4 * vFill(s));
#else
    return Vec3(x * s, y * s, z * s);
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  friend Vec3 operator * (float s, const Vec3& v)
  {
#ifdef OZ_SIMD
    return Vec3(vFill(s) * v.f4);
#else
    return Vec3(s * v.x, s * v.y, s * v.z);
#endif
  }

  /**
   * Vector divided by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator / (float s) const
  {
    hard_assert(s != 0.0f);

#ifdef OZ_SIMD
    return Vec3(f4 / vFill(s));
#else
    s = 1.0f / s;
    return Vec3(x * s, y * s, z * s);
#endif
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator += (const Vec3& v)
  {
#ifdef OZ_SIMD
    f4 += v.f4;
#else
    x += v.x;
    y += v.y;
    z += v.z;
#endif
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator -= (const Vec3& v)
  {
#ifdef OZ_SIMD
    f4 -= v.f4;
#else
    x -= v.x;
    y -= v.y;
    z -= v.z;
#endif
    return *this;
  }

  /**
   * Multiplication by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator *= (float s)
  {
#ifdef OZ_SIMD
    f4 *= vFill(s);
#else
    x *= s;
    y *= s;
    z *= s;
#endif
    return *this;
  }

  /**
   * Division by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec3& operator /= (float s)
  {
    hard_assert(s != 0.0f);

#ifdef OZ_SIMD
    f4 /= vFill(s);
#else
    s  = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
#endif
    return *this;
  }

  /**
   * Scalar product.
   */
  OZ_ALWAYS_INLINE
  float operator * (const Vec3& v) const
  {
#ifdef OZ_SIMD
    return vDot(f4, v.f4)[0];
#else
    return x*v.x + y*v.y + z*v.z;
#endif
  }

  /**
   * Vector product.
   */
  Vec3 operator ^ (const Vec3& v) const
  {
#ifdef OZ_SIMD
    float4 a  = vShuffle(f4, 1, 2, 0, 3);
    float4 b  = vShuffle(f4, 2, 0, 1, 3);
    float4 va = vShuffle(v.f4, 2, 0, 1, 3);
    float4 vb = vShuffle(v.f4, 1, 2, 0, 3);

    return Vec3(a*va - b*vb);
#else
    return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
#endif
  }

  /**
   * Spherical linear interpolation.
   */
  static Vec3 slerp(const Vec3& a, const Vec3& b, float t);

};

/**
 * Per-component absolute value of a vector.
 */
OZ_ALWAYS_INLINE
inline Vec3 abs(const Vec3& a)
{
#ifdef OZ_SIMD
  return Vec3(vAbs(a.f4));
#else
  return Vec3(abs<float>(a.x), abs<float>(a.y), abs<float>(a.z));
#endif
}

/**
 * Per-component minimum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 min(const Vec3& a, const Vec3& b)
{
#ifdef OZ_SIMD
  return Vec3(vMin(a.f4, b.f4));
#else
  return Vec3(min<float>(a.x, b.x), min<float>(a.y, b.y), min<float>(a.z, b.z));
#endif
}

/**
 * Per-component maximum of two vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 max(const Vec3& a, const Vec3& b)
{
#ifdef OZ_SIMD
  return Vec3(vMax(a.f4, b.f4));
#else
  return Vec3(max<float>(a.x, b.x), max<float>(a.y, b.y), max<float>(a.z, b.z));
#endif
}

/**
 * Per-component clamped value of vectors.
 */
OZ_ALWAYS_INLINE
inline Vec3 clamp(const Vec3& c, const Vec3& a, const Vec3& b)
{
#ifdef OZ_SIMD
  return Vec3(vMin(b.f4, vMax(a.f4, c.f4)));
#else
  return Vec3(clamp<float>(c.x, a.x, b.x), clamp<float>(c.y, a.y, b.y),
              clamp<float>(c.z, a.z, b.z));
#endif
}

}
