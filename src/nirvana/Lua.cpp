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

  static int ozBindAABBOverlaps( lua_State *l )
  {
    AABB aabb = AABB( Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ),
                      Vec3( lua_tonumber( l, 4 ), lua_tonumber( l, 5 ), lua_tonumber( l, 6 ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.index = 0;
    return 0;
  }

  static int ozBindOverlaps( lua_State *l )
  {
    Vec3  p = ( *lua.obj )->p;
    float dim = lua_tonumber( l, 1 );
    AABB aabb = AABB( p, Vec3( dim, dim, dim ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.index = 0;
    return 0;
  }

  static int ozBindNext( lua_State *l )
  {
    if( lua.index < lua.objects.length() ) {
      lua.obj = &lua.objects[lua.index];
      lua.index++;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  static int ozIsSelf( lua_State *l )
  {
    lua_pushboolean( l, *lua.obj == lua.self );
    return 1;
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
    lua_pushboolean( l, ( *lua.obj )->cell != null );
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

  static int ozGetHeadingTo( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == reinterpret_cast<Object**>( &lua.self ) ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.self->p.x - ( *lua.obj )->p.x;
    float dy = ( *lua.obj )->p.y - lua.self->p.y;
    float angle = Math::deg( Math::atan2( dx, dy ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozGetDistanceTo( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == reinterpret_cast<Object**>( &lua.self ) ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.self->p.x - ( *lua.obj )->p.x;
    float dy = lua.self->p.y - ( *lua.obj )->p.y;
    float angle = Math::sqrt( dx*dx + dy*dy );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozGetIndex( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, ( *lua.obj )->index );
    return 1;
  }

  static int ozGetType( lua_State *l )
  {
    if( *lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushstring( l, ( *lua.obj )->type->name );
    return 1;
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

  static int ozActionUse( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_USE;
    return 0;
  }

  static int ozActionTake( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_TAKE;
    return 0;
  }

  static int ozActionGrab( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  static int ozActionThrow( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_THROW;
    return 0;
  }

  static int ozActionSuicide( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_SUICIDE;
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

  void Lua::callFunc( const char *functionName, int botIndex )
  {
    assert( self != null );
    assert( lua_gettop( l ) == 1 );

    obj    = reinterpret_cast<Object**>( &self );
    target = null;

    lua_getglobal( l, functionName );
    lua_rawgeti( l, 1, botIndex );
    lua_pcall( l, 1, 0, 0 );

    if( lua_gettop( l ) > 1 && lua_isstring( l, -1 ) ) {
      log.println( "N! %s", lua_tostring( l, -1 ) );
      lua_settop( l, 0 );
      lua_getglobal( l, "ozMindData" );
    }
  }

  void Lua::registerMind( int botIndex )
  {
    assert( lua_gettop( l ) == 1 );

    lua_newtable( l );
    lua_rawseti( l, 1, botIndex );
  }

  void Lua::unregisterMind( int botIndex )
  {
    assert( lua_gettop( l ) == 1 );

    lua_pushnil( l );
    lua_rawseti( l, 1, botIndex );
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
    OZ_LUA_REGISTER( ozBindOverlaps );
    OZ_LUA_REGISTER( ozBindAABBOverlaps );
    OZ_LUA_REGISTER( ozBindNext );

    OZ_LUA_REGISTER( ozIsSelf );
    OZ_LUA_REGISTER( ozIsNull );
    OZ_LUA_REGISTER( ozIsPut );
    OZ_LUA_REGISTER( ozIsDynObj );
    OZ_LUA_REGISTER( ozIsBot );

    OZ_LUA_REGISTER( ozGetPos );
    OZ_LUA_REGISTER( ozGetDim );

    OZ_LUA_REGISTER( ozGetHeadingTo );
    OZ_LUA_REGISTER( ozGetDistanceTo );

    OZ_LUA_REGISTER( ozGetIndex );
    OZ_LUA_REGISTER( ozGetType );

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
    OZ_LUA_REGISTER( ozActionUse );
    OZ_LUA_REGISTER( ozActionTake );
    OZ_LUA_REGISTER( ozActionGrab );
    OZ_LUA_REGISTER( ozActionThrow );
    OZ_LUA_REGISTER( ozActionSuicide );

    OZ_LUA_REGISTER( ozStateIsRunning );
    OZ_LUA_REGISTER( ozStateSetRunning );
    OZ_LUA_REGISTER( ozStateToggleRunning );

    lua_newtable( l );
    lua_setglobal( l, "ozMindData" );
    lua_getglobal( l, "ozMindData" );

    for( int i = 0; i < translator.nirvanaScripts.length(); i++ ) {
      const Translator::Resource &res = translator.nirvanaScripts[i];

      log.print( "Processing '%s' ...", res.path.cstr() );

      if( luaL_dofile( l, res.path ) != 0 ) {
        log.printEnd( " Failed" );
        throw Exception( "Nirvana Lua script error" );
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
