/*
 *  FreeCamProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "FreeCamProxy.h"

#include "matrix/Collider.h"
#include "ui/UI.h"
#include "Camera.h"

namespace oz
{
namespace client
{

  const float FreeCamProxy::LOW_SPEED  = 0.05f;
  const float FreeCamProxy::HIGH_SPEED = 0.50f;

  void FreeCamProxy::begin()
  {
    fastMove = false;
    camera.setTagged( null );
  }

  void FreeCamProxy::update()
  {}

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
      return;
    }

    camera.align();
    camera.move( p );

    collider.translate( camera.p, camera.at * 2.0f );
    camera.setTagged( collider.hit.obj );
  }

}
}
