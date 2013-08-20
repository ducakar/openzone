/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <client/Camera.hh>

#include <common/Timer.hh>
#include <client/Input.hh>
#include <client/ui/Area.hh>
#include <client/ui/UI.hh>

namespace oz
{
namespace client
{

const float  Camera::ROT_LIMIT          = Math::TAU / 2.0f;
const float  Camera::MIN_DISTANCE       = 0.10f;
const float  Camera::SMOOTHING_COEF     = 0.35f;
const float  Camera::ROT_SMOOTHING_COEF = 0.50f;
const Mat44  Camera::NV_COLOUR          = Mat44( 0.25f, 2.00f, 0.25f, 0.00f,
                                                 0.25f, 2.00f, 0.25f, 0.00f,
                                                 0.25f, 2.00f, 0.25f, 0.00f,
                                                 0.00f, 0.00f, 0.00f, 1.00f );

Proxy* const Camera::PROXIES[] = {
  nullptr,
  &strategic,
  &unit,
  &cinematic
};

StrategicProxy Camera::strategic;
UnitProxy      Camera::unit;
CinematicProxy Camera::cinematic;

void Camera::shake( float intensity )
{
  shakedRot = Quat::rotationAxis( right, intensity );
}

void Camera::updateReferences()
{
  objectObj  = orbis.obj( object );
  object     = objectObj == nullptr ? -1 : object;

  entityObj  = orbis.ent( entity );
  entity     = entityObj == nullptr ? -1 : entity;

  botObj     = static_cast<Bot*>( orbis.obj( bot ) );
  bot        = botObj == nullptr ? -1 : bot;

  vehicleObj = botObj == nullptr ? nullptr : static_cast<Vehicle*>( orbis.obj( botObj->parent ) );
  vehicle    = vehicleObj == nullptr ? -1 : botObj->parent;

  for( int i = 0; i < switchableUnits.length(); ) {
    const Bot* unit = static_cast<const Bot*>( orbis.obj( switchableUnits[i] ) );

    if( unit == nullptr ) {
      switchableUnits.erase( i );
    }
    else {
      ++i;
    }
  }
}

void Camera::align()
{
  rot      = ~Quat::slerp( rot, desiredRot * shakedRot, ROT_SMOOTHING_COEF );
  mag      = Math::mix( mag, desiredMag, SMOOTHING_COEF );
  p        = Math::mix( p, desiredPos, SMOOTHING_COEF );
  velocity = ( p - oldPos ) / Timer::TICK_TIME;
  oldPos   = p;

  rotMat   = Mat44::rotation( rot );
  rotTMat  = ~rotMat;

  right    = +rotMat.x.vec3();
  up       = +rotMat.y.vec3();
  at       = -rotMat.z.vec3();
}

void Camera::prepare()
{
  updateReferences();

  ui::mouse.update();

  relH = clamp( input.lookX * mag, -ROT_LIMIT, +ROT_LIMIT );
  relV = clamp( input.lookY * mag, -ROT_LIMIT, +ROT_LIMIT );

  if( newState != state ) {
    if( proxy != nullptr ) {
      proxy->end();
    }

    proxy = PROXIES[newState];

    if( proxy != nullptr ) {
      proxy->begin();
    }
    else {
      bot       = -1;
      botObj    = nullptr;

      object    = -1;
      objectObj = nullptr;

      entity    = -1;
      entityObj = nullptr;
    }

    state = newState;
  }

  if( proxy != nullptr ) {
    proxy->prepare();
  }

  ui::ui.update();

  if( Window::width() != width || Window::height() != height ) {
    width   = Window::width();
    height  = Window::height();

    centreX = Window::width() / 2;
    centreY = Window::height() / 2;

    aspect  = isFixedAspect ? aspect : float( width ) / float( height );

    ui::ui.root->width  = camera.width;
    ui::ui.root->height = camera.height;
    ui::ui.root->reposition();
  }
}

void Camera::update()
{
  updateReferences();

  if( proxy != nullptr ) {
    proxy->update();
  }

  horizPlane = coeff * mag * MIN_DISTANCE;
  vertPlane  = aspect * horizPlane;
}

void Camera::reset()
{
  rot        = Quat::ID;
  mag        = 1.0f;
  p          = Point::ORIGIN;
  velocity   = Vec3::ZERO;

  desiredRot = Quat::ID;
  shakedRot  = Quat::ID;
  desiredMag = 1.0f;
  desiredPos = Point::ORIGIN;
  oldPos     = Point::ORIGIN;

  relH       = 0.0f;
  relV       = 0.0f;

  rotMat     = Mat44::rotation( rot );
  rotTMat    = ~rotTMat;

  colour     = Mat44::ID;
  baseColour = Mat44::ID;
  nvColour   = NV_COLOUR;

  right      = rotMat.x.vec3();
  up         = rotMat.y.vec3();
  at         = -rotMat.z.vec3();

  object     = -1;
  objectObj  = nullptr;
  entity     = -1;
  entityObj  = nullptr;
  bot        = -1;
  botObj     = nullptr;
  vehicle    = -1;
  vehicleObj = nullptr;

  switchableUnits.clear();
  switchableUnits.deallocate();

  allowReincarnation = true;
  nightVision        = false;
  isExternal         = true;

  state    = NONE;
  newState = NONE;

  strategic.reset();
  unit.reset();
  cinematic.reset();

  if( proxy != nullptr ) {
    proxy->end();
    proxy = nullptr;
  }
}

void Camera::read( InputStream* istream )
{
  rot        = istream->readQuat();
  mag        = istream->readFloat();
  p          = istream->readPoint();
  velocity   = Vec3::ZERO;

  desiredRot = rot;
  desiredMag = mag;
  desiredPos = p;
  oldPos     = p;

  relH       = istream->readFloat();
  relV       = istream->readFloat();

  rotMat     = Mat44::rotation( rot );
  rotTMat    = ~rotMat;

  colour     = istream->readMat44();
  baseColour = istream->readMat44();
  nvColour   = istream->readMat44();

  right      = rotMat.x.vec3();
  up         = rotMat.y.vec3();
  at         = -rotMat.z.vec3();

  object     = -1;
  objectObj  = nullptr;
  entity     = -1;
  entityObj  = nullptr;
  bot        = istream->readInt();
  botObj     = static_cast<Bot*>( orbis.obj( bot ) );
  vehicle    = istream->readInt();
  vehicleObj = static_cast<Vehicle*>( orbis.obj( vehicle ) );

  hard_assert( switchableUnits.isEmpty() );

  int nSwitchableUnits = istream->readInt();
  for( int i = 0; i < nSwitchableUnits; ++i ) {
    switchableUnits.add( istream->readInt() );
  }

  allowReincarnation = istream->readBool();
  nightVision        = istream->readBool();
  isExternal         = istream->readBool();

  state     = NONE;
  newState  = State( istream->readInt() );

  strategic.read( istream );
  unit.read( istream );
  cinematic.read( istream );

  proxy = PROXIES[newState];

  if( proxy != nullptr ) {
    proxy->begin();
  }

  state = newState;
}

void Camera::write( OutputStream* ostream ) const
{
  ostream->writeQuat( desiredRot );
  ostream->writeFloat( desiredMag );
  ostream->writePoint( desiredPos );

  ostream->writeFloat( relH );
  ostream->writeFloat( relV );

  ostream->writeMat44( colour );
  ostream->writeMat44( baseColour );
  ostream->writeMat44( nvColour );

  ostream->writeInt( bot );
  ostream->writeInt( vehicle );

  ostream->writeInt( switchableUnits.length() );
  for( int i = 0; i < switchableUnits.length(); ++i ) {
    ostream->writeInt( switchableUnits[i] );
  }

  ostream->writeBool( allowReincarnation );
  ostream->writeBool( nightVision );
  ostream->writeBool( isExternal );

  ostream->writeInt( state );

  strategic.write( ostream );
  unit.write( ostream );
  cinematic.write( ostream );
}

void Camera::init()
{
  width         = Window::width();
  height        = Window::height();
  centreX       = Window::width() / 2;
  centreY       = Window::height() / 2;

  float angle   = Math::rad( config.include( "camera.angle", 80.0f ).asFloat() );
  aspect        = config.include( "camera.aspect", 0.0f ).asFloat();
  isFixedAspect = aspect != 0.0f;
  aspect        = isFixedAspect ? aspect : float( width ) / float( height );
  coeff         = Math::tan( angle / 2.0f );

  state    = NONE;
  newState = NONE;

  reset();
}

Camera camera;

}
}
