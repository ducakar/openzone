/*
 *  FreeCamProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.h"

#include "client/FreeCamProxy.h"

#include "matrix/Collider.h"
#include "ui/UI.h"
#include "client/Camera.h"

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
      if( camera.tagged != -1 && ( camera.taggedObj->flags & Object::BOT_BIT ) ) {
        Bot* me = const_cast<Bot*>( static_cast<const Bot*>( camera.taggedObj ) );

        me->state |= Bot::PLAYER_BIT;
        camera.setBot( me );
        camera.setState( Camera::BOT );
      }
    }
  }

  void FreeCamProxy::prepare()
  {
    if( ui::keyboard.keys[SDLK_LSHIFT] && !ui::keyboard.oldKeys[SDLK_LSHIFT] ) {
      fastMove = !fastMove;
    }

    Vec3  p = camera.p;
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
    fastMove = false;
  }

}
}
