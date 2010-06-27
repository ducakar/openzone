/*
 *  BotClass.hh
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "matrix/DynamicClass.hh"

namespace oz
{

  class BotClass : public DynamicClass
  {
    private:

      static const int INVENTORY_ITEMS = 16;
      static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::HIT_FUNC_BIT |
          Object::UPDATE_FUNC_BIT | Object::ACT_FUNC_BIT | Object::BOT_BIT;

    public:

      Vec3   dimCrouch;

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

      float  airControl;
      float  climbControl;
      float  waterControl;

      float  grabDistance;
      float  grabMass;
      float  throwMomentum;

      float  stamina;
      float  staminaGain;
      float  staminaWaterDrain;
      float  staminaRunDrain;
      float  staminaJumpDrain;

      int    state;

      Vector<String, INVENTORY_ITEMS> inventoryItems;
      int    weaponItem;

      String mindType;
      String mindFunction;

      static ObjectClass* init( const String& name, const Config* config );

      virtual Object* create( int index, const Vec3& pos ) const;
      virtual Object* create( int index, InputStream* istream ) const;

  };

}
