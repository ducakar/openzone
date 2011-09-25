/*
 *  StrategicProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/StrategicProxy.hpp"

#include "matrix/Vehicle.hpp"

#include "client/ui/UI.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  const float StrategicProxy::MIN_HEIGHT      = 5.00f;
  const float StrategicProxy::MAX_HEIGHT      = 50.0f;
  const float StrategicProxy::DEFAULT_HEIGHT  = 15.0f;
  const float StrategicProxy::DEFAULT_ANGLE   = 30.0f;
  const float StrategicProxy::FREE_LOW_SPEED  = 0.04f;
  const float StrategicProxy::FREE_HIGH_SPEED = 0.50f;
  const float StrategicProxy::RTS_LOW_SPEED   = 0.25f;
  const float StrategicProxy::RTS_HIGH_SPEED  = 0.75f;
  const float StrategicProxy::ZOOM_FACTOR     = 0.15f;

  StrategicProxy::StrategicProxy() : height( DEFAULT_HEIGHT )
  {}

  void StrategicProxy::begin()
  {
    camera.v = isFree ? camera.v : Math::rad( DEFAULT_ANGLE );
    camera.w = 0.0f;
    camera.setTagged( null );

    ui::ui.strategicArea->tagged.clear();
    ui::ui.hudArea->show( false );
    ui::ui.strategicArea->show( true );
    ui::mouse.doShow = true;
  }

  void StrategicProxy::update()
  {
    ui::ui.strategicArea->show( true );

    camera.h += camera.relH;
    camera.v += camera.relV;

    if( ui::keyboard.keys[SDLK_KP_ENTER] && !ui::keyboard.oldKeys[SDLK_KP_ENTER] ) {
      isFree = !isFree;

      if( isFree ) {
        isRTSFast = false;
      }
      else {
        isFreeFast = true;
      }
    }

    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      if( ui::ui.strategicArea->tagged.length() == 1 ) {
        Object* tagged = orbis.objects[ ui::ui.strategicArea->tagged.first() ];
        Bot*    me = null;

        if( tagged->flags & Object::BOT_BIT ) {
          me = const_cast<Bot*>( static_cast<const Bot*>( tagged ) );
        }
        else if( tagged->flags & Object::VEHICLE_BIT ) {
          Vehicle* veh = const_cast<Vehicle*>( static_cast<const Vehicle*>( tagged ) );

          if( veh->pilot != -1 && orbis.objects[veh->pilot] != null ) {
            me = const_cast<Bot*>( static_cast<const Bot*>( orbis.objects[veh->pilot] ) );
          }
        }

        if( me != null ) {
          me->state |= Bot::PLAYER_BIT;
          camera.setBot( me );
          camera.setState( Camera::BOT );
        }
      }
    }
  }

  void StrategicProxy::prepare()
  {
    camera.align();

    Point3 p = camera.newP;

    if( isFree ) {
      // free camera mode
      if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
        isFreeFast = !isFreeFast;
      }

      float speed = isFreeFast ? FREE_HIGH_SPEED : FREE_LOW_SPEED;

      if( ui::keyboard.keys[SDLK_w] ) {
        p += camera.at * speed;
      }
      if( ui::keyboard.keys[SDLK_s] ) {
        p -= camera.at * speed;
      }
      if( ui::keyboard.keys[SDLK_d] ) {
        p += camera.right * speed;
      }
      if( ui::keyboard.keys[SDLK_a] ) {
        p -= camera.right * speed;
      }
      if( ui::keyboard.keys[SDLK_SPACE] ) {
        p.z += speed;
      }
      if( ui::keyboard.keys[SDLK_LCTRL] ) {
        p.z -= speed;
      }
    }
    else {
      // RTS camera mode
      if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
        isRTSFast = !isRTSFast;
      }

      Vec3  up = Vec3( -camera.right.y, camera.right.x, 0.0f );
      float logHeight = Math::log( height );
      float speed = ( isRTSFast ? RTS_HIGH_SPEED : RTS_LOW_SPEED ) * logHeight;

      if( ui::keyboard.keys[SDLK_w] ) {
        p += up * speed;
      }
      if( ui::keyboard.keys[SDLK_s] ) {
        p -= up * speed;
      }
      if( ui::keyboard.keys[SDLK_d] ) {
        p += camera.right * speed;
      }
      if( ui::keyboard.keys[SDLK_a] ) {
        p -= camera.right * speed;
      }
      if( ui::keyboard.keys[SDLK_SPACE] ) {
        height = min( MAX_HEIGHT, height + logHeight * ZOOM_FACTOR );
      }
      if( ui::keyboard.keys[SDLK_LCTRL] ) {
        height = max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR );
      }
      if( ui::mouse.wheelDown ) {
        float wheelFactor = float( ui::mouse.relZ ) * 10.0f;

        height = min( MAX_HEIGHT, height - logHeight * ZOOM_FACTOR * wheelFactor );
      }
      if( ui::mouse.wheelUp ) {
        float wheelFactor = float( ui::mouse.relZ ) * 10.0f;

        height = max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR * wheelFactor );
      }

      p.z = max( 0.0f, orbis.terra.height( p.x, p.y ) ) + height;
    }

    camera.move( p );
  }

  void StrategicProxy::read( InputStream* istream )
  {
    height     = istream->readFloat();
    isFree     = istream->readBool();
    isFreeFast = istream->readBool();
    isRTSFast  = istream->readBool();
  }

  void StrategicProxy::write( OutputStream* ostream ) const
  {
    ostream->writeFloat( height );
    ostream->writeBool( isFree );
    ostream->writeBool( isFreeFast );
    ostream->writeBool( isRTSFast );
  }

  void StrategicProxy::init()
  {
    isFree     = false;
    isFreeFast = true;
    isRTSFast  = false;
  }

}
}
