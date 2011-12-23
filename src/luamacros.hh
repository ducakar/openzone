/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file luamacros.hh
 */

#pragma once

#include <lua.hpp>

#define OZ_LUA_DOBUFFER( begin, length, name ) \
  ( luaL_loadbuffer( l, begin, size_t( length ), name ) || lua_pcall( l, 0, LUA_MULTRET, 0 ) )

#define OZ_LUA_FUNC( func ) \
  lua.registerFunction( #func, func )

#define OZ_LUA_CONST( name, value ) \
  lua.registerConstant( name, value )

#define ARG( n ) \
  hard_assert( lua_gettop( l ) == ( n ) ); \
  static_cast<void>( l )

#define ARG_VAR( n ) \
  hard_assert( lua_gettop( l ) >= ( n ) ); \
  static_cast<void>( l );

#define ERROR( message ) \
  luaL_error( l, "%s: %s", __PRETTY_FUNCTION__, message )

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

#define FRAG_NOT_NULL() \
  if( lua.frag == null ) { \
    ERROR( "bound fragment is null" ); \
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
