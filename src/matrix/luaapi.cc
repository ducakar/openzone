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
 * @file matrix/luaapi.cc
 */

#include "matrix/Orbis.hh"
#include "matrix/Vehicle.hh"

#include "common/lua.hh"

namespace oz
{
namespace matrix
{

void importLuaConstants( lua_State* l )
{
  registerLuaConstant( l, "OZ_ORBIS_DIM",                   Orbis::DIM );

  registerLuaConstant( l, "OZ_NORTH",                       NORTH );
  registerLuaConstant( l, "OZ_WEST",                        WEST );
  registerLuaConstant( l, "OZ_SOUTH",                       SOUTH );
  registerLuaConstant( l, "OZ_EAST",                        EAST );

  registerLuaConstant( l, "OZ_EVENT_CREATE",                Object::EVENT_CREATE );
  registerLuaConstant( l, "OZ_EVENT_DESTROY",               Object::EVENT_DESTROY );
  registerLuaConstant( l, "OZ_EVENT_DAMAGE",                Object::EVENT_DAMAGE );
  registerLuaConstant( l, "OZ_EVENT_HIT",                   Object::EVENT_HIT );
  registerLuaConstant( l, "OZ_EVENT_SPLASH",                Object::EVENT_SPLASH );
  registerLuaConstant( l, "OZ_EVENT_FRICTING",              Object::EVENT_FRICTING );
  registerLuaConstant( l, "OZ_EVENT_USE",                   Object::EVENT_USE );
  registerLuaConstant( l, "OZ_EVENT_SHOT",                  Weapon::EVENT_SHOT );
  registerLuaConstant( l, "OZ_EVENT_SHOT_EMPTY",            Weapon::EVENT_SHOT_EMPTY );
  registerLuaConstant( l, "OZ_EVENT_HIT_HARD",              Bot::EVENT_HIT_HARD );
  registerLuaConstant( l, "OZ_EVENT_LAND",                  Bot::EVENT_LAND );
  registerLuaConstant( l, "OZ_EVENT_JUMP",                  Bot::EVENT_JUMP );
  registerLuaConstant( l, "OZ_EVENT_FLIP",                  Bot::EVENT_FLIP );
  registerLuaConstant( l, "OZ_EVENT_DEATH",                 Bot::EVENT_DEATH );
  registerLuaConstant( l, "OZ_EVENT_STEP",                  Bot::EVENT_STEP );
  registerLuaConstant( l, "OZ_EVENT_WATERSTEP",             Bot::EVENT_WATERSTEP );
  registerLuaConstant( l, "OZ_EVENT_SWIM",                  Bot::EVENT_SWIM );
  registerLuaConstant( l, "OZ_EVENT_ENGINE",                Vehicle::EVENT_ENGINE );
  registerLuaConstant( l, "OZ_EVENT_NEXT_WEAPON",           Vehicle::EVENT_NEXT_WEAPON );
  registerLuaConstant( l, "OZ_EVENT_SHOT0",                 Vehicle::EVENT_SHOT0 );
  registerLuaConstant( l, "OZ_EVENT_SHOT1",                 Vehicle::EVENT_SHOT1 );
  registerLuaConstant( l, "OZ_EVENT_SHOT2",                 Vehicle::EVENT_SHOT2 );
  registerLuaConstant( l, "OZ_EVENT_SHOT3",                 Vehicle::EVENT_SHOT3 );
  registerLuaConstant( l, "OZ_EVENT_SHOT_EMPTY",            Vehicle::EVENT_SHOT_EMPTY );

  registerLuaConstant( l, "OZ_OBJECT_DYNAMIC_BIT",          Object::DYNAMIC_BIT );
  registerLuaConstant( l, "OZ_OBJECT_WEAPON_BIT",           Object::WEAPON_BIT );
  registerLuaConstant( l, "OZ_OBJECT_BOT_BIT",              Object::BOT_BIT );
  registerLuaConstant( l, "OZ_OBJECT_VEHICLE_BIT",          Object::VEHICLE_BIT );
  registerLuaConstant( l, "OZ_OBJECT_ITEM_BIT",             Object::ITEM_BIT );
  registerLuaConstant( l, "OZ_OBJECT_BROWSABLE_BIT",        Object::BROWSABLE_BIT );

  registerLuaConstant( l, "OZ_OBJECT_LUA_BIT",              Object::LUA_BIT );
  registerLuaConstant( l, "OZ_OBJECT_DESTROY_FUNC_BIT",     Object::DESTROY_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJECT_DAMAGE_FUNC_BIT",      Object::DAMAGE_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJECT_HIT_FUNC_BIT",         Object::HIT_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJECT_USE_FUNC_BIT",         Object::USE_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );

  registerLuaConstant( l, "OZ_OBJECT_DEVICE_BIT",           Object::DEVICE_BIT );
  registerLuaConstant( l, "OZ_OBJECT_IMAGO_BIT",            Object::IMAGO_BIT );
  registerLuaConstant( l, "OZ_OBJECT_AUDIO_BIT",            Object::AUDIO_BIT );

  registerLuaConstant( l, "OZ_OBJECT_DESTROYED_BIT",        Object::DESTROYED_BIT );

  registerLuaConstant( l, "OZ_OBJECT_SOLID_BIT",            Object::SOLID_BIT );
  registerLuaConstant( l, "OZ_OBJECT_CYLINDER_BIT",         Object::CYLINDER_BIT );
  registerLuaConstant( l, "OZ_OBJECT_DISABLED_BIT",         Object::DISABLED_BIT );
  registerLuaConstant( l, "OZ_OBJECT_ENABLE_BIT",           Object::ENABLE_BIT );
  registerLuaConstant( l, "OZ_OBJECT_FRICTING_BIT",         Object::FRICTING_BIT );
  registerLuaConstant( l, "OZ_OBJECT_BELOW_BIT",            Object::BELOW_BIT );
  registerLuaConstant( l, "OZ_OBJECT_ON_FLOOR_BIT",         Object::ON_FLOOR_BIT );
  registerLuaConstant( l, "OZ_OBJECT_ON_SLICK_BIT",         Object::ON_SLICK_BIT );
  registerLuaConstant( l, "OZ_OBJECT_IN_LIQUID_BIT",        Object::IN_LIQUID_BIT );
  registerLuaConstant( l, "OZ_OBJECT_IN_LAVA_BIT",          Object::IN_LAVA_BIT );
  registerLuaConstant( l, "OZ_OBJECT_ON_LADDER_BIT",        Object::ON_LADDER_BIT );

  registerLuaConstant( l, "OZ_OBJECT_NO_DRAW_BIT",          Object::NO_DRAW_BIT );
  registerLuaConstant( l, "OZ_OBJECT_WIDE_CULL_BIT",        Object::WIDE_CULL_BIT );

  registerLuaConstant( l, "OZ_BOT_DEAD_BIT",                Bot::DEAD_BIT );
  registerLuaConstant( l, "OZ_BOT_MECHANICAL_BIT",          Bot::MECHANICAL_BIT );
  registerLuaConstant( l, "OZ_BOT_INCARNATABLE_BIT",        Bot::INCARNATABLE_BIT );
  registerLuaConstant( l, "OZ_BOT_PLAYER_BIT",              Bot::PLAYER_BIT );

  registerLuaConstant( l, "OZ_BOT_MOVING_BIT",              Bot::MOVING_BIT );
  registerLuaConstant( l, "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );
  registerLuaConstant( l, "OZ_BOT_RUNNING_BIT",             Bot::RUNNING_BIT );
  registerLuaConstant( l, "OZ_BOT_ATTACKING_BIT",           Bot::ATTACKING_BIT );

  registerLuaConstant( l, "OZ_BOT_JUMP_SCHED_BIT",          Bot::JUMP_SCHED_BIT );
  registerLuaConstant( l, "OZ_BOT_GROUNDED_BIT",            Bot::GROUNDED_BIT );
  registerLuaConstant( l, "OZ_BOT_ON_STAIRS_BIT",           Bot::ON_STAIRS_BIT );
  registerLuaConstant( l, "OZ_BOT_CLIMBING_BIT",            Bot::CLIMBING_BIT );
  registerLuaConstant( l, "OZ_BOT_SWIMMING_BIT",            Bot::SWIMMING_BIT );
  registerLuaConstant( l, "OZ_BOT_SUBMERGED_BIT",           Bot::SUBMERGED_BIT );

  registerLuaConstant( l, "OZ_BOT_GESTURE0_BIT",            Bot::GESTURE0_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE1_BIT",            Bot::GESTURE1_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE2_BIT",            Bot::GESTURE2_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE3_BIT",            Bot::GESTURE3_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE4_BIT",            Bot::GESTURE4_BIT );

  registerLuaConstant( l, "OZ_VEHICLE_CREW_VISIBLE_BIT",    Vehicle::CREW_VISIBLE_BIT );
  registerLuaConstant( l, "OZ_VEHICLE_HAS_EJECT_BIT",       Vehicle::HAS_EJECT_BIT );
  registerLuaConstant( l, "OZ_VEHICLE_AUTO_EJECT_BIT",      Vehicle::AUTO_EJECT_BIT );

  registerLuaConstant( l, "OZ_ATTRIB_NIGHT_VISION_BIT",     ObjectClass::NIGHT_VISION_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_BINOCULARS_BIT",       ObjectClass::BINOCULARS_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_GALILEO_BIT",          ObjectClass::GALILEO_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_MUSIC_PLAYER_BIT",     ObjectClass::MUSIC_PLAYER_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_SUICIDE_BIT",          ObjectClass::SUICIDE_BIT );
}

}
}
