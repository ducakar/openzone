/*
 *  Camera.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Camera.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Collider.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/Vehicle.hpp"
#include "ui/UI.hpp"

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
    width        = config.get( "screen.width",         1024 );
    height       = config.get( "screen.height",        768  );
    centreX      = width / 2;
    centreY      = height / 2;
    angle        = config.getSet( "camera.angle",      80.0f );
    aspect       = config.getSet( "camera.aspect",     0.0f );
    aspect       = aspect != 0.0f ? aspect : float( width ) / float( height );
    minDist      = config.getSet( "camera.minDist",    0.1f );
    maxDist      = config.getSet( "camera.maxDist",    400.0f );
    mouseXSens   = config.getSet( "camera.mouseXSens", 0.20f );
    mouseYSens   = config.getSet( "camera.mouseYSens", 0.20f );
    keyXSens     = config.getSet( "camera.keysXSens",  100.0f );
    keyYSens     = config.getSet( "camera.keysYSens",  100.0f );
    smoothCoef   = config.getSet( "camera.smoothCoef", 0.50f );
    isExternal   = true;

    String sDefaultState = config.getSet( "camera.defaultState", "FREECAM" );
    if( sDefaultState.equals( "FREECAM" ) ) {
      defaultState = FREECAM;
    }
    else if( sDefaultState.equals( "STRATEGIC" ) ) {
      defaultState = STRATEGIC;
    }
    else {
      log.println( "WARNING: invalid camera enum %s, must be euther FREECAM or STRATEGIC",
                   sDefaultState.cstr() );
      defaultState = FREECAM;
    }

    p            = Point3::ORIGIN;
    oldP         = Point3::ORIGIN;
    newP         = Point3::ORIGIN;
    h            = 0.0f;
    v            = 0.0f;
    w            = 0.0f;

    rot          = Quat::ID;
    relRot       = Quat::ID;

    rotMat       = rot.rotMat44();
    rotTMat      = ~rotTMat;

    right        = rotMat.x;
    at           = rotTMat.y;
    up           = rotTMat.z;

    bot          = -1;

    freeCamProxy.init();
    strategicProxy.init();
    botProxy.init();

    state        = NONE;
    newState     = defaultState;
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

    botObj = bot == -1 ? null : static_cast<const Bot*>( orbis.objects[bot] );

    if( botObj == null || ( botObj->state & Bot::DEATH_BIT ) ) {
      bot = -1;
      botObj = null;
    }

    if( newState != state ) {
      switch( newState ) {
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
        case NONE: {
          hard_assert( false );
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
