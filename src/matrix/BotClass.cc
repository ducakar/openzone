/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file matrix/BotClass.cc
 */

#include "stable.hh"

#include "matrix/BotClass.hh"

#include "matrix/Bot.hh"

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
      Object::UPDATE_FUNC_BIT | Object::CYLINDER_BIT | Object::CLIMBER_BIT | Object::PUSHER_BIT;

  OZ_CLASS_SET_FLAG( Object::DESTROY_FUNC_BIT,   "flag.onDestroy",    true  );
  OZ_CLASS_SET_FLAG( Object::USE_FUNC_BIT,       "flag.onUse",        false );
  OZ_CLASS_SET_FLAG( Object::DAMAGE_FUNC_BIT,    "flag.onDamage",     false );
  OZ_CLASS_SET_FLAG( Object::SOLID_BIT,          "flag.solid",        true  );
  OZ_CLASS_SET_FLAG( Object::NO_DRAW_BIT,        "flag.noDraw",       false );
  OZ_CLASS_SET_FLAG( Object::WIDE_CULL_BIT,      "flag.wideCull",     false );

  fillCommonConfig( config );

  if( audioType != -1 ) {
    const char* soundName;
    int         soundIndex;

    soundName  = config->get( "audioSound.splash", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_SPLASH] = soundIndex;

    soundName  = config->get( "audioSound.fricting", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Object::EVENT_FRICTING] = soundIndex;

    soundName  = config->get( "audioSound.hitHard", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Bot::EVENT_HIT_HARD] = soundIndex;

    soundName  = config->get( "audioSound.land", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Bot::EVENT_LAND] = soundIndex;

    soundName  = config->get( "audioSound.jump", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Bot::EVENT_JUMP] = soundIndex;

    soundName  = config->get( "audioSound.flip", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Bot::EVENT_FLIP] = soundIndex;

    soundName  = config->get( "audioSound.death", "" );
    soundIndex = String::isEmpty( soundName ) ? -1 : library.soundIndex( soundName );
    audioSounds[Bot::EVENT_DEATH] = soundIndex;
  }

  // we don't allow browsing bots' inventory as long as they are alive
  flags &= ~Object::BROWSABLE_BIT;

  life *= 2.0f;

  mass = config->get( "mass", 100.0f );
  lift = config->get( "lift", 13.0f );

  if( mass < 0.01f ) {
    throw Exception( "%s: Invalid object mass. Should be >= 0.01.", name.cstr() );
  }
  if( lift < 0.0f ) {
    throw Exception( "%s: Invalid object lift. Should be >= 0.", name.cstr() );
  }

  state = Bot::RUNNING_BIT;

  OZ_CLASS_SET_STATE( Bot::MECHANICAL_BIT, "state.mechanical", false );

  crouchDim.x = dim.x;
  crouchDim.y = dim.y;
  crouchDim.z = config->get( "crouchDim.z", 0.80f );

  if( crouchDim.z < 0.0f ) {
    throw Exception( "%s: Invalid bot crouch dimensions. Should be >= 0.", name.cstr() );
  }

  corpseDim.x = config->get( "corpseDim.x", 2.0f * dim.x );
  corpseDim.y = config->get( "corpseDim.y", 2.0f * dim.y );
  corpseDim.z = config->get( "corpseDim.z", 0.20f );

  if( corpseDim.x < 0.0f || corpseDim.y < 0.0f || corpseDim.z < 0.0f ) {
    throw Exception( "%s: Invalid bot corpse dimensions. Should be >= 0.", name.cstr() );
  }

  camZ              = config->get( "camZ", 0.79f );
  crouchCamZ        = config->get( "crouchCamZ", 0.69f );

  bobWalkInc        = Math::rad( config->get( "bobWalkInc", 300.0f ) ) * Timer::TICK_TIME;
  bobRunInc         = Math::rad( config->get( "bobRunInc", 600.0f ) ) * Timer::TICK_TIME;
  bobSwimInc        = Math::rad( config->get( "bobSwimInc", 150.0f ) ) * Timer::TICK_TIME;
  bobSwimRunInc     = Math::rad( config->get( "bobSwimRunInc", 300.0f ) ) * Timer::TICK_TIME;
  bobRotation       = Math::rad( config->get( "bobRotation", 0.35f ) );
  bobAmplitude      = config->get( "bobAmplitude", 0.02f );
  bobSwimAmplitude  = config->get( "bobSwimAmplitude", 0.05f );

  walkMomentum      = config->get( "walkMomentum", 0.7f );
  runMomentum       = config->get( "runMomentum", 1.6f );
  crouchMomentum    = config->get( "crouchMomentum", 0.7f );
  jumpMomentum      = config->get( "jumpMomentum", 5.0f );

  airControl        = config->get( "airControl", 0.025f );
  climbControl      = config->get( "climbControl", 1.50f );
  waterControl      = config->get( "waterControl", 0.08f );
  slickControl      = config->get( "slickControl", 0.08f );

  stepInc           = config->get( "stepInc", 0.25f );
  stepMax           = config->get( "stepMax", 0.50f );
  stepRateLimit     = config->get( "stepRateLimit", 0.00f );
  stepRateCoeff     = config->get( "stepRateCoeff", 500.0f );
  stepRateSupp      = config->get( "stepRateSupp", 0.50f );

  climbInc          = config->get( "climbInc", 0.25f );
  climbMax          = config->get( "climbMax", 2.0f );
  climbMomentum     = config->get( "climbMomentum", 2.0f );

  stamina           = config->get( "stamina", 100.0f );
  staminaGain       = config->get( "staminaGain", 2.5f ) * Timer::TICK_TIME;
  staminaRunDrain   = config->get( "staminaRunDrain", 4.0f ) * Timer::TICK_TIME;
  staminaWaterDrain = config->get( "staminaWaterDrain", 5.0f ) * Timer::TICK_TIME;
  staminaClimbDrain = config->get( "staminaClimbDrain", 10.0f ) * Timer::TICK_TIME;
  staminaJumpDrain  = config->get( "staminaJumpDrain", 5.0f );
  staminaThrowDrain = config->get( "staminaThrowDrain", 8.0f );

  regeneration      = config->get( "regeneration", 0.0f ) * Timer::TICK_TIME;

  reachDist         = config->get( "reachDist", 2.0f );
  grabMass          = config->get( "grabMass", 50.0f );
  throwMomentum     = config->get( "throwMomentum", 6.0f );

  weaponItem        = config->get( "weaponItem", -1 );

  const char* sNameList = config->get( "nameList", "" );
  nameList          = String::isEmpty( sNameList ) ? -1 : library.nameListIndex( sNameList );

  mindFunc          = config->get( "mindFunc", "" );
}

Object* BotClass::create( int index, const Point3& pos, Heading heading ) const
{
  return new Bot( this, index, pos, heading );
}

Object* BotClass::create( InputStream* istream ) const
{
  return new Bot( this, istream );
}

}
}
