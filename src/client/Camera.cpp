/*
 *  Camera.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Camera.hpp"

#include "matrix/Timer.hpp"
#include "matrix/Collider.hpp"
#include "matrix/BotClass.hpp"
#include "matrix/Vehicle.hpp"
#include "client/ui/UI.hpp"

namespace oz
{
namespace client
{

  Camera camera;

  StrategicProxy Camera::strategicProxy;
  BotProxy       Camera::botProxy;

  void Camera::align()
  {
    h       = Math::mod( h + Math::TAU, Math::TAU );
    v       = clamp( v, 0.0f, Math::TAU / 2.0f );

    rot     = Quat::rotZ( h ) ^ Quat::rotX( v ) ^ Quat::rotZ( w );
    rotMat  = Mat44::rotation( rot );
    rotTMat = ~rotMat;

    right   = rotMat.x;
    up      = rotMat.y;
    at      = -rotMat.z;
  }

  void Camera::update()
  {
    relH = -float( ui::mouse.overEdgeX ) * mouseXSens;
    relV = +float( ui::mouse.overEdgeY ) * mouseYSens;

    if( ui::keyboard.keys[SDLK_LEFT] | ui::keyboard.keys[SDLK_KP1] |
        ui::keyboard.keys[SDLK_KP4] | ui::keyboard.keys[SDLK_KP7] )
    {
      relH += keyYSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_RIGHT] | ui::keyboard.keys[SDLK_KP3] |
        ui::keyboard.keys[SDLK_KP6] | ui::keyboard.keys[SDLK_KP9] )
    {
      relH -= keyYSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_DOWN] | ui::keyboard.keys[SDLK_KP1] |
        ui::keyboard.keys[SDLK_KP2] | ui::keyboard.keys[SDLK_KP3] )
    {
      relV -= keyXSens * Timer::TICK_TIME;
    }
    if( ui::keyboard.keys[SDLK_UP] | ui::keyboard.keys[SDLK_KP7] |
        ui::keyboard.keys[SDLK_KP8] | ui::keyboard.keys[SDLK_KP9] )
    {
      relV += keyXSens * Timer::TICK_TIME;
    }

    botObj = bot == -1 ? null : static_cast<Bot*>( orbis.objects[bot] );

    if( botObj == null || ( botObj->state & Bot::DEATH_BIT ) ) {
      bot = -1;
      botObj = null;
    }

    if( newState != state ) {
      switch( newState ) {
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
    botObj = bot == -1 ? null : static_cast<Bot*>( orbis.objects[bot] );

    if( botObj == null || ( botObj->state & Bot::DEATH_BIT ) ) {
      bot = -1;
      botObj = null;
    }

    proxy->prepare();
  }

  void Camera::read( InputStream* istream )
  {
    ui::mouse.doShow = istream->readBool();

    p         = istream->readPoint3();
    oldP      = p;
    newP      = p;

    h         = istream->readFloat();
    v         = istream->readFloat();
    w         = istream->readFloat();
    relH      = istream->readFloat();
    relV      = istream->readFloat();

    rot       = Quat::rotZ( h ) ^ Quat::rotX( v ) ^ Quat::rotZ( w );
    rotMat    = Mat44::rotation( rot );
    rotTMat   = ~rotMat;

    right     = rotMat.x;
    up        = rotMat.y;
    at        = -rotMat.z;

    tagged    = -1;
    taggedObj = null;
    bot       = istream->readInt();
    botObj    = bot == -1 ? null : static_cast<Bot*>( orbis.objects[bot] );

    state     = State( istream->readInt() );
    newState  = state;

    isExternal         = istream->readBool();
    allowReincarnation = istream->readBool();

    strategicProxy.read( istream );
    botProxy.read( istream );

    switch( state ) {
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
  }

  void Camera::write( OutputStream* ostream ) const
  {
    ostream->writeBool( ui::mouse.doShow );

    ostream->writePoint3( newP );

    ostream->writeFloat( h );
    ostream->writeFloat( v );
    ostream->writeFloat( w );
    ostream->writeFloat( relH );
    ostream->writeFloat( relV );

    ostream->writeInt( bot );

    ostream->writeInt( state );

    ostream->writeBool( isExternal );
    ostream->writeBool( allowReincarnation );

    strategicProxy.write( ostream );
    botProxy.write( ostream );
  }

  void Camera::clear()
  {
    w         = 0.0f;
    relH      = 0.0f;
    relV      = 0.0f;

    tagged    = -1;
    taggedObj = null;
    bot       = -1;
    botObj    = null;
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

    float angle  = Math::rad( config.getSet( "camera.angle", 80.0f ) );

    aspect       = aspect != 0.0f ? aspect : float( width ) / float( height );
    coeff        = Math::tan( angle / 2.0f );
    horizPlane   = coeff * minDist;
    vertPlane    = aspect * horizPlane;

    String sDefaultState = config.getSet( "camera.defaultState", "STRATEGIC" );
    if( sDefaultState.equals( "STRATEGIC" ) ) {
      defaultState = STRATEGIC;
    }
    else {
      log.println( "WARNING: invalid camera enum %s, must be STRATEGIC",
                   sDefaultState.cstr() );
      defaultState = STRATEGIC;
    }

    p         = Point3::ORIGIN;
    oldP      = Point3::ORIGIN;
    newP      = Point3::ORIGIN;
    h         = 0.0f;
    v         = Math::TAU / 4.0f;
    w         = 0.0f;
    relH      = 0.0f;
    relV      = 0.0f;

    rot       = Quat::ID;
    relRot    = Quat::ID;

    rotMat    = Mat44::rotation( rot );
    rotTMat   = ~rotTMat;

    right     = rotMat.x;
    up        = rotMat.y;
    at        = -rotMat.z;

    tagged    = -1;
    taggedObj = null;
    bot       = -1;
    botObj    = null;

    strategicProxy.init();
    botProxy.init();

    state     = NONE;
    newState  = defaultState;

    isExternal         = true;
    allowReincarnation = true;
  }

}
}
