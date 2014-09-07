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
 * @file ozCore/Vec4.hh
 *
 * `Vec4` class.
 */

#pragma once

#include "Point.hh"

namespace oz
{

/**
 * Generic four-component vector.
 *
 * @sa `oz::Vec3`
 */
class Vec4 : public VectorBase4
{
public:

  /// Zero vector, [0, 0, 0, 0].
  static const Vec4 ZERO;

  /// Vector of ones, [1, 1, 1, 1].
  static const Vec4 ONE;

  /// Equivalent to origin point or identity quaternion, [0, 0, 0, 1].
  static const Vec4 ID;

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Vec4() :
    VectorBase4(0.0f, 0.0f, 0.0f, 0.0f)
  {}

#ifdef OZ_SIMD_MATH

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(float4 f4) :
    VectorBase4(f4)
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(uint4 u4) :
    VectorBase4(u4)
  {}

#endif

  /**
   * Create a vector with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(float x, float y, float z, float w) :
    VectorBase4(x, y, z, w)
  {}

  /**
   * Create from an array of 4 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(const float* v) :
    VectorBase4(v[0], v[1], v[2], v[3])
  {}

  /**
   * Create vector from a 3D vector (the additional component is zero).
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(const Vec3& v) :
#ifdef OZ_SIMD_MATH
    VectorBase4(v.f4)
#else
    VectorBase4(v.x, v.y, v.z, 0.0f)
#endif
  {}

  /**
   * Create vector from a point (the additional component is one).
   */
  OZ_ALWAYS_INLINE
  explicit Vec4(const Point& p) :
#ifdef OZ_SIMD_MATH
    VectorBase4(p.f4)
#else
    VectorBase4(p.x, p.y, p.z, 1.0f)
#endif
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == (const Vec4& v) const
  {
    return x == v.x && y == v.y && z == v.z && w == v.w;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != (const Vec4& v) const
  {
    return !operator == (v);
  }

  /**
   * Return the 3D vector this vector represents (last component should be 0).
   */
  OZ_ALWAYS_INLINE
  Vec3 vec3() const
  {
    hard_assert(w == 0.0f);

#ifdef OZ_SIMD_MATH
    return Vec3(f4);
#else
    return Vec3(x, y, z);
#endif
  }

  /**
   * Return the point vector represents (last component should be 1).
   */
  OZ_ALWAYS_INLINE
  Point point() const
  {
    hard_assert(w == 1.0f);

#ifdef OZ_SIMD_MATH
    return Point(f4);
#else
    return Point(x, y, z);
#endif
  }

  /**
   * Norm.
   */
  float operator ! () const
  {
#ifdef OZ_SIMD_MATH
    return Math::sqrt(vFirst(vDot(f4, f4)));
#else
    return Math::sqrt(x*x + y*y + z*z + w*w);
#endif
  }

  /**
   * Approximate norm.
   */
  float fastN() const
  {
#ifdef OZ_SIMD_MATH
    return Math::fastSqrt(vFirst(vDot(f4, f4)));
#else
    return Math::fastSqrt(x*x + y*y + z*z + w*w);
#endif
  }

  /**
   * Squared norm.
   */
  OZ_ALWAYS_INLINE
  float sqN() const
  {
#ifdef OZ_SIMD_MATH
    return vFirst(vDot(f4, f4));
#else
    return x*x + y*y + z*z + w*w;
#endif
  }

  /**
   * Unit vector.
   */
  Vec4 operator ~ () const
  {
    hard_assert(x*x + y*y + z*z + w*w > 0.0f);

#ifdef OZ_SIMD_MATH
    float4 k = vFill(1.0f) / vSqrt(vDot(f4, f4));
    return Vec4(f4 * k);
#else
    float k = 1.0f / Math::sqrt(x*x + y*y + z*z + w*w);
    return Vec4(x * k, y * k, z * k, w * k);
#endif
  }

  /**
   * Approximate unit vector.
   */
  Vec4 fastUnit() const
  {
    hard_assert(x*x + y*y + z*z + w*w > 0.0f);

#ifdef OZ_SIMD_MATH
    float4 k = vFastInvSqrt(vDot(f4, f4));
    return Vec4(f4 * k);
#else
    float k = Math::fastInvSqrt(x*x + y*y + z*z + w*w);
    return Vec4(x * k, y * k, z * k, w * k);
#endif
  }

  /**
   * Original vector.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator + () const
  {
    return *this;
  }

  /**
   * Opposite vector.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator - () const
  {
#ifdef OZ_SIMD_MATH
    return Vec4(-f4);
#else
    return Vec4(-x, -y, -z, -w);
#endif
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator + (const Vec4& v) const
  {
#ifdef OZ_SIMD_MATH
    return Vec4(f4 + v.f4);
#else
    return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
#endif
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator - (const Vec4& v) const
  {
#ifdef OZ_SIMD_MATH
    return Vec4(f4 - v.f4);
#else
    return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator * (float s) const
  {
#ifdef OZ_SIMD_MATH
    return Vec4(f4 * vFill(s));
#else
    return Vec4(x * s, y * s, z * s, w * s);
#endif
  }

  /**
   * Vector multiplied by a scalar.
   */
  OZ_ALWAYS_INLINE
  friend Vec4 operator * (float s, const Vec4& v)
  {
#ifdef OZ_SIMD_MATH
    return Vec4(vFill(s) * v.f4);
#else
    return Vec4(s * v.x, s * v.y, s * v.z, s * v.w);
#endif
  }

  /**
   * Vector divided by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator / (float s) const
  {
    hard_assert(s != 0.0f);

#ifdef OZ_SIMD_MATH
    return Vec4(f4 / vFill(s));
#else
    s = 1.0f / s;
    return Vec4(x * s, y * s, z * s, w * s);
#endif
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator += (const Vec4& v)
  {
#ifdef OZ_SIMD_MATH
    f4 += v.f4;
#else
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
#endif
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator -= (const Vec4& v)
  {
#ifdef OZ_SIMD_MATH
    f4 -= v.f4;
#else
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
#endif
    return *this;
  }

  /**
   * Multiplication by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator *= (float s)
  {
#ifdef OZ_SIMD_MATH
    f4 *= vFill(s);
#else
    x *= s;
    y *= s;
    z *= s;
    w *= s;
#endif
    return *this;
  }

  /**
   * Division by a scalar.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator /= (float s)
  {
    hard_assert(s != 0.0f);

#ifdef OZ_SIMD_MATH
    f4 /= vFill(s);
#else
    s  = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
    w *= s;
#endif
    return *this;
  }

  /**
   * Scalar product.
   */
  OZ_ALWAYS_INLINE
  float operator * (const Vec4& v) const
  {
#ifdef OZ_SIMD_MATH
    return vFirst(vDot(f4, v.f4));
#else
    return x*v.x + y*v.y + z*v.z + w*v.w;
#endif
  }

  /**
   * Per-component absolute value of a vector.
   */
  OZ_ALWAYS_INLINE
  friend Vec4 abs(const Vec4& a)
  {
#ifdef OZ_SIMD_MATH
    return Vec4(vAbs(a.u4));
#else
    return Vec4(abs<float>(a.x), abs<float>(a.y), abs<float>(a.z), abs<float>(a.w));
#endif
  }

  /**
   * Per-component minimum of two vectors.
   */
  OZ_ALWAYS_INLINE
  friend Vec4 min(const Vec4& a, const Vec4& b)
  {
#ifdef OZ_SIMD_MATH
    return Vec4(vMin(a.f4, b.f4));
#else
    return Vec4(min<float>(a.x, b.x), min<float>(a.y, b.y), min<float>(a.z, b.z),
                min<float>(a.w, b.w));
#endif
  }

  /**
   * Per-component maximum of two vectors.
   */
  OZ_ALWAYS_INLINE
  friend Vec4 max(const Vec4& a, const Vec4& b)
  {
#ifdef OZ_SIMD_MATH
    return Vec4(vMax(a.f4, b.f4));
#else
    return Vec4(max<float>(a.x, b.x), max<float>(a.y, b.y), max<float>(a.z, b.z),
                max<float>(a.w, b.w));
#endif
  }

  /**
   * Per-component clamped value of vectors.
   */
  OZ_ALWAYS_INLINE
  friend Vec4 clamp(const Vec4& c, const Vec4& a, const Vec4& b)
  {
#ifdef OZ_SIMD_MATH
    return Vec4(vMin(b.f4, vMax(a.f4, c.f4)));
#else
    return Vec4(clamp<float>(c.x, a.x, b.x), clamp<float>(c.y, a.y, b.y),
                clamp<float>(c.z, a.z, b.z), clamp<float>(c.w, a.w, b.w));
#endif
  }

};

}
