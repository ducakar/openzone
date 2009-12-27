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

#include "matrix/Timer.h"
#include "matrix/Collider.h"
#include "matrix/BotClass.h"
#include "matrix/Vehicle.h"
#include "ui/UI.h"

namespace oz
{
namespace client
{

  Camera camera;

  FreeCamProxy   Camera::freeCamProxy;
  StrategicProxy Camera::strategicProxy;
  BotProxy       Camera::botProxy;

  void Camera::align()
  {
    h = Math::mod( h + 360.0f, 360.0f );
    v = bound( v, -90.0f, 90.0f );
    w = 0.0f;

    rot     = Quat::rotZYX( Math::rad( h ), Math::rad( w ), Math::rad( v ) );
    rotMat  = rot.rotMat44();
    rotTMat = ~rotMat;

    right   = rotMat.x;
    at      = rotMat.y;
    up      = rotMat.z;
  }

  void Camera::init()
  {
    width              = config.get( "screen.width",         1024 );
    height             = config.get( "screen.height",        768  );
    centerX            = width / 2;
    centerY            = height / 2;
    angle              = config.getSet( "camera.angle",      80.0f );
    aspect             = config.getSet( "camera.aspect",     0.0f );
    minDist            = config.getSet( "camera.minDist",    0.1f );
    maxDist            = config.getSet( "camera.maxDist",    400.0f );

    aspect             = aspect != 0.0f ?
        aspect : static_cast<float>( width ) / static_cast<float>( height );

    mouseXSens         = config.getSet( "camera.mouseXSens", 0.20f );
    mouseYSens         = config.getSet( "camera.mouseYSens", 0.20f );
    keyXSens           = config.getSet( "camera.keysXSens",  100.0f );
    keyYSens           = config.getSet( "camera.keysYSens",  100.0f );
    smoothCoef         = config.getSet( "camera.smoothCoef", 0.3f );
    smoothCoef_1       = 1.0f - smoothCoef;
    isExternal         = config.getSet( "camera.external", false );
    fastMove           = false;

    p.setZero();
    oldP.setZero();
    newP.setZero();
    h                  = 0.0f;
    v                  = 0.0f;
    w                  = 0.0f;

    rot.setId();
    relRot.setId();

    rotMat             = rot.rotMat44();
    rotTMat            = ~rotTMat;

    right              = rotMat.x;
    at                 = rotTMat.y;
    up                 = rotTMat.z;

    bot                = -1;

    state              = FREECAM;
    proxy              = &freeCamProxy;
  }

  void Camera::setState( State state_ )
  {
    if( state_ != state ) {
      state = state_;

      switch( state ) {
        default: {
          assert( false );
        }
        case FREECAM: {
          proxy = &freeCamProxy;
          break;
        }
        case STRATEGIC: {
          proxy = &strategicProxy;
          break;
        }
        case INTERNAL:
        case EXTERNAL: {
          proxy = &botProxy;
          break;
        }
      }
      proxy->begin();
    }
  }

  void Camera::update()
  {
    h -= ui::mouse.overEdgeX * mouseXSens;
    v += ui::mouse.overEdgeY * mouseYSens;

    if( ui::keyboard.keys[SDLK_UP] ) {
      v += keyXSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_DOWN] ) {
      v -= keyXSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_RIGHT] ) {
      h -= keyYSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_LEFT] ) {
      h += keyYSens * Timer::TICK_TIME;
    }

    botObj = bot == -1 ? null : static_cast<const Bot*>( world.objects[bot] );

    if( botObj == null || ( botObj->state & Bot::DEATH_BIT ) ) {
      bot = -1;
      botObj = null;
    }

    proxy->update();
  }

  void Camera::prepare()
  {
    proxy->prepare();
  }

}
}
