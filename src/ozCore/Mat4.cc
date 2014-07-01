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
 * @file ozCore/Mat4.cc
 */

#include "Mat4.hh"

namespace oz
{

const Mat4 Mat4::ZERO = Mat4(0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 0.0f);

const Mat4 Mat4::ID   = Mat4(1.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, 1.0f);

Mat4 Mat4::inverse() const
{
  // LLVM/Clang is faster with Cayley-Hamilton while GCC is faster with Cramer.
#ifdef OZ_CLANG

  const Mat4& m = *this;

  // Cayley–Hamilton's decomposition.
  Mat4  m2   = m * m;
  Mat4  m3   = m * m2;
  float mtr  = m.tr();
  float mtr2 = mtr*mtr;
  float m2tr = m2.tr();
  float k    = (mtr*mtr2 - 3.0f*mtr*m2tr + 2.0f*m3.tr()) / 6.0f;
  Mat4  kI   = Mat4(k, 0.0f, 0.0f, 0.0f,
                    0.0f, k, 0.0f, 0.0f,
                    0.0f, 0.0f, k, 0.0f,
                    0.0f, 0.0f, 0.0f, k);

  return (kI - (mtr2 - m2tr) / 2.0f * m + mtr * m2 - m3) / m.det();

#else

  Mat4 inv;

  // Cramer's rule.
  for (int j = 0; j < 4; ++j) {
    inv[j][0] = Mat4(Mat4::ID[j], y, z, w).det();
    inv[j][1] = Mat4(x, Mat4::ID[j], z, w).det();
    inv[j][2] = Mat4(x, y, Mat4::ID[j], w).det();
    inv[j][3] = Mat4(x, y, z, Mat4::ID[j]).det();
  }

  inv /= det();
  return inv;

#endif
}

Quat Mat4::toQuat() const
{
  float w2 = Math::sqrt(1.0f + x.x + y.y + z.z);
  float w4 = 2.0f * w2;

  return ~Quat((y.z - z.y) / w4, (z.x - x.z) / w4, (x.y - y.x) / w4, w2 / 2.0f);
}

void Mat4::rotate(const Quat& q)
{
  *this = *this ^ rotation(q);
}

void Mat4::rotateX(float theta)
{
  Vec4 j = y;
  Vec4 k = z;

  float s, c;
  Math::sincos(theta, &s, &c);

  y = j * c + k * s;
  z = k * c - j * s;
}

void Mat4::rotateY(float theta)
{
  Vec4 i = x;
  Vec4 k = z;

  float s, c;
  Math::sincos(theta, &s, &c);

  x = i * c - k * s;
  z = k * c + i * s;
}

void Mat4::rotateZ(float theta)
{
  Vec4 i = x;
  Vec4 j = y;

  float s, c;
  Math::sincos(theta, &s, &c);

  x = i * c + j * s;
  y = j * c - i * s;
}

Mat4 Mat4::rotation(const Quat& q)
{
  //
  // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy    0 ]
  // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx    0 ]
  // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy  0 ]
  // [       0              0              0        1 ]
  //

  float x2 = q.x + q.x;
  float y2 = q.y + q.y;
  float z2 = q.z + q.z;
  float xx2 = x2 * q.x;
  float yy2 = y2 * q.y;
  float zz2 = z2 * q.z;
  float xy2 = x2 * q.y;
  float xz2 = x2 * q.z;
  float xw2 = x2 * q.w;
  float yz2 = y2 * q.z;
  float yw2 = y2 * q.w;
  float zw2 = z2 * q.w;
  float xx1 = 1.0f - xx2;
  float yy1 = 1.0f - yy2;

  return Mat4(yy1 - zz2, xy2 + zw2, xz2 - yw2, 0.0f,
              xy2 - zw2, xx1 - zz2, yz2 + xw2, 0.0f,
              xz2 + yw2, yz2 - xw2, xx1 - yy2, 0.0f,
                   0.0f,      0.0f,      0.0f, 1.0f);
}

Mat4 Mat4::rotationX(float theta)
{
  float s, c;
  Math::sincos(theta, &s, &c);

  return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
              0.0f,    c,    s, 0.0f,
              0.0f,   -s,    c, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::rotationY(float theta)
{
  float s, c;
  Math::sincos(theta, &s, &c);

  return Mat4(   c, 0.0f,   -s, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
                 s, 0.0f,    c, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::rotationZ(float theta)
{
  float s, c;
  Math::sincos(theta, &s, &c);

  return Mat4(   c,    s, 0.0f, 0.0f,
                -s,    c, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::rotationZXZ(float heading, float pitch, float roll)
{
  float hs, hc, ps, pc, rs, rc;

  Math::sincos(heading, &hs, &hc);
  Math::sincos(pitch, &ps, &pc);
  Math::sincos(roll, &rs, &rc);

  float hspc = hs*pc;
  float hcpc = hc*pc;

  return Mat4( hc*rc - hspc*rs,  hs*rc + hcpc*rs, ps*rs, 0.0f,
              -hc*rs - hspc*rc, -hs*rs + hcpc*rc, ps*rc, 0.0f,
                         hs*ps,           -hc*ps,    pc, 0.0f,
                          0.0f,             0.0f,  0.0f, 1.0f);
}

}
