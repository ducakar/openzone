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
#include "matrix/BotClass.h"
#include "matrix/Vehicle.h"

namespace oz
{
namespace client
{

  Camera camera;

  void Camera::init()
  {
    p.setZero();
    h = 0.0f;
    v = 0.0f;

    rot.setId();
    relRot.setId();

    rotMat  = rot.rotMat44();
    rotTMat = ~rotTMat;

    right = rotMat.x;
    at    = rotTMat.y;
    up    = rotTMat.z;

    botIndex = -1;
    bot = null;

    bobPhi   = 0.0f;
    bobTheta = 0.0f;
    bobBias  = 0.0f;

    smoothCoef         = config.getSet( "camera.smoothCoef", 0.3f );
    smoothCoef_1       = 1.0f - smoothCoef;
    externalDistFactor = config.getSet( "camera.externalDistFactor", 2.75f );
    isExternal         = config.getSet( "camera.external", false );
    isFreeLook         = false;
  }

  void Camera::update()
  {
    h       = Math::mod( h + 360.0f, 360.0f );
    v       = bound( v, -90.0f, 90.0f );

    rot     = Quat::rotZYX( Math::rad( h ), Math::rad( bobTheta ), Math::rad( v ) );
    rotMat  = rot.rotMat44();
    rotTMat = ~rotMat;

    right   = rotMat.x;
    at      = rotMat.y;
    up      = rotMat.z;

    oldP    = p;

    bot     = botIndex == -1 ? null : static_cast<Bot*>( world.objects[botIndex] );

    // world.objects[botIndex] might be null
    if( bot == null || ( bot->state & Bot::DEATH_BIT ) ) {
      botIndex = -1;
      bot = null;

      bobPhi   = 0.0f;
      bobTheta = 0.0f;
      bobBias  = 0.0f;
    }
    else if( isExternal ) {
      float dist;
      if( bot->parent != -1 ) {
        Vehicle *veh = static_cast<Vehicle*>( world.objects[bot->parent] );

        assert( veh->flags & Object::VEHICLE_BIT );

        dist = !veh->dim * externalDistFactor;
      }
      else {
        dist = !bot->dim * externalDistFactor;
      }

      Vec3 origin = bot->p + bot->camPos;
      Vec3 offset = -at * dist;

      collider.translate( origin, offset, bot );
      offset *= collider.hit.ratio;
      offset += at * THIRD_PERSON_CLIP_DIST;

      p.x = origin.x + offset.x;
      p.y = origin.y + offset.y;
      p.z = ( origin.z + offset.z ) * smoothCoef_1 + oldP.z * smoothCoef;

      bobPhi   = 0.0f;
      bobTheta = 0.0f;
      bobBias  = 0.0f;
    }
    else if( bot->parent != -1 ) {
      Vehicle *veh = static_cast<Vehicle*>( world.objects[bot->parent] );

      assert( veh->flags & Object::VEHICLE_BIT );

      rot     = veh->rot;
      rotMat  = rot.rotMat44();
      rotTMat = ~rotMat;

      right   = rotMat.x;
      at      = rotMat.y;
      up      = rotMat.z;

      p = bot->p + rotMat * bot->camPos;

      bobPhi   = 0.0f;
      bobTheta = 0.0f;
      bobBias  = 0.0f;
    }
    else {
      BotClass *clazz = static_cast<BotClass*>( bot->type );

      p = ( bot->p + bot->camPos ) * smoothCoef_1 + oldP * smoothCoef;

      if( bot->state & Bot::MOVING_BIT ) {
        float bobInc = ( bot->state & Bot::RUNNING_BIT ) && bot->grabObjIndex == -1 ?
            clazz->bobRunInc : clazz->bobWalkInc;

        bobPhi   = Math::mod( bobPhi + bobInc, 360.0f );
        bobTheta = Math::sin( Math::rad( bobPhi ) ) * clazz->bobRotation;
        bobBias  = Math::sin( Math::rad( 2.0f * bobPhi ) ) * clazz->bobAmplitude;
      }
      else {
        bobTheta *= BOB_SUPPRESSION_COEF;
        bobBias  *= BOB_SUPPRESSION_COEF;
      }
      p.z += bobBias;
    }
  }

}
}
