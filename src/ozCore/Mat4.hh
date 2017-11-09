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
 * @file ozCore/Mat4.hh
 *
 * `Mat4` class.
 */

#pragma once

#include "Vec4.hh"
#include "Mat3.hh"

namespace oz
{

/**
 * Column-major 4x4 matrix.
 *
 * @sa `oz::Mat3`
 */
class Mat4
{
public:

  /// Zero matrix.
  static const Mat4 ZERO;

  /// Identity.
  static const Mat4 ID;

  Vec4 x; ///< First column (image of i base vector).
  Vec4 y; ///< Second column (image of j base vector).
  Vec4 z; ///< Third column (image of k base vector).
  Vec4 w; ///< Fourth column (translation).

public:

  /**
   * Create an uninitialised instance.
   */
  OZ_ALWAYS_INLINE
  constexpr Mat4() = default;

  /**
   * Create matrix with given columns.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr Mat4(const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d)
    : x(a), y(b), z(c), w(d)
  {}

  /**
   * Create matrix for base vector images `a`, `b`, `c` and translation `d`.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr Mat4(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& d)
    : x(a), y(b), z(c), w(d.x, d.y, d.z, 1.0f)
  {}

  /**
   * Create matrix with given components.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr Mat4(float xx, float xy, float xz, float xw,
                          float yx, float yy, float yz, float yw,
                          float zx, float zy, float zz, float zw,
                          float wx, float wy, float wz, float ww)
    : x(xx, xy, xz, xw),
      y(yx, yy, yz, yw),
      z(zx, zy, zz, zw),
      w(wx, wy, wz, ww)
  {}

  /**
   * Create matrix from an array of 16 floats.
   */
  OZ_ALWAYS_INLINE
  explicit Mat4(const float* v)
    : x(&v[0]), y(&v[4]), z(&v[8]), w(&v[12])
  {}

  /**
   * Create from a 3x3 matrix.
   */
  OZ_ALWAYS_INLINE
  explicit constexpr Mat4(const Mat3& m)
    : x(m.x), y(m.y), z(m.z), w(0.0f, 0.0f, 0.0f, 1.0f)
  {}

  /**
   * Equality.
   */
  bool operator==(const Mat4& m) const
  {
    return x == m.x && y == m.y && z == m.z && w == m.w;
  }

  /**
   * Constant pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator const float*() const
  {
    return &x.x;
  }

  /**
   * Pointer to the members.
   */
  OZ_ALWAYS_INLINE
  operator float*()
  {
    return &x.x;
  }

  /**
   * Constant reference to the `i`-th column.
   */
  OZ_ALWAYS_INLINE
  const Vec4& operator[](int i) const
  {
    return (&x)[i];
  }

  /**
   * Reference to the `i`-th column.
   */
  OZ_ALWAYS_INLINE
  Vec4& operator[](int i)
  {
    return (&x)[i];
  }

  /**
   * `i`-th row.
   */
  OZ_ALWAYS_INLINE
  Vec4 row(int i) const
  {
    return Vec4(x[i], y[i], z[i], w[i]);
  }

  /**
   * Transposed matrix.
   */
  Mat4 operator~() const
  {
    return Mat4(x.x, y.x, z.x, w.x,
                x.y, y.y, z.y, w.y,
                x.z, y.z, z.z, w.z,
                x.w, y.w, z.w, w.w);
  }

  /**
   * Determinant.
   */
  float det() const
  {
    float klop = z.z * w.w - w.z * z.w;
    float jlnp = y.z * w.w - w.z * y.w;
    float jkno = y.z * z.w - z.z * y.w;
    float ilmp = x.z * w.w - w.z * x.w;
    float ikmo = x.z * z.w - z.z * x.w;
    float ijmn = x.z * y.w - y.z * x.w;

    return x.x * y.y * klop -
           x.x * z.y * jlnp +
           x.x * w.y * jkno -
           y.x * x.y * klop +
           y.x * z.y * ilmp -
           y.x * w.y * ikmo +
           z.x * x.y * jlnp -
           z.x * y.y * ilmp +
           z.x * w.y * ijmn -
           w.x * x.y * jkno +
           w.x * y.y * ikmo -
           w.x * z.y * ijmn;
  }

