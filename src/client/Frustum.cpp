/*
 *  Frustum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Frustum.hpp"

namespace oz
{
namespace client
{

  Frustum frustum;

  void Frustum::getExtrems( Span& span, const Point3& p )
  {
    span.minX = max( int( ( p.x - radius + Orbis::DIM ) * Cell::INV_SIZE ), 0 );
    span.minY = max( int( ( p.y - radius + Orbis::DIM ) * Cell::INV_SIZE ), 0 );
    span.maxX = min( int( ( p.x + radius + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 );
    span.maxY = min( int( ( p.y + radius + Orbis::DIM ) * Cell::INV_SIZE ), Orbis::MAX - 1 );
  }

  void Frustum::update()
  {
    radius = camera.maxDist / cx;

    Vec3 nLeft  = camera.rotMat * nLeft0;
    Vec3 nRight = camera.rotMat * nRight0;
    Vec3 nDown  = camera.rotMat * nDown0;
    Vec3 nUp    = camera.rotMat * nUp0;
    Vec3 nFront = camera.at;

    float dLeft  = camera.p * nLeft;
    float dRight = camera.p * nRight;
    float dDown  = camera.p * nDown;
    float dUp    = camera.p * nUp;
    float dFront = camera.p * nFront + camera.maxDist;

    left  = Plane( nLeft,  dLeft  );
    right = Plane( nRight, dRight );
    down  = Plane( nDown,  dDown  );
    up    = Plane( nUp,    dUp    );
    front = Plane( nFront, dFront );
  }

  void Frustum::init()
  {
    fovX = Math::atan( camera.coeff * camera.aspect );
    fovY = Math::atan( camera.coeff );

    Math::sincos( fovX, &sx, &cx );
    Math::sincos( fovY, &sy, &cy );

    nLeft0  = Vec3(   cx, 0.0f, -sx );
    nRight0 = Vec3(  -cx, 0.0f, -sx );
    nDown0  = Vec3( 0.0f,   cy, -sy );
    nUp0    = Vec3( 0.0f,  -cy, -sy );

    radius  = camera.maxDist / cx;
  }

}
}
