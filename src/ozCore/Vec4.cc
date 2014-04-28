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
 * @file ozCore/Vec4.cc
 */

#include "Vec4.hh"

namespace oz
{

const Vec4 Vec4::ZERO = Vec4( 0.0f, 0.0f, 0.0f, 0.0f );
const Vec4 Vec4::ONE  = Vec4( 1.0f, 1.0f, 1.0f, 1.0f );
const Vec4 Vec4::ID   = Vec4( 0.0f, 0.0f, 0.0f, 1.0f );

float Vec4::operator ! () const
{
#ifdef OZ_SIMD_MATH
  return Math::sqrt( vFirst( vDot( f4, f4 ) ) );
#else
  return Math::sqrt( x*x + y*y + z*z + w*w );
#endif
}

Vec4 Vec4::operator ~ () const
{
#ifdef OZ_SIMD_MATH
  scalar s = 1.0f / Math::sqrt( vFirst( vDot( f4, f4 ) ) );
  return Vec4( f4 * s.f4 );
#else
  hard_assert( x*x + y*y + z*z + w*w > 0.0f );

  float k = 1.0f / Math::sqrt( x*x + y*y + z*z + w*w );
  return Vec4( x * k, y * k, z * k, w * k );
#endif
}

}
