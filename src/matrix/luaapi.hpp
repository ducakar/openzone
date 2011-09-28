/*
 *  luamacros.h
 *
 *  Utility Lua macros for API functions. They don't include prefixes, so are not safe to be
 *  included in general.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "matrix/common.hpp"

#include <lua.hpp>

#define OZ_LUA_FUNC( func ) \
  oz::registerLuaFunction( l, #func, func )

#define OZ_LUA_CONST( name, value ) \
  oz::registerLuaConstant( l, name, value )

#define ARG( n ) \
  hard_assert( lua_gettop( l ) == ( n ) ); \
  static_cast<void>( l )

#define ERROR( message ) \
  luaL_error( l, "%s: %s", __FUNCTION__, message )

#define STR_NOT_NULL() \
  if( lua.str == null ) { \
    ERROR( "bound structure is null" ); \
  }

#define EVENT_NOT_NULL() \
  if( !lua.event.isValid() ) { \
    ERROR( "bound event is null" ); \
  }

#define OBJ_NOT_NULL() \
  if( lua.obj == null ) { \
    ERROR( "bound object is null" ); \
  }

#define OBJ_NOT_SELF() \
  if( lua.obj == lua.self ) { \
    ERROR( "bound object if self" ); \
  }

#define OBJ_DYNAMIC() \
  if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) { \
    ERROR( "bound object is not dynamic" ); \
  } \
  Dynamic* dyn = static_cast<Dynamic*>( lua.obj );

#define OBJ_WEAPON() \
  if( !( lua.obj->flags & Object::WEAPON_BIT ) ) { \
    ERROR( "bound object is not a weapon" ); \
  } \
  Weapon* weapon = static_cast<Weapon*>( lua.obj );

#define OBJ_BOT() \
  if( !( lua.obj->flags & Object::BOT_BIT ) ) { \
    ERROR( "bound object is not a bot" ); \
  } \
  Bot* bot = static_cast<Bot*>( lua.obj );

#define OBJ_VEHICLE() \
  if( !( lua.obj->flags & Object::VEHICLE_BIT ) ) { \
    ERROR( "bound object is not a vehicle" ); \
  } \
  Vehicle* vehicle = static_cast<Vehicle*>( lua.obj );

#define PART_NOT_NULL() \
  if( lua.part == null ) { \
    ERROR( "bound particle is null" ); \
  }

#define SELF_BOT() \
  if( lua.self == null || !( lua.self->flags & Object::BOT_BIT ) ) { \
    ERROR( "self is not a bot" ); \
  } \
  Bot* self = static_cast<Bot*>( lua.self );

#define settop( i )             lua_settop( l, i )
#define gettop()                lua_gettop( l )
#define pop( i )                lua_pop( l, i )

#define type( i )               lua_type( l, i )
#define tobool( i )             ( lua_toboolean( l, i ) != 0 )
#define toint( i )              int( lua_tointeger( l, i ) )
#define tofloat( i )            float( lua_tonumber( l, i ) )
#define todouble( i )           lua_tonumber( l, i )
#define tostring( i )           lua_tostring( l, i )

#define pushnil()               lua_pushnil( l )
#define pushbool( b )           lua_pushboolean( l, b )
#define pushint( i )            lua_pushinteger( l, i )
#define pushfloat( f )          lua_pushnumber( l, f )
#define pushdouble( d )         lua_pushnumber( l, d )
#define pushstring( s )         lua_pushstring( l, s )
#define pushvalue( i )          lua_pushvalue( l, i )

#define newtable()              lua_newtable( l )
#define next( t )               lua_next( l, t )
#define rawget( t )             lua_rawget( l, t )
#define rawset( t )             lua_rawset( l, t )
#define rawgeti( t, i )         lua_rawgeti( l, t, i )
#define rawseti( t, i )         lua_rawseti( l, t, i )

#define getglobal( n )          lua_getglobal( l, n )
#define setglobal( n )          lua_setglobal( l, n )

namespace oz
{

  void registerLuaFunction( lua_State* l, const char* name, LuaAPI func );
  void registerLuaConstant( lua_State* l, const char* name, bool value );
  void registerLuaConstant( lua_State* l, const char* name, int value );
  void registerLuaConstant( lua_State* l, const char* name, float value );
  void registerLuaConstant( lua_State* l, const char* name, const char* value );

}
