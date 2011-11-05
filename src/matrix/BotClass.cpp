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
 * @file matrix/BotClass.cpp
 */

#include "stable.hpp"

#include "matrix/BotClass.hpp"

#include "matrix/Bot.hpp"
#include "matrix/NamePool.hpp"
#include "matrix/Synapse.hpp"

#define OZ_CLASS_SET_STATE( stateBit, varName, defValue ) \
  if( config->get( varName, defValue ) ) { \
    state |= stateBit; \
  }

namespace oz
{
namespace matrix
{

ObjectClass* BotClass::createClass()
{
  return new BotClass();
}

void BotClass::initClass( const Config* config )
{
  flags = Object::DYNAMIC_BIT | Object::BOT_BIT | Object::HIT_FUNC_BIT |
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

  fillCommonConfig( config );

  // we don't allow browsing bots' inventory as long as they are alive
  flags &= ~Object::BROWSABLE_BIT;

  life *= 2.0f;

  mass = config->get( "mass", 100.0f );
  lift = config->get( "lift", 13.0f );

  if( mass < 0.01f ) {
    throw Exception( "Invalid object mass. Should be >= 0.01." );
  }
  if( lift < 0.0f ) {
    throw Exception( "Invalid object lift. Should be >= 0." );
  }

  crouchDim.x = dim.x;
  crouchDim.y = dim.y;
  crouchDim.z = config->get( "crouchDim.z", 0.80f );

  if( crouchDim.z < 0.0f ) {
    throw Exception( "Invalid bot crouch dimensions. Should be >= 0." );
  }

  corpseDim.x = config->get( "corpseDim.x", 2.0f * dim.x );
  corpseDim.y = config->get( "corpseDim.y", 2.0f * dim.y );
  corpseDim.z = config->get( "corpseDim.z", 0.20f );

  if( corpseDim.z < 0.0f || corpseDim.y < 0.0f || corpseDim.z < 0.0f ) {
    throw Exception( "Invalid bot corpse dimensions. Should be >= 0." );
  }

  camZ              = config->get( "camZ", 0.79f );
  crouchCamZ        = config->get( "crouchCamZ", 0.69f );

  bobWalkInc        = Math::rad( config->get( "bobWalkInc", 300.0f ) ) * Timer::TICK_TIME;
  bobRunInc         = Math::rad( config->get( "bobRunInc", 600.0f ) ) * Timer::TICK_TIME;
  bobSwimInc        = Math::rad( config->get( "bobSwimInc", 100.0f ) ) * Timer::TICK_TIME;
  bobSwimRunInc     = Math::rad( config->get( "bobSwimRunInc", 200.0f ) ) * Timer::TICK_TIME;
  bobRotation       = Math::rad( config->get( "bobRotation", 0.35f ) );
  bobAmplitude      = config->get( "bobAmplitude", 0.02f );
  bobSwimAmplitude  = config->get( "bobSwimAmplitude", 0.05f );

  walkMomentum      = config->get( "walkMomentum", 0.7f );
  runMomentum       = config->get( "runMomentum", 1.6f );
  crouchMomentum    = config->get( "crouchMomentum", 0.7f );
  jumpMomentum      = config->get( "jumpMomentum", 5.0f );

  stepInc           = config->get( "stepInc", 0.25f );
  stepMax           = config->get( "stepMax", 0.50f );
  stepRateLimit     = config->get( "stepRateLimit", 0.00f );
  stepRateCoeff     = config->get( "stepRateCoeff", 500.0f );
  stepRateSupp      = config->get( "stepRatesupp", 0.50f );

  climbInc          = config->get( "climbInc", 0.25f );
  climbMax          = config->get( "climbMax", 2.0f );
  climbMomentum     = config->get( "climbMomentum", 2.0f );

  airControl        = config->get( "airControl", 0.025f );
  climbControl      = config->get( "climbControl", 1.50f );
  waterControl      = config->get( "waterControl", 0.08f );
  slickControl      = config->get( "slickControl", 0.04f );

  reachDist         = config->get( "reachDist", 2.0f );

  grabMass          = config->get( "grabMass", 50.0f );
  throwMomentum     = config->get( "throwMomentum", 6.0f );

  regeneration      = config->get( "regeneration", 0.0f ) * Timer::TICK_TIME;

  stamina           = config->get( "stamina", 100.0f );
  staminaGain       = config->get( "staminaGain", 2.5f ) * Timer::TICK_TIME;
  staminaRunDrain   = config->get( "staminaRunDrain", 4.0f ) * Timer::TICK_TIME;
  staminaWaterDrain = config->get( "staminaWaterDrain", 5.0f ) * Timer::TICK_TIME;
  staminaClimbDrain = config->get( "staminaClimbDrain", 10.0f ) * Timer::TICK_TIME;
  staminaJumpDrain  = config->get( "staminaJumpDrain", 5.0f );
  staminaThrowDrain = config->get( "staminaThrowDrain", 8.0f );

  state = 0;

  OZ_CLASS_SET_STATE( Bot::MECHANICAL_BIT, "state.mechanical", false );
  OZ_CLASS_SET_STATE( Bot::STEPABLE_BIT,   "state.stepable",   true );
  OZ_CLASS_SET_STATE( Bot::CROUCHING_BIT,  "state.crouching",  false );
  OZ_CLASS_SET_STATE( Bot::RUNNING_BIT,    "state.running",    true );

  weaponItem           = config->get( "weaponItem", -1 );

  mindFunction         = config->get( "mindFunction", "" );

  String sNameList     = config->get( "nameList", "" );
  nameList             = sNameList.isEmpty() ? -1 : library.nameListIndex( sNameList );
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

  obj->camZ = obj->state & Bot::CROUCHING_BIT ? crouchCamZ : camZ;

  return obj;
}

}
}
