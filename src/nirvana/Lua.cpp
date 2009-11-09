/*
 *  Lua.cpp
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Lua.h"

#include "nirvana.h"
#include "matrix/BotClass.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define OZ_LUA_ERROR( message ) luaL_error( l, "[%s] %s", __FUNCTION__, message );
#define OZ_LUA_REGISTER( func ) lua_register( l, #func, func )

namespace oz
{
namespace nirvana
{

  Lua lua;

  static int ozPrintln( lua_State *l )
  {
    log.println( "N> %s", lua_tostring( l, 1 ) );
    return 0;
  }

  static int ozException( lua_State *l )
  {
    const char *message = lua_tostring( l, 1 );
    throw Exception( message );
  }

  static int ozBindSelf( lua_State* )
  {
    lua.obj = reinterpret_cast<Object**>( &lua.self );
    return 0;
  }

  static int ozBindTarget( lua_State* )
  {
    lua.obj = &lua.target;
    return 0;
  }

  static int ozIsNull( lua_State *l )
  {
    lua_pushboolean( l, *lua.obj == null );
    return 1;
  }

  static int ozIsPut( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua_pushboolean( l, ( *lua.obj )->cell != null && ( ~( *lua.obj )->flags & Object::CUT_BIT ) );
    return 1;
  }

  static int ozIsDynObj( lua_State *l )
  {
    lua_pushboolean( l, *lua.obj != null && ( ( *lua.obj )->flags & Object::DYNAMIC_BIT ) );
    return 1;
  }

  static int ozIsBot( lua_State *l )
  {
    lua_pushboolean( l, *lua.obj != null && ( ( *lua.obj )->flags & Object::BOT_BIT ) );
    return 1;
  }

  static int ozGetPos( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, ( *lua.obj )->p.x );
    lua_pushnumber( l, ( *lua.obj )->p.y );
    lua_pushnumber( l, ( *lua.obj )->p.z );
    return 3;
  }

  static int ozGetDim( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, ( *lua.obj )->dim.x );
    lua_pushnumber( l, ( *lua.obj )->dim.y );
    lua_pushnumber( l, ( *lua.obj )->dim.z );
    return 3;
  }

  static int ozGetLife( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, ( *lua.obj )->life );
    return 1;
  }

  static int ozGetStamina( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~( *lua.obj )->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a Bot" );
    }

    Bot *bot = static_cast<Bot*>( *lua.obj );
    lua_pushnumber( l, bot->stamina );
    return 1;
  }

  static int ozGetH( lua_State *l )
  {
    lua_pushnumber( l, lua.self->h );
    return 1;
  }

  static int ozSetH( lua_State *l )
  {
    lua.self->h = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozAddH( lua_State *l )
  {
    lua.self->h += lua_tonumber( l, 1 );
    return 1;
  }

  static int ozGetV( lua_State *l )
  {
    lua_pushnumber( l, lua.self->v );
    return 1;
  }

  static int ozSetV( lua_State *l )
  {
    lua.self->v = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozAddV( lua_State *l )
  {
    lua.self->v += lua_tonumber( l, 1 );
    return 1;
  }

  static int ozActionForward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_FORWARD;
    return 0;
  }

  static int ozActionBackward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_BACKWARD;
    return 0;
  }

  static int ozActionRight( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_RIGHT;
    return 0;
  }

  static int ozActionLeft( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_LEFT;
    return 0;
  }

  static int ozActionJump( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_JUMP;
    return 0;
  }

  static int ozActionCrouch( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_CROUCH;
    return 0;
  }

  static int ozActionGrab( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  static int ozStateIsRunning( lua_State *l )
  {
    lua_pushboolean( l, lua.self->state & Bot::RUNNING_BIT );
    return 1;
  }

  static int ozStateSetRunning( lua_State *l )
  {
    if( lua_toboolean( l, 1 ) ) {
      lua.self->state |= Bot::RUNNING_BIT;
    }
    else {
      lua.self->state &= ~Bot::RUNNING_BIT;
    }
    return 0;
  }

  static int ozStateToggleRunning( lua_State* )
  {
    lua.self->state ^= Bot::RUNNING_BIT;
    return 0;
  }

  void Lua::callFunc( const char *functionName )
  {
    assert( self != null );

    obj    = reinterpret_cast<Object**>( &self );
    target = null;

    lua_getglobal( l, functionName );
    lua_pcall( l, 0, 0, 0 );

    if( lua_gettop( l ) != 0 ) {
      if( lua_isstring( l, 1 ) ) {
        log.println( "N! %s", lua_tostring( l, 1 ) );
      }
      lua_settop( l, 0 );
    }
  }

  void Lua::init()
  {
    log.println( "Initializing Nirvana Lua {" );
    log.indent();

    l = lua_open();
    luaL_openlibs( l );

    OZ_LUA_REGISTER( ozPrintln );
    OZ_LUA_REGISTER( ozException );

    OZ_LUA_REGISTER( ozBindSelf );
    OZ_LUA_REGISTER( ozBindTarget );
    OZ_LUA_REGISTER( ozIsNull );
    OZ_LUA_REGISTER( ozIsPut );
    OZ_LUA_REGISTER( ozIsDynObj );
    OZ_LUA_REGISTER( ozIsBot );

    OZ_LUA_REGISTER( ozGetPos );
    OZ_LUA_REGISTER( ozGetDim );
    OZ_LUA_REGISTER( ozGetLife );
    OZ_LUA_REGISTER( ozGetStamina );

    OZ_LUA_REGISTER( ozGetH );
    OZ_LUA_REGISTER( ozSetH );
    OZ_LUA_REGISTER( ozAddH );
    OZ_LUA_REGISTER( ozGetV );
    OZ_LUA_REGISTER( ozSetV );
    OZ_LUA_REGISTER( ozAddV );

    OZ_LUA_REGISTER( ozActionForward );
    OZ_LUA_REGISTER( ozActionBackward );
    OZ_LUA_REGISTER( ozActionRight );
    OZ_LUA_REGISTER( ozActionLeft );
    OZ_LUA_REGISTER( ozActionJump );
    OZ_LUA_REGISTER( ozActionCrouch );
    OZ_LUA_REGISTER( ozActionGrab );

    OZ_LUA_REGISTER( ozStateIsRunning );
    OZ_LUA_REGISTER( ozStateSetRunning );
    OZ_LUA_REGISTER( ozStateToggleRunning );

    for( int i = 0; i < translator.nirvanaScripts.length(); i++ ) {
      const Translator::Resource &res = translator.nirvanaScripts[i];

      log.print( "Processing '%s' ...", res.path.cstr() );

      if( luaL_dofile( l, res.path ) != 0 ) {
        log.printEnd( " Failed" );
      }
      else {
        log.printEnd( " OK" );
      }
    }

    log.unindent();
    log.println( "}" );
  }

  void Lua::free()
  {
    log.print( "Freeing Nirvana Lua ..." );
    lua_close( l );
    log.printEnd( " OK" );
  }

}
}
