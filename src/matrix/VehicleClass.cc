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
 * @file matrix/VehicleClass.cc
 */

#include "stable.hh"

#include "matrix/VehicleClass.hh"

#include "matrix/Vehicle.hh"

namespace oz
{
namespace matrix
{

ObjectClass* VehicleClass::createClass()
{
  return new VehicleClass();
}

void VehicleClass::init( InputStream* is, const char* name )
{
  DynamicClass::init( is, name );

  type                   = Type( is->readInt() );
  state                  = is->readInt();

  pilotPos               = is->readVec3();

  lookHMin               = is->readFloat();
  lookHMax               = is->readFloat();
  lookVMin               = is->readFloat();
  lookVMax               = is->readFloat();

  rotVelLimit            = is->readFloat();

  moveMomentum           = is->readFloat();

  hoverHeight            = is->readFloat();
  hoverHeightStiffness   = is->readFloat();
  hoverMomentumStiffness = is->readFloat();

  enginePitchBias        = is->readFloat();
  enginePitchRatio       = is->readFloat();
  enginePitchLimit       = is->readFloat();

  fuel                   = is->readFloat();
  fuelConsumption        = is->readFloat();

  nWeapons               = is->readInt();

  for( int i = 0; i < nWeapons; ++i ) {
    weaponTitles[i]        = is->readString();
    onWeaponShot[i]        = is->readString();
    nWeaponRounds[i]       = is->readInt();
    weaponShotIntervals[i] = is->readFloat();
  }
}

Object* VehicleClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Vehicle( this, index, pos, heading );
}

Object* VehicleClass::create( InputStream* istream ) const
{
  return new Vehicle( this, istream );
}

}
}
