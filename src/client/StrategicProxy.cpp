/*
 *  StrategicProxy.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "StrategicProxy.h"

#include "ui/UI.h"
#include "Camera.h"

namespace oz
{
namespace client
{

  const float StrategicProxy::MIN_HEIGHT     = 5.00f;
  const float StrategicProxy::MAX_HEIGHT     = 50.0f;
  const float StrategicProxy::DEFAULT_HEIGHT = 20.0f;
  const float StrategicProxy::LOW_SPEED      = 0.20f;
  const float StrategicProxy::HIGH_SPEED     = 0.40f;
  const float StrategicProxy::ZOOM_FACTOR    = 0.10f;

  void StrategicProxy::begin()
  {
    height   = DEFAULT_HEIGHT;
    fastMove = false;
    camera.w = 0.0f;
    camera.setTagged( null );

    ui::ui.strategic->show( true );
    ui::mouse.doShow = true;
  }

  void StrategicProxy::update()
  {}

  void StrategicProxy::prepare()
  {
    camera.align();

    Vec3 up = Vec3( -camera.rotMat.x.y, camera.rotMat.x.x, 0.0f );
    Vec3 p  = camera.p;

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
      height = max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR );
    }

    if( ui::keyboard.keys[SDLK_m] && !ui::keyboard.oldKeys[SDLK_m] ) {
      ui::mouse.doShow = false;
      camera.setState( Camera::FREECAM );
    }

    p.z = max( 0.0f, world.terra.height( p.x, p.y ) ) + height;
    camera.move( p );
  }

}
}
