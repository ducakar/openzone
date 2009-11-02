/*
 *  Camera.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Camera.h"

#include "matrix/Collider.h"

namespace oz
{
namespace client
{

  Camera camera;

  const float Camera::THIRD_PERSON_CLIP_DIST = 0.2f;

  Camera::Camera()
  {
    p.setZero();
    h = 0.0f;
    v = 0.0f;

    rot.setId();
    relRot.setId();

    rotMat = rot.rotMat44();
    rotTMat = ~rotTMat;

    at = rotTMat.y;
    up = rotTMat.z;
  }

  void Camera::init()
  {
    isThirdPerson   = config.get( "render.camera.3rdPerson", false );
    thirdPersonDist = config.get( "render.camera.3rdPersonDistance", 2.5f );
    smoothCoef      = config.get( "render.camera.smoothCoef", 0.3f );
    smoothCoef_1    = 1.0f - smoothCoef;
  }

  void Camera::update()
  {
    oldP = p;
    relRot = Quat::rotZYX( Math::rad( h ), 0.0f, Math::rad( v ) );

    bot = botIndex == -1 ? null : static_cast<Bot*>( world.objects[botIndex] );

    // world.objects[botIndex] might be null
    if( bot == null || ( bot->state & Bot::DEATH_BIT ) ) {
      botIndex = -1;
      bot = null;
      rot = relRot;

      rotMat = rot.rotMat44();
      rotTMat = ~rotMat;

      right = rotMat.x;
      at = rotMat.y;
      up = rotMat.z;
    }
    else {
      rot = Quat::rotZYX( Math::rad( bot->h + h ), 0.0f, Math::rad( bot->v + v ) );

      rotMat = rot.rotMat44();
      rotTMat = ~rotMat;

      right = rotMat.x;
      at = rotMat.y;
      up = rotMat.z;

      if( isThirdPerson ) {
        Vec3 origin = bot->p + bot->camPos;
        Vec3 offset = -at * thirdPersonDist;

        collider.translate( origin, offset, bot );
        offset *= collider.hit.ratio;
        offset += at * THIRD_PERSON_CLIP_DIST;

        p.x = origin.x + offset.x;
        p.y = origin.y + offset.y;
        p.z = ( origin.z + offset.z ) * smoothCoef_1 + oldP.z * smoothCoef;
      }
      else {
        p = ( bot->p + bot->camPos ) * smoothCoef_1 + oldP * smoothCoef;
      }
    }
  }

}
}
