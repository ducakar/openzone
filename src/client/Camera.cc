/*
 *  Camera.cc
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hh"

#include "client/Camera.hh"

#include "matrix/Timer.hh"
#include "matrix/Collider.hh"
#include "matrix/BotClass.hh"
#include "matrix/Vehicle.hh"
#include "ui/UI.hh"

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
    v = Math::bound( v, -90.0f, 90.0f );

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
    aspect             = aspect != 0.0f ? aspect : float( width ) / float( height );
    minDist            = config.getSet( "camera.minDist",    0.1f );
    maxDist            = config.getSet( "camera.maxDist",    400.0f );
    mouseXSens         = config.getSet( "camera.mouseXSens", 0.20f );
    mouseYSens         = config.getSet( "camera.mouseYSens", 0.20f );
    keyXSens           = config.getSet( "camera.keysXSens",  100.0f );
    keyYSens           = config.getSet( "camera.keysYSens",  100.0f );
    smoothCoef         = config.getSet( "camera.smoothCoef", 0.50f );
    smoothCoef_1       = 1.0f - smoothCoef;
    isExternal         = true;

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

    freeCamProxy.init();
    strategicProxy.init();
    botProxy.init();

    state              = NONE;
    newState           = STRATEGIC;
  }

  void Camera::update()
  {
    h -= float( ui::mouse.overEdgeX ) * mouseXSens;
    v += float( ui::mouse.overEdgeY ) * mouseYSens;

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

    if( newState != state ) {
      switch( newState ) {
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
        case BOT: {
          proxy = &botProxy;
          break;
        }
      }
      isExternal = true;
      proxy->begin();
      state = newState;
    }

    proxy->update();
  }

  void Camera::prepare()
  {
    proxy->prepare();
  }

}
}
