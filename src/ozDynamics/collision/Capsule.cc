/*
 * ozDynamics - OpenZone Dynamics Library.
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
 * @file ozDynamics/collision/Capsule.cc
 */

#include "Capsule.hh"

namespace oz
{

Pool<Capsule> Capsule::pool;

Capsule::~Capsule()
{}

Bounds Capsule::getBounds(const Point& pos, const Mat3& rot) const
{
  // Capsule is a convex hull of two spheres so minimum and maximum coordinates are always reached
  // on (at least) one of the spheres.
  float rm      = radius + MARGIN;
  Vec3  radius3 = Vec3(rm, rm, rm);
  Vec3  dim     = ext*abs(rot.z) + radius3;

  return Bounds(pos - dim, pos + dim);
}

}
