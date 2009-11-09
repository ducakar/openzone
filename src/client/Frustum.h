/*
 *  Frustum.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "matrix/matrix.h"
#include "Camera.h"

namespace oz
{
namespace client
{

  class Frustum
  {
    private:

      Vec3  nLeft0, nRight0, nDown0, nUp0;
      Vec3  nLeft, nRight, nDown, nUp, nFront;
      float dLeft, dRight, dDown, dUp, dFront;

      uint visibility( const Vec3 &p );
      uint visibility( const Vec3 &p, float radius );

    public:

      float maxDistance;
      float radius;

      int minX;
      int minY;
      int maxX;
      int maxY;

      void init( float fovY, float aspect, float maxDistance );
      void update();

      bool isVisible( const Vec3 &p )
      {
        return
            p * nLeft  > dLeft  &&
            p * nRight > dRight &&
            p * nUp    > dUp    &&
            p * nDown  > dDown  &&
            p * nFront < dFront;
      }

      bool isVisible( const Vec3 &p, float radius )
      {
        return
            p * nLeft  > dLeft  - radius &&
            p * nRight > dRight - radius &&
            p * nUp    > dUp    - radius &&
            p * nDown  > dDown  - radius &&
            p * nFront < dFront + radius;
      }

      bool isVisible( const Sphere &s )
      {
        return isVisible( s.p, s.r );
      }

      bool isVisible( const AABB &bb )
      {
        return isVisible( bb.p, Math::sqrt( bb.dim * bb.dim ) );
      }

      bool isVisible( const Bounds &b )
      {
        Vec3 dim = b.maxs - b.mins;
        return isVisible( ( b.mins + b.maxs ) * 0.5f, Math::sqrt( dim * dim ) );
      }

      bool isVisible( float x, float y, float radius )
      {
        Vec3 min = Vec3( x, y, -World::DIM );
        Vec3 max = Vec3( x, y,  World::DIM );

        return
            ( min * nLeft  > dLeft  - radius || max * nLeft  > dLeft  - radius ) &&
            ( min * nRight > dRight - radius || max * nRight > dRight - radius ) &&
            ( min * nUp    > dUp    - radius || max * nUp    > dUp    - radius ) &&
            ( min * nDown  > dDown  - radius || max * nDown  > dDown  - radius ) &&
            ( min * nFront < dFront + radius || max * nFront < dFront + radius );
      }

      // get min and max index for cells per each axis, which should be included in pvs
      void getExtrems( const Vec3 &p )
      {
        minX = max( static_cast<int>( p.x - radius + World::DIM ) / Cell::SIZEI, 0 );
        minY = max( static_cast<int>( p.y - radius + World::DIM ) / Cell::SIZEI, 0 );
        maxX = min( static_cast<int>( p.x + radius + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
        maxY = min( static_cast<int>( p.y + radius + World::DIM ) / Cell::SIZEI, World::MAX - 1 );
      }

  };

  extern Frustum frustum;

}
}
