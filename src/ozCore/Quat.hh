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
 * @file ozCore/Quat.hh
 *
 * `Quat` class.
 */

#pragma once

#include "Vec4.hh"

namespace oz
{

/**
 * Quaternion.
 */
class Quat : public VectorBase4
{
public:

  /// Zero quaternion.
  static const Quat ZERO;

  /// Quaternion representing rotation identity.
  static const Quat ID;

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  Quat() :
    VectorBase4(0.0f, 0.0f, 0.0f, 1.0f)
  {}

#ifdef OZ_SIMD_MATH

  /**
   * Create from a float SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Quat(float4 f4) :
    VectorBase4(f4)
  {}

  /**
   * Create from an uint SIMD vector.
   */
  OZ_ALWAYS_INLINE
  explicit Quat(uint4 u4) :
    VectorBase4(u4)
  {}

#endif

  /**
   * Create a quaternion with given components.
   */
  OZ_ALWAYS_INLINE
  explicit Quat(float x, float y, float z, float w) :
    VectorBase4(x, y, z, w)
  {}

  /**
   * Create from an array of 4 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Quat(const float* q) :
    VectorBase4(q[0], q[1], q[2], q[3])
  {}

  /**
   * Equality.
   */
  OZ_ALWAYS_INLINE
  bool operator == (const Quat& q) const
  {
    return x == q.x && y == q.y && z == q.z && w == q.w;
  }

  /**
   * Inequality.
   */
  OZ_ALWAYS_INLINE
  bool operator != (const Quat& q) const
  {
    return !operator == (q);
  }

  /**
   * Cast to four-component vector.
   */
  operator Vec4() const
  {
#ifdef OZ_SIMD_MATH
    return Vec4(f4);
#else
    return Vec4(x, y, z, w);
#endif
  }

  /**
   * Conjugated quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator * () const
  {
#ifdef OZ_SIMD_MATH
    return Quat(u4 ^ vFill(0x80000000u, 0x80000000u, 0x80000000u, 0u));
#else
    return Quat(-x, -y, -z, w);
#endif
  }

  /**
   * Norm.
   */
  OZ_ALWAYS_INLINE
  float operator ! () const
  {
#ifdef OZ_SIMD_MATH
    return vFirst(vDot(f4, f4));
#else
    return x*x + y*y + z*z + w*w;
#endif
  }

  /**
   * Unit quaternion.
   */
  Quat operator ~ () const
  {
    hard_assert(x*x + y*y + z*z + w*w > 0.0f);

#ifdef OZ_SIMD_MATH
    float4 k = vInvSqrt(vDot(f4, f4));
    return Quat(f4 * k);
#else
    float k = 1.0f / Math::sqrt(x*x + y*y + z*z + w*w);
    return Quat(x * k, y * k, z * k, w * k);
#endif
  }

  /**
   * Approximate unit quaternion.
   */
  Quat fastUnit() const
  {
    hard_assert(x*x + y*y + z*z + w*w > 0.0f);

#ifdef OZ_SIMD_MATH
    float4 k = vFastInvSqrt(vDot(f4, f4));
    return Quat(f4 * k);
#else
    float k = Math::fastInvSqrt(x*x + y*y + z*z + w*w);
    return Quat(x * k, y * k, z * k, w * k);
#endif
  }

  /**
   * Original quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator + () const
  {
    return *this;
  }

  /**
   * Opposite quaternion.
   */
  OZ_ALWAYS_INLINE
  Quat operator - () const
  {
#ifdef OZ_SIMD_MATH
    return Quat(-f4);
#else
    return Quat(-x, -y, -z, -w);
#endif
  }

  /**
   * Sum.
   */
  OZ_ALWAYS_INLINE
  Quat operator + (const Quat& q) const
  {
#ifdef OZ_SIMD_MATH
    return Quat(f4 + q.f4);
#else
    return Quat(x + q.x, y + q.y, z + q.z, w + q.w);
#endif
  }

