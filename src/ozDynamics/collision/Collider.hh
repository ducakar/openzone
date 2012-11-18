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
 * @file ozDynamics/collision/Collider.hh
 *
 * Collider class.
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

    typedef bool OverlapFunc( const Mat33& rot0, const Shape* shape0,
                              const Mat33& rot1, const Shape* shape1,
                              const Vec3& relPos, Result* result );

    static OverlapFunc* const dispatchMatrix[Shape::COMPOUND + 1][Shape::COMPOUND + 1];

  private:

    static bool boxBox( const Mat33& rot0, const Shape* box0,
                        const Mat33& rot1, const Shape* box1,
                        const Vec3& relPos, Result* result );

    static bool boxCapsule( const Mat33& rot0, const Shape* box,
                            const Mat33& rot1, const Shape* capsule,
                            const Vec3& relPos, Result* result );

    static bool boxMesh( const Mat33& rot0, const Shape* box,
                         const Mat33& rot1, const Shape* mesh,
                         const Vec3& relPos, Result* result );

    static bool boxCompound( const Mat33& rot0, const Shape* box,
                             const Mat33& rot1, const Shape* compound,
                             const Vec3& relPos, Result* result );

    static bool capsuleCapsule( const Mat33& rot0, const Shape* capsule0,
                                const Mat33& rot1, const Shape* capsule1,
                                const Vec3& relPos, Result* result );

    static bool capsuleMesh( const Mat33& rot0, const Shape* capsule,
                             const Mat33& rot1, const Shape* mesh,
                             const Vec3& relPos, Result* result );

    static bool capsuleCompound( const Mat33& rot0, const Shape* capsule,
                                 const Mat33& rot1, const Shape* compound,
                                 const Vec3& relPos, Result* result );

    static bool meshMesh( const Mat33& rot0, const Shape* mesh0,
                          const Mat33& rot1, const Shape* mesh1,
                          const Vec3& relPos, Result* result );

    static bool meshCompound( const Mat33& rot0, const Shape* mesh,
                              const Mat33& rot1, const Shape* compound,
                              const Vec3& relPos, Result* result );

    static bool compoundCompound( const Mat33& rot0, const Shape* compound0,
                                  const Mat33& rot1, const Shape* compound1,
                                  const Vec3& relPos, Result* result );

  public:

    static bool overlaps( const Body* body0, const Body* body1, Result* result = nullptr );

};

}
