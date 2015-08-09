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
 * @file ozCore/Vec3.cc
 */

#include "Vec3.hh"

namespace oz
{

const Vec3 Vec3::ZERO = Vec3(0.0f, 0.0f, 0.0f);
const Vec3 Vec3::ONE  = Vec3(1.0f, 1.0f, 1.0f);

Vec3 Vec3::slerp(const Vec3& a, const Vec3& b, float t)
{
#ifdef OZ_SIMD
  float angle = Math::acos(vDot(a.f4, b.f4)[0]);
#else
  float angle = Math::acos(a.x*b.x + a.y*b.y + a.z*b.z);
#endif
  float sine  = Math::sin(angle);

  if (sine == 0.0f) {
    OZ_ASSERT(a * b > 0.0f);

    return a;
  }
  else {
    float alpha = Math::sin((1.0f - t) * angle) / sine;
    float beta  = Math::sin(t * angle) / sine;

    return alpha * a + beta * b;
  }
}

}
