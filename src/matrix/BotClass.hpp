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
 * @file matrix/BotClass.hpp
 */

#pragma once

#include "matrix/DynamicClass.hpp"

namespace oz
{
namespace matrix
{

class BotClass : public DynamicClass
{
  public:

    Vec3   crouchDim;
    Vec3   corpseDim;

    float  camZ;
    float  crouchCamZ;

    float  bobWalkInc;
    float  bobRunInc;
    float  bobSwimInc;
    float  bobSwimRunInc;
    float  bobRotation;
    float  bobAmplitude;
    float  bobSwimAmplitude;

    float  walkMomentum;
    float  runMomentum;
    float  crouchMomentum;
    float  jumpMomentum;

    float  stepInc;
    float  stepMax;
    float  stepRateLimit;
    float  stepRateCoeff;
    float  stepRateSupp;

    float  climbInc;
    float  climbMax;
    float  climbMomentum;

    float  airControl;
    float  climbControl;
    float  waterControl;
    float  slickControl;

    float  reachDist;

    float  grabMass;
    float  throwMomentum;

    float  regeneration;

    float  stamina;
    float  staminaGain;
    float  staminaRunDrain;
    float  staminaWaterDrain;
    float  staminaClimbDrain;
    float  staminaJumpDrain;
    float  staminaThrowDrain;

    int    state;

    int    weaponItem;

    String mindFunction;

    int    nameList;

    static ObjectClass* createClass();

    virtual void initClass( const Config* config );

    virtual Object* create( int index, const Point3& pos, Heading heading ) const;
    virtual Object* create( int index, InputStream* istream ) const;

};

}
}
