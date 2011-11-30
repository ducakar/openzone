/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file matrix/VehicleClass.hpp
 */

#pragma once

#include "matrix/DynamicClass.hpp"

namespace oz
{
namespace matrix
{

class VehicleClass : public DynamicClass
{
  private:

    static const int MAX_WEAPONS = 4;

  public:

    int    state;
    int    type;

    Vec3   pilotPos;
    Quat   pilotRot;

    float  rotVelLimit;

    float  moveMomentum;

    float  hoverHeight;
    float  hoverHeightStiffness;
    float  hoverMomentumStiffness;

    float  enginePitchBias;
    float  enginePitchRatio;
    float  enginePitchLimit;

    float  fuel;
    float  fuelConsumption;

    int    nWeapons;
    String weaponNames[MAX_WEAPONS];
    String onShot[MAX_WEAPONS];
    int    nRounds[MAX_WEAPONS];
    float  shotInterval[MAX_WEAPONS];

    static ObjectClass* createClass();

    virtual void initClass( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( InputStream* istream ) const;

};

}
}
