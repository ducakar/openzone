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
 * @file ozdyn/collision/Collider.hh
 */

#pragma once

#include "Space.hh"

namespace oz
{

class Collider
{
  public:

    struct Result
    {
      Vec3  axis;
      float depth;
    };

  private:

    typedef bool ( OverlapFunc )( const Mat44& tf0, const Shape* shape0,
                                  const Mat44& tf1, const Shape* shape2 );

  private:

    static OverlapFunc* const dispatcher[Shape::MAX][Shape::MAX];

    static bool overlapsBoxBox( const Vec3& ext0, const Mat33& rot0,
                                const Vec3& ext1, const Mat33& rot1,
                                const Vec3& relPos, Result* result );

};

}
