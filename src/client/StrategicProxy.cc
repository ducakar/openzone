/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file client/StrategicProxy.cc
 */

#include "stable.hh"

#include "client/StrategicProxy.hh"

#include "client/Camera.hh"
#include "client/ui/UI.hh"
#include "client/ui/GalileoFrame.hh"

namespace oz
{
namespace client
{

const float StrategicProxy::MIN_HEIGHT      = 2.00f;
const float StrategicProxy::MAX_HEIGHT      = 50.0f;
const float StrategicProxy::DEFAULT_HEIGHT  = 15.0f;
const float StrategicProxy::DEFAULT_ANGLE   = 45.0f;
const float StrategicProxy::FREE_LOW_SPEED  = 2.40f;
const float StrategicProxy::FREE_HIGH_SPEED = 30.0f;
const float StrategicProxy::RTS_LOW_SPEED   = 15.0f;
const float StrategicProxy::RTS_HIGH_SPEED  = 45.0f;
const float StrategicProxy::ZOOM_FACTOR     = 0.15f;

StrategicProxy::StrategicProxy() :
  strategicArea( null ), height( DEFAULT_HEIGHT )
{}

void StrategicProxy::begin()
{
  camera.v = isFree || camera.state == Camera::NONE ? camera.v : Math::rad( DEFAULT_ANGLE );
  camera.w = 0.0f;
  camera.mag = 1.0f;
  camera.setTaggedObj( null );
  camera.setTaggedEnt( null );

  ui::mouse.doShow = true;

  strategicArea = new ui::StrategicArea();
  ui::ui.root->add( strategicArea );
  strategicArea->sink();
}

void StrategicProxy::end()
{
  if( strategicArea != null ) {
    ui::ui.root->remove( strategicArea );
    strategicArea = null;
  }

  ui::mouse.doShow = true;
}

void StrategicProxy::prepare()
{
  const ubyte* keys    = ui::keyboard.keys;
  const ubyte* oldKeys = ui::keyboard.oldKeys;

  bool alt = keys[SDLK_LALT] || keys[SDLK_RALT];

  camera.h += camera.relH;
  camera.v += camera.relV;

  if( !alt && keys[SDLK_n] && !oldKeys[SDLK_n] ) {
    camera.nightVision = !camera.nightVision;
  }
  if( !alt && keys[SDLK_m] && !oldKeys[SDLK_m] ) {
    ui::ui.galileoFrame->setMaximised( !ui::ui.galileoFrame->isMaximised );
  }

  if( keys[SDLK_KP_ENTER] && !oldKeys[SDLK_KP_ENTER] ) {
    isFree = !isFree;

    if( isFree ) {
      isRTSFast = false;
    }
    else {
      isFreeFast = true;
    }
  }

  if( !alt && keys[SDLK_o] ) {
    if( keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT] ) {
      orbis.caelum.time -= 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
    else {
      orbis.caelum.time += 0.1f * Timer::TICK_TIME * orbis.caelum.period;
    }
  }

  if( !alt && keys[SDLK_i] && !oldKeys[SDLK_i] && camera.allowReincarnation ) {
    if( strategicArea->taggedObjs.length() == 1 ) {
      const Object* tagged = orbis.objects[ strategicArea->taggedObjs.first() ];
      const Bot*    me     = null;

      if( tagged != null ) {
        if( tagged->flags & Object::BOT_BIT ) {
          me = static_cast<const Bot*>( tagged );
        }
        else if( tagged->flags & Object::VEHICLE_BIT ) {
          const Vehicle* veh = static_cast<const Vehicle*>( tagged );

          if( veh->pilot != -1 && orbis.objects[veh->pilot] != null ) {
            me = static_cast<const Bot*>( orbis.objects[veh->pilot] );
          }
        }
      }

      if( me != null && !( me->state & Bot::DEAD_BIT ) ) {
        camera.setBot( const_cast<Bot*>( me ) );
        camera.setState( Camera::BOT );
      }
    }
  }

  if( keys[SDLK_TAB] && !oldKeys[SDLK_TAB] ) {
    ui::mouse.doShow = !ui::mouse.doShow;
  }
}

void StrategicProxy::update()
{
  const ubyte* keys    = ui::keyboard.keys;
  const ubyte* oldKeys = ui::keyboard.oldKeys;

  camera.align();

  Point p = camera.newP;

  if( isFree ) {
    // free camera mode
    if( keys[SDLK_LSHIFT] && !oldKeys[SDLK_LSHIFT] ) {
      isFreeFast = !isFreeFast;
    }

    float speed = ( isFreeFast ? FREE_HIGH_SPEED : FREE_LOW_SPEED ) * Timer::TICK_TIME;

    if( keys[SDLK_w] ) {
      p += camera.at * speed;
    }
    if( keys[SDLK_s] ) {
      p -= camera.at * speed;
    }
    if( keys[SDLK_d] ) {
      p += camera.right * speed;
    }
    if( keys[SDLK_a] ) {
      p -= camera.right * speed;
    }
    if( keys[SDLK_SPACE] ) {
      p.z += speed;
    }
    if( keys[SDLK_LCTRL] ) {
      p.z -= speed;
    }

    p.x = clamp<float>( p.x, -Orbis::DIM, +Orbis::DIM );
    p.y = clamp<float>( p.y, -Orbis::DIM, +Orbis::DIM );
    p.z = clamp<float>( p.z, -Orbis::DIM, +Orbis::DIM );
  }
  else {
    // RTS camera mode
    if( keys[SDLK_LSHIFT] && !oldKeys[SDLK_LSHIFT] ) {
      isRTSFast = !isRTSFast;
    }

    Vec3  up = Vec3( -camera.right.y, camera.right.x, 0.0f );
    float logHeight = Math::log( height );
    float speed = ( isRTSFast ? RTS_HIGH_SPEED : RTS_LOW_SPEED ) * Timer::TICK_TIME * logHeight;

    if( keys[SDLK_w] ) {
      p += up * speed;
    }
    if( keys[SDLK_s] ) {
      p -= up * speed;
    }
    if( keys[SDLK_d] ) {
      p += camera.right * speed;
    }
    if( keys[SDLK_a] ) {
      p -= camera.right * speed;
    }
    if( keys[SDLK_SPACE] ) {
      height = min( MAX_HEIGHT, height + logHeight * ZOOM_FACTOR );
    }
    if( keys[SDLK_LCTRL] ) {
      height = max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR );
    }
    if( ui::mouse.wheelDown ) {
      float wheelFactor = float( ui::mouse.relW ) * 10.0f;

      height = min( MAX_HEIGHT, height - logHeight * ZOOM_FACTOR * wheelFactor );
    }
    if( ui::mouse.wheelUp ) {
      float wheelFactor = float( ui::mouse.relW ) * 10.0f;

      height = max( MIN_HEIGHT, height - logHeight * ZOOM_FACTOR * wheelFactor );
    }

    p.x = clamp<float>( p.x, -Orbis::DIM, +Orbis::DIM );
    p.y = clamp<float>( p.y, -Orbis::DIM, +Orbis::DIM );
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

}
}
