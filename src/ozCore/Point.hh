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
 * @file ozCore/Point.hh
 *
 * `Point` class.
 */

#pragma once

#include "Vec3.hh"

namespace oz
{

/**
 * 3D point.
 *
 * @sa `oz::Vec3`
 */
class Point : public VectorBase3
{
public:

  /// Origin, [0, 0, 0].
  static const Point ORIGIN;

public:

  // Import (protected) SIMD constructors.
  using VectorBase3::VectorBase3;

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Point() = default;

  /**
   * Create a point with given coordinates.
   */
  OZ_ALWAYS_INLINE
  explicit Point(float x, float y, float z) :
    VectorBase3(x, y, z, 1.0f)
  {}

  /**
   * Create from an array of 3 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Point(const float* v) :
    VectorBase3(v[0], v[1], v[2], 1.0f)
  {}

  /**
   * Point translated for `v`.
   */
  OZ_ALWAYS_INLINE
  Point operator + (const Vec3& v) const
  {
#ifdef OZ_SIMD
    return Point(f4 + v.f4);
#else
    return Point(x + v.x, y + v.y, z + v.z);
#endif
  }

  /**
   * Point translated for `-v`.
   */
  OZ_ALWAYS_INLINE
  Point operator - (const Vec3& v) const
  {
#ifdef OZ_SIMD
    return Point(f4 - v.f4);
#else
    return Point(x - v.x, y - v.y, z - v.z);
#endif
  }

  /**
   * Difference of two points.
   */
  OZ_ALWAYS_INLINE
  Vec3 operator - (const Point& p) const
  {
#ifdef OZ_SIMD
    return Vec3(f4 - p.f4);
#else
    return Vec3(x - p.x, y - p.y, z - p.z);
#endif
  }

  /**
   * Translate for `v`.
   */
  OZ_ALWAYS_INLINE
  Point& operator += (const Vec3& v)
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
   * Translate for `-v`.
   */
  OZ_ALWAYS_INLINE
  Point& operator -= (const Vec3& v)
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
   * Projection of the point to a given vector.
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
   * Per-component absolute value of a point.
   */
  OZ_ALWAYS_INLINE
  friend Point abs(const Point& a)
  {
#ifdef OZ_SIMD
    return Point(vAbs(a.f4));
#else
    return Point(abs<float>(a.x), abs<float>(a.y), abs<float>(a.z));
#endif
  }

  /**
   * Per-component minimum of two points.
   */
  OZ_ALWAYS_INLINE
  friend Point min(const Point& a, const Point& b)
  {
#ifdef OZ_SIMD
    return Point(vMin(a.f4, b.f4));
#else
    return Point(min<float>(a.x, b.x), min<float>(a.y, b.y), min<float>(a.z, b.z));
#endif
  }

  /**
   * Per-component maximum of two points.
   */
  OZ_ALWAYS_INLINE
  friend Point max(const Point& a, const Point& b)
  {
#ifdef OZ_SIMD
    return Point(vMax(a.f4, b.f4));
#else
    return Point(max<float>(a.x, b.x), max<float>(a.y, b.y), max<float>(a.z, b.z));
#endif
  }

  /**
   * Per-component clamped value of points.
   */
  OZ_ALWAYS_INLINE
  friend Point clamp(const Point& c, const Point& a, const Point& b)
  {
#ifdef OZ_SIMD
    return Point(vMin(b.f4, vMax(a.f4, c.f4)));
#else
    return Point(clamp<float>(c.x, a.x, b.x), clamp<float>(c.y, a.y, b.y),
                 clamp<float>(c.z, a.z, b.z));
#endif
  }

};

}
