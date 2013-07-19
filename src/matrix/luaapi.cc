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
 * @file matrix/luaapi.cc
 */

#include <common/luabase.hh>
#include <matrix/Orbis.hh>
#include <matrix/Vehicle.hh>

namespace oz
{

void importMatrixConstants( lua_State* l );

void importMatrixConstants( lua_State* l )
{
  enum AddMode
  {
    ADD_FORCE = 0,
    ADD_TRY   = 1
  };

  enum CollideMode
  {
    COLLIDE_STRUCTS_BIT     = 0x01,
    COLLIDE_OBJECTS_BIT     = 0x02,
    COLLIDE_ALL_OBJECTS_BIT = 0x04
  };

  registerLuaConstant( l, "OZ_ORBIS_DIM",                   Orbis::DIM );

  registerLuaConstant( l, "OZ_NORTH",                       NORTH );
  registerLuaConstant( l, "OZ_WEST",                        WEST );
  registerLuaConstant( l, "OZ_SOUTH",                       SOUTH );
  registerLuaConstant( l, "OZ_EAST",                        EAST );

  registerLuaConstant( l, "OZ_FORCE",                       ADD_FORCE );
  registerLuaConstant( l, "OZ_TRY",                         ADD_TRY );

  registerLuaConstant( l, "OZ_STRUCTS_BIT",                 COLLIDE_STRUCTS_BIT );
  registerLuaConstant( l, "OZ_OBJECTS_BIT",                 COLLIDE_OBJECTS_BIT );
  registerLuaConstant( l, "OZ_ALL_OBJECTS_BIT",             COLLIDE_ALL_OBJECTS_BIT );

  registerLuaConstant( l, "OZ_ENTITY_CLOSED",               Entity::CLOSED );
  registerLuaConstant( l, "OZ_ENTITY_OPENED",               Entity::OPENED );

  registerLuaConstant( l, "OZ_EVENT_CREATE",                Object::EVENT_CREATE );
  registerLuaConstant( l, "OZ_EVENT_DESTROY",               Object::EVENT_DESTROY );
  registerLuaConstant( l, "OZ_EVENT_DAMAGE",                Object::EVENT_DAMAGE );
  registerLuaConstant( l, "OZ_EVENT_HIT",                   Object::EVENT_HIT );
  registerLuaConstant( l, "OZ_EVENT_LAND",                  Object::EVENT_LAND );
  registerLuaConstant( l, "OZ_EVENT_SPLASH",                Object::EVENT_SPLASH );
  registerLuaConstant( l, "OZ_EVENT_FRICTING",              Object::EVENT_FRICTING );
  registerLuaConstant( l, "OZ_EVENT_USE",                   Object::EVENT_USE );
  registerLuaConstant( l, "OZ_EVENT_SHOT",                  Weapon::EVENT_SHOT );
  registerLuaConstant( l, "OZ_EVENT_SHOT_EMPTY",            Weapon::EVENT_SHOT_EMPTY );
  registerLuaConstant( l, "OZ_EVENT_STEP",                  Bot::EVENT_STEP );
  registerLuaConstant( l, "OZ_EVENT_WATERSTEP",             Bot::EVENT_WATERSTEP );
  registerLuaConstant( l, "OZ_EVENT_SWIM_SURFACE",          Bot::EVENT_SWIM_SURFACE );
  registerLuaConstant( l, "OZ_EVENT_SWIM_SUBMERGED",        Bot::EVENT_SWIM_SUBMERGED );
  registerLuaConstant( l, "OZ_EVENT_DEATH",                 Bot::EVENT_DEATH );
  registerLuaConstant( l, "OZ_EVENT_JUMP",                  Bot::EVENT_JUMP );
  registerLuaConstant( l, "OZ_EVENT_MELEE",                 Bot::EVENT_MELEE );
  registerLuaConstant( l, "OZ_EVENT_TAKE",                  Bot::EVENT_TAKE );
  registerLuaConstant( l, "OZ_EVENT_POINT",                 Bot::EVENT_POINT );
  registerLuaConstant( l, "OZ_EVENT_FALL_BACK",             Bot::EVENT_FALL_BACK );
  registerLuaConstant( l, "OZ_EVENT_SALUTE",                Bot::EVENT_SALUTE );
  registerLuaConstant( l, "OZ_EVENT_WAVE",                  Bot::EVENT_WAVE );
  registerLuaConstant( l, "OZ_EVENT_FLOP",                  Bot::EVENT_FLIP );
  registerLuaConstant( l, "OZ_EVENT_ENGINE",                Vehicle::EVENT_ENGINE );
  registerLuaConstant( l, "OZ_EVENT_SHOT0",                 Vehicle::EVENT_SHOT0 );
  registerLuaConstant( l, "OZ_EVENT_SHOT1",                 Vehicle::EVENT_SHOT1 );
  registerLuaConstant( l, "OZ_EVENT_SHOT2",                 Vehicle::EVENT_SHOT2 );
  registerLuaConstant( l, "OZ_EVENT_SHOT3",                 Vehicle::EVENT_SHOT3 );
  registerLuaConstant( l, "OZ_EVENT_SHOT_EMPTY",            Vehicle::EVENT_SHOT_EMPTY );

  registerLuaConstant( l, "OZ_DYNAMIC_BIT",                 Object::DYNAMIC_BIT );
  registerLuaConstant( l, "OZ_WEAPON_BIT",                  Object::WEAPON_BIT );
  registerLuaConstant( l, "OZ_BOT_BIT",                     Object::BOT_BIT );
  registerLuaConstant( l, "OZ_VEHICLE_BIT",                 Object::VEHICLE_BIT );
  registerLuaConstant( l, "OZ_ITEM_BIT",                    Object::ITEM_BIT );
  registerLuaConstant( l, "OZ_BROWSABLE_BIT",               Object::BROWSABLE_BIT );

  registerLuaConstant( l, "OZ_OBJ_LUA_BIT",                 Object::LUA_BIT );
  registerLuaConstant( l, "OZ_OBJ_DESTROY_FUNC_BIT",        Object::DESTROY_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJ_USE_FUNC_BIT",            Object::USE_FUNC_BIT );
  registerLuaConstant( l, "OZ_OBJ_UPDATE_FUNC_BIT",         Object::UPDATE_FUNC_BIT );

  registerLuaConstant( l, "OZ_OBJ_DEVICE_BIT",              Object::DEVICE_BIT );
  registerLuaConstant( l, "OZ_OBJ_IMAGO_BIT",               Object::IMAGO_BIT );
  registerLuaConstant( l, "OZ_OBJ_AUDIO_BIT",               Object::AUDIO_BIT );

  registerLuaConstant( l, "OZ_OBJ_DESTROYED_BIT",           Object::DESTROYED_BIT );

  registerLuaConstant( l, "OZ_OBJ_SOLID_BIT",               Object::SOLID_BIT );
  registerLuaConstant( l, "OZ_OBJ_CYLINDER_BIT",            Object::CYLINDER_BIT );
  registerLuaConstant( l, "OZ_OBJ_DISABLED_BIT",            Object::DISABLED_BIT );
  registerLuaConstant( l, "OZ_OBJ_ENABLE_BIT",              Object::ENABLE_BIT );
  registerLuaConstant( l, "OZ_OBJ_FRICTING_BIT",            Object::FRICTING_BIT );
  registerLuaConstant( l, "OZ_OBJ_BELOW_BIT",               Object::BELOW_BIT );
  registerLuaConstant( l, "OZ_OBJ_ON_FLOOR_BIT",            Object::ON_FLOOR_BIT );
  registerLuaConstant( l, "OZ_OBJ_ON_SLICK_BIT",            Object::ON_SLICK_BIT );
  registerLuaConstant( l, "OZ_OBJ_IN_LIQUID_BIT",           Object::IN_LIQUID_BIT );
  registerLuaConstant( l, "OZ_OBJ_IN_LAVA_BIT",             Object::IN_LAVA_BIT );
  registerLuaConstant( l, "OZ_OBJ_ON_LADDER_BIT",           Object::ON_LADDER_BIT );

  registerLuaConstant( l, "OZ_OBJ_WIDE_CULL_BIT",           Object::WIDE_CULL_BIT );

  registerLuaConstant( l, "OZ_BOT_DEAD_BIT",                Bot::DEAD_BIT );
  registerLuaConstant( l, "OZ_BOT_MECHANICAL_BIT",          Bot::MECHANICAL_BIT );
  registerLuaConstant( l, "OZ_BOT_INCARNATABLE_BIT",        Bot::INCARNATABLE_BIT );
  registerLuaConstant( l, "OZ_BOT_PLAYER_BIT",              Bot::PLAYER_BIT );

  registerLuaConstant( l, "OZ_BOT_MOVING_BIT",              Bot::MOVING_BIT );
  registerLuaConstant( l, "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );
  registerLuaConstant( l, "OZ_BOT_WALKING_BIT",             Bot::WALKING_BIT );
  registerLuaConstant( l, "OZ_BOT_ATTACKING_BIT",           Bot::ATTACKING_BIT );

  registerLuaConstant( l, "OZ_BOT_JUMP_SCHED_BIT",          Bot::JUMP_SCHED_BIT );
  registerLuaConstant( l, "OZ_BOT_GROUNDED_BIT",            Bot::GROUNDED_BIT );
  registerLuaConstant( l, "OZ_BOT_LADDER_BIT",              Bot::LADDER_BIT );
  registerLuaConstant( l, "OZ_BOT_LEDGE_BIT",               Bot::LEDGE_BIT );
  registerLuaConstant( l, "OZ_BOT_SWIMMING_BIT",            Bot::SWIMMING_BIT );
  registerLuaConstant( l, "OZ_BOT_SUBMERGED_BIT",           Bot::SUBMERGED_BIT );

  registerLuaConstant( l, "OZ_BOT_GESTURE_POINT_BIT",       Bot::GESTURE_POINT_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE_FALL_BACK_BIT",   Bot::GESTURE_FALL_BACK_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE_SALUTE_BIT",      Bot::GESTURE_SALUTE_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE_WAVE_BIT",        Bot::GESTURE_WAVE_BIT );
  registerLuaConstant( l, "OZ_BOT_GESTURE_FLIP_BIT",        Bot::GESTURE_FLIP_BIT );

  registerLuaConstant( l, "OZ_VEHICLE_CREW_VISIBLE_BIT",    Vehicle::CREW_VISIBLE_BIT );
  registerLuaConstant( l, "OZ_VEHICLE_HAS_EJECT_BIT",       Vehicle::HAS_EJECT_BIT );
  registerLuaConstant( l, "OZ_VEHICLE_AUTO_EJECT_BIT",      Vehicle::AUTO_EJECT_BIT );

  registerLuaConstant( l, "OZ_ATTRIB_NIGHT_VISION_BIT",     ObjectClass::NIGHT_VISION_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_BINOCULARS_BIT",       ObjectClass::BINOCULARS_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_GALILEO_BIT",          ObjectClass::GALILEO_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_MUSIC_PLAYER_BIT",     ObjectClass::MUSIC_PLAYER_BIT );
  registerLuaConstant( l, "OZ_ATTRIB_SUICIDE_BIT",          ObjectClass::SUICIDE_BIT );

  registerLuaConstant( l, "OZ_ACTION_FORWARD",              Bot::ACTION_FORWARD );
  registerLuaConstant( l, "OZ_ACTION_BACKWARD",             Bot::ACTION_BACKWARD );
  registerLuaConstant( l, "OZ_ACTION_RIGHT",                Bot::ACTION_RIGHT );
  registerLuaConstant( l, "OZ_ACTION_LEFT",                 Bot::ACTION_LEFT );
  registerLuaConstant( l, "OZ_ACTION_JUMP",                 Bot::ACTION_JUMP );
  registerLuaConstant( l, "OZ_ACTION_CROUCH",               Bot::ACTION_CROUCH );
  registerLuaConstant( l, "OZ_ACTION_WALK",                 Bot::ACTION_WALK );
  registerLuaConstant( l, "OZ_ACTION_ATTACK",               Bot::ACTION_ATTACK );
  registerLuaConstant( l, "OZ_ACTION_EXIT",                 Bot::ACTION_EXIT );
  registerLuaConstant( l, "OZ_ACTION_EJECT",                Bot::ACTION_EJECT );
  registerLuaConstant( l, "OZ_ACTION_SUICIDE",              Bot::ACTION_SUICIDE );
  registerLuaConstant( l, "OZ_ACTION_VEH_UP",               Bot::ACTION_VEH_UP );
  registerLuaConstant( l, "OZ_ACTION_VEH_DOWN",             Bot::ACTION_VEH_DOWN );
  registerLuaConstant( l, "OZ_ACTION_VEH_NEXT_WEAPON",      Bot::ACTION_VEH_NEXT_WEAPON );
  registerLuaConstant( l, "OZ_ACTION_POINT",                Bot::ACTION_POINT );
  registerLuaConstant( l, "OZ_ACTION_BACK",                 Bot::ACTION_BACK );
  registerLuaConstant( l, "OZ_ACTION_SALUTE",               Bot::ACTION_SALUTE );
  registerLuaConstant( l, "OZ_ACTION_WAVE",                 Bot::ACTION_WAVE );
  registerLuaConstant( l, "OZ_ACTION_FLIP",                 Bot::ACTION_FLIP );
  registerLuaConstant( l, "OZ_ACTION_TRIGGER",              Bot::ACTION_TRIGGER );
  registerLuaConstant( l, "OZ_ACTION_LOCK",                 Bot::ACTION_LOCK );
  registerLuaConstant( l, "OZ_ACTION_USE",                  Bot::ACTION_USE );
  registerLuaConstant( l, "OZ_ACTION_TAKE",                 Bot::ACTION_TAKE );
  registerLuaConstant( l, "OZ_ACTION_GRAB",                 Bot::ACTION_GRAB );
  registerLuaConstant( l, "OZ_ACTION_ROTATE",               Bot::ACTION_ROTATE );
  registerLuaConstant( l, "OZ_ACTION_THROW",                Bot::ACTION_THROW );
  registerLuaConstant( l, "OZ_ACTION_INV_USE",              Bot::ACTION_INV_USE );
  registerLuaConstant( l, "OZ_ACTION_INV_TAKE",             Bot::ACTION_INV_TAKE );
  registerLuaConstant( l, "OZ_ACTION_INV_GIVE",             Bot::ACTION_INV_GIVE );
  registerLuaConstant( l, "OZ_ACTION_INV_DROP",             Bot::ACTION_INV_DROP );
  registerLuaConstant( l, "OZ_ACTION_INV_GRAB",             Bot::ACTION_INV_GRAB );
}

}
