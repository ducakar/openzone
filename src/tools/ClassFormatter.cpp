/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file tools/ClassFormatter.cpp
 */

#include "tools/ClassFormatter.hpp"

#include "matrix/Library.hpp"
#include "matrix/VehicleClass.hpp"
#include "matrix/Vehicle.hpp"

using namespace oz::matrix;

namespace oz
{

ClassFormatter classFormatter;

void ClassFormatter::writeObjectClass() const
{
  fprintf( fs, "\n" );
  fprintf( fs, "title                   \"%s\"\n", clazz->title.cstr() );
  fprintf( fs, "description             \"%s\"\n", clazz->description.cstr() );

  bool isFirstFlag = true;

  if( !clazz->onDestroy.isEmpty() && !( clazz->flags & Object::DESTROY_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDestroy          \"false\"\n" );
  }
  if( !clazz->onUse.isEmpty() && !( clazz->flags & Object::USE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUse              \"false\"\n" );
  }
  if( !clazz->onDamage.isEmpty() && !( clazz->flags & Object::DAMAGE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDamage           \"false\"\n" );
  }
  if( !clazz->onHit.isEmpty() && !( clazz->flags & Object::HIT_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onHit              \"false\"\n" );
  }
  if( !clazz->onUpdate.isEmpty() && !( clazz->flags & Object::UPDATE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUpdate           \"false\"\n" );
  }
  if( !( clazz->flags & Object::SOLID_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.solid              \"false\"\n" );
  }
  if( !( clazz->flags & Object::CYLINDER_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.cylinder           \"false\"\n" );
  }
  if( clazz->flags & Object::NO_DRAW_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.noDraw             \"true\"\n" );
  }
  if( clazz->flags & Object::WIDE_CULL_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.wideCull           \"true\"\n" );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "dim.x                   \"%g\"\n", clazz->dim.x );
  fprintf( fs, "dim.y                   \"%g\"\n", clazz->dim.y );
  fprintf( fs, "dim.z                   \"%g\"\n", clazz->dim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "life                    \"%g\"\n", clazz->life );
  fprintf( fs, "resistance              \"%g\"\n", clazz->resistance );

  bool isFirstHandler = true;

  if( !clazz->onDestroy.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDestroy               \"%s\"\n", clazz->onDestroy.cstr() );
  }
  if( !clazz->onUse.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUse                   \"%s\"\n", clazz->onUse.cstr() );
  }
  if( !clazz->onDamage.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDamage                \"%s\"\n", clazz->onDamage.cstr() );
  }
  if( !clazz->onHit.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onHit                   \"%s\"\n", clazz->onHit.cstr() );
  }
  if( !clazz->onUpdate.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUpdate                \"%s\"\n", clazz->onUpdate.cstr() );
  }
}

