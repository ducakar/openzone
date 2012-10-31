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
 * @file ozdynamics/collision/Object.hh
 *
 * Object class.
 */

#pragma once

#include "Shape.hh"

namespace oz
{

/**
 * Collision object.
 */
class Object
{
  public:

    static Pool<Object> pool; ///< Memory pool.

    Object* prev[3];          ///< Previous objects in linked lists used in spatial structures.
    Object* next[3];          ///< Next objects in linked lists used in spatial structures.

    Bounds  bb;               ///< Cached axis-aligned bounding box in absolute coordinates.

    Point   pos;              ///< Position.
    Quat    rot;              ///< Rotation.
    Mat33   rotMat;           ///< Cached rotation matrix.

    int     flags;            ///< Flags.
    int     mask;             ///< Collision bitmask.
    Shape*  shape;            ///< Collision shape.

  public:

    /**
     * Create uninitialised instance.
     */
    explicit Object() :
      mask( ~0 ), shape( nullptr )
    {}

    OZ_STATIC_POOL_ALLOC( pool )

};

}
