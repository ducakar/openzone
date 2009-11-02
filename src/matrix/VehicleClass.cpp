/*
 *  VehicleClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "VehicleClass.h"

#include "Vehicle.h"
#include "Translator.h"

namespace oz
{

  void VehicleClass::fill( VehicleClass *clazz, Config *config )
  {
    DynObjectClass::fill( clazz, config );

    clazz->dimCrouch.x       = config->get( "dimCrouch.x", 0.39f );
    clazz->dimCrouch.y       = config->get( "dimCrouch.y", 0.39f );
    clazz->dimCrouch.z       = config->get( "dimCrouch.z", 0.39f );

    if( clazz->dimCrouch.x < 0.0f || clazz->dimCrouch.x > AABB::REAL_MAX_DIM ||
        clazz->dimCrouch.y < 0.0f || clazz->dimCrouch.y > AABB::REAL_MAX_DIM ||
        clazz->dimCrouch.z < 0.0f )
    {
      throw Exception( "Invalid object crouch dimensions. Should be >= 0 and <= 2.99." );
    }

    clazz->camPos.x          = config->get( "camPos.x", 0.0f );
    clazz->camPos.y          = config->get( "camPos.y", 0.0f );
    clazz->camPos.z          = config->get( "camPos.z", 0.0f );

    clazz->camPosCrouch.x    = config->get( "camPosCrouch.x", 0.0f );
    clazz->camPosCrouch.y    = config->get( "camPosCrouch.y", 0.0f );
    clazz->camPosCrouch.z    = config->get( "camPosCrouch.z", 0.0f );

    clazz->bobInc            = config->get( "bobInc", 0.05f );
    clazz->bobAmplitude      = config->get( "bobAmplitude", 0.05f );

    clazz->walkMomentum      = config->get( "walkMomentum", 1.0f );
    clazz->runMomentum       = config->get( "runMomentum", 4.0f );
    clazz->crouchMomentum    = config->get( "crouchMomentum", 1.0f );
    clazz->jumpMomentum      = config->get( "jumpMomentum", 4.0f );

    clazz->stepInc           = config->get( "stepInc", 0.10f );
    clazz->stepMax           = config->get( "stepMax", 0.50f );
    clazz->stepRate          = config->get( "stepRate", 0.60f );
    clazz->stepRateSupp      = config->get( "stepRateSupp", 0.95f );

    clazz->airControl        = config->get( "airControl", 0.025f );
    clazz->climbControl      = config->get( "climbControl", 1.50f );
    clazz->waterControl      = config->get( "waterControl", 0.05f );

    clazz->grabDistance      = config->get( "grabDistance", 1.2f );
    clazz->grabMass          = config->get( "grabMass", 30.0f );
    clazz->throwMomentum     = config->get( "throwMomentum", 4.0f );

    clazz->stamina           = config->get( "stamina", 100.0f );
    clazz->staminaGain       = config->get( "staminaGain", 0.05f );
    clazz->staminaWaterDrain = config->get( "staminaWaterDrain", 0.15f );
    clazz->staminaRunDrain   = config->get( "staminaRunDrain", 0.08f );
    clazz->staminaJumpDrain  = config->get( "staminaJumpDrain", 5.0f );

    clazz->state             = config->get( "state", Bot::STEPPING_BIT );

    clazz->lookLimitHMin     = config->get( "lookLimitHMin", -90.0f );
    clazz->lookLimitHMax     = config->get( "lookLimitHMax", +90.0f );
    clazz->lookLimitVMin     = config->get( "lookLimitVMin", -30.0f );
    clazz->lookLimitVMax     = config->get( "lookLimitVMax", +30.0f );

    clazz->mindType          = config->get( "mindType", "" );
  }

  ObjectClass *VehicleClass::init( const String &name, Config *config )
  {
    VehicleClass *clazz = new VehicleClass();

    clazz->name  = name;
    clazz->flags = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    fill( clazz, config );

    return clazz;
  }

  Object *VehicleClass::create( const Vec3 &pos )
  {
    Vehicle *obj = new Vehicle();

    obj->p        = pos;
    obj->dim      = dim;

    obj->flags    = flags;
    obj->oldFlags = flags;
    obj->type     = this;
    obj->life     = life;

    obj->mass     = mass;
    obj->lift     = lift;

    obj->camPos   = camPos;
    obj->state    = state;
    obj->stamina  = stamina;

    return obj;
  }

  Object *VehicleClass::create( InputStream *istream )
  {
    Vehicle *obj = new Vehicle();

    obj->dim    = dim;
    obj->cell   = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    obj->bob    = 0.0f;
    obj->camPos = ( obj->state & Vehicle::CROUCHING_BIT ) ? camPosCrouch : camPos;

    return obj;
  }

}