void ClassFormatter::writeDynamicClass() const
{
  const DynamicClass* clazz = static_cast<const DynamicClass*>( this->clazz );

  fprintf( fs, "\n" );
  fprintf( fs, "title                   \"%s\"\n", clazz->title.cstr() );
  fprintf( fs, "description             \"%s\"\n", clazz->description.cstr() );

  bool isFirstFlag = true;

  if( !clazz->onDestroy.isEmpty() && !( clazz->flags & Object::DESTROY_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDestroy          \"false\"\n" );
  }
  if( !clazz->onUse.isEmpty() && !( clazz->flags & Object::USE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUse              \"false\"\n" );
  }
  if( !clazz->onDamage.isEmpty() && !( clazz->flags & Object::DAMAGE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDamage           \"false\"\n" );
  }
  if( !clazz->onHit.isEmpty() && !( clazz->flags & Object::HIT_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onHit              \"false\"\n" );
  }
  if( !clazz->onUpdate.isEmpty() && !( clazz->flags & Object::UPDATE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUpdate           \"false\"\n" );
  }
  if( clazz->flags & Object::ITEM_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.item               \"true\"\n" );
  }
  if( !( clazz->flags & Object::SOLID_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.solid              \"false\"\n" );
  }
  if( !( clazz->flags & Object::CYLINDER_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.cylinder           \"false\"\n" );
  }
  if( clazz->flags & Object::NO_DRAW_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.noDraw             \"true\"\n" );
  }
  if( clazz->flags & Object::WIDE_CULL_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.wideCull           \"true\"\n" );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "dim.x                   \"%g\"\n", clazz->dim.x );
  fprintf( fs, "dim.y                   \"%g\"\n", clazz->dim.y );
  fprintf( fs, "dim.z                   \"%g\"\n", clazz->dim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "life                    \"%g\"\n", clazz->life );
  fprintf( fs, "resistance              \"%g\"\n", clazz->resistance );
  fprintf( fs, "\n" );
  fprintf( fs, "mass                    \"%g\"\n", clazz->mass );
  fprintf( fs, "lift                    \"%g\"\n", clazz->lift );

  bool isFirstHandler = true;

  if( !clazz->onDestroy.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDestroy               \"%s\"\n", clazz->onDestroy.cstr() );
  }
  if( !clazz->onUse.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUse                   \"%s\"\n", clazz->onUse.cstr() );
  }
  if( !clazz->onDamage.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDamage                \"%s\"\n", clazz->onDamage.cstr() );
  }
  if( !clazz->onHit.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onHit                   \"%s\"\n", clazz->onHit.cstr() );
  }
  if( !clazz->onUpdate.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUpdate                \"%s\"\n", clazz->onUpdate.cstr() );
  }
}

void ClassFormatter::writeWeaponClass() const
{
  const WeaponClass* clazz = static_cast<const WeaponClass*>( this->clazz );

  fprintf( fs, "\n" );
  fprintf( fs, "title                   \"%s\"\n", clazz->title.cstr() );
  fprintf( fs, "description             \"%s\"\n", clazz->description.cstr() );

  bool isFirstFlag = true;

  if( !clazz->onDestroy.isEmpty() && !( clazz->flags & Object::DESTROY_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDestroy          \"false\"\n" );
  }
  if( !clazz->onDamage.isEmpty() && !( clazz->flags & Object::DAMAGE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDamage           \"false\"\n" );
  }
  if( !clazz->onHit.isEmpty() && !( clazz->flags & Object::HIT_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onHit              \"false\"\n" );
  }
  if( !clazz->onUpdate.isEmpty() && !( clazz->flags & Object::UPDATE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUpdate           \"false\"\n" );
  }
  if( !( clazz->flags & Object::SOLID_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.solid              \"false\"\n" );
  }
  if( !( clazz->flags & Object::CYLINDER_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.cylinder           \"false\"\n" );
  }
  if( clazz->flags & Object::NO_DRAW_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.noDraw             \"true\"\n" );
  }
  if( clazz->flags & Object::WIDE_CULL_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.wideCull           \"true\"\n" );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "dim.x                   \"%g\"\n", clazz->dim.x );
  fprintf( fs, "dim.y                   \"%g\"\n", clazz->dim.y );
  fprintf( fs, "dim.z                   \"%g\"\n", clazz->dim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "life                    \"%g\"\n", clazz->life );
  fprintf( fs, "resistance              \"%g\"\n", clazz->resistance );
  fprintf( fs, "\n" );
  fprintf( fs, "mass                    \"%g\"\n", clazz->mass );
  fprintf( fs, "lift                    \"%g\"\n", clazz->lift );
  fprintf( fs, "\n" );
  fprintf( fs, "nRounds                 \"%d\"\n", clazz->nRounds );
  fprintf( fs, "shotInterval            \"%g\"\n", clazz->shotInterval );

  bool isFirstHandler = true;

  if( !clazz->onDestroy.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDestroy               \"%s\"\n", clazz->onDestroy.cstr() );
  }
  if( !clazz->onDamage.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDamage                \"%s\"\n", clazz->onDamage.cstr() );
  }
  if( !clazz->onHit.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onHit                   \"%s\"\n", clazz->onHit.cstr() );
  }
  if( !clazz->onUpdate.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUpdate                \"%s\"\n", clazz->onUpdate.cstr() );
  }
  if( !clazz->onShot.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onShot                  \"%s\"\n", clazz->onShot.cstr() );
  }
}

