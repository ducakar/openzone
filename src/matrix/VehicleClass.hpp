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
 * @file matrix/VehicleClass.hpp
 */

#pragma once

#include "matrix/DynamicClass.hpp"
#include "matrix/Vehicle.hpp"

namespace oz
{
namespace matrix
{

class VehicleClass : public DynamicClass
{
  public:

    int    type;
    int    state;

    float  turnLimitH;
    float  turnLimitV;

    float  enginePitchBias;
    float  enginePitchRatio;
    float  enginePitchLimit;

    int    nWeapons;
    String weaponNames[Vehicle::WEAPONS_MAX];
    String onShot[Vehicle::WEAPONS_MAX];
    int    nRounds[Vehicle::WEAPONS_MAX];
    float  shotInterval[Vehicle::WEAPONS_MAX];

    Vec3   pilotPos;
    Quat   pilotRot;

    float  moveMomentum;

    float  hoverHeight;
    float  hoverHeightStiffness;
    float  hoverMomentumStiffness;

    static ObjectClass* createClass();

    virtual void initClass( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( int index, InputStream* istream ) const;

};

}
}
