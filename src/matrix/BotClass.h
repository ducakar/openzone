/*
 *  BotClass.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "DynamicClass.h"

namespace oz
{

  struct BotClass : DynamicClass
  {
    static const int INVENTORY_ITEMS = 16;
    static const int BASE_FLAGS = Object::DYNAMIC_BIT | Object::UPDATE_FUNC_BIT |
        Object::HIT_FUNC_BIT | Object::BOT_BIT;

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
    float  stepRate;
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

    SVector<String, INVENTORY_ITEMS> inventoryItems;
    int    weaponItem;

    String mindType;
    String mindFunction;

    static ObjectClass* init( const String& name, const Config* config );

    virtual Object* create( int index, const Vec3& pos );
    virtual Object* create( int index, InputStream* istream );
  };

}
