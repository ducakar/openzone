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

#include "Mat3.hh"

namespace oz
{

const Mat3 Mat3::ZERO = Mat3(0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f);

const Mat3 Mat3::ID   = Mat3(1.0f, 0.0f, 0.0f,
                             0.0f, 1.0f, 0.0f,
                             0.0f, 0.0f, 1.0f);

Mat3 Mat3::inverse() const
{
  const Mat3& m = *this;

  // Cayley–Hamilton's decomposition.
  Mat3  m2     = m * m;
  float mTrace = m.trace();
  float k      = (mTrace*mTrace - m2.trace()) / 2.0f;
  Mat3  kI     = Mat3(   k, 0.0f, 0.0f,
                      0.0f,    k, 0.0f,
                      0.0f, 0.0f,    k);

  return (kI - m * mTrace + m2) / m.det();
}

Quat Mat3::toQuat() const
{
  float w2 = Math::sqrt(1.0f + x.x + y.y + z.z);
  float w4 = 2.0f * w2;

  return ~Quat((y.z - z.y) / w4, (z.x - x.z) / w4, (x.y - y.x) / w4, w2 / 2.0f);
}

void Mat3::rotate(const Quat& q)
{
  *this = *this * rotation(q);
}

void Mat3::rotateX(float theta)
{
  Vec3 j = y;
  Vec3 k = z;

  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  y = j * c + k * s;
  z = k * c - j * s;
}

void Mat3::rotateY(float theta)
{
  Vec3 i = x;
  Vec3 k = z;

  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  x = i * c - k * s;
  z = k * c + i * s;
}

void Mat3::rotateZ(float theta)
{
  Vec3 i = x;
  Vec3 j = y;

  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  x = i * c + j * s;
  y = j * c - i * s;
}

Mat3 Mat3::rotation(const Quat& q)
{
  //
  // [ 1 - 2yy - 2zz    2xy - 2wz      2xz + 2wy   ]
  // [   2xy + 2wz    1 - 2xx - 2zz    2yz - 2wx   ]
  // [   2xz - 2wy      2yz + 2wx    1 - 2xx - 2yy ]
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

  return Mat3(yy1 - zz2, xy2 + zw2, xz2 - yw2,
              xy2 - zw2, xx1 - zz2, yz2 + xw2,
              xz2 + yw2, yz2 - xw2, xx1 - yy2);
}

Mat3 Mat3::rotationX(float theta)
{
  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  return Mat3(1.0f, 0.0f, 0.0f,
              0.0f,    c,    s,
              0.0f,   -s,    c);
}

Mat3 Mat3::rotationY(float theta)
{
  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  return Mat3(   c, 0.0f,   -s,
              0.0f, 1.0f, 0.0f,
                 s, 0.0f,    c);
}

Mat3 Mat3::rotationZ(float theta)
{
  float s = 0.0f, c = 0.0f;
  Math::sincos(theta, &s, &c);

  return Mat3(   c,    s, 0.0f,
                -s,    c, 0.0f,
              0.0f, 0.0f, 1.0f);
}

Mat3 Mat3::rotationZXZ(float heading, float pitch, float roll)
{
  float hs = 0.0f, hc = 0.0f, ps = 0.0f, pc = 0.0f, rs = 0.0f, rc = 0.0f;

  Math::sincos(heading, &hs, &hc);
  Math::sincos(pitch, &ps, &pc);
  Math::sincos(roll, &rs, &rc);

  float hspc = hs*pc;
  float hcpc = hc*pc;

  return Mat3( hc*rc - hspc*rs,  hs*rc + hcpc*rs, ps*rs,
              -hc*rs - hspc*rc, -hs*rs + hcpc*rc, ps*rc,
                         hs*ps,           -hc*ps,    pc);
}

}
