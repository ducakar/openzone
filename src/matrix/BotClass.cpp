/*
 *  BotClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "BotClass.h"

#include "Bot.h"
#include "Translator.h"

namespace oz
{

  ObjectClass *BotClass::init( const String &name, Config *config )
  {
    BotClass *clazz = new BotClass();

    clazz->name             = name;
    clazz->description      = config->get( "description", "" );

    clazz->dim.x            = config->get( "dim.x", 0.39f );
    clazz->dim.y            = config->get( "dim.y", 0.39f );
    clazz->dim.z            = config->get( "dim.z", 0.39f );

    clazz->flags            = config->get( "flags", DEFAULT_FLAGS ) | BASE_FLAGS;
    clazz->life             = config->get( "life", 100.0f );
    clazz->damageTreshold   = config->get( "damageTreshold", 100.0f );
    clazz->damageRatio      = config->get( "damageRatio", 1.0f );

    clazz->modelType        = config->get( "modelType", "MD2" );
    clazz->modelPath        = config->get( "modelPath", "mdl/goblin.md2" );

    if( clazz->modelType.length() > 0 ) {
      clazz->flags |= Object::MODEL_BIT;
    }

    clazz->audioType        = config->get( "audioType", "" );

    if( clazz->audioType.length() > 0 ) {
      clazz->flags |= Object::AUDIO_BIT;

      char buffer[] = "audioSample  ";
      for( int i = 0; i < AUDIO_SAMPLES; i++ ) {
        assert( 0 <= i && i < 100 );

        buffer[ sizeof( buffer ) - 3 ] = '0' + ( i / 10 );
        buffer[ sizeof( buffer ) - 2 ] = '0' + ( i % 10 );

        String sampleName = config->get( buffer, "" );
        clazz->audioSamples[i] = sampleName.length() > 0 ? translator.soundIndex( sampleName ) : -1;
      }
    }

    clazz->mass              = config->get( "mass", 100.0f );
    clazz->lift              = config->get( "lift", 0.03f );

    clazz->dimCrouch.x       = config->get( "dimCrouch.x", 0.39f );
    clazz->dimCrouch.y       = config->get( "dimCrouch.y", 0.39f );
    clazz->dimCrouch.z       = config->get( "dimCrouch.z", 0.39f );

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
    clazz->stepRate          = config->get( "stepRate", 0.20f );
    clazz->stepRateSupp      = config->get( "stepRateSupp", 0.90f );

    clazz->airControl        = config->get( "airControl", 0.025f );
    clazz->climbControl      = config->get( "climbControl", 1.50f );
    clazz->waterControl      = config->get( "waterControl", 0.05f );
    clazz->grabDistance      = config->get( "grabDistance", 1.2f );

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

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f || clazz->dim.z > AABB::REAL_MAX_DIM )
    {
      assert( false );
      throw Exception( 0, "Invalid object dimensions. Should be >= 0 and <= 2.99." );
    }
    if( clazz->dimCrouch.x < 0.0f || clazz->dimCrouch.x > AABB::REAL_MAX_DIM ||
        clazz->dimCrouch.y < 0.0f || clazz->dimCrouch.y > AABB::REAL_MAX_DIM ||
        clazz->dimCrouch.z < 0.0f )
    {
      assert( false );
      throw Exception( 0, "Invalid object crouch dimensions. Should be >= 0 and <= 2.99." );
    }
    if( clazz->life <= 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object life. Should be > 0." );
    }
    if( clazz->damageTreshold < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object damageTreshold. Should be >= 0." );
    }
    if( clazz->damageRatio < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object damageRatio. Should be >= 0." );
    }
    if( clazz->mass < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object mass. Should be >= 0." );
    }
    if( clazz->lift < 0.0f ) {
      assert( false );
      throw Exception( 0, "Invalid object lift. Should be >= 0." );
    }
    return clazz;
  }

  Object *BotClass::create( const Vec3 &pos )
  {
    Bot *obj = new Bot();

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

  Object *BotClass::create( InputStream *istream )
  {
    Bot *obj = new Bot();

    obj->sector = null;
    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    obj->bob    = 0.0f;
    obj->camPos = ( obj->state & Bot::CROUCHING_BIT ) ? camPosCrouch : camPos;

    return obj;
  }

}
