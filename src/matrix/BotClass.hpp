/*
 *  BotClass.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/DynamicClass.hpp"

namespace oz
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

      static ObjectClass* init( const Config* config );

      virtual Object* create( int index, const Point3& pos, Heading heading ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
