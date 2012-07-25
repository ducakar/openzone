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
    h = Math::mod( h + Math::TAU, Math::TAU );
    v = clamp( v, 0.0f, Math::TAU / 2.0f );

    rot     = Quat::rotZ( h ) ^ Quat::rotX( v ) ^ Quat::rotZ( w );
    rotMat  = Mat44::rotation( rot );
    rotTMat = ~rotMat;

    right   = rotMat.x;
    up      = rotMat.y;
    at      = -rotMat.z;
  }

  void Camera::init()
  {
    width        = config.get( "screen.width",         1024 );
    height       = config.get( "screen.height",        768  );
    centreX      = width / 2;
    centreY      = height / 2;

    aspect       = config.getSet( "camera.aspect",     0.0f );
    minDist      = config.getSet( "camera.minDist",    0.1f );
    mouseXSens   = config.getSet( "camera.mouseXSens", 0.003f );
    mouseYSens   = config.getSet( "camera.mouseYSens", 0.003f );
    keyXSens     = config.getSet( "camera.keysXSens",  2.0f );
    keyYSens     = config.getSet( "camera.keysYSens",  2.0f );
    smoothCoef   = config.getSet( "camera.smoothCoef", 0.50f );
    isExternal   = true;

    float angle = Math::rad( config.getSet( "camera.angle", 80.0f ) );

    aspect     = aspect != 0.0f ? aspect : float( width ) / float( height );
    coeff      = Math::tan( angle / 2.0f );
    horizPlane = coeff * minDist;
    vertPlane  = aspect * horizPlane;

    String sDefaultState = config.getSet( "camera.defaultState", "STRATEGIC" );
    if( sDefaultState.equals( "FREECAM" ) ) {
      defaultState = FREECAM;
    }
    else if( sDefaultState.equals( "STRATEGIC" ) ) {
      defaultState = STRATEGIC;
    }
    else {
      log.println( "WARNING: invalid camera enum %s, must be either FREECAM or STRATEGIC",
                   sDefaultState.cstr() );
      defaultState = FREECAM;
    }

    p            = Point3::ORIGIN;
    oldP         = Point3::ORIGIN;
    newP         = Point3::ORIGIN;
    h            = 0.0f;
    v            = Math::TAU / 4.0f;
    w            = 0.0f;

    rot          = Quat::ID;
    relRot       = Quat::ID;

    rotMat       = Mat44::rotation( rot );
    rotTMat      = ~rotTMat;

    right        = rotMat.x;
    up           = rotMat.y;
    at           = -rotMat.z;

    tagged       = -1;
    taggedObj    = null;
    bot          = -1;
    botObj       = null;

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
