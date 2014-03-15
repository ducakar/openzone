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
 * @file ozCore/Quat.cc
 */

#include "Quat.hh"

namespace oz
{

const Quat Quat::ZERO = Quat( 0.0f, 0.0f, 0.0f, 0.0f );
const Quat Quat::ID   = Quat( 0.0f, 0.0f, 0.0f, 1.0f );

Quat Quat::slerp( const Quat& a, const Quat& b, float t )
{
  hard_assert( 0.0f <= t && t <= 1.0f );

  float cosTheta  = a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
  float sinTheta2 = 1.0f - cosTheta*cosTheta;

  if( sinTheta2 <= 0.0f ) {
    return a;
  }

  float sinThetaInv = 1.0f / Math::sqrt( sinTheta2 );
  float theta       = Math::acos( cosTheta );
  float c1          = Math::sin( ( 1 - t ) * theta ) * sinThetaInv;
  float c2          = Math::sin( t * theta ) * sinThetaInv;
  Quat  q           = cosTheta < 0.0f ? c1*a - c2*b : c1*a + c2*b;

  return ~q;
}

}
