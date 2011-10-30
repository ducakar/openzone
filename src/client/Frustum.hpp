/*
 *  Frustum.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

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

    OZ_ALWAYS_INLINE
    bool isVisible( const Point3& p, float radius = 0.0f ) const
    {
      return
          p * left  > -radius &&
          p * right > -radius &&
          p * up    > -radius &&
          p * down  > -radius &&
          p * front < +radius;
    }

    OZ_ALWAYS_INLINE
    bool isVisible( float x, float y, float radius ) const
    {
      Point3 mins = Point3( x, y, -Orbis::DIM );
      Point3 maxs = Point3( x, y,  Orbis::DIM );

      return
          ( mins * left  > -radius || maxs * left  > -radius ) &&
          ( mins * right > -radius || maxs * right > -radius ) &&
          ( mins * up    > -radius || maxs * up    > -radius ) &&
          ( mins * down  > -radius || maxs * down  > -radius ) &&
          ( mins * front < +radius || maxs * front < +radius );
    }

    // get min and max index for cells per each axis, which should be included in PVS
    void getExtrems( Span& span, const Point3& p );

    void update();
    void init();

};

extern Frustum frustum;

}
}
