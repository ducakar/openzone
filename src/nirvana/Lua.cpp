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

  static int ozBindAllOverlaps( lua_State *l )
  {
    AABB aabb = AABB( Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ),
                      Vec3( lua_tonumber( l, 4 ), lua_tonumber( l, 5 ), lua_tonumber( l, 6 ) ) );
    lua.objects.clear();
    lua.structs.clear();
    collider.getOverlaps( aabb, &lua.objects, &lua.structs );
    lua.objIndex = 0;
    lua.strIndex = 0;
    return 0;
  }

  static int ozBindStrOverlaps( lua_State *l )
  {
    AABB aabb = AABB( Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ),
                      Vec3( lua_tonumber( l, 4 ), lua_tonumber( l, 5 ), lua_tonumber( l, 6 ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  static int ozBindObjOverlaps( lua_State *l )
  {
    AABB aabb = AABB( Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ),
                      Vec3( lua_tonumber( l, 4 ), lua_tonumber( l, 5 ), lua_tonumber( l, 6 ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  static int ozSelfBindAllOverlaps( lua_State *l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ) );
    lua.objects.clear();
    lua.structs.clear();
    collider.getOverlaps( aabb, &lua.objects, &lua.structs );
    lua.objIndex = 0;
    lua.strIndex = 0;
    return 0;
  }

  static int ozSelfBindStrOverlaps( lua_State *l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  static int ozSelfBindObjOverlaps( lua_State *l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( lua_tonumber( l, 1 ), lua_tonumber( l, 2 ), lua_tonumber( l, 3 ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  static int ozStrBindIndex( lua_State *l )
  {
    int index = lua_tonumber( l, 1 );
    if( index < 0 || world.structures.length() <= index ) {
      OZ_LUA_ERROR( "invalid index" );
    }
    lua.str = world.structures[index];
    return 0;
  }

  static int ozStrBindNext( lua_State *l )
  {
    if( lua.strIndex < lua.structs.length() ) {
      lua.str = lua.structs[lua.strIndex];
      lua.strIndex++;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  static int ozStrIsNull( lua_State *l )
  {
    lua_pushboolean( l, lua.str == null );
    return 1;
  }

  static int ozStrGetBounds( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->mins.x );
    lua_pushnumber( l, lua.str->mins.y );
    lua_pushnumber( l, lua.str->mins.z );
    lua_pushnumber( l, lua.str->maxs.x );
    lua_pushnumber( l, lua.str->maxs.y );
    lua_pushnumber( l, lua.str->maxs.z );
    return 6;
  }

  static int ozStrGetPos( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->p.x );
    lua_pushnumber( l, lua.str->p.y );
    lua_pushnumber( l, lua.str->p.z );
    return 3;
  }

  static int ozStrGetIndex( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->index );
    return 1;
  }

  static int ozStrGetBSP( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->bsp );
    return 1;
  }

  static int ozObjBindIndex( lua_State *l )
  {
    int index = lua_tonumber( l, 1 );
    if( index < 0 || world.objects.length() <= index ) {
      OZ_LUA_ERROR( "invalid index" );
    }
    lua.obj = world.objects[index];
    return 0;
  }

  static int ozObjBindSelf( lua_State* )
  {
    lua.obj = lua.self;
    return 0;
  }

  static int ozObjBindNext( lua_State *l )
  {
    if( lua.objIndex < lua.objects.length() ) {
      lua.obj = lua.objects[lua.objIndex];
      lua.objIndex++;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  static int ozObjIsNull( lua_State *l )
  {
    lua_pushboolean( l, lua.obj == null );
    return 1;
  }

  static int ozObjIsSelf( lua_State *l )
  {
    lua_pushboolean( l, lua.obj == lua.self );
    return 1;
  }

  static int ozObjIsPut( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua_pushboolean( l, lua.obj->cell != null );
    return 1;
  }

  static int ozObjIsDynamic( lua_State *l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::DYNAMIC_BIT ) );
    return 1;
  }

  static int ozObjIsBot( lua_State *l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::BOT_BIT ) );
    return 1;
  }

  static int ozObjGetPos( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->p.x );
    lua_pushnumber( l, lua.obj->p.y );
    lua_pushnumber( l, lua.obj->p.z );
    return 3;
  }

  static int ozObjGetDim( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->dim.x );
    lua_pushnumber( l, lua.obj->dim.y );
    lua_pushnumber( l, lua.obj->dim.z );
    return 3;
  }

  static int ozObjGetIndex( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->index );
    return 1;
  }

  static int ozObjGetTypeName( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushstring( l, lua.obj->type->name );
    return 1;
  }

  static int ozObjGetLife( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->life );
    return 1;
  }

  static int ozObjHeadingToSelf( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.self->p.x - lua.obj->p.x;
    float dy = lua.obj->p.y - lua.self->p.y;
    float angle = Math::deg( Math::atan2( dx, dy ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozObjDistanceToSelf( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.self->p.x - lua.obj->p.x;
    float dy = lua.self->p.y - lua.obj->p.y;
    float dist = Math::sqrt( dx*dx + dy*dy );

    lua_pushnumber( l, dist );
    return 1;
  }

  static int ozDynGetVelocity( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    lua_pushnumber( l, obj->velocity.x );
    lua_pushnumber( l, obj->velocity.y );
    lua_pushnumber( l, obj->velocity.z );
    return 3;
  }

  static int ozDynGetMomentum( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    lua_pushnumber( l, obj->momentum.x );
    lua_pushnumber( l, obj->momentum.y );
    lua_pushnumber( l, obj->momentum.z );
    return 3;
  }

  static int ozDynGetMass( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    lua_pushnumber( l, obj->mass );
    return 1;
  }

  static int ozDynGetLift( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    lua_pushnumber( l, obj->lift );
    return 1;
  }

  static int ozBotGetH( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    lua_pushnumber( l, bot->h );
    return 1;
  }

  static int ozBotGetV( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    lua_pushnumber( l, bot->v );
    return 1;
  }

  static int ozBotGetStamina( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    lua_pushnumber( l, bot->stamina );
    return 1;
  }

  static int ozBotStateIsRunning( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    lua_pushboolean( l, bot->state & Bot::RUNNING_BIT );
    return 1;
  }

  static int ozSelfGetH( lua_State *l )
  {
    lua_pushnumber( l, lua.self->h );
    return 1;
  }

  static int ozSelfSetH( lua_State *l )
  {
    lua.self->h = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozSelfAddH( lua_State *l )
  {
    lua.self->h += lua_tonumber( l, 1 );
    return 1;
  }

  static int ozSelfGetV( lua_State *l )
  {
    lua_pushnumber( l, lua.self->v );
    return 1;
  }

  static int ozSelfSetV( lua_State *l )
  {
    lua.self->v = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozSelfAddV( lua_State *l )
  {
    lua.self->v += lua_tonumber( l, 1 );
    return 1;
  }

  static int ozSelfActionForward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_FORWARD;
    return 0;
  }

  static int ozSelfActionBackward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_BACKWARD;
    return 0;
  }

  static int ozSelfActionRight( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_RIGHT;
    return 0;
  }

  static int ozSelfActionLeft( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_LEFT;
    return 0;
  }

  static int ozSelfActionJump( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_JUMP;
    return 0;
  }

  static int ozSelfActionCrouch( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_CROUCH;
    return 0;
  }

  static int ozSelfActionUse( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_USE;
    return 0;
  }

  static int ozSelfActionTake( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_TAKE;
    return 0;
  }

  static int ozSelfActionGrab( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  static int ozSelfActionThrow( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_THROW;
    return 0;
  }

  static int ozSelfActionSuicide( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_SUICIDE;
    return 0;
  }

  static int ozSelfStateIsRunning( lua_State *l )
  {
    lua_pushboolean( l, lua.self->state & Bot::RUNNING_BIT );
    return 1;
  }

  static int ozSelfStateSetRunning( lua_State *l )
  {
    if( lua_toboolean( l, 1 ) ) {
      lua.self->state |= Bot::RUNNING_BIT;
    }
    else {
      lua.self->state &= ~Bot::RUNNING_BIT;
    }
    return 0;
  }

  static int ozSelfStateToggleRunning( lua_State* )
  {
    lua.self->state ^= Bot::RUNNING_BIT;
    return 0;
  }

  void Lua::callFunc( const char *functionName, int botIndex )
  {
    assert( self != null );
    assert( lua_gettop( l ) == 1 && lua_istable( l, 1 ) );

    obj      = self;
    str      = null;

    objIndex = 0;
    strIndex = 0;

    lua_getglobal( l, functionName );
    lua_rawgeti( l, 1, botIndex );
    lua_pcall( l, 1, 0, 0 );

    if( lua_gettop( l ) != 1 ){
      if( lua_isstring( l, -1 ) ) {
        log.println( "N! %s", lua_tostring( l, -1 ) );
      }
      lua_settop( l, 1 );
    }
    assert( lua_gettop( l ) == 1 && lua_istable( l, 1 ) );
  }

  void Lua::registerMind( int botIndex )
  {
    assert( lua_istable( l, 1 ) );

    lua_newtable( l );
    lua_rawseti( l, 1, botIndex );
  }

  void Lua::unregisterMind( int botIndex )
  {
    assert( lua_istable( l, 1 ) );

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

    OZ_LUA_REGISTER( ozBindAllOverlaps );
    OZ_LUA_REGISTER( ozBindStrOverlaps );
    OZ_LUA_REGISTER( ozBindObjOverlaps );
    OZ_LUA_REGISTER( ozSelfBindAllOverlaps );
    OZ_LUA_REGISTER( ozSelfBindStrOverlaps );
    OZ_LUA_REGISTER( ozSelfBindObjOverlaps );

    OZ_LUA_REGISTER( ozStrBindIndex );
    OZ_LUA_REGISTER( ozStrBindNext );

    OZ_LUA_REGISTER( ozStrIsNull );
    OZ_LUA_REGISTER( ozStrGetBounds );
    OZ_LUA_REGISTER( ozStrGetIndex );
    OZ_LUA_REGISTER( ozStrGetPos );
    OZ_LUA_REGISTER( ozStrGetBSP );

    OZ_LUA_REGISTER( ozObjBindIndex );
    OZ_LUA_REGISTER( ozObjBindSelf );
    OZ_LUA_REGISTER( ozObjBindNext );

    OZ_LUA_REGISTER( ozObjIsNull );
    OZ_LUA_REGISTER( ozObjIsSelf );
    OZ_LUA_REGISTER( ozObjIsPut );
    OZ_LUA_REGISTER( ozObjIsDynamic );
    OZ_LUA_REGISTER( ozObjIsBot );
    OZ_LUA_REGISTER( ozObjGetPos );
    OZ_LUA_REGISTER( ozObjGetDim );
    OZ_LUA_REGISTER( ozObjGetIndex );
    OZ_LUA_REGISTER( ozObjGetTypeName );
    OZ_LUA_REGISTER( ozObjGetLife );

    OZ_LUA_REGISTER( ozObjHeadingToSelf );
    OZ_LUA_REGISTER( ozObjDistanceToSelf );

    OZ_LUA_REGISTER( ozDynGetVelocity );
    OZ_LUA_REGISTER( ozDynGetMomentum );
    OZ_LUA_REGISTER( ozDynGetMass );
    OZ_LUA_REGISTER( ozDynGetLift );

    OZ_LUA_REGISTER( ozBotGetH );
    OZ_LUA_REGISTER( ozBotGetV );
    OZ_LUA_REGISTER( ozBotStateIsRunning );
    OZ_LUA_REGISTER( ozBotGetStamina );

    OZ_LUA_REGISTER( ozSelfGetH );
    OZ_LUA_REGISTER( ozSelfSetH );
    OZ_LUA_REGISTER( ozSelfAddH );
    OZ_LUA_REGISTER( ozSelfGetV );
    OZ_LUA_REGISTER( ozSelfSetV );
    OZ_LUA_REGISTER( ozSelfAddV );
    OZ_LUA_REGISTER( ozSelfActionForward );
    OZ_LUA_REGISTER( ozSelfActionBackward );
    OZ_LUA_REGISTER( ozSelfActionRight );
    OZ_LUA_REGISTER( ozSelfActionLeft );
    OZ_LUA_REGISTER( ozSelfActionJump );
    OZ_LUA_REGISTER( ozSelfActionCrouch );
    OZ_LUA_REGISTER( ozSelfActionUse );
    OZ_LUA_REGISTER( ozSelfActionTake );
    OZ_LUA_REGISTER( ozSelfActionGrab );
    OZ_LUA_REGISTER( ozSelfActionThrow );
    OZ_LUA_REGISTER( ozSelfActionSuicide );
    OZ_LUA_REGISTER( ozSelfStateIsRunning );
    OZ_LUA_REGISTER( ozSelfStateSetRunning );
    OZ_LUA_REGISTER( ozSelfStateToggleRunning );

    lua_newtable( l );
    lua_setglobal( l, "ozLocalData" );
    lua_getglobal( l, "ozLocalData" );

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
