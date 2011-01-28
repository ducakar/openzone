/*
 *  FreeCamProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/FreeCamProxy.hpp"

#include "matrix/Vehicle.hpp"
#include "ui/UI.hpp"
#include "client/Camera.hpp"

namespace oz
{
namespace client
{

  const float FreeCamProxy::LOW_SPEED  = 0.05f;
  const float FreeCamProxy::HIGH_SPEED = 0.50f;

  void FreeCamProxy::begin()
  {
    camera.w = 0.0f;
  }

  void FreeCamProxy::update()
  {
    if( ui::keyboard.keys[SDLK_TAB] && !ui::keyboard.oldKeys[SDLK_TAB] ) {
      ui::mouse.doShow = !ui::mouse.doShow;
    }
    if( ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
      if( camera.tagged != -1 ) {
        Bot* me = null;

        if( camera.taggedObj->flags & Object::BOT_BIT ) {
          me = const_cast<Bot*>( static_cast<const Bot*>( camera.taggedObj ) );
        }
        else if( camera.taggedObj->flags & Object::VEHICLE_BIT ) {
          Vehicle* veh = const_cast<Vehicle*>( static_cast<const Vehicle*>( camera.taggedObj ) );

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

  void FreeCamProxy::prepare()
  {
    if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
      fastMove = !fastMove;
    }

    Point3 p = camera.newP;
    float speed = fastMove ? HIGH_SPEED : LOW_SPEED;

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

    if( ui::keyboard.keys[SDLK_m] && !ui::keyboard.oldKeys[SDLK_m] ) {
      camera.setState( Camera::STRATEGIC );
    }

    camera.align();
    camera.move( p );

    collider.translate( camera.p, camera.at * 2.0f );
    camera.setTagged( collider.hit.obj );
  }

  void FreeCamProxy::init()
  {
    fastMove = true;
  }

}
}