  /**
   * Difference.
   */
  OZ_ALWAYS_INLINE
  Quat operator - (const Quat& q) const
  {
#ifdef OZ_SIMD_MATH
    return Quat(f4 - q.f4);
#else
    return Quat(x - q.x, y - q.y, z - q.z, w - q.w);
#endif
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  Quat operator * (float s) const
  {
#ifdef OZ_SIMD_MATH
    return Quat(f4 * vFill(s));
#else
    return Quat(x * s, y * s, z * s, w * s);
#endif
  }

  /**
   * Product.
   */
  OZ_ALWAYS_INLINE
  friend Quat operator * (float s, const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    return Quat(vFill(s) * q.f4);
#else
    return Quat(s * q.x, s * q.y, s * q.z, s * q.w);
#endif
  }

  /**
   * Quaternion product.
   */
  Quat operator * (const Quat& q) const
  {
#ifdef OZ_SIMD_MATH
    float4 k0 = vFill(w);
    float4 k1 = f4;
    float4 k2 = vShuffle(f4, 1, 2, 0, 3);
    float4 k3 = vShuffle(f4, 2, 0, 1, 3);

    float4 q0 = q.f4;
    float4 q1 = vFill(q.w);
    float4 q2 = vShuffle(q.f4, 2, 0, 1, 3);
    float4 q3 = vShuffle(q.f4, 1, 2, 0, 3);

    Quat tq = Quat(k0*q0 + k1*q1 + k2*q2 - k3*q3);
    tq.w   -= vFirst(vDot(k1, q.f4));

    return tq;
#else
    return Quat(w*q.x + x*q.w + y*q.z - z*q.y,
                w*q.y + y*q.w + z*q.x - x*q.z,
                w*q.z + z*q.w + x*q.y - y*q.x,
                w*q.w - x*q.x - y*q.y - z*q.z);
#endif
  }

  /**
   * Quotient.
   */
  OZ_ALWAYS_INLINE
  Quat operator / (float s) const
  {
    hard_assert(s != 0.0f);

#ifdef OZ_SIMD_MATH
    return Quat(f4 / vFill(s));
#else
    s = 1.0f / s;
    return Quat(x * s, y * s, z * s, w * s);
#endif
  }

  /**
   * Quotient.
   */
  friend Quat operator / (float s, const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    float4 k = vFill(s);

    k /= vDot(q.f4, q.f4);
    k *= vFill(-1.0f, -1.0f, -1.0f, +1.0f);

    return Quat(q.f4 * k);
#else
    s /= (q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);

    float ns = -s;
    return Quat(q.x * ns, q.y * ns, q.z * ns, q.w * s);
#endif
  }

  /**
   * Quaternion quotient.
   */
  Quat operator / (const Quat& q) const
  {
#ifdef OZ_SIMD_MATH
    float4 k0 = f4;
    float4 k1 = vFill(w);
    float4 k2 = vShuffle(f4, 2, 0, 1, 3);
    float4 k3 = vShuffle(f4, 1, 2, 0, 3);

    float4 q0 = vFill(q.w);
    float4 q1 = q.f4;
    float4 q2 = vShuffle(q.f4, 1, 2, 0, 3);
    float4 q3 = vShuffle(q.f4, 2, 0, 1, 3);

    Quat tq = Quat(k0*q0 - k1*q1 + k2*q2 - k3*q3);
    tq.w    = vFirst(vDot(k0, q.f4));
    tq.f4  *= vDot(q.f4, q.f4);

    return tq;
#else
    float k = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    return Quat(k * (x*q.w - w*q.x + z*q.y - y*q.z),
                k * (y*q.w - w*q.y + x*q.z - z*q.x),
                k * (z*q.w - w*q.z + y*q.x - x*q.y),
                k * (w*q.w + x*q.x + y*q.y + z*q.z));
#endif
  }

  /**
   * Addition.
   */
  OZ_ALWAYS_INLINE
  Quat& operator += (const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    f4 += q.f4;
#else
    x += q.x;
    y += q.y;
    z += q.z;
    w += q.w;
#endif
    return *this;
  }

  /**
   * Subtraction.
   */
  OZ_ALWAYS_INLINE
  Quat& operator -= (const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    f4 -= q.f4;
#else
    x -= q.x;
    y -= q.y;
    z -= q.z;
    w -= q.w;
#endif
    return *this;
  }

  /**
   * Multiplication.
   */
  OZ_ALWAYS_INLINE
  Quat& operator *= (float s)
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
   * Quaternion multiplication.
   */
  Quat& operator *= (const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    float4 k0 = vFill(w);
    float4 k1 = f4;
    float4 k2 = vShuffle(f4, 1, 2, 0, 3);
    float4 k3 = vShuffle(f4, 2, 0, 1, 3);

    float4 q0 = q.f4;
    float4 q1 = vFill(q.w);
    float4 q2 = vShuffle(q.f4, 2, 0, 1, 3);
    float4 q3 = vShuffle(q.f4, 1, 2, 0, 3);

    f4 = k0*q0 + k1*q1 + k2*q2 - k3*q3;
    w -= vFirst(vDot(k1, q0));
#else
    float tx = x, ty = y, tz = z;

    x = w*q.x + tx*q.w + ty*q.z - tz*q.y;
    y = w*q.y + ty*q.w + tz*q.x - tx*q.z;
    z = w*q.z + tz*q.w + tx*q.y - ty*q.x;
    w = w*q.w - tx*q.x - ty*q.y - tz*q.z;
#endif
    return *this;
  }

  /**
   * Division.
   */
  OZ_ALWAYS_INLINE
  Quat& operator /= (float s)
  {
#ifdef OZ_SIMD_MATH
    f4 /= vFill(s);
#else
    hard_assert(s != 0.0f);

    s  = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
    w *= s;
#endif
    return *this;
  }

  /**
   * Quaternion division.
   */
  Quat& operator /= (const Quat& q)
  {
#ifdef OZ_SIMD_MATH
    float4 k0 = f4;
    float4 k1 = vFill(w);
    float4 k2 = vShuffle(f4, 2, 0, 1, 3);
    float4 k3 = vShuffle(f4, 1, 2, 0, 3);

    float4 q0 = vFill(q.w);
    float4 q1 = q.f4;
    float4 q2 = vShuffle(q.f4, 1, 2, 0, 3);
    float4 q3 = vShuffle(q.f4, 2, 0, 1, 3);

    f4  = k0*q0 - k1*q1 + k2*q2 - k3*q3;
    w   = vFirst(vDot(k0, q.f4));
    f4 *= vDot(q.f4, q.f4);
#else
    float k = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
    Quat  t = Quat(x*q.w - w*q.x + z*q.y - y*q.z,
                   y*q.w - w*q.y + x*q.z - z*q.x,
                   z*q.w - w*q.z + y*q.x - x*q.y,
                   w*q.w + x*q.x + y*q.y + z*q.z);

    x = k * t.x;
    y = k * t.y;
    z = k * t.z;
    w = k * t.w;
#endif
    return *this;
  }

  /**
   * Vector rotation.
   */
  Vec3 operator * (const Vec3& v) const
  {
    float x2  = x + x;
    float y2  = y + y;
    float z2  = z + z;
    float xx2 = x2 * x;
    float yy2 = y2 * y;
    float zz2 = z2 * z;
    float xy2 = x2 * y;
    float xz2 = x2 * z;
    float xw2 = x2 * w;
    float yz2 = y2 * z;
    float yw2 = y2 * w;
    float zw2 = z2 * w;
    float yy1 = 1.0f - yy2;
    float xx1 = 1.0f - xx2;

    return Vec3((yy1 - zz2) * v.x + (xy2 - zw2) * v.y + (xz2 + yw2) * v.z,
                (xy2 + zw2) * v.x + (xx1 - zz2) * v.y + (yz2 - xw2) * v.z,
                (xz2 - yw2) * v.x + (yz2 + xw2) * v.y + (xx1 - yy2) * v.z);
  }

  /**
   * Create quaternion for rotation around a given axis.
   */
  static Quat rotationAxis(const Vec3& axis, float theta);

  /**
   * Create quaternion for rotation around x axis.
   */
  static Quat rotationX(float theta);

  /**
   * Create quaternion for rotation around y axis.
   */
  static Quat rotationY(float theta);

  /**
   * Create quaternion for rotation around z axis.
   */
  static Quat rotationZ(float theta);

  /**
   * `rotationZ(heading) * rotationX(pitch) * rotationZ(roll)`.
   */
  static Quat rotationZXZ(float heading, float pitch, float roll);

  /**
   * Get rotation axis and angle.
   */
  void toAxisAngle(Vec3* axis, float* angle) const;

  /**
   * Spherical linear interpolation between two orientations.
   */
  static Quat slerp(const Quat& a, const Quat& b, float t);

  /**
   * Approximate but much faster spherical linear interpolation between two orientations.
   */
  static Quat fastSlerp(const Quat& a, const Quat& b, float t);

};

/**
 * Per-component absolute value of a quaternion.
 */
OZ_ALWAYS_INLINE
inline Quat abs(const Quat& a)
{
#ifdef OZ_SIMD_MATH
  return Quat(vAbs(a.u4));
#else
  return Quat(abs<float>(a.x), abs<float>(a.y), abs<float>(a.z), abs<float>(a.w));
#endif
}

/**
 * Per-component minimum of two quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat min(const Quat& a, const Quat& b)
{
#ifdef OZ_SIMD_MATH
  return Quat(vMin(a.f4, b.f4));
#else
  return Quat(min<float>(a.x, b.x), min<float>(a.y, b.y), min<float>(a.z, b.z),
              min<float>(a.w, b.w));
#endif
}

/**
 * Per-component maximum of two quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat max(const Quat& a, const Quat& b)
{
#ifdef OZ_SIMD_MATH
  return Quat(vMax(a.f4, b.f4));
#else
  return Quat(max<float>(a.x, b.x), max<float>(a.y, b.y), max<float>(a.z, b.z),
              max<float>(a.w, b.w));
#endif
}

/**
 * Per-component clamped value of quaternions.
 */
OZ_ALWAYS_INLINE
inline Quat clamp(const Quat& c, const Quat& a, const Quat& b)
{
#ifdef OZ_SIMD_MATH
  return Quat(vMin(b.f4, vMax(a.f4, c.f4)));
#else
  return Quat(clamp<float>(c.x, a.x, b.x), clamp<float>(c.y, a.y, b.y),
              clamp<float>(c.z, a.z, b.z), clamp<float>(c.w, a.w, b.w));
#endif
}

}
