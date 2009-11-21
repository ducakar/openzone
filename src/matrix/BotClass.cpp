/*
 *  BotClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "BotClass.h"

#include "Bot.h"
#include "Translator.h"

namespace oz
{

  ObjectClass *BotClass::init( const String &name, const Config *config )
  {
    BotClass *clazz = new BotClass();

    clazz->name                 = name;
    clazz->description          = config->get( "description", "A Bot" );

    clazz->dim.x                = config->get( "dim.x", 0.44f );
    clazz->dim.y                = config->get( "dim.y", 0.44f );
    clazz->dim.z                = config->get( "dim.z", 0.99f );

    if( clazz->dim.x < 0.0f || clazz->dim.x > AABB::REAL_MAX_DIM ||
        clazz->dim.y < 0.0f || clazz->dim.y > AABB::REAL_MAX_DIM ||
        clazz->dim.z < 0.0f )
    {
      throw Exception( "Invalid object dimensions. Should be >= 0 and <= 3.99." );
    }

    clazz->flags = 0;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT, "flag.destroyFunc", true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,  "flag.damageFunc",  false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,     "flag.useFunc",     false );
    OZ_CLASS_SET_FLAG( Object::ITEM_BIT,         "flag.item",        false );
    OZ_CLASS_SET_FLAG( Object::CLIP_BIT,         "flag.clip",        true  );
    OZ_CLASS_SET_FLAG( Object::CLIMBER_BIT,      "flag.climber",     true  );
    OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,       "flag.pusher",      true  );
    OZ_CLASS_SET_FLAG( Object::HOVER_BIT,        "flag.hover",       false );
    OZ_CLASS_SET_FLAG( Object::BLEND_BIT,        "flag.blend",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,    "flag.wideCull",    false );

    clazz->life                 = config->get( "life", 100.0f );
    clazz->damageTreshold       = config->get( "damageTreshold", 100.0f );
    clazz->damageRatio          = config->get( "damageRatio", 1.0f );

    if( clazz->life <= 0.0f ) {
      throw Exception( "Invalid object life. Should be > 0." );
    }
    if( clazz->damageTreshold < 0.0f ) {
      throw Exception( "Invalid object damageTreshold. Should be >= 0." );
    }
    if( clazz->damageRatio < 0.0f ) {
      throw Exception( "Invalid object damageRatio. Should be >= 0." );
    }

    clazz->nDebris              = config->get( "nDebris", 8 );
    clazz->debrisVelocitySpread = config->get( "debrisVelocitySpread", 4.0f );
    clazz->debrisRejection      = config->get( "debrisRejection", 1.90f );
    clazz->debrisMass           = config->get( "debrisMass", 0.0f );
    clazz->debrisLifeTime       = config->get( "debrisLifeTime", 2.5f );
    clazz->debrisColor.x        = config->get( "debrisColor.r", 1.0f );
    clazz->debrisColor.y        = config->get( "debrisColor.g", 0.0f );
    clazz->debrisColor.z        = config->get( "debrisColor.b", 0.0f );
    clazz->debrisColorSpread    = config->get( "debrisColorSpread", 0.2f );

    clazz->mass                 = config->get( "mass", 100.0f );
    clazz->lift                 = config->get( "lift", 12.0f );

    if( clazz->mass < 0.1f ) {
      throw Exception( "Invalid object mass. Should be >= 0.1." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    clazz->dimCrouch.x          = clazz->dim.x;
    clazz->dimCrouch.y          = clazz->dim.y;
    clazz->dimCrouch.z          = config->get( "dimCrouch.z", 0.79f );

    if( clazz->dimCrouch.z < 0.0f ) {
      throw Exception( "Invalid bot crouch dimensions. Should be >= 0." );
    }

    clazz->camPos.x             = config->get( "camPos.x", 0.00f );
    clazz->camPos.y             = config->get( "camPos.y", 0.00f );
    clazz->camPos.z             = config->get( "camPos.z", 0.89f );

    clazz->camPosCrouch.x       = config->get( "camPosCrouch.x", 0.00f );
    clazz->camPosCrouch.y       = config->get( "camPosCrouch.y", 0.00f );
    clazz->camPosCrouch.z       = config->get( "camPosCrouch.z", 0.69f );

    clazz->bobInc               = config->get( "bobInc", 0.05f );
    clazz->bobAmplitude         = config->get( "bobAmplitude", 0.05f );

    clazz->walkMomentum         = config->get( "walkMomentum", 1.2f );
    clazz->runMomentum          = config->get( "runMomentum", 4.0f );
    clazz->crouchMomentum       = config->get( "crouchMomentum", 1.0f );
    clazz->jumpMomentum         = config->get( "jumpMomentum", 4.0f );

    clazz->stepInc              = config->get( "stepInc", 0.10f );
    clazz->stepMax              = config->get( "stepMax", 0.50f );
    clazz->stepRate             = config->get( "stepRate", 0.60f );
    clazz->stepRateSupp         = config->get( "stepRateSupp", 0.95f );

    clazz->airControl           = config->get( "airControl", 0.025f );
    clazz->climbControl         = config->get( "climbControl", 1.50f );
    clazz->waterControl         = config->get( "waterControl", 0.05f );

    clazz->grabDistance         = config->get( "grabDistance", 1.5f );
    clazz->grabMass             = config->get( "grabMass", 50.0f );
    clazz->throwMomentum        = config->get( "throwMomentum", 6.0f );

    clazz->stamina              = config->get( "stamina", 100.0f );
    clazz->staminaGain          = config->get( "staminaGain", 0.05f );
    clazz->staminaWaterDrain    = config->get( "staminaWaterDrain", 0.12f );
    clazz->staminaRunDrain      = config->get( "staminaRunDrain", 0.08f );
    clazz->staminaJumpDrain     = config->get( "staminaJumpDrain", 4.0f );

    clazz->state                = config->get( "state", Bot::STEPPING_BIT );

    clazz->mindType             = config->get( "mindType", "" );
    clazz->mindFunction         = config->get( "mindFunction", "" );

    fillCommon( clazz, config );
    clazz->flags |= BASE_FLAGS;

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
    obj->oldState = state;
    obj->stamina  = stamina;

    return obj;
  }

  Object *BotClass::create( InputStream *istream )
  {
    Bot *obj = new Bot();

    obj->type   = this;

    obj->mass   = mass;
    obj->lift   = lift;

    obj->readFull( istream );

    obj->bob    = 0.0f;
    obj->camPos = ( obj->state & Bot::CROUCHING_BIT ) ? camPosCrouch : camPos;

    return obj;
  }

}
