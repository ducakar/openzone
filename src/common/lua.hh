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
 * @file lua.hh
 *
 * Handy macros and methods for %Lua API implementation.
 *
 * This is internal header, it should only be included by modules that contain %Lua API
 * implementation. It contains macros for parameter checking, reporting errors, checking validity of
 * %Lua API calls etc. that can be used as some basic building blocks for %Lua APIs. Furthermore
 * there are macros that wrap %Lua library calls so, one can e.g. write
 * @code
 *   tofloat( 1 );
 * @endcode
 * instead of
 * @code
 *   float( lua_tonumber( l, 1 ) );
 * @endcode
 * Finally it defines overloaded methods for registering %Lua API constants that simplify code in
 * similar way as the macro above.
 */

#pragma once

#include <lua.hpp>

/// @addtogroup common
/// @{

/**
 * @def IMPORT_LIBS
 * Loads standard table, string and math libraries.
 */
#if LUA_VERSION_NUM < 502
# define IMPORT_LIBS() \
    lua_pushcfunction( l, luaopen_table ); \
    lua_pushcfunction( l, luaopen_string ); \
    lua_pushcfunction( l, luaopen_math ); \
    lua_pcall( l, 0, 0, 0 ); \
    lua_pcall( l, 0, 0, 0 ); \
    lua_pcall( l, 0, 0, 0 );
#else
# define IMPORT_LIBS() \
    luaL_requiref( l, LUA_TABLIBNAME,  luaopen_table,  true ); \
    luaL_requiref( l, LUA_STRLIBNAME,  luaopen_string, true ); \
    luaL_requiref( l, LUA_MATHLIBNAME, luaopen_math,   true ); \
    lua_settop( l, 0 );
#endif

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
 * @def IMPORT_BUFFER
 * Loads (executes) %Lua code from the given buffer.
 */
#define IMPORT_BUFFER( begin, length, name ) \
  ( luaL_loadbuffer( l, begin, size_t( length ), name ) || lua_pcall( l, 0, LUA_MULTRET, 0 ) )

/**
 * @def ERROR
 * Exits %Lua API function call with the giver error message.
 */
#define ERROR( message ) \
  luaL_error( l, "%s: %s", __func__, message )

/**
 * @def ARG( n )
 * Exits %Lua API function call with an error if number of parameters is not <tt>n</tt>.
 */
