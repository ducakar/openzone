/*
 *  Frustum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Frustum.hpp"

namespace oz
{
namespace client
{

  Frustum frustum;

  void Frustum::init()
  {
    fovX = Math::atan( camera.coeff * camera.aspect );
    fovY = Math::atan( camera.coeff );

    Math::sincos( fovX, &sx, &cx );
    Math::sincos( fovY, &sy, &cy );

    nLeft0  = Vec3(   cx, sx,  0.0f );
    nRight0 = Vec3(  -cx, sx,  0.0f );
    nUp0    = Vec3( 0.0f, sy,   -cy );
    nDown0  = Vec3( 0.0f, sy,    cy );

    radius  = camera.maxDist / cx;
  }

  void Frustum::update()
  {
    radius = camera.maxDist / cx;

    nLeft  = camera.rotMat * nLeft0;
    nRight = camera.rotMat * nRight0;
    nDown  = camera.rotMat * nDown0;
    nUp    = camera.rotMat * nUp0;
    nFront = camera.at;

    dLeft  = camera.p * nLeft;
    dRight = camera.p * nRight;
    dDown  = camera.p * nDown;
    dUp    = camera.p * nUp;
    dFront = camera.p * nFront + camera.maxDist;
  }

}
}
