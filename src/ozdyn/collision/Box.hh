/*
 * libozdyn - OpenZone Dynamics Library.
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
 * @file ozdyn/collision/Box.hh
 */

#pragma once

#include "Shape.hh"

namespace oz
{

class Box : public Shape
{
  public:

    Vec3  ext; ///< Extents.
    Vec3  off; ///< Offset relative to body.
    Quat  rot; ///< Rotation relative to body.

  public:

    OZ_ALWAYS_INLINE
    explicit Box() :
      Shape( Shape::BOX )
    {}

    OZ_ALWAYS_INLINE
    explicit Box( const Vec3& ext_, const Vec3& off_ = Vec3::ZERO, const Quat& rot_ = Quat::ID ) :
      Shape( Shape::BOX ), ext( ext_ ), off( off_ ), rot( rot_ )
    {}

    bool overlaps( const Box& b ) const;

};

}
