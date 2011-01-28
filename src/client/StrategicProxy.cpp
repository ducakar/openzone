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

  const float StrategicProxy::MIN_HEIGHT     = 5.00f;
  const float StrategicProxy::MAX_HEIGHT     = 50.0f;
  const float StrategicProxy::DEFAULT_HEIGHT = 15.0f;
  const float StrategicProxy::LOW_SPEED      = 0.25f;
  const float StrategicProxy::HIGH_SPEED     = 0.75f;
  const float StrategicProxy::ZOOM_FACTOR    = 0.15f;

  void StrategicProxy::begin()
  {
    height   = DEFAULT_HEIGHT;
    camera.v = -75.0f;
    camera.w = 0.0f;
    camera.setTagged( null );

    ui::ui.strategic->show( true );
    ui::mouse.doShow = true;
  }

  void StrategicProxy::update()
  {
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

    Vec3   up = Vec3( -camera.rotMat.x.y, camera.rotMat.x.x, 0.0f );
    Point3 p  = camera.newP;

    if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
      fastMove = !fastMove;
    }

    float logHeight = Math::log( height );
    float speed = ( fastMove ? HIGH_SPEED : LOW_SPEED ) * logHeight;

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
      height = Math::max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR );
    }

    if( ui::keyboard.keys[SDLK_m] && !ui::keyboard.oldKeys[SDLK_m] ) {
      ui::mouse.doShow = false;
      camera.setState( Camera::FREECAM );
    }

    p.z = Math::max( 0.0f, orbis.terra.height( p.x, p.y ) ) + height;
    camera.move( p );
  }

  void StrategicProxy::init()
  {
    fastMove = false;
  }

}
}
