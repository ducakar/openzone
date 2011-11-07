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
 * @file matrix/VehicleClass.cpp
 */

#include "stable.hpp"

#include "matrix/VehicleClass.hpp"

#include "matrix/Library.hpp"
#include "matrix/Vehicle.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    state |= stateBit; \
  }

namespace oz
{
namespace matrix
{

ObjectClass* VehicleClass::createClass()
{
  return new VehicleClass();
}

void VehicleClass::initClass( const Config* config )
{
  flags = Object::DYNAMIC_BIT | Object::VEHICLE_BIT | Object::USE_FUNC_BIT |
      Object::UPDATE_FUNC_BIT | Object::CYLINDER_BIT;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",    true  );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",     false );
  OZ_CLASS_SET_FLAG( Object::HIT_FUNC_BIT,       "flag.onHit",        false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

  fillCommonConfig( config );

  mass = config->get( "mass", 100.0f );
  lift = config->get( "lift", 13.0f );

  if( mass < 0.01f ) {
    throw Exception( "%s: Invalid object mass. Should be >= 0.01.", name.cstr() );
  }
  if( lift < 0.0f ) {
    throw Exception( "%s: Invalid object lift. Should be >= 0.", name.cstr() );
  }

  const char* sType = config->get( "type", "" );
  if( String::equals( sType, "STATIC" ) ) {
    type = Vehicle::WHEELED;
  }
  else if( String::equals( sType, "WHEELED" ) ) {
    type = Vehicle::WHEELED;
  }
  else if( String::equals( sType, "TRACKED" ) ) {
    type = Vehicle::TRACKED;
  }
  else if( String::equals( sType, "MECH" ) ) {
    type = Vehicle::TRACKED;
  }
  else if( String::equals( sType, "HOVER" ) ) {
    type = Vehicle::HOVER;
  }
  else if( String::equals( sType, "AIR" ) ) {
    type = Vehicle::AIR;
  }
  else {
    throw Exception( "%s: Invalid vehicle type, should be either STATIC, WHEELED, TRACKED, MECH, "
                     "HOVER or AIR", name.cstr() );
  }

  state = 0;

  OZ_CLASS_SET_STATE( Vehicle::CREW_VISIBLE_BIT, "state.crewVisible", true );

  turnLimitH = config->get( "turnLimitH", 300.0f );
  turnLimitV = config->get( "turnLimitV", 300.0f );

  turnLimitH = Math::rad( turnLimitH ) * Timer::TICK_TIME;
  turnLimitV = Math::rad( turnLimitV ) * Timer::TICK_TIME;

  enginePitchBias  = config->get( "enginePitchBias", 1.0f );
  enginePitchRatio = config->get( "enginePitchRatio", 0.001f );
  enginePitchLimit = config->get( "enginePitchLimit", 2.00f );

  char weaponNameBuffer[]   = "weapon  .name";
  char onShotBuffer[]       = "weapon  .onShot";
  char nRoundsBuffer[]      = "weapon  .nRounds";
  char shotIntervalBuffer[] = "weapon  .shotInterval";

  nWeapons = MAX_WEAPONS;
  for( int i = 0; i < MAX_WEAPONS; ++i ) {
    hard_assert( i < 100 );

    weaponNameBuffer[6] = char( '0' + ( i / 10 ) );
    weaponNameBuffer[7] = char( '0' + ( i % 10 ) );

    onShotBuffer[6] = char( '0' + ( i / 10 ) );
    onShotBuffer[7] = char( '0' + ( i % 10 ) );

    nRoundsBuffer[6] = char( '0' + ( i / 10 ) );
    nRoundsBuffer[7] = char( '0' + ( i % 10 ) );

    shotIntervalBuffer[6] = char( '0' + ( i / 10 ) );
    shotIntervalBuffer[7] = char( '0' + ( i % 10 ) );

    weaponNames[i]  = gettext( config->get( weaponNameBuffer, "" ) );
    onShot[i]       = config->get( onShotBuffer, "" );
    nRounds[i]      = config->get( nRoundsBuffer, -1 );
    shotInterval[i] = config->get( shotIntervalBuffer, 0.5f );

    if( weaponNames[i].isEmpty() && nWeapons > i ) {
      nWeapons = i;
    }
    if( !String::isEmpty( onShot[i] ) ) {
      flags |= Object::LUA_BIT;
    }
  }

  pilotPos = Vec3( config->get( "pilot.pos.x", 0.0f ),
                   config->get( "pilot.pos.y", 0.0f ),
                   config->get( "pilot.pos.z", 0.0f ) );
  pilotRot = Quat::rotZYX( config->get( "pilot.rot.z", 0.0f ),
                           0.0f,
                           config->get( "pilot.rot.x", 0.0f ) );

  moveMomentum           = config->get( "moveMomentum", 2.0f );

  hoverHeight            = config->get( "hoverHeight", 2.0f );
  hoverHeightStiffness   = config->get( "hoverHeightStiffness", 40.0f );
  hoverMomentumStiffness = config->get( "hoverMomentumStiffness", 160.0f );
}

Object* VehicleClass::create( int index, const Point3& pos, Heading heading ) const
{
  return new Vehicle( this, index, pos, heading );
}

Object* VehicleClass::create( InputStream* istream ) const
{
  return new Vehicle( this, istream );
}

}
}
