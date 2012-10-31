/*
 * libozdynamics - OpenZone Dynamics Library.
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
 * @file ozdynamics/collision/Compound.cc
 */

#include "Compound.hh"

namespace oz
{

Pool<Compound> Compound::pool;

Compound::~Compound()
{}

Bounds Compound::getBounds( const Point& pos, const Mat33& rot ) const
{
  hard_assert( c[0].shape != nullptr );

  Bounds b = c[0].shape->getBounds( pos + c[0].off, rot * c[0].rot );

  for( int i = 1; c[i].shape != nullptr; ++i ) {
    b |= c[i].shape->getBounds( pos + c[i].off, rot * c[i].rot );
  }
  return b;
}

}