  /**
   * Determinant of the upper-left 3x3 submatrix.
   */
  float det3() const
  {
#ifdef OZ_SIMD
    float4 d = x.f4 * (vShuffle(y.f4, 1, 2, 0, 3) * vShuffle(z.f4, 2, 0, 1, 3) -
                       vShuffle(z.f4, 1, 2, 0, 3) * vShuffle(y.f4, 2, 0, 1, 3));

    d += vShuffle(d, 1, 0, 3, 2);
    d += vShuffle(d, 2, 3, 0, 1);
    return d[0];
#else
    return x.x * (y.y*z.z - z.y*y.z) +
           x.y * (y.z*z.x - z.z*y.x) +
           x.z * (y.x*z.y - z.x*y.y);
#endif
  }

  /**
   * Trace.
   */
  float trace() const
  {
    return x.x + y.y + z.z + w.w;
  }

  /**
   * Inverse.
   */
  Mat4 inverse() const;

  /**
   * Convert 3x3 rotation submatrix to a quaternion.
   */
  Quat toQuat() const;

  /**
   * Original matrix.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator+() const
  {
    return *this;
  }

  /**
   * Matrix with negated elements.
   */
  OZ_ALWAYS_INLINE
  Mat4 operator-() const
  {
    return Mat4(-x, -y, -z, -w);
  }

  /**
   * Sum.
   */
  Mat4 operator+(const Mat4& a) const
  {
    return Mat4(x + a.x, y + a.y, z + a.z, w + a.w);
  }

  /**
   * Difference.
   */
  Mat4 operator-(const Mat4& a) const
  {
    return Mat4(x - a.x, y - a.y, z - a.z, w - a.w);
  }

  /**
   * Product.
   */
  Mat4 operator*(float s) const
  {
    return Mat4(x * s, y * s, z * s, w * s);
  }

  /**
   * Product.
   */
  friend Mat4 operator*(float s, const Mat4& m)
  {
    return Mat4(s * m.x, s * m.y, s * m.z, s * m.w);
  }

  /**
   * Product, composite of linear transformations.
   */
  Mat4 operator*(const Mat4& m) const
  {
    return Mat4(x * m.x.x + y * m.x.y + z * m.x.z + w * m.x.w,
                x * m.y.x + y * m.y.y + z * m.y.z + w * m.y.w,
                x * m.z.x + y * m.z.y + z * m.z.z + w * m.z.w,
                x * m.w.x + y * m.w.y + z * m.w.z + w * m.w.w);
  }

  /**
   * Transformed 3D vector (no translation is applied).
   */
  OZ_ALWAYS_INLINE
  Vec3 operator*(const Vec3& v) const
  {
    return Vec3(x * v.x + y * v.y + z * v.z);
  }

  /**
   * Transformed point (translation is applied).
   */
  OZ_ALWAYS_INLINE
  Point operator*(const Point& p) const
  {
    return Point(x * p.x + y * p.y + z * p.z + w);
  }

  /**
   * Transformed plane.
   */
  OZ_ALWAYS_INLINE
  Plane operator*(const Plane& p) const
  {
    return Plane(*this * p.n, p.d + Vec3(w) * p.n);
  }

  /**
   * Product with a four-component vector.
   */
  OZ_ALWAYS_INLINE
  Vec4 operator*(const Vec4& v) const
  {
    return x * v.x + y * v.y + z * v.z + w * v.w;
  }

  /**
   * Quotient.
   */
  Mat4 operator/(float s) const
  {
    OZ_ASSERT(s != 0.0f);

    s = 1.0f / s;
    return Mat4(x * s, y * s, z * s, w * s);
  }

  /**
   * Composite of affine transformations in 3D.
   *
   * This is a faster version of multiplication where the last row of both matrices is assumed to
   * be [0, 0, 0, 1].
   */
  OZ_ALWAYS_INLINE
  Mat4 operator^(const Mat4& m) const
  {
    return Mat4(x * m.x.x + y * m.x.y + z * m.x.z,
                x * m.y.x + y * m.y.y + z * m.y.z,
                x * m.z.x + y * m.z.y + z * m.z.z,
                x * m.w.x + y * m.w.y + z * m.w.z + w);
  }

