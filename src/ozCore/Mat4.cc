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

const Mat4 Mat4::ZERO = Mat4( 0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 0.0f );

const Mat4 Mat4::ID   = Mat4( 1.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f );

void Mat4::rotateX( float theta )
{
  Vec4 j = y;
  Vec4 k = z;

  float s, c;
  Math::sincos( theta, &s, &c );

  y = j * c + k * s;
  z = k * c - j * s;
}

void Mat4::rotateY( float theta )
{
  Vec4 i = x;
  Vec4 k = z;

  float s, c;
  Math::sincos( theta, &s, &c );

  x = i * c - k * s;
  z = k * c + i * s;
}

void Mat4::rotateZ( float theta )
{
  Vec4 i = x;
  Vec4 j = y;

  float s, c;
  Math::sincos( theta, &s, &c );

  x = i * c + j * s;
  y = j * c - i * s;
}

Mat4 Mat4::rotationX( float theta )
{
  float s, c;
  Math::sincos( theta, &s, &c );

  return Mat4( 1.0f, 0.0f, 0.0f, 0.0f,
               0.0f,    c,    s, 0.0f,
               0.0f,   -s,    c, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f );
}

Mat4 Mat4::rotationY( float theta )
{
  float s, c;
  Math::sincos( theta, &s, &c );

  return Mat4(    c, 0.0f,   -s, 0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
                  s, 0.0f,    c, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f );
}

Mat4 Mat4::rotationZ( float theta )
{
  float s, c;
  Math::sincos( theta, &s, &c );

  return Mat4(    c,    s, 0.0f, 0.0f,
                 -s,    c, 0.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f );
}

Mat4 Mat4::rotationZXZ( float heading, float pitch, float roll )
{
  float hs, hc, ps, pc, rs, rc;

  Math::sincos( heading, &hs, &hc );
  Math::sincos( pitch, &ps, &pc );
  Math::sincos( roll, &rs, &rc );

  float hspc = hs*pc;
  float hcpc = hc*pc;

  return Mat4(  hc*rc - hspc*rs,  hs*rc + hcpc*rs, ps*rs, 0.0f,
               -hc*rs - hspc*rc, -hs*rs + hcpc*rc, ps*rc, 0.0f,
                          hs*ps,           -hc*ps,    pc, 0.0f,
                           0.0f,             0.0f,  0.0f, 1.0f );
}

}