void ClassFormatter::writeBotClass() const
{
  const BotClass* clazz = static_cast<const BotClass*>( this->clazz );

  fprintf( fs, "\n" );
  fprintf( fs, "title                   \"%s\"\n", clazz->title.cstr() );
  fprintf( fs, "description             \"%s\"\n", clazz->description.cstr() );

  bool isFirstFlag = true;

  if( !clazz->onDestroy.isEmpty() && !( clazz->flags & Object::DESTROY_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDestroy          \"false\"\n" );
  }
  if( !clazz->onUse.isEmpty() && !( clazz->flags & Object::USE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onUse              \"false\"\n" );
  }
  if( !clazz->onDamage.isEmpty() && !( clazz->flags & Object::DAMAGE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDamage           \"false\"\n" );
  }
  if( !( clazz->flags & Object::SOLID_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.solid              \"false\"\n" );
  }
  if( clazz->flags & Object::NO_DRAW_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.noDraw             \"true\"\n" );
  }
  if( clazz->flags & Object::WIDE_CULL_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.wideCull           \"true\"\n" );
  }

  bool isFirstState = true;

  if( clazz->state & Bot::MECHANICAL_BIT ) {
    if( isFirstState ) {
      isFirstState = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "state.mechanical        \"true\"\n" );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "dim.x                   \"%g\"\n", clazz->dim.x );
  fprintf( fs, "dim.y                   \"%g\"\n", clazz->dim.y );
  fprintf( fs, "dim.z                   \"%g\"\n", clazz->dim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "crouchDim.z             \"%g\"\n", clazz->crouchDim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "corpseDim.x             \"%g\"\n", clazz->corpseDim.x );
  fprintf( fs, "corpseDim.y             \"%g\"\n", clazz->corpseDim.y );
  fprintf( fs, "corpseDim.z             \"%g\"\n", clazz->corpseDim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "life                    \"%g\"\n", clazz->life );
  fprintf( fs, "resistance              \"%g\"\n", clazz->resistance );
  fprintf( fs, "\n" );
  fprintf( fs, "mass                    \"%g\"\n", clazz->mass );
  fprintf( fs, "lift                    \"%g\"\n", clazz->lift );
  fprintf( fs, "\n" );
  fprintf( fs, "camZ                    \"%g\"\n", clazz->camZ );
  fprintf( fs, "crouchCamZ              \"%g\"\n", clazz->crouchCamZ );
  fprintf( fs, "\n" );
  fprintf( fs, "bobWalkInc              \"%.4g\"\n", Math::deg( clazz->bobWalkInc / Timer::TICK_TIME ) );
  fprintf( fs, "bobRunInc               \"%.4g\"\n", Math::deg( clazz->bobRunInc / Timer::TICK_TIME ) );
  fprintf( fs, "bobSwimInc              \"%.4g\"\n", Math::deg( clazz->bobSwimInc / Timer::TICK_TIME ) );
  fprintf( fs, "bobSwimRunInc           \"%.4g\"\n", Math::deg( clazz->bobSwimRunInc / Timer::TICK_TIME ) );
  fprintf( fs, "bobRotation             \"%.4g\"\n", Math::deg( clazz->bobRotation ) );
  fprintf( fs, "bobAmplitude            \"%g\"\n", clazz->bobAmplitude );
  fprintf( fs, "bobSwimAmplitude        \"%g\"\n", clazz->bobSwimAmplitude );
  fprintf( fs, "\n" );
  fprintf( fs, "walkMomentum            \"%g\"\n", clazz->walkMomentum );
  fprintf( fs, "runMomentum             \"%g\"\n", clazz->runMomentum );
  fprintf( fs, "crouchMomentum          \"%g\"\n", clazz->crouchMomentum );
  fprintf( fs, "jumpMomentum            \"%g\"\n", clazz->jumpMomentum );
  fprintf( fs, "\n" );
  fprintf( fs, "stepInc                 \"%g\"\n", clazz->stepInc );
  fprintf( fs, "stepMax                 \"%g\"\n", clazz->stepMax );
  fprintf( fs, "stepRateLimit           \"%g\"\n", clazz->stepRateLimit );
  fprintf( fs, "stepRateCoeff           \"%g\"\n", clazz->stepRateCoeff );
  fprintf( fs, "stepRateSupp            \"%g\"\n", clazz->stepRateSupp );
  fprintf( fs, "\n" );
  fprintf( fs, "climbInc                \"%g\"\n", clazz->climbInc );
  fprintf( fs, "climbMax                \"%g\"\n", clazz->climbMax );
  fprintf( fs, "climbMomentum           \"%g\"\n", clazz->climbMomentum );
  fprintf( fs, "\n" );
  fprintf( fs, "stamina                 \"%g\"\n", clazz->stamina );
  fprintf( fs, "staminaGain             \"%.4g\"\n", clazz->staminaGain / Timer::TICK_TIME );
  fprintf( fs, "staminaRunDrain         \"%.4g\"\n", clazz->staminaRunDrain / Timer::TICK_TIME );
  fprintf( fs, "staminaWaterDrain       \"%.4g\"\n", clazz->staminaWaterDrain / Timer::TICK_TIME );
  fprintf( fs, "staminaClimbDrain       \"%.4g\"\n", clazz->staminaClimbDrain / Timer::TICK_TIME );
  fprintf( fs, "staminaJumpDrain        \"%g\"\n", clazz->staminaJumpDrain );
  fprintf( fs, "staminaThrowDrain       \"%g\"\n", clazz->staminaThrowDrain );
  fprintf( fs, "\n" );
  fprintf( fs, "regeneration            \"%.4g\"\n", clazz->regeneration / Timer::TICK_TIME );
  fprintf( fs, "\n" );
  fprintf( fs, "reachDist               \"%g\"\n", clazz->reachDist );
  fprintf( fs, "grabMass                \"%g\"\n", clazz->grabMass );
  fprintf( fs, "throwMomentum           \"%g\"\n", clazz->throwMomentum );
  fprintf( fs, "\n" );
  fprintf( fs, "weaponItem              \"%d\"\n", clazz->weaponItem );

  if( clazz->nameList != -1 ) {
    fprintf( fs, "\n" );
    fprintf( fs, "nameList                \"%s\"\n", library.nameLists[clazz->nameList].name.cstr() );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "mindFunc                \"%s\"\n", clazz->mindFunc.cstr() );

  bool isFirstHandler = true;

  if( !clazz->onDestroy.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDestroy               \"%s\"\n", clazz->onDestroy.cstr() );
  }
  if( !clazz->onUse.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onUse                   \"%s\"\n", clazz->onUse.cstr() );
  }
  if( !clazz->onDamage.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDamage                \"%s\"\n", clazz->onDamage.cstr() );
  }
}