#define ARG( n ) \
  if( lua_gettop( l ) != ( n ) ) { \
    ERROR( #n " arguments expected" ); \
  }

/**
 * @def ARG_VAR( n )
 * Exits %Lua API function call with an error if number of parameters is less than <tt>n</tt>.
 */
#define ARG_VAR( n ) \
  if( lua_gettop( l ) < ( n ) ) { \
    ERROR( "At least " #n " arguments expected" ); \
  }

/**
 * @def STR
 * Exits %Lua API function call with an error if there is no structure bound.
 */
#define STR() \
  if( ms.str == null ) { \
    ERROR( "No structure bound" ); \
  }

/**
 * @def OBJ
 * Exits %Lua API function call with an error if there is no object bound.
 */
#define OBJ() \
  if( ms.obj == null ) { \
    ERROR( "No object bound" ); \
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
  Vehicle* vehicle = static_cast<Vehicle*>( ms.obj );

/**
 * @def FRAG
 * Exits %Lua API function call with an error if there is no fragment bound.
 */
#define FRAG() \
  if( ms.frag == null ) { \
    ERROR( "No fragment bound" ); \
  }

/**
 * @def STR
 * Exits %Lua API function call with an error if self object is not bound or not a bot (Bot class).
 */
#define SELF_BOT() \
  if( ms.self == null || !( ms.self->flags & Object::BOT_BIT ) ) { \
    ERROR( "Self object is not a bot" ); \
  } \
  Bot* self = static_cast<Bot*>( ms.self );

/**
 * @def gettop
 * Shorthand for lua_gettop
 */
#define gettop() \
  lua_gettop( l )

/**
 * @def settop
 * Shorthand for lua_settop
 */
#define settop( i ) \
  lua_settop( l, i )

/**
 * @def pop
 * Shorthand for lua_pop
 */
#define pop( i ) \
  lua_pop( l, i )

/**
 * @def type
 * Shorthand for lua_type
 */
#define type( i ) \
  lua_type( l, i )

/**
 * @def tobool
 * Shorthand for lua_toboolean (plus cast to bool)
 */
#define tobool( i ) \
  ( lua_toboolean( l, i ) != 0 )

/**
 * @def toint
 * Shorthand for lua_tointeger (plus cast to int)
 */
#define toint( i ) \
  int( lua_tointeger( l, i ) )

/**
 * @def tofloat
 * Shorthand for lua_tonumber (plus cast to float)
 */
#define tofloat( i ) \
  float( lua_tonumber( l, i ) )

/**
 * @def todouble
 * Shorthand for lua_tonumber
 */
#define todouble( i ) \
  lua_tonumber( l, i )

/**
 * @def tostring
 * Shorthand for lua_tostring
 */
#define tostring( i ) \
  lua_tostring( l, i )

/**
 * @def pushnil
 * Shorthand for lua_pushnil
 */
#define pushnil() \
  lua_pushnil( l )

/**
 * @def pushbool
 * Shorthand for lua_pushboolean
 */
#define pushbool( b ) \
  lua_pushboolean( l, b )

/**
 * @def pushint
 * Shorthand for lua_pushinteger
 */
#define pushint( i ) \
  lua_pushinteger( l, i )

/**
 * @def pushfloat
 * Shorthand for lua_pushnumber
 */
#define pushfloat( f ) \
  lua_pushnumber( l, f )

/**
 * @def pushdouble
 * Shorthand for lua_pushnumber
 */
#define pushdouble( d ) \
  lua_pushnumber( l, d )

/**
 * @def pushstring
 * Shorthand for lua_pushstring
 */
#define pushstring( s ) \
  lua_pushstring( l, s )

/**
 * @def pushvalue
 * Shorthand for lua_pushvalue
 */
#define pushvalue( i ) \
  lua_pushvalue( l, i )

/**
 * @def newtable
 * Shorthand for lua_newtable
 */
#define newtable() \
  lua_newtable( l )

/**
 * @def next
 * Shorthand for lua_next
 */
#define next( t ) \
  lua_next( l, t )

/**
 * @def rawget
 * Shorthand for lua_rawget
 */
#define rawget( t ) \
  lua_rawget( l, t )

/**
 * @def rawset
 * Shorthand for lua_rawset
 */
#define rawset( t ) \
  lua_rawset( l, t )

/**
 * @def rawgeti
 * Shorthand for lua_rawgeti
 */
#define rawgeti( t, i ) \
  lua_rawgeti( l, t, i )

/**
 * @def rawseti
 * Shorthand for lua_rawseti
 */
#define rawseti( t, i ) \
  lua_rawseti( l, t, i )

/**
 * @def getglobal
 * Shorthand for lua_getglobal
 */
#define getglobal( n ) \
  lua_getglobal( l, n )

/**
 * @def setglobal
 * Shorthand for lua_setglobal
 */
#define setglobal( n ) \
  lua_setglobal( l, n )

/// @}

namespace oz
{

/**
 * Overload for registering boolean %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 *
 * @ingroup common
 */
void registerLuaConstant( lua_State* l, const char* name, bool value );

/**
 * Overload for registering integer %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 *
 * @ingroup common
 */
void registerLuaConstant( lua_State* l, const char* name, int value );

/**
 * Overload for registering number %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 *
 * @ingroup common
 */
void registerLuaConstant( lua_State* l, const char* name, float value );

/**
 * Overload for registering string %Lua constant.
 *
 * This method is only intended for use inside methods that register %Lua constants.
 *
 * @ingroup common
 */
void registerLuaConstant( lua_State* l, const char* name, const char* value );

}
