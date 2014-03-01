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
 * @file ozDynamics/collision/Shape.hh
 *
 * `Shape` class.
 */

#pragma once

#include "Bounds.hh"

namespace oz
{

class Shape
{
public:

  /// Margin used for calculation of axis-aligned bounding boxes.
  static constexpr float MARGIN = 0.01f;

  /**
   * Shape types.
   */
  enum Type
  {
    BOX,
    CAPSULE,
    POLYTOPE,
    COMPOUND
  };

public:

  Type type;   ///< Shape type.
  int  nUsers; ///< Reference counter.

public:

  OZ_ALWAYS_INLINE
  explicit Shape( Type type_ ) :
    type( type_ ), nUsers( 0 )
  {}

  virtual ~Shape();

  /**
   * Calculate axis-aligned bounding box for the shape in absolute coordinates.
   */
  virtual Bounds getBounds( const Point& pos, const Mat3& rot ) const = 0;

};

}
