/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/StrategicProxy.cpp
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
const float StrategicProxy::FREE_LOW_SPEED  = 2.40f * Timer::TICK_TIME;
const float StrategicProxy::FREE_HIGH_SPEED = 30.0f * Timer::TICK_TIME;
const float StrategicProxy::RTS_LOW_SPEED   = 15.0f * Timer::TICK_TIME;
const float StrategicProxy::RTS_HIGH_SPEED  = 45.0f * Timer::TICK_TIME;
const float StrategicProxy::ZOOM_FACTOR     = 0.15f;

StrategicProxy::StrategicProxy() : height( DEFAULT_HEIGHT )
{}

void StrategicProxy::begin()
{
  camera.v = isFree ? camera.v : Math::rad( DEFAULT_ANGLE );
  camera.w = 0.0f;
  camera.setTagged( null );

  ui::ui.strategicArea->taggedStrs.clear();
  ui::ui.strategicArea->taggedObjs.clear();
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

  if( camera.allowReincarnation && ui::keyboard.keys[SDLK_i] && !ui::keyboard.oldKeys[SDLK_i] ) {
    if( ui::ui.strategicArea->taggedObjs.length() == 1 ) {
      const Object* tagged = orbis.objects[ ui::ui.strategicArea->taggedObjs.first() ];
      const Bot*    me = null;

      if( tagged->flags & Object::BOT_BIT ) {
        me = static_cast<const Bot*>( tagged );
      }
      else if( tagged->flags & Object::VEHICLE_BIT ) {
        const Vehicle* veh = static_cast<const Vehicle*>( tagged );

        if( veh->pilot != -1 && orbis.objects[veh->pilot] != null ) {
          me = static_cast<const Bot*>( orbis.objects[veh->pilot] );
        }
      }

      if( me != null && !( me->state & Bot::DEAD_BIT ) ) {
        camera.setBot( const_cast<Bot*>( me ) );
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

    p.x = clamp( p.x, -Orbis::DIM, +Orbis::DIM );
    p.y = clamp( p.y, -Orbis::DIM, +Orbis::DIM );
    p.z = clamp( p.z, -Orbis::DIM, +Orbis::DIM );
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

    p.x = clamp( p.x, -Orbis::DIM, +Orbis::DIM );
    p.y = clamp( p.y, -Orbis::DIM, +Orbis::DIM );
    p.z = max( 0.0f, orbis.terra.height( p.x, p.y ) ) + height;
  }

  camera.move( p );
}

void StrategicProxy::reset()
{
  height     = DEFAULT_HEIGHT;
  isFree     = false;
  isFreeFast = true;
  isRTSFast  = false;
}

void StrategicProxy::read( InputStream* istream )
{
  height     = istream->readFloat();
  isFree     = istream->readBool();
  isFreeFast = istream->readBool();
  isRTSFast  = istream->readBool();
}

void StrategicProxy::write( BufferStream* ostream ) const
{
  ostream->writeFloat( height );
  ostream->writeBool( isFree );
  ostream->writeBool( isFreeFast );
  ostream->writeBool( isRTSFast );
}

void StrategicProxy::init()
{}

}
}
