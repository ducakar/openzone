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
      Plane left, right, down, up, front;

    public:

      float radius;

      void init();
      void update();

      OZ_ALWAYS_INLINE
      bool isVisible( const Point3& p, float radius = 0.0f )
      {
        return
            left  * p > -radius &&
            right * p > -radius &&
            up    * p > -radius &&
            down  * p > -radius &&
            front * p < +radius;
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const Sphere& s, float factor = 1.0f )
      {
        return isVisible( s.p, s.r * factor );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const Bounds& b, float factor = 1.0f )
      {
        Vec3 dim = b.maxs - b.mins;
        return isVisible( b.mins + 0.5f * dim, dim.fastL() * factor );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( const AABB& bb, float factor = 1.0f )
      {
        return isVisible( bb.p, bb.dim.fastL() * factor );
      }

      OZ_ALWAYS_INLINE
      bool isVisible( float x, float y, float radius )
      {
        Point3 mins = Point3( x, y, -Orbis::DIM );
        Point3 maxs = Point3( x, y,  Orbis::DIM );

        return
            ( left  * mins > -radius || left  * maxs > -radius ) &&
            ( right * mins > -radius || right * maxs > -radius ) &&
            ( up    * mins > -radius || up    * maxs > -radius ) &&
            ( down  * mins > -radius || down  * maxs > -radius ) &&
            ( front * mins < +radius || front * maxs < +radius );
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