void ClassFormatter::writeVehicleClass() const
{
  const VehicleClass* clazz = static_cast<const VehicleClass*>( this->clazz );

  fprintf( fs, "\n" );
  fprintf( fs, "title                   \"%s\"\n", clazz->title.cstr() );
  fprintf( fs, "description             \"%s\"\n", clazz->description.cstr() );

  bool isFirstFlag = true;

  if( !clazz->onDestroy.isEmpty() && !( clazz->flags & Object::DESTROY_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDestroy          \"false\"\n" );
  }
  if( !clazz->onDamage.isEmpty() && !( clazz->flags & Object::DAMAGE_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onDamage           \"false\"\n" );
  }
  if( !clazz->onHit.isEmpty() && !( clazz->flags & Object::HIT_FUNC_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.onHit              \"false\"\n" );
  }
  if( !( clazz->flags & Object::SOLID_BIT ) ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.solid              \"false\"\n" );
  }
  if( clazz->flags & Object::NO_DRAW_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.noDraw             \"true\"\n" );
  }
  if( clazz->flags & Object::WIDE_CULL_BIT ) {
    if( isFirstFlag ) {
      isFirstFlag = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "flag.wideCull           \"true\"\n" );
  }

  bool isFirstState = true;

  if( clazz->state & Vehicle::AUTO_EJECT_BIT ) {
    if( isFirstState ) {
      isFirstState = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "state.autoEject         \"true\"\n" );
  }
  if( clazz->state & Vehicle::CREW_VISIBLE_BIT ) {
    if( isFirstState ) {
      isFirstState = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "state.crewVisible       \"true\"\n" );
  }

  fprintf( fs, "\n" );
  fprintf( fs, "dim.x                   \"%g\"\n", clazz->dim.x );
  fprintf( fs, "dim.y                   \"%g\"\n", clazz->dim.y );
  fprintf( fs, "dim.z                   \"%g\"\n", clazz->dim.z );
  fprintf( fs, "\n" );
  fprintf( fs, "life                    \"%g\"\n", clazz->life );
  fprintf( fs, "resistance              \"%g\"\n", clazz->resistance );
  fprintf( fs, "\n" );
  fprintf( fs, "mass                    \"%g\"\n", clazz->mass );
  fprintf( fs, "lift                    \"%g\"\n", clazz->lift );
  fprintf( fs, "\n" );
  fprintf( fs, "pilotPos.x              \"%g\"\n", clazz->pilotPos.x );
  fprintf( fs, "pilotPos.y              \"%g\"\n", clazz->pilotPos.y );
  fprintf( fs, "pilotPos.z              \"%g\"\n", clazz->pilotPos.z );
  fprintf( fs, "\n" );
  fprintf( fs, "pilotRot.x              \"%g\"\n", clazz->pilotRot.x );
  fprintf( fs, "pilotRot.z              \"%g\"\n", clazz->pilotRot.z );
  fprintf( fs, "\n" );
  fprintf( fs, "type                    \"%s\"\n", config->get( "type", "" ) );
  fprintf( fs, "\n" );
  fprintf( fs, "moveMomentum            \"%g\"\n", clazz->moveMomentum );
  fprintf( fs, "\n" );
  fprintf( fs, "hoverHeight             \"%g\"\n", clazz->hoverHeight );
  fprintf( fs, "hoverHeightStiffness    \"%g\"\n", clazz->hoverHeightStiffness );
  fprintf( fs, "hoverMomentumStiffness  \"%g\"\n", clazz->hoverMomentumStiffness );
  fprintf( fs, "\n" );
  fprintf( fs, "enginePitchBias         \"%g\"\n", clazz->enginePitchBias );
  fprintf( fs, "enginePitchRatio        \"%g\"\n", clazz->enginePitchRatio );
  fprintf( fs, "enginePitchLimit        \"%g\"\n", clazz->enginePitchLimit );
  fprintf( fs, "\n" );
  fprintf( fs, "fuel                    \"%g\"\n", clazz->fuel );
  fprintf( fs, "fuelConsumption         \"%.4g\"\n", clazz->fuelConsumption / Timer::TICK_TIME );

  for( int i = 0; i < clazz->nWeapons; ++i ) {
    fprintf( fs, "\n" );
    fprintf( fs, "weapon%02d.name           \"%s\"\n", i, clazz->weaponNames[i].cstr() );
    fprintf( fs, "weapon%02d.onShot         \"%s\"\n", i, clazz->onShot[i].cstr() );
    fprintf( fs, "weapon%02d.nRounds        \"%d\"\n", i, clazz->nRounds[i] );
    fprintf( fs, "weapon%02d.shotInterval   \"%g\"\n", i, clazz->shotInterval[i] );
  }

  bool isFirstHandler = true;

  if( !clazz->onDestroy.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDestroy               \"%s\"\n", clazz->onDestroy.cstr() );
  }
  if( !clazz->onDamage.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onDamage                \"%s\"\n", clazz->onDamage.cstr() );
  }
  if( !clazz->onHit.isEmpty() ) {
    if( isFirstHandler ) {
      isFirstHandler = false;
      fprintf( fs, "\n" );
    }
    fprintf( fs, "onHit                   \"%s\"\n", clazz->onHit.cstr() );
  }
}

