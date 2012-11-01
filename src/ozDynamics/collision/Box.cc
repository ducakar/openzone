/*
 * ozDynamics - OpenZone Dynamics Library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file ozDynamics/collision/Box.cc
 */

#include "Box.hh"

namespace oz
{

Pool<Box> Box::pool;

Box::~Box()
{}

Bounds Box::getBounds( const Point& pos, const Mat33& rot ) const
{
  // Vertices of a box are {pos ± ext.x·rot.x ± ext.y·rot.y ± ext.z·rot.z}, so minimum and maximum
  // coordinates are reached in {pos[i] + ext.x·|rot.x[i]| + ext.y·|rot.y[i]| + ext.z·|rot.z[i]|}
  // for the i-th component.
  Vec3 margin3 = Vec3( MARGIN, MARGIN, MARGIN );
  Vec3 dim     = ext.x*abs( rot.x ) + ext.y*abs( rot.y ) + ext.z*abs( rot.z ) + margin3;

  return Bounds( pos - dim, pos + dim );
}

}
