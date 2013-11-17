/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <matrix/BotClass.hh>

#include <matrix/Liber.hh>
#include <matrix/Bot.hh>

namespace oz
{

ObjectClass* BotClass::createClass()
{
  return new BotClass();
}

void BotClass::init( InputStream* is, const char* name )
{
  DynamicClass::init( is, name );

  state             = is->readInt();

  crouchDim         = is->readVec3();
  corpseDim         = is->readVec3();

  camZ              = is->readFloat();
  crouchCamZ        = is->readFloat();

  walkMomentum      = is->readFloat();
  runMomentum       = is->readFloat();
  jumpMomentum      = is->readFloat();

  airControl        = is->readFloat();
  ladderControl     = is->readFloat();
  waterControl      = is->readFloat();
  slickControl      = is->readFloat();

  stepWalkInc       = is->readFloat();
  stepRunInc        = is->readFloat();

  stairInc          = is->readFloat();
  stairMax          = is->readFloat();
  stairRateLimit    = is->readFloat();
  stairRateSupp     = is->readFloat();

  climbInc          = is->readFloat();
  climbMax          = is->readFloat();
  climbMomentum     = is->readFloat();

  stamina           = is->readFloat();
  staminaGain       = is->readFloat();
  staminaRunDrain   = is->readFloat();
  staminaWaterDrain = is->readFloat();
  staminaClimbDrain = is->readFloat();
  staminaJumpDrain  = is->readFloat();
  staminaThrowDrain = is->readFloat();

  regeneration      = is->readFloat();

  reachDist         = is->readFloat();
  grabWeight        = is->readFloat();
  throwMomentum     = is->readFloat();

  weaponItem        = is->readInt();
  meleeInterval     = is->readFloat();
  onMelee           = is->readString();

  const char* sNameList = is->readString();
  nameList = String::isEmpty( sNameList ) ? -1 : liber.nameListIndex( sNameList );

  mindFunc          = is->readString();

  bobRotation       = is->readFloat();
  bobAmplitude      = is->readFloat();
  bobSwimAmplitude  = is->readFloat();

  baseColour        = is->readMat44();
  nvColour          = is->readMat44();
  injuryColour      = is->readMat44();
}

Object* BotClass::create( int index, const Point& pos, Heading heading ) const
{
  return new Bot( this, index, pos, heading );
}

Object* BotClass::create( InputStream* is ) const
{
  return new Bot( this, is );
}

Object* BotClass::create( const JSON& json ) const
{
  return new Bot( this, json );
}

}
