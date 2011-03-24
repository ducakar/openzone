/*
 *  StrategicProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/StrategicProxy.hpp"

#include "matrix/Vehicle.hpp"

#include "ui/UI.hpp"

#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  const float StrategicProxy::MIN_HEIGHT      = 5.00f;
  const float StrategicProxy::MAX_HEIGHT      = 50.0f;
  const float StrategicProxy::DEFAULT_HEIGHT  = 15.0f;
  const float StrategicProxy::FREE_LOW_SPEED  = 0.04f;
  const float StrategicProxy::FREE_HIGH_SPEED = 0.50f;
  const float StrategicProxy::RTS_LOW_SPEED   = 0.25f;
  const float StrategicProxy::RTS_HIGH_SPEED  = 0.75f;
  const float StrategicProxy::ZOOM_FACTOR     = 0.15f;

  void StrategicProxy::begin()
  {
    height   = DEFAULT_HEIGHT;
    camera.v = Math::rad( 15.0f );
    camera.w = 0.0f;
    camera.setTagged( null );

    ui::ui.strategic->show( true );
    ui::mouse.doShow = true;
  }

  void StrategicProxy::update()
  {
    if( ui::keyboard.keys[SDLK_KP_ENTER] && !ui::keyboard.oldKeys[SDLK_KP_ENTER] ) {
      isFree = !isFree;

      if( isFree ) {
        ui::mouse.doShow = false;
        isRTSFast = false;
      }
      else {
        ui::mouse.doShow = true;
        isFreeFast = true;
      }
    }
    if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] && isFree ) {
      ui::mouse.doShow = !ui::mouse.doShow;
    }

    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      if( ui::ui.strategic->tagged.length() == 1 ) {
        Object* tagged = orbis.objects[ ui::ui.strategic->tagged.first() ];
        Bot*    me = null;

        if( tagged->flags & Object::BOT_BIT ) {
          me = const_cast<Bot*>( static_cast<const Bot*>( tagged ) );
        }
        else if( tagged->flags & Object::VEHICLE_BIT ) {
          Vehicle* veh = const_cast<Vehicle*>( static_cast<const Vehicle*>( tagged ) );

          for( int i = 0; i < Vehicle::CREW_MAX; ++i ) {
            if( veh->crew[i] != -1 && orbis.objects[ veh->crew[i] ] != null ) {
              me = const_cast<Bot*>( static_cast<const Bot*>( orbis.objects[ veh->crew[i] ] ) );
              break;
            }
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

  void StrategicProxy::init()
  {
    isFree     = false;
    isFreeFast = true;
    isRTSFast  = false;
  }

}
}
