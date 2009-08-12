/*
 *  VehicleClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "VehicleClass.h"

#include "Vehicle.h"
#include "Translator.h"

namespace oz
{

  ObjectClass *VehicleClass::init( const String &name, Config *config )
  {
    VehicleClass *clazz = new VehicleClass();

    clazz->name             = name;
    clazz->description      = config->get( "description", "" );

    clazz->dim.x            = config->get( "dim.x", 0.5f );
    clazz->dim.y            = config->get( "dim.y", 0.5f );
    clazz->dim.z            = config->get( "dim.z", 0.5f );

    clazz->flags            = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    clazz->life             = config->get( "life", 1.0f );
    clazz->damageTreshold   = config->get( "damage.treshold", 8.0f );
    clazz->damageRatio      = config->get( "damage.ratio", 1.0f );

    clazz->mass             = config->get( "mass", 100.0f );
    clazz->lift             = config->get( "lift", 0.10f );

    clazz->modelType        = config->get( "model.type", "MD2" );
    clazz->modelPath        = config->get( "model.path", "mdl/goblin.md2" );

    clazz->audioType        = config->get( "audio.type", "" );

    if( clazz->audioType.length() > 0 ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audio.sample  ";
      for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
        assert( 0 <= i && i < 100 );

        buffer[ sizeof( buffer ) - 3 ] = '0' + ( i / 10 );
        buffer[ sizeof( buffer ) - 2 ] = '0' + ( i % 10 );

        String sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName.length() > 0 ? translator.soundIndex( sampleName ) : -1;
      }
    }

    clazz->dimCrouch.x      = config->get( "dimCrouch.x", 0.5f );
    clazz->dimCrouch.y      = config->get( "dimCrouch.y", 0.5f );
    clazz->dimCrouch.z      = config->get( "dimCrouch.z", 0.5f );

    clazz->camPos.x         = config->get( "camPos.x", 0.0f );
    clazz->camPos.y         = config->get( "camPos.y", 0.0f );
    clazz->camPos.z         = config->get( "camPos.z", 0.0f );

    clazz->camPosCrouch.x   = config->get( "camPosCrouch.x", 0.0f );
    clazz->camPosCrouch.y   = config->get( "camPosCrouch.y", 0.0f );
    clazz->camPosCrouch.z   = config->get( "camPosCrouch.z", 0.0f );

    clazz->bobInc           = config->get( "bobInc", 0.0f );
    clazz->bobAmplitude     = config->get( "bobAmplitude", 0.0f );

    clazz->walkMomentum     = config->get( "walkMomentum", 1.0f );
    clazz->runMomentum      = config->get( "runMomentum", 4.0f );
    clazz->crouchMomentum   = config->get( "crouchMomentum", 1.0f );
    clazz->jumpMomentum     = config->get( "jumpMomentum", 4.0f );

    clazz->stepInc          = config->get( "stepInc", 1.0f );
    clazz->stepMax          = config->get( "stepMax", 1.0f );

    clazz->airControl       = config->get( "airControl", 0.05f );
    clazz->waterControl     = config->get( "waterControl", 0.15f );
    clazz->grabDistance     = config->get( "grabDistance", 1.0f );

    clazz->stamina          = config->get( "stamina", 100.0f );
    clazz->staminaRunDrain  = config->get( "staminaRunDrain", 0.1f );
    clazz->staminaJumpDrain = config->get( "staminaJumpDrain", 10.0f );

    clazz->state            = config->get( "state", Vehicle::STEPPING_BIT );

    clazz->lookLimitHMin    = config->get( "lookLimit.h.min", -90.0f );
    clazz->lookLimitHMax    = config->get( "lookLimit.h.max", +90.0f );
    clazz->lookLimitVMin    = config->get( "lookLimit.v.min", -45.0f );
    clazz->lookLimitVMax    = config->get( "lookLimit.v.max", +45.0f );

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
    obj->mind     = null;

    return obj;
  }

  Object *VehicleClass::create( InputStream *istream )
  {
    Vehicle *obj = new Vehicle();

    obj->dim    = dim;
    obj->sector = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    obj->bob    = 0.0f;
    obj->camPos = ( obj->state & Vehicle::CROUCHING_BIT ) ? camPosCrouch : camPos;

    return obj;
  }

}
