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
 * @file client/Camera.cc
 */

#include "stable.hh"

#include "client/Camera.hh"

#include "client/ui/UI.hh"

namespace oz
{
namespace client
{

Camera camera;

const float Camera::ROT_LIMIT      = Math::TAU / 2.0f;
const float Camera::MIN_DISTANCE   = 0.1f;
const float Camera::SMOOTHING_COEF = 0.5f;

StrategicProxy Camera::strategicProxy;
BotProxy       Camera::botProxy;

void Camera::updateReferences()
{
  if( object == -1 ) {
    objectObj = null;
  }
  else {
    objectObj = orbis.objects[object];
    object = objectObj == null ? -1 : object;
  }

  if( entity == -1 ) {
    entityObj = null;
  }
  else {
    int strIndex = entity / Struct::MAX_ENTITIES;
    int entIndex = entity % Struct::MAX_ENTITIES;

    const Struct* str = orbis.structs[strIndex];

    if( str == null ) {
      entity    = -1;
      entityObj = null;
    }
    else {
      entityObj = &str->entities[entIndex];
    }
  }

  if( bot == -1 ) {
    botObj = null;
  }
  else {
    botObj = static_cast<Bot*>( orbis.objects[bot] );

    if( botObj == null || ( botObj->state & Bot::DEAD_BIT ) ) {
      bot    = -1;
      botObj = null;
    }
  }

  if( botObj == null || botObj->parent == -1 ) {
    vehicle    = -1;
    vehicleObj = null;
  }
  else {
    vehicle    = botObj->parent;
    vehicleObj = static_cast<Vehicle*>( orbis.objects[vehicle] );

    if( vehicleObj == null ) {
      vehicle    = -1;
      vehicleObj = null;
    }
  }
}

void Camera::align()
{
  hard_assert( 0.0f <= h && h < Math::TAU );
  hard_assert( 0.0f <= v && v <= Math::TAU / 2.0f );

  rot     = Quat::rotZ( h ) * Quat::rotX( v ) * Quat::rotZ( w );
  rot     = vehicleObj == null ? rot : vehicleObj->rot * rot;
  rotMat  = Mat44::rotation( rot );
  rotTMat = ~rotMat;

  right   = +rotMat.x;
  up      = +rotMat.y;
  at      = -rotMat.z;
}

void Camera::prepare()
{
  updateReferences();

  relH = float( -ui::mouse.overEdgeX ) * mouseXSens * mag;
  relV = float( +ui::mouse.overEdgeY ) * mouseYSens * mag;

  relH = clamp( relH, -ROT_LIMIT, +ROT_LIMIT );
  relV = clamp( relV, -ROT_LIMIT, +ROT_LIMIT );

  if( ui::keyboard.keys[SDLK_LEFT] | ui::keyboard.keys[SDLK_KP1] |
      ui::keyboard.keys[SDLK_KP4] | ui::keyboard.keys[SDLK_KP7] )
  {
    relH += keyYSens;
  }
  if( ui::keyboard.keys[SDLK_RIGHT] | ui::keyboard.keys[SDLK_KP3] |
      ui::keyboard.keys[SDLK_KP6] | ui::keyboard.keys[SDLK_KP9] )
  {
    relH -= keyYSens;
  }
  if( ui::keyboard.keys[SDLK_DOWN] | ui::keyboard.keys[SDLK_KP1] |
      ui::keyboard.keys[SDLK_KP2] | ui::keyboard.keys[SDLK_KP3] )
  {
    relV -= keyXSens;
  }
  if( ui::keyboard.keys[SDLK_UP] | ui::keyboard.keys[SDLK_KP7] |
      ui::keyboard.keys[SDLK_KP8] | ui::keyboard.keys[SDLK_KP9] )
  {
    relV += keyXSens;
  }

  if( newState != state ) {
    isExternal = true;

    if( proxy != null ) {
      proxy->end();
    }

    switch( newState ) {
      case NONE: {
        proxy = null;
        break;
      }
      case STRATEGIC: {
        proxy = &strategicProxy;
        break;
      }
      case BOT: {
        proxy = &botProxy;
        break;
      }
    }

    if( proxy != null ) {
      proxy->begin();
    }
    else {
      bot       = -1;
      botObj    = null;

      object    = -1;
      objectObj = null;

      entity    = -1;
      entityObj = null;
    }

    state = newState;
  }

  if( proxy != null ) {
    proxy->prepare();
  }
}

void Camera::update()
{
  updateReferences();

  horizPlane = coeff * mag * MIN_DISTANCE;
  vertPlane  = aspect * horizPlane;

  if( proxy != null ) {
    proxy->update();
  }
}

void Camera::reset()
{
  p          = Point::ORIGIN;
  oldP       = Point::ORIGIN;
  newP       = Point::ORIGIN;
  h          = 0.0f;
  v          = Math::TAU / 4.0f;
  w          = 0.0f;
  mag        = 1.0f;

  relH       = 0.0f;
  relV       = 0.0f;

  rot        = Quat::ID;
  rotMat     = Mat44::rotation( rot );
  rotTMat    = ~rotTMat;

  right      = rotMat.x;
  up         = rotMat.y;
  at         = -rotMat.z;

  object     = -1;
  objectObj  = null;
  entity     = -1;
  entityObj  = null;
  bot        = -1;
  botObj     = null;
  vehicle    = -1;
  vehicleObj = null;

  isExternal         = false;
  allowReincarnation = true;
  nightVision        = false;

  state    = NONE;
  newState = NONE;

  strategicProxy.reset();
  botProxy.reset();

  if( proxy != null ) {
    proxy->end();
    proxy = null;
  }
}

void Camera::read( InputStream* istream )
{
  p          = istream->readPoint();
  oldP       = p;
  newP       = p;

  h          = istream->readFloat();
  v          = istream->readFloat();
  w          = istream->readFloat();
  mag        = istream->readFloat();
  relH       = istream->readFloat();
  relV       = istream->readFloat();

  rot        = Quat::rotZ( h ) * Quat::rotX( v ) * Quat::rotZ( w );
  rotMat     = Mat44::rotation( rot );
  rotTMat    = ~rotMat;

  right      = rotMat.x;
  up         = rotMat.y;
  at         = -rotMat.z;

  object     = -1;
  objectObj  = null;
  entity     = -1;
  entityObj  = null;
  bot        =  istream->readInt();
  botObj     = bot == -1 ? null : static_cast<Bot*>( orbis.objects[bot] );
  vehicle    = istream->readInt();
  vehicleObj = vehicle == -1 ? null : static_cast<Vehicle*>( orbis.objects[vehicle] );

  isExternal         = istream->readBool();
  allowReincarnation = istream->readBool();
  nightVision        = istream->readBool();

  state     = NONE;
  newState  = State( istream->readInt() );

  strategicProxy.read( istream );
  botProxy.read( istream );

  switch( newState ) {
    case NONE: {
      proxy = null;
      break;
    }
    case STRATEGIC: {
      proxy = &strategicProxy;
      break;
    }
    case BOT: {
      proxy = &botProxy;
      break;
    }
  }

  if( proxy != null ) {
    proxy->begin();
  }

  state = newState;
}

void Camera::write( BufferStream* ostream ) const
{
  ostream->writePoint( newP );

  ostream->writeFloat( h );
  ostream->writeFloat( v );
  ostream->writeFloat( w );
  ostream->writeFloat( mag );

  ostream->writeFloat( relH );
  ostream->writeFloat( relV );

  ostream->writeInt( bot );
  ostream->writeInt( vehicle );

  ostream->writeBool( isExternal );
  ostream->writeBool( allowReincarnation );
  ostream->writeBool( nightVision );

  ostream->writeInt( state );

  strategicProxy.write( ostream );
  botProxy.write( ostream );
}

void Camera::init( int screenWidth, int screenHeight )
{
  width         = screenWidth;
  height        = screenHeight;
  centreX       = width  / 2;
  centreY       = height / 2;

  aspect        = config.getSet( "camera.aspect",     0.0f );
  mouseXSens    = config.getSet( "camera.mouseXSens", 1.0f ) * 0.004f;
  mouseYSens    = config.getSet( "camera.mouseYSens", 1.0f ) * 0.004f;
  keyXSens      = config.getSet( "camera.keysXSens",  1.0f ) * 0.04f;
  keyYSens      = config.getSet( "camera.keysYSens",  1.0f ) * 0.04f;

  float angle   = Math::rad( config.getSet( "camera.angle", 80.0f ) );

  aspect        = aspect != 0.0f ? aspect : float( width ) / float( height );
  coeff         = Math::tan( angle / 2.0f );

  isExternal         = false;
  allowReincarnation = true;
  nightVision        = false;

  state    = NONE;
  newState = NONE;

  reset();
}

}
}
