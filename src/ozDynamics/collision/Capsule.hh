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
 * @file ozDynamics/collision/Capsule.hh
 *
 * Capsule class.
 */

#pragma once

#include "Shape.hh"

namespace oz
{

/**
 * Capsule, represents a radius-region around a line segment.
 */
class Capsule : public Shape
{
  public:

    static Pool<Capsule> pool; ///< Memory pool.

    float ext;                 ///< Line segment length.
    float radius;              ///< Distance from line segment representing capsule surface.

  public:

    OZ_ALWAYS_INLINE
    explicit Capsule() :
      Shape( CAPSULE )
    {}

    OZ_ALWAYS_INLINE
    explicit Capsule( float ext_, float radius_ ) :
      Shape( CAPSULE ), ext( ext_ ), radius( radius_ )
    {}

    ~Capsule() override;

    Bounds getBounds( const Point& pos, const Mat33& rot ) const override;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
