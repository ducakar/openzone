/*
 *  BotClass.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/BotClass.hpp"

#include "matrix/Bot.hpp"
#include "matrix/NamePool.hpp"
#include "matrix/Synapse.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    clazz->state |= stateBit; \
  }

namespace oz
{

  ObjectClass* BotClass::init( const Config* config )
  {
    BotClass* clazz = new BotClass();

    clazz->flags = Object::DYNAMIC_BIT | Object::BOT_BIT | Object::HIT_FUNC_BIT |
        Object::UPDATE_FUNC_BIT | Object::CYLINDER_BIT;

    OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",    true  );
    OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",     false );
    OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",        false );
    OZ_CLASS_SET_FLAG( Object::ITEM_BIT,           "flag.item",         false );
    OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
    OZ_CLASS_SET_FLAG( Object::CLIMBER_BIT,        "flag.climber",      true  );
    OZ_CLASS_SET_FLAG( Object::PUSHER_BIT,         "flag.pusher",       true  );
    OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
    OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

    clazz->fillCommonConfig( config );

    // we don't allow browsing bots' inventory as long as they are alive
    clazz->flags &= ~Object::BROWSABLE_BIT;

    clazz->life *= 2.0f;

    clazz->mass = config->get( "mass", 100.0f );
    clazz->lift = config->get( "lift", 13.0f );

    if( clazz->mass < 0.01f ) {
      throw Exception( "Invalid object mass. Should be >= 0.01." );
    }
    if( clazz->lift < 0.0f ) {
      throw Exception( "Invalid object lift. Should be >= 0." );
    }

    clazz->crouchDim.x = clazz->dim.x;
    clazz->crouchDim.y = clazz->dim.y;
    clazz->crouchDim.z = config->get( "crouchDim.z", 0.80f );

    if( clazz->crouchDim.z < 0.0f ) {
      throw Exception( "Invalid bot crouch dimensions. Should be >= 0." );
    }

    clazz->corpseDim.x = config->get( "corpseDim.x", 2.0f * clazz->dim.x );
    clazz->corpseDim.y = config->get( "corpseDim.y", 2.0f * clazz->dim.y );
    clazz->corpseDim.z = config->get( "corpseDim.z", 0.20f );

    if( clazz->corpseDim.z < 0.0f || clazz->corpseDim.y < 0.0f || clazz->corpseDim.z < 0.0f ) {
      throw Exception( "Invalid bot corpse dimensions. Should be >= 0." );
    }

    clazz->camZ              = config->get( "camZ", 0.79f );
    clazz->crouchCamZ        = config->get( "crouchCamZ", 0.69f );

    clazz->bobWalkInc        = Math::rad( config->get( "bobWalkInc", 8.00f ) );
    clazz->bobRunInc         = Math::rad( config->get( "bobRunInc", 16.00f ) );
    clazz->bobSwimInc        = Math::rad( config->get( "bobSwimInc", 2.00f ) );
    clazz->bobSwimRunInc     = Math::rad( config->get( "bobSwimRunInc", 4.00f ) );
    clazz->bobRotation       = Math::rad( config->get( "bobRotation", 0.25f ) );
    clazz->bobAmplitude      = config->get( "bobAmplitude", 0.02f );
    clazz->bobSwimAmplitude  = config->get( "bobSwimAmplitude", 0.05f );

    clazz->walkMomentum      = config->get( "walkMomentum", 1.5f );
    clazz->runMomentum       = config->get( "runMomentum", 3.5f );
    clazz->crouchMomentum    = config->get( "crouchMomentum", 1.2f );
    clazz->jumpMomentum      = config->get( "jumpMomentum", 5.0f );

    clazz->stepInc           = config->get( "stepInc", 0.25f );
    clazz->stepMax           = config->get( "stepMax", 0.50f );
    clazz->stepRateLimit     = config->get( "stepRateLimit", 0.00f );
    clazz->stepRateCoeff     = config->get( "stepRateCoeff", 500.0f );
    clazz->stepRateSupp      = config->get( "stepRatesupp", 0.50f );

    clazz->climbInc          = config->get( "climbInc", 0.25f );
    clazz->climbMax          = config->get( "climbMax", 2.0f );
    clazz->climbMomentum     = config->get( "climbMomentum", 2.0f );

    clazz->airControl        = config->get( "airControl", 0.025f );
    clazz->climbControl      = config->get( "climbControl", 1.50f );
    clazz->waterControl      = config->get( "waterControl", 0.05f );
    clazz->slickControl      = config->get( "slickControl", 0.04f );

    clazz->reachDist         = config->get( "reachDist", 2.0f );

    clazz->grabMass          = config->get( "grabMass", 50.0f );
    clazz->throwMomentum     = config->get( "throwMomentum", 6.0f );

    clazz->regeneration      = config->get( "regeneration", 0.0f ) * Timer::TICK_TIME;

    clazz->stamina           = config->get( "stamina", 100.0f );
    clazz->staminaGain       = config->get( "staminaGain", 2.5f ) * Timer::TICK_TIME;
    clazz->staminaRunDrain   = config->get( "staminaRunDrain", 4.0f ) * Timer::TICK_TIME;
    clazz->staminaWaterDrain = config->get( "staminaWaterDrain", 5.0f ) * Timer::TICK_TIME;
    clazz->staminaClimbDrain = config->get( "staminaClimbDrain", 10.0f ) * Timer::TICK_TIME;
    clazz->staminaJumpDrain  = config->get( "staminaJumpDrain", 5.0f );
    clazz->staminaThrowDrain = config->get( "staminaThrowDrain", 8.0f );

    clazz->state = 0;

    OZ_CLASS_SET_STATE( Bot::MECHANICAL_BIT, "state.mechanical", false );
    OZ_CLASS_SET_STATE( Bot::STEPABLE_BIT,   "state.stepable",   true );
    OZ_CLASS_SET_STATE( Bot::CROUCHING_BIT,  "state.crouching",  false );
    OZ_CLASS_SET_STATE( Bot::RUNNING_BIT,    "state.running",    true );

    clazz->weaponItem           = config->get( "weaponItem", -1 );

    clazz->mindFunction         = config->get( "mindFunction", "" );

    String sNameList            = config->get( "nameList", "" );
    clazz->nameList             = sNameList.isEmpty() ? -1 : library.nameListIndex( sNameList );

    return clazz;
  }

  Object* BotClass::create( int index, const Point3& pos, Heading heading ) const
  {
    Bot* obj = new Bot();

    hard_assert( obj->index == -1 && obj->cell == null && obj->parent == -1 );

    obj->p          = pos;
    obj->index      = index;
    obj->resistance = resistance;
    obj->mass       = mass;
    obj->lift       = lift;
    obj->h          = float( heading ) * Math::TAU / 4.0f;
    obj->v          = Math::TAU / 4.0f;
    obj->camZ       = camZ;
    obj->state      = state;
    obj->oldState   = state;
    obj->stamina    = stamina;
    obj->name       = namePool.genName( nameList );
    obj->mindFunc   = mindFunction;

    fillCommonFields( obj );

    for( int i = 0; i < obj->items.length(); ++i ) {
      if( weaponItem == i ) {
        const ObjectClass* itemClazz = orbis.objects[ obj->items[i] ]->clazz;
        const WeaponClass* weaponClazz = dynamic_cast<const WeaponClass*>( itemClazz );

        if( weaponClazz == null ) {
          throw Exception( "Default weapon of '" + name + "' is of non-weapon class" );
        }
        else if( !weaponClazz->allowedUsers.contains( this ) ) {
          throw Exception( "Default weapon of '" + name + "' is not allowed for this class" );
        }

        obj->weapon = obj->items[i];
        break;
      }
    }

    return obj;
  }

  Object* BotClass::create( int index, InputStream* istream ) const
  {
    Bot* obj = new Bot();

    obj->index      = index;
    obj->clazz      = this;
    obj->resistance = resistance;
    obj->mass       = mass;
    obj->lift       = lift;

    obj->readFull( istream );

    obj->camZ = ( obj->state & Bot::CROUCHING_BIT ) ? crouchCamZ : camZ;

    return obj;
  }

}