  /**
   * Addition.
   */
  Mat4& operator+=(const Mat4& a)
  {
    x += a.x;
    y += a.y;
    z += a.z;
    w += a.w;
    return *this;
  }

  /**
   * Subtraction.
   */
  Mat4& operator-=(const Mat4& a)
  {
    x -= a.x;
    y -= a.y;
    z -= a.z;
    w -= a.w;
    return *this;
  }

  /**
   * Multiplication.
   */
  Mat4& operator*=(float s)
  {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /**
   * Division.
   */
  Mat4& operator/=(float s)
  {
    OZ_ASSERT(s != 0.0f);

    s = 1.0f / s;
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
  }

  /**
   * Compose with a translation from the right.
   */
  void translate(const Vec3& v)
  {
    w += x * v.x + y * v.y + z * v.z;
  }

  /**
   * Compose with a rotation from the right.
   */
  void rotate(const Quat& q);

  /**
   * Compose with a rotation from the right.
   */
  void rotateX(float theta);

  /**
   * Compose with a rotation from the right.
   */
  void rotateY(float theta);

  /**
   * Compose with a rotation from the right.
   */
  void rotateZ(float theta);

  /**
   * Compose with a scale from the right (fourth vector component is assumed 1.0).
   */
  void scale(const Vec3& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }

  /**
   * Compose with a scale from the right.
   */
  void scale(const Vec4& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
  }

  /**
   * Create matrix for translation.
   */
  static Mat4 translation(const Vec3& v)
  {
    return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                 v.x,  v.y,  v.z, 1.0f);
  }

  /**
   * Create matrix for rotation from a quaternion.
   */
  static Mat4 rotation(const Quat& q);

  /**
   * Create matrix for rotation around x axis.
   */
  static Mat4 rotationX(float theta);

  /**
   * Create matrix for rotation around y axis.
   */
  static Mat4 rotationY(float theta);

  /**
   * Create matrix for rotation around z axis.
   */
  static Mat4 rotationZ(float theta);

  /**
   * `rotationZ(heading) * rotationX(pitch) * rotationZ(roll)`.
   */
  static Mat4 rotationZXZ(float heading, float pitch, float roll);

  /**
   * Create matrix for scaling (fourth vector component is assumed 1.0).
   */
  static Mat4 scaling(const Vec3& v)
  {
    return Mat4( v.x, 0.0f, 0.0f, 0.0f,
                0.0f,  v.y, 0.0f, 0.0f,
                0.0f, 0.0f,  v.z, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
  }

  /**
   * Create matrix for scaling.
   */
  static Mat4 scaling(const Vec4& v)
  {
    return Mat4( v.x, 0.0f, 0.0f, 0.0f,
                0.0f,  v.y, 0.0f, 0.0f,
                0.0f, 0.0f,  v.z, 0.0f,
                0.0f, 0.0f, 0.0f,  v.w);
  }

};

/**
 * Per-component absolute value of a matrix.
 */
inline Mat4 abs(const Mat4& a)
{
  return Mat4(abs(a.x), abs(a.y), abs(a.z), abs(a.w));
}

/**
 * Per-component minimum of two matrices.
 */
inline Mat4 min(const Mat4& a, const Mat4& b)
{
  return Mat4(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z), min(a.w, b.w));
}

/**
 * Per-component maximum of two matrices.
 */
inline Mat4 max(const Mat4& a, const Mat4& b)
{
  return Mat4(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z), max(a.w, b.w));
}

/**
 * Per-component clamped value of matrices.
 */
inline Mat4 clamp(const Mat4& c, const Mat4& a, const Mat4& b)
{
  return Mat4(clamp(c.x, a.x, b.x), clamp(c.y, a.y, b.y), clamp(c.z, a.z, b.z),
              clamp(c.w, a.w, b.w));
}

inline constexpr Mat3::Mat3(const Mat4& m)
  : Mat3(Vec3(m.x), Vec3(m.y), Vec3(m.z))
{}

}
