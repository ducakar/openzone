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
 * @file common/luabase.hh
 *
 * Handy macros and methods for %Lua API implementation.
 *
 * This is internal header, it should only be included by modules that contain %Lua API
 * implementation. It contains macros for parameter checking, reporting errors, checking validity of
 * %Lua API calls etc. that can be used as some basic building blocks for %Lua APIs. Furthermore
 * there are macros that wrap %Lua library calls so, one can e.g. write
 * @code
 *   l_tofloat( 1 );
 * @endcode
 * instead of
 * @code
 *   float( lua_tonumber( l, 1 ) );
 * @endcode
 * Finally it defines overloaded methods for registering %Lua API constants that simplify code in
 * similar way as the macro above.
 */

#pragma once

#include <common/common.hh>

#include <lua.hpp>

/**
 * @def IMPORT_FUNC
 * Registers %Lua API function with the Lua class in the current namespace.
 */
#define IMPORT_FUNC( func ) \
  lua.registerFunction( #func, func )

/**
 * @def IGNORE_FUNC
 * Does nothing except silences warning that %Lua API function was not used.
 */
#define IGNORE_FUNC( func ) \
  static_cast<void>( func )

/**
 * @def IMPORT_CONST
 * Registers %Lua API constant with the Lua class in the current namespace.
 */
#define IMPORT_CONST( name, value ) \
  lua.registerConstant( name, value )

/**
 * @def ERROR
 * Exits %Lua API function call with the giver error message.
 */
#define ERROR( message ) \
  luaL_error( l, "%s: %s", __func__, message )

/**
 * @def ARG( n )
 * Exits %Lua API function call with an error if number of parameters is not `n`.
 */
#define ARG( n ) \
  if( lua_gettop( l ) != ( n ) ) { \
    ERROR( #n " arguments expected" ); \
  }

/**
 * @def VARG( min, max )
 * Exits %Lua API function call with an error if number of parameters is not between `min` and
 * `max`.
 */
#define VARG( min, max ) \
  { \
    int n = lua_gettop( l ); \
    if( n < ( min ) || ( max ) < n ) { \
      ERROR( "Between " #min " and " #max " arguments expected" ); \
    } \
  }

/**
 * @def ARG_VAR( n )
 * Exits %Lua API function call with an error if number of parameters is less than `n`.
 */
#define ARG_VAR( n ) \
  if( lua_gettop( l ) < ( n ) ) { \
    ERROR( #n " or more arguments expected" ); \
  }

/**
 * @def STR
 * Exits %Lua API function call with an error if there is no structure bound.
 */
#define STR() \
  if( ms.str == nullptr ) { \
    ERROR( "No structure bound" ); \
  }

/**
 * @def STR_INDEX
 * Initialises variable `str` to structure with a given index or returns with error for invalid
 * indices.
 */
#define STR_INDEX( index ) \
  if( uint( index ) >= uint( orbis.structs.length() ) ) { \
    ERROR( "Invalid structure index (out of range)" ); \
  } \
  Struct* str = orbis.structs[index]; \
  if( str == nullptr ) { \
    ERROR( "Invalid structures index (null)" ); \
  }

/**
 * @def ENT
 * Exits %Lua API function call with an error if there is no structure entity bound.
 */
#define ENT() \
  if( ms.ent == nullptr ) { \
    ERROR( "No structure entity bound" ); \
  }

/**
 * @def ENT_INDEX
 * Initialises variable `ent` to structure entity with a given index (i.e.
 * structureIndex * 128 + entityIndex)or returns with error for invalid indices.
 */
#define ENT_INDEX( index ) \
  if( uint( index ) >= uint( ms.str->entities.length() ) ) { \
    ERROR( "Invalid structure entity index (out of range)" ); \
  } \
  Entity* ent = &ms.str->entities[index];

/**
 * @def OBJ
 * Exits %Lua API function call with an error if there is no object bound.
 */
#define OBJ() \
  if( ms.obj == nullptr ) { \
    ERROR( "No object bound" ); \
  }

/**
 * @def SELF
 * Exits %Lua API function call with an error if self object is `nullptr`.
 */
#define SELF() \
  if( ms.self == nullptr ) { \
    ERROR( "Self object is null" ); \
  }

/**
 * @def SELF_BOT
 * Exits %Lua API function call with an error if self object is `nullptr` or not a bot.
 */
#define SELF_BOT() \
  SELF(); \
  Bot* self = static_cast<Bot*>( ms.self ); \
  if( !( self->flags & Object::BOT_BIT ) ) { \
    ERROR( "Self object is not a bot" ); \
  }

/**
 * @def OBJ_INDEX
 * Initialises variable `obj` to object with a given index or returns with error for invalid
 * indices.
 */
#define OBJ_INDEX( index ) \
  if( uint( index ) >= uint( orbis.objects.length() ) ) { \
    ERROR( "Invalid object index (out of range)" ); \
  } \
  Object* obj = orbis.objects[index]; \
  if( obj == nullptr ) { \
    ERROR( "Invalid object index (null)" ); \
  }

/**
 * @def ITEM_INDEX
 * Initialises variable `item` to item with a given index or returns with error for invalid indices
 * or objects that are not items.
 */
#define ITEM_INDEX( index ) \
  if( uint( index ) >= uint( orbis.objects.length() ) ) { \
    ERROR( "Invalid item index (out of range)" ); \
  } \
  Dynamic* item = static_cast<Dynamic*>( orbis.objects[index] ); \
  if( item == nullptr ) { \
    ERROR( "Invalid item index (null)" ); \
  } \
  if( !( item->flags & Object::ITEM_BIT ) ) { \
    ERROR( "Invalid item index (not an item)" ); \
  }

/**
 * @def BOT_INDEX
 * Initialises variable `bot` to bot with a given index or returns with error for invalid indices
 * or objects that are not bots.
 */
#define BOT_INDEX( index ) \
  if( uint( index ) >= uint( orbis.objects.length() ) ) { \
    ERROR( "Invalid bot index (out of range)" ); \
  } \
  Bot* bot = static_cast<Bot*>( orbis.objects[index] ); \
  if( bot == nullptr ) { \
    ERROR( "Invalid bot index (null)" ); \
  } \
  if( !( bot->flags & Object::BOT_BIT ) ) { \
    ERROR( "Invalid bot index (not a bot)" ); \
  }

/**
 * @def OBJ_NOT_SELF
 * Exits %Lua API function call with an error if the bound object is self.
 */
#define OBJ_NOT_SELF() \
  if( ms.obj == ms.self ) { \
    ERROR( "Bound object should not be self" ); \
  }

/**
 * @def OBJ_DYNAMIC
 * Exits %Lua API function call with an error if the bound object is not dynamic (Dynamic class).
 */
#define OBJ_DYNAMIC() \
  if( !( ms.obj->flags & Object::DYNAMIC_BIT ) ) { \
    ERROR( "Bound object is not dynamic" ); \
  } \
  Dynamic* dyn = static_cast<Dynamic*>( ms.obj );

/**
 * @def OBJ_WEAPON
 * Exits %Lua API function call with an error if the bound object is not a weapon (Weapon class).
 */
#define OBJ_WEAPON() \
  if( !( ms.obj->flags & Object::WEAPON_BIT ) ) { \
    ERROR( "Bound object is not a weapon" ); \
  } \
  Weapon* weapon = static_cast<Weapon*>( ms.obj );

/**
 * @def OBJ_BOT
 * Exits %Lua API function call with an error if the bound object is not a bot (Bot class).
 */
#define OBJ_BOT() \
  if( !( ms.obj->flags & Object::BOT_BIT ) ) { \
    ERROR( "Bound object is not a bot" ); \
  } \
  Bot* bot = static_cast<Bot*>( ms.obj );

/**
 * @def OBJ_VEHICLE
 * Exits %Lua API function call with an error if the bound object is not a vehicle (Vehicle class).
 */
#define OBJ_VEHICLE() \
  if( !( ms.obj->flags & Object::VEHICLE_BIT ) ) { \
    ERROR( "Bound object is not a vehicle" ); \
  } \
  Vehicle* veh = static_cast<Vehicle*>( ms.obj );

/**
 * @def FRAG
 * Exits %Lua API function call with an error if there is no fragment bound.
 */
#define FRAG() \
  if( ms.frag == nullptr ) { \
    ERROR( "No fragment bound" ); \
  }

/**
 * @def l_gettop
 * Shorthand for lua_gettop
 */
#define l_gettop() \
  lua_gettop( l )

/**
 * @def l_settop
 * Shorthand for lua_settop
 */
#define l_settop( i ) \
  lua_settop( l, i )

/**
 * @def l_pop
 * Shorthand for lua_pop
 */
#define l_pop( i ) \
  lua_pop( l, i )

/**
 * @def l_type
 * Shorthand for lua_type
 */
#define l_type( i ) \
  lua_type( l, i )

/**
 * @def l_tobool
 * Shorthand for lua_toboolean (plus cast to bool)
 */
#define l_tobool( i ) \
  ( lua_toboolean( l, i ) != 0 )

/**
 * @def l_toint
 * Shorthand for lua_tointeger (plus cast to int)
 */
#define l_toint( i ) \
  int( lua_tointeger( l, i ) )

/**
 * @def l_tofloat
 * Shorthand for lua_tonumber (plus cast to float)
 */
#define l_tofloat( i ) \
  float( lua_tonumber( l, i ) )

/**
 * @def l_todouble
 * Shorthand for lua_tonumber
 */
#define l_todouble( i ) \
  lua_tonumber( l, i )

/**
 * @def l_tostring
 * Shorthand for lua_tostring
 */
#define l_tostring( i ) \
  lua_tolstring( l, i, nullptr )

/**
 * @def l_tolstring
 * Shorthand for lua_tolstring
 */
#define l_tolstring( i, len ) \
  lua_tolstring( l, i, len )

/**
 * @def l_pushnil
 * Shorthand for lua_pushnil
 */
#define l_pushnil() \
  lua_pushnil( l )

/**
 * @def l_pushbool
 * Shorthand for lua_pushboolean
 */
#define l_pushbool( b ) \
  lua_pushboolean( l, b )

/**
 * @def l_pushint
 * Shorthand for lua_pushinteger
 */
#define l_pushint( i ) \
  lua_pushinteger( l, i )

/**
 * @def l_pushfloat
 * Shorthand for lua_pushnumber
 */
#define l_pushfloat( f ) \
  lua_pushnumber( l, f )

/**
 * @def l_pushdouble
 * Shorthand for lua_pushnumber
 */
#define l_pushdouble( d ) \
  lua_pushnumber( l, d )

/**
 * @def l_pushstring
 * Shorthand for lua_pushstring
 */
#define l_pushstring( s ) \
  lua_pushstring( l, s )

/**
 * @def l_pushvalue
 * Shorthand for lua_pushvalue
 */
#define l_pushvalue( i ) \
  lua_pushvalue( l, i )

/**
 * @def l_pushglobaltable
 * Shorthand for lua_pushglobaltable
 */
#define l_pushglobaltable() \
  lua_pushglobaltable( l )

/**
 * @def l_newtable
 * Shorthand for lua_newtable
 */
#define l_newtable() \
  lua_newtable( l )

/**
 * @def l_next
 * Shorthand for lua_next
 */
#define l_next( t ) \
  lua_next( l, t )

/**
 * @def l_rawget
 * Shorthand for lua_rawget
 */
#define l_rawget( t ) \
  lua_rawget( l, t )

/**
 * @def l_rawset
 * Shorthand for lua_rawset
 */
#define l_rawset( t ) \
  lua_rawset( l, t )

/**
 * @def l_rawgeti
 * Shorthand for lua_rawgeti
 */
#define l_rawgeti( t, i ) \
  lua_rawgeti( l, t, i )

/**
 * @def l_rawseti
 * Shorthand for lua_rawseti
 */
#define l_rawseti( t, i ) \
  lua_rawseti( l, t, i )

/**
 * @def l_getglobal
 * Shorthand for lua_getglobal
 */
#define l_getglobal( n ) \
  lua_getglobal( l, n )

/**
 * @def l_setglobal
 * Shorthand for lua_setglobal
 */
#define l_setglobal( n ) \
  lua_setglobal( l, n )

/**
 * @def l_register
 * Shorthand for lua_register
 */
#define l_register( name, func ) \
  lua_register( l, name, func )

/**
 * @def l_pcall
 * Shorthand for lua_pcall
 */
#define l_pcall( nArg, nRet ) \
  lua_pcall( l, nArg, nRet, 0 )

/**
 * @def l_dobuffer
 * Loads (executes) %Lua code from a given buffer.
 */
#define l_dobuffer( begin, length, name ) \
  ( luaL_loadbuffer( l, begin, size_t( length ), name ) || lua_pcall( l, 0, LUA_MULTRET, 0 ) )

/**
 * @def l_dostring
 * Loads (executes) %Lua code from a given null-terminated string.
 */
#define l_dostring( code ) \
  luaL_dostring( l, code )

namespace oz
{
namespace common
{

/**
 * Overload for registering boolean %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 */
void registerLuaConstant( lua_State* l, const char* name, bool value );

/**
 * Overload for registering integer %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 */
void registerLuaConstant( lua_State* l, const char* name, int value );

/**
 * Overload for registering number %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 */
void registerLuaConstant( lua_State* l, const char* name, float value );

/**
 * Overload for registering string %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 */
void registerLuaConstant( lua_State* l, const char* name, const char* value );

}
}
