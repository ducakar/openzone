/*
 *  MatrixLua.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "MatrixLua.h"

#include "Bot.h"
#include "BotClass.h"
#include "Synapse.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define OZ_LUA_REGISTER( func ) lua_register( l, #func, func )

namespace oz
{

  MatrixLua matrixLua;

  static lua_State *l;

  static int ozPrintln( lua_State *l )
  {
    log.println( "L> %s", lua_tostring( l, 1 ) );
    return 0;
  }

  static int ozException( lua_State *l )
  {
    const char *message = lua_tostring( l, 1 );
    log.println( "L! %s", message );
    throw Exception( message );
  }

  static int ozBindTarget( lua_State* )
  {
    matrixLua.obj = matrixLua.target;
    return 0;
  }

  static int ozBindUser( lua_State *l )
  {
    if( matrixLua.user == null ) {
      lua_pushstring( l, "user is null" );
      lua_error( l );
    }
    else {
      matrixLua.obj = matrixLua.user;
    }
    return 0;
  }

  static int ozIsObj( lua_State *l )
  {
    lua_pushboolean( l, matrixLua.obj != null );
    return 1;
  }

  static int ozIsDynObj( lua_State *l )
  {
    lua_pushboolean( l, matrixLua.obj != null && ( matrixLua.obj->flags & Object::DYNAMIC_BIT ) );
    return 1;
  }

  static int ozIsBot( lua_State *l )
  {
    lua_pushboolean( l, matrixLua.obj != null && ( matrixLua.obj->flags & Object::BOT_BIT ) );
    return 1;
  }

  static int ozGetP( lua_State *l )
  {
    lua_pushnumber( l, matrixLua.obj->p.x );
    lua_pushnumber( l, matrixLua.obj->p.y );
    lua_pushnumber( l, matrixLua.obj->p.z );
    return 3;
  }

  static int ozSetP( lua_State *l )
  {
    matrixLua.obj->p.x = lua_tonumber( l, 1 );
    matrixLua.obj->p.y = lua_tonumber( l, 2 );
    matrixLua.obj->p.z = lua_tonumber( l, 3 );
    return 0;
  }

  static int ozAddP( lua_State *l )
  {
    matrixLua.obj->p.x += lua_tonumber( l, 1 );
    matrixLua.obj->p.y += lua_tonumber( l, 2 );
    matrixLua.obj->p.z += lua_tonumber( l, 3 );
    return 0;
  }

  static int ozGetDim( lua_State *l )
  {
    lua_pushnumber( l, matrixLua.obj->dim.x );
    lua_pushnumber( l, matrixLua.obj->dim.y );
    lua_pushnumber( l, matrixLua.obj->dim.z );
    return 3;
  }

  static int ozGetLife( lua_State *l )
  {
    lua_pushnumber( l, matrixLua.obj->life );
    return 1;
  }

  static int ozSetLife( lua_State *l )
  {
    matrixLua.obj->life = bound( lua_tonumber( l, 1 ), 0, matrixLua.obj->type->life );
    return 0;
  }

  static int ozAddLife( lua_State *l )
  {
    matrixLua.obj->life = bound( matrixLua.obj->life + lua_tonumber( l, 1 ),
                                 0,
                                 matrixLua.obj->type->life );
    return 0;
  }

  static int ozGetStamina( lua_State *l )
  {
    if( ~matrixLua.obj->flags & Object::BOT_BIT ) {
      lua_pushstring( l, "cannot call ozGetStamina on non-Bot" );
      lua_error( l );
    }
    else {
      Bot *bot = static_cast<Bot*>( matrixLua.obj );

      lua_pushnumber( l, bot->stamina );
    }
    return 1;
  }

  static int ozSetStamina( lua_State *l )
  {
    if( ~matrixLua.obj->flags & Object::BOT_BIT ) {
      lua_pushstring( l, "cannot call ozGetStamina on non-Bot" );
      lua_error( l );
    }
    else {
      Bot *bot = static_cast<Bot*>( matrixLua.obj );
      BotClass *clazz = static_cast<BotClass*>( bot->type );

      bot->stamina = bound( lua_tonumber( l, 1 ), 0, clazz->stamina );
    }
    return 0;
  }

  static int ozAddStamina( lua_State *l )
  {
    if( ~matrixLua.obj->flags & Object::BOT_BIT ) {
      lua_pushstring( l, "cannot call ozGetStamina on non-Bot" );
      lua_error( l );
    }
    else {
      Bot *bot = static_cast<Bot*>( matrixLua.obj );
      BotClass *clazz = static_cast<BotClass*>( bot->type );

      bot->stamina = bound( bot->stamina + lua_tonumber( l, 1 ), 0, clazz->stamina );
    }
    return 0;
  }

  static int ozRemove( lua_State* )
  {
    synapse.remove( matrixLua.obj );
    return 0;
  }

  static int ozAddObj( lua_State *l )
  {
    synapse.addObject( lua_tostring( l, 1 ),
                       Vec3( lua_tonumber( l, 2 ), lua_tonumber( l, 3 ), lua_tonumber( l, 4 ) ) );
    return 0;
  }

  void MatrixLua::callFunc( const char *functionName )
  {
    assert( obj != null && target != null );

    lua_getglobal( l, functionName );
    lua_pcall( l, 0, 0, 0 );

    if( lua_gettop( l ) != 0 ) {
      if( lua_isstring( l, 1 ) ) {
        log.println( "L! %s", lua_tostring( l, 1 ) );
      }
      do {
        lua_pop( l, 1 );
      }
      while( lua_gettop( l ) != 0 );
    }
  }

  void MatrixLua::init()
  {
    l = lua_open();
    luaL_openlibs( l );

    OZ_LUA_REGISTER( ozPrintln );
    OZ_LUA_REGISTER( ozException );

    OZ_LUA_REGISTER( ozBindTarget );
    OZ_LUA_REGISTER( ozBindUser );
    OZ_LUA_REGISTER( ozIsObj );
    OZ_LUA_REGISTER( ozIsDynObj );
    OZ_LUA_REGISTER( ozIsBot );

    OZ_LUA_REGISTER( ozGetP );
    OZ_LUA_REGISTER( ozSetP );
    OZ_LUA_REGISTER( ozAddP );
    OZ_LUA_REGISTER( ozGetDim );

    OZ_LUA_REGISTER( ozGetLife );
    OZ_LUA_REGISTER( ozSetLife );
    OZ_LUA_REGISTER( ozAddLife );

    OZ_LUA_REGISTER( ozGetStamina );
    OZ_LUA_REGISTER( ozSetStamina );
    OZ_LUA_REGISTER( ozAddStamina );

    OZ_LUA_REGISTER( ozRemove );

    OZ_LUA_REGISTER( ozAddObj );

    luaL_dofile( l, "lua/handlers.lua" );
  }

  void MatrixLua::free()
  {
    lua_close( l );
  }

}
