/*
 * ozDynamics - OpenZone Dynamics Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file ozDynamics/physics/DBody.hh
 *
 * Body class.
 */

#pragma once

#include "../collision/Body.hh"

namespace oz
{

/**
 * Dynamic body.
 */
class DBody : public Body
{
  public:

    static Pool<DBody> pool; ///< Memory pool.

    Vec3    mass;            ///< Mass.
    Vec3    lift;            ///< Lift.
    Mat33   inertia;         ///< Inertia.

    Vec3    velocity;        ///< Velocity.
    Vec3    rotVelocity;     ///< Rotational velocity.

  public:

    explicit DBody() : Body()
    {
      flags = DYNAMIC_BIT;
    }

    ~DBody() override;

    OZ_STATIC_POOL_ALLOC( pool )

};

}
