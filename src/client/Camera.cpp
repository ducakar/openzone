/*
 *  Camera.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Camera.h"

namespace oz
{
namespace client
{

  Camera camera;

  Camera::Camera()
  {
    p.setZero();
    h = 0.0f;
    v = 0.0f;

    rot.setId();
    relRot.setId();

    rotMat = rot.rotMat44();
    rotTMat = ~rotTMat;

    at = rotTMat.y();
    up = rotTMat.z();
  }

  void Camera::init()
  {
    smoothCoef = config.get( "render.camera.smoothCoef", 0.3f );
    smoothCoef_1 = 1.0f - smoothCoef;
  }

  void Camera::postUpdate()
  {
    oldP = p;
    relRot = Quat::rotZYX( Math::rad( h ), 0.0f, Math::rad( v ) );

    if( bot != null ) {
      p = ( bot->p + bot->camPos ) * smoothCoef_1 + oldP * smoothCoef;
      rot = Quat::rotZYX( Math::rad( bot->h + h ), 0.0f, Math::rad( bot->v + v ) );
    }
    else {
      rot = relRot;
    }

    rotMat = rot.rotMat44();
    rotTMat = ~rotMat;

    right = rotMat.x();
    at = rotMat.y();
    up = rotMat.z();
  }

}
}
