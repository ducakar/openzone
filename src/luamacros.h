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

#define API( func ) \
  int Lua::func( lua_State* l )

#define ERROR( message ) \
  luaL_error( l, "[%s] %s", __FUNCTION__, message )

#define OBJ( obj, i ) \
  if( uint( i ) >= uint( orbis.objects.length() ) ) { \
    luaL_error( l, "object index %d out of bounds", i ); \
  } \
  Object* obj = orbis.objects[i]; \
  if( obj == null ) { \
    luaL_error( l, "bound object at %d is null", i ); \
  }

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