void ClassFormatter::writeCommon() const
{
  if( clazz->nItems != 0 ) {
    fprintf( fs, "\n" );
    fprintf( fs, "nItems                  \"%d\"\n", clazz->nItems );

    for( int i = 0; i < clazz->defaultItems.length(); ++i ) {
      fprintf( fs, "item%02d                  \"%s\"\n", i, clazz->defaultItems[i]->name.cstr() );
    }
  }

  if( clazz->fragPool != null ) {
    fprintf( fs, "\n" );
    fprintf( fs, "fragPool                \"%s\"\n", clazz->fragPool->name.cstr() );
    fprintf( fs, "nFrags                  \"%d\"\n", clazz->nFrags );
  }

  if( clazz->deviceType != -1 ) {
    fprintf( fs, "\n" );
    fprintf( fs, "deviceType              \"%s\"\n", config->get( "deviceType", "" ) );
  }

  if( clazz->imagoType != -1 ) {
    fprintf( fs, "\n" );
    fprintf( fs, "imagoType               \"%s\"\n", config->get( "imagoType", "" ) );
    fprintf( fs, "imagoModel              \"%s\"\n", config->get( "imagoModel", "" ) );
  }

  if( clazz->audioType != -1 ) {
    int createSample  = clazz->audioSounds[Object::EVENT_CREATE];
    int destroySample = clazz->audioSounds[Object::EVENT_DESTROY];
    int useSample     = clazz->audioSounds[Object::EVENT_USE];
    int damageSample  = clazz->audioSounds[Object::EVENT_DAMAGE];
    int hitSample     = clazz->audioSounds[Object::EVENT_HIT];

    fprintf( fs, "\n" );
    fprintf( fs, "audioType               \"%s\"\n", config->get( "audioType", "" ) );

    fprintf( fs, "audioSound.create       \"%s\"\n",
             createSample == -1 ? "" : library.sounds[createSample].name.cstr() );
    fprintf( fs, "audioSound.destroy      \"%s\"\n",
             destroySample == -1 ? "" : library.sounds[destroySample].name.cstr() );
    fprintf( fs, "audioSound.use          \"%s\"\n",
             useSample == -1 ? "" : library.sounds[useSample].name.cstr() );
    fprintf( fs, "audioSound.damage       \"%s\"\n",
             damageSample == -1 ? "" : library.sounds[damageSample].name.cstr() );
    fprintf( fs, "audioSound.hit          \"%s\"\n",
             hitSample == -1 ? "" : library.sounds[hitSample].name.cstr() );

    if( clazz->flags & Object::DYNAMIC_BIT ) {
      int splashSample   = clazz->audioSounds[Object::EVENT_SPLASH];
      int frictingSample = clazz->audioSounds[Object::EVENT_FRICTING];

      fprintf( fs, "audioSound.splash       \"%s\"\n",
               splashSample == -1 ? "" : library.sounds[splashSample].name.cstr() );
      fprintf( fs, "audioSound.fricting     \"%s\"\n",
               frictingSample == -1 ? "" : library.sounds[frictingSample].name.cstr() );
    }

    if( clazz->flags & Object::WEAPON_BIT ) {
      int shotSample      = clazz->audioSounds[Weapon::EVENT_SHOT];
      int shotEmptySample = clazz->audioSounds[Weapon::EVENT_SHOT_EMPTY];

      fprintf( fs, "audioSound.shot         \"%s\"\n",
               shotSample == -1 ? "" : library.sounds[shotSample].name.cstr() );
      fprintf( fs, "audioSound.shotEmpty    \"%s\"\n",
               shotEmptySample == -1 ? "" : library.sounds[shotEmptySample].name.cstr() );
    }

    if( clazz->flags & Object::BOT_BIT ) {
      int hitHardSample = clazz->audioSounds[Bot::EVENT_HIT_HARD];
      int landSample    = clazz->audioSounds[Bot::EVENT_LAND];
      int jumpSample    = clazz->audioSounds[Bot::EVENT_JUMP];
      int flipSample    = clazz->audioSounds[Bot::EVENT_FLIP];
      int deathSample   = clazz->audioSounds[Bot::EVENT_DEATH];

      fprintf( fs, "audioSound.hitHard      \"%s\"\n",
               hitHardSample == -1 ? "" : library.sounds[hitHardSample].name.cstr() );
      fprintf( fs, "audioSound.land         \"%s\"\n",
               landSample == -1 ? "" : library.sounds[landSample].name.cstr() );
      fprintf( fs, "audioSound.jump         \"%s\"\n",
               jumpSample == -1 ? "" : library.sounds[jumpSample].name.cstr() );
      fprintf( fs, "audioSound.flip         \"%s\"\n",
               flipSample == -1 ? "" : library.sounds[flipSample].name.cstr() );
      fprintf( fs, "audioSound.death        \"%s\"\n",
               deathSample == -1 ? "" : library.sounds[deathSample].name.cstr() );
    }

    if( clazz->flags & Object::VEHICLE_BIT ) {
      int engineSample     = clazz->audioSounds[Vehicle::EVENT_ENGINE];
      int nextWeaponSample = clazz->audioSounds[Vehicle::EVENT_NEXT_WEAPON];
      int shot0Sample      = clazz->audioSounds[Vehicle::EVENT_SHOT0];
      int shot1Sample      = clazz->audioSounds[Vehicle::EVENT_SHOT1];
      int shot2Sample      = clazz->audioSounds[Vehicle::EVENT_SHOT2];
      int shot3Sample      = clazz->audioSounds[Vehicle::EVENT_SHOT3];
      int shotEmptySample  = clazz->audioSounds[Vehicle::EVENT_SHOT_EMPTY];

      fprintf( fs, "audioSound.engine       \"%s\"\n",
               engineSample == -1 ? "" : library.sounds[engineSample].name.cstr() );
      fprintf( fs, "audioSound.nextWeapon   \"%s\"\n",
               nextWeaponSample == -1 ? "" : library.sounds[nextWeaponSample].name.cstr() );
      fprintf( fs, "audioSound.shot0        \"%s\"\n",
               shot0Sample == -1 ? "" : library.sounds[shot0Sample].name.cstr() );
      fprintf( fs, "audioSound.shot1        \"%s\"\n",
               shot1Sample == -1 ? "" : library.sounds[shot1Sample].name.cstr() );
      fprintf( fs, "audioSound.shot2        \"%s\"\n",
               shot2Sample == -1 ? "" : library.sounds[shot2Sample].name.cstr() );
      fprintf( fs, "audioSound.shot3        \"%s\"\n",
               shot3Sample == -1 ? "" : library.sounds[shot3Sample].name.cstr() );
      fprintf( fs, "audioSound.shotEmpty    \"%s\"\n",
               shotEmptySample == -1 ? "" : library.sounds[shotEmptySample].name.cstr() );
    }
  }
}

void ClassFormatter::format( const char* name )
{
  log.print( "Formatting class '%s' ...", name );

  String sPath = "class/" + String( name ) + ".rc";

  Config classConfig;
  if( !classConfig.load( sPath ) ) {
    throw Exception( "Cannot read class file" );
  }

  fs = fopen( sPath, "w" );
  if( fs == null ) {
    throw Exception( "Cannot open class file for writing" );
  }

  clazz  = library.objClass( name );
  config = &classConfig;

  fprintf( fs, "base                    \"%s\"\n", config->get( "base", "" ) );

  if( clazz->flags & Object::VEHICLE_BIT ) {
    writeVehicleClass();
  }
  else if( clazz->flags & Object::BOT_BIT ) {
    writeBotClass();
  }
  else if( clazz->flags & Object::WEAPON_BIT ) {
    writeWeaponClass();
  }
  else if( clazz->flags & Object::DYNAMIC_BIT ) {
    writeDynamicClass();
  }
  else {
    writeObjectClass();
  }

  writeCommon();

  fclose( fs );
  classConfig.clear();

  log.printEnd( " OK" );
}

}
