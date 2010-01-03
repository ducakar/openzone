/*
 *  Frustum.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "Frustum.h"

namespace oz
{
namespace client
{

  Frustum frustum;

  void Frustum::init( float fovY_, float aspect, float maxDistance_ )
  {
    fovX = Math::atan( Math::tan( Math::rad( fovY_ / 2.0f ) ) * aspect );
    fovY = Math::atan( Math::tan( Math::rad( fovY_ / 2.0f ) ) );

    Math::sincos( fovX, &sx, &cx );
    Math::sincos( fovY, &sy, &cy );

    nLeft0  = Vec3(   cx, sx,  0.0f );
    nRight0 = Vec3(  -cx, sx,  0.0f );
    nUp0    = Vec3( 0.0f, sy,   -cy );
    nDown0  = Vec3( 0.0f, sy,    cy );

    maxDistance = maxDistance_;
    radius      = maxDistance / cx;
  }

  void Frustum::update( float maxDistance_ )
  {
    maxDistance = maxDistance_;
    radius      = maxDistance / cx;

    nLeft  = camera.rotMat * nLeft0;
    nRight = camera.rotMat * nRight0;
    nDown  = camera.rotMat * nDown0;
    nUp    = camera.rotMat * nUp0;
    nFront = camera.at;

    dLeft  = camera.p * nLeft;
    dRight = camera.p * nRight;
    dDown  = camera.p * nDown;
    dUp    = camera.p * nUp;
    dFront = camera.p * nFront + maxDistance;
  }

}
}
