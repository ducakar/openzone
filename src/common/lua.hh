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
 */

#pragma once

#include <lua.hpp>

// Doxygen should skip those macros, we don't want documentation generated for them.
#ifndef OZ_DOXYGEN

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

#define IMPORT_FUNC( func ) \
  lua.registerFunction( #func, func )

#define IGNORE_FUNC( func ) \
  static_cast<void>( func )

#define IMPORT_CONST( name, value ) \
  lua.registerConstant( name, value )

#define IMPORT_BUFFER( begin, length, name ) \
  ( luaL_loadbuffer( l, begin, size_t( length ), name ) || lua_pcall( l, 0, LUA_MULTRET, 0 ) )

#define ARG( n ) \
  hard_assert( lua_gettop( l ) == ( n ) ); \
  static_cast<void>( l )

#define ARG_VAR( n ) \
  hard_assert( lua_gettop( l ) >= ( n ) ); \
  static_cast<void>( l );

#define ERROR( message ) \
  luaL_error( l, "%s: %s", __PRETTY_FUNCTION__, message )

#define STR_NOT_NULL() \
  if( ms.str == null ) { \
    ERROR( "bound structure is null" ); \
  }

#define EVENT_NOT_NULL() \
  if( !s.event.isValid() ) { \
    ERROR( "bound event is null" ); \
  }

#define OBJ_NOT_NULL() \
  if( ms.obj == null ) { \
    ERROR( "bound object is null" ); \
  }

#define OBJ_NOT_SELF() \
  if( ms.obj == ms.self ) { \
    ERROR( "bound object if self" ); \
  }

#define OBJ_DYNAMIC() \
  if( !( ms.obj->flags & Object::DYNAMIC_BIT ) ) { \
    ERROR( "bound object is not dynamic" ); \
  } \
  Dynamic* dyn = static_cast<Dynamic*>( ms.obj );

#define OBJ_WEAPON() \
  if( !( ms.obj->flags & Object::WEAPON_BIT ) ) { \
    ERROR( "bound object is not a weapon" ); \
  } \
  Weapon* weapon = static_cast<Weapon*>( ms.obj );

#define OBJ_BOT() \
  if( !( ms.obj->flags & Object::BOT_BIT ) ) { \
    ERROR( "bound object is not a bot" ); \
  } \
  Bot* bot = static_cast<Bot*>( ms.obj );

#define OBJ_VEHICLE() \
  if( !( ms.obj->flags & Object::VEHICLE_BIT ) ) { \
    ERROR( "bound object is not a vehicle" ); \
  } \
  Vehicle* vehicle = static_cast<Vehicle*>( ms.obj );

#define FRAG_NOT_NULL() \
  if( ms.frag == null ) { \
    ERROR( "bound fragment is null" ); \
  }

#define SELF_BOT() \
  if( ms.self == null || !( ms.self->flags & Object::BOT_BIT ) ) { \
    ERROR( "self is not a bot" ); \
  } \
  Bot* self = static_cast<Bot*>( ms.self );

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

#endif // OZ_DOXYGEN_SKIP
