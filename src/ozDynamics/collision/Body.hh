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
 * @file ozDynamics/collision/Body.hh
 *
 * `Body` class.
 */

#pragma once

#include "Shape.hh"

// Forward declaration for ODE.
struct dxBody;

namespace oz
{

/**
 * Static body.
 */
class Body
{
public:

  /// Body is of `DBody` class.
  static const int DYNAMIC_BIT = 0x01;

public:

  static Pool<Body> pool;  ///< Memory pool.

  Body*   prev[3];         ///< Previous bodies in linked lists used in spatial structures.
  Body*   next[3];         ///< Next bodies in linked lists used in spatial structures.

  Bounds  bb;              ///< Cached axis-aligned bounding box in absolute coordinates.

  Point   p;               ///< Position.
  Quat    o;               ///< Orientation.
  Mat33   oMat;            ///< Cached orientation matrix.

  int     flags;           ///< Flags.
  int     mask;            ///< Collision bitmask.

  dxBody* odeId;           ///< ODE dBodyID.

private:

  Shape*  bodyShape;       ///< Collision shape.

public:

  /**
   * Create uninitialised instance.
   */
  explicit Body() :
    flags( 0 ), mask( ~0 ), odeId( nullptr ), bodyShape( nullptr )
  {}

  virtual ~Body();

  OZ_ALWAYS_INLINE
  Shape* shape() const
  {
    return bodyShape;
  }

  void setShape( Shape* shape )
  {
    if( bodyShape != nullptr ) {
      --bodyShape->nUsers;
    }

    bodyShape = shape;

    if( bodyShape != nullptr ) {
      ++bodyShape->nUsers;
    }
  }

  /**
   * Normalise rotation quaternion and update cached rotation matrix and bounding box.
   */
  void update()
  {
    o    = ~o;
    oMat = Mat33::rotation( o );
    bb   = bodyShape == nullptr ? Bounds( p, p ) : bodyShape->getBounds( p, oMat );
  }

  OZ_STATIC_POOL_ALLOC( pool )
};

}
