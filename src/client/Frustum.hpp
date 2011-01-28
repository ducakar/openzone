/*
 *  Frustum.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/common.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  class Frustum
  {
    private:

      float fovX, fovY;
      float sx, cx, sy, cy;
      Vec3  nLeft0, nRight0, nDown0, nUp0;
      Vec3  nLeft, nRight, nDown, nUp, nFront;
      float dLeft, dRight, dDown, dUp, dFront;

    public:

      float maxDistance;
      float radius;

      void init( float fovY, float aspect, float maxDistance );
      void update( float maxDistance );

      OZ_ALWAYS_INLINE
      bool isVisible( const Point3& p, float radius = 0.0f )
      {
        return
            p * nLeft  > dLeft  - radius &&
            p * nRight > dRight - radius &&
            p * nUp    > dUp    - radius &&
            p * nDown  > dDown  - radius &&
            p * nFront < dFront + radius;
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const Sphere& s, float factor = 1.0f )
      {
        return isVisible( s.p, s.r * factor );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const AABB& bb, float factor = 1.0f )
      {
        return isVisible( bb.p, bb.r * factor );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const Bounds& b )
      {
        Vec3 dim = b.maxs - b.mins;
        return isVisible( b.mins + 0.5f * ( b.maxs - b.mins ), !dim );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( float x, float y, float radius )
      {
        Vec3 min = Vec3( x, y, -Orbis::DIM );
        Vec3 max = Vec3( x, y,  Orbis::DIM );

        return
            ( min * nLeft  > dLeft  - radius || max * nLeft  > dLeft  - radius ) &&
            ( min * nRight > dRight - radius || max * nRight > dRight - radius ) &&
            ( min * nUp    > dUp    - radius || max * nUp    > dUp    - radius ) &&
            ( min * nDown  > dDown  - radius || max * nDown  > dDown  - radius ) &&
            ( min * nFront < dFront + radius || max * nFront < dFront + radius );
      }

      // get min and max index for cells per each axis, which should be included in pvs
      void getExtrems( Span& span, const Point3& p )
      {
        span.minX = max( int( ( p.x - radius + Orbis::DIM ) * Cell::INV_SIZE ), 0 );
        span.minY = max( int( ( p.y - radius + Orbis::DIM ) * Cell::INV_SIZE ), 0 );
        span.maxX = min( int( ( p.x + radius + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 );
        span.maxY = min( int( ( p.y + radius + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 );
      }

  };

  extern Frustum frustum;

}
}
