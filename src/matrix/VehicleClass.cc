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
 * @file matrix/VehicleClass.cc
 */

#include <matrix/VehicleClass.hh>

#include <common/Lingua.hh>
#include <matrix/Vehicle.hh>

namespace oz
{

ObjectClass* VehicleClass::createClass()
{
  return new VehicleClass();
}

void VehicleClass::init( InputStream* is, const char* name )
{
  DynamicClass::init( is, name );

  type        = Type( is->readInt() );
  state       = is->readInt();
  fuel        = is->readFloat();

  pilotPos    = is->readVec3();

  lookHMin    = is->readFloat();
  lookHMax    = is->readFloat();
  lookVMin    = is->readFloat();
  lookVMax    = is->readFloat();

  rotVelLimit = is->readFloat();

  nWeapons    = is->readInt();

  for( int i = 0; i < nWeapons; ++i ) {
    weaponTitles[i]        = lingua.get( is->readString() );
    onWeaponShot[i]        = is->readString();
    nWeaponRounds[i]       = is->readInt();
    weaponShotIntervals[i] = is->readFloat();
  }

  engine.consumption     = is->readFloat();
  engine.idleConsumption = is->readFloat();
  engine.pitchBias       = is->readFloat();
  engine.pitchRatio      = is->readFloat();
  engine.pitchLimit      = is->readFloat();

  switch( type ) {
    case TURRET: {
      break;
    }
    case WHEELED: {
      wheeled.moveMomentum = is->readFloat();
      break;
    }
    case TRACKED: {
      tracked.moveMomentum = is->readFloat();
      break;
    }
    case MECH: {
      mech.walkMomentum   = is->readFloat();
      mech.runMomentum    = is->readFloat();

      mech.stepWalkInc    = is->readFloat();
      mech.stepRunInc     = is->readFloat();

      mech.stairInc       = is->readFloat();
      mech.stairMax       = is->readFloat();
      mech.stairRateLimit = is->readFloat();
      mech.stairRateSupp  = is->readFloat();
      break;
    }
    case HOVER: {
      hover.moveMomentum      = is->readFloat();
      hover.height            = is->readFloat();
      hover.heightStiffness   = is->readFloat();
      hover.momentumStiffness = is->readFloat();
      break;
    }
    case AIR: {
      hover.moveMomentum = is->readFloat();
      break;
    }
    case SUB: {
      sub.moveMomentum = is->readFloat();
      break;
    }
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

Object* VehicleClass::create( const JSON& json ) const
{
  return new Vehicle( this, json );
}

}
