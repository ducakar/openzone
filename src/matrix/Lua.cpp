/*
 *  Lua.cpp
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Lua.h"

#include "BotClass.h"
#include "World.h"
#include "Synapse.h"
#include "Collider.h"

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

  Lua lua;

  const char *Lua::HANDLERS_FILE = "lua/handlers.luac";

  static int ozPrintln( lua_State *l )
  {
    log.println( "M> %s", lua_tostring( l, 1 ) );
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

  static int ozStrDamage( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua.str->damage( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozStrDestroy( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua.str->destroy();
    return 0;
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

  static int ozObjBindUser( lua_State* )
  {
    lua.obj = lua.user;
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

  static int ozObjIsUser( lua_State *l )
  {
    lua_pushboolean( l, lua.obj == lua.user );
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

  static int ozObjSetPos( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->p.x = lua_tonumber( l, 1 );
    lua.obj->p.y = lua_tonumber( l, 2 );
    lua.obj->p.z = lua_tonumber( l, 3 );
    return 0;
  }

  static int ozObjAddPos( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->p.x += lua_tonumber( l, 1 );
    lua.obj->p.y += lua_tonumber( l, 2 );
    lua.obj->p.z += lua_tonumber( l, 3 );
    return 0;
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

  static int ozObjSetLife( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = bound<double>( lua_tonumber( l, 1 ), 0, lua.obj->type->life );
    return 0;
  }

  static int ozObjAddLife( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = bound<double>( lua.obj->life + lua_tonumber( l, 1 ),
                                   0,
                                   lua.obj->type->life );
    return 0;
  }

  static int ozObjDamage( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->damage( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozObjDestroy( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = 0.0f;
    return 0;
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

  static int ozDynSetMomentum( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->momentum.x = lua_tonumber( l, 1 );
    obj->momentum.y = lua_tonumber( l, 2 );
    obj->momentum.z = lua_tonumber( l, 3 );
    return 0;
  }

  static int ozDynAddMomentum( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->momentum.x += lua_tonumber( l, 1 );
    obj->momentum.y += lua_tonumber( l, 2 );
    obj->momentum.z += lua_tonumber( l, 3 );
    return 0;
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

  static int ozDynSetMass( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->mass = lua_tonumber( l, 1 );
    return 0;
  }

  static int ozDynAddMass( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->mass += lua_tonumber( l, 1 );
    return 0;
  }

  static int ozDynResetMass( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );
    DynObjectClass *clazz = static_cast<DynObjectClass*>( lua.obj->type );

    obj->mass = clazz->mass;
    return 0;
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

  static int ozDynSetLift( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->lift = lua_tonumber( l, 1 );
    return 0;
  }

  static int ozDynAddLift( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );

    obj->lift += lua_tonumber( l, 1 );
    return 0;
  }

  static int ozDynResetLift( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::DYNAMIC_BIT ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    DynObject *obj = static_cast<DynObject*>( lua.obj );
    DynObjectClass *clazz = static_cast<DynObjectClass*>( lua.obj->type );

    obj->lift = clazz->lift;
    return 0;
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

  static int ozBotSetH( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->h = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozBotAddH( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->h += lua_tonumber( l, 1 );
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

  static int ozBotSetV( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->v = lua_tonumber( l, 1 );
    return 1;
  }

  static int ozBotAddV( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->v += lua_tonumber( l, 1 );
    return 1;
  }

  static int ozBotActionForward( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_FORWARD;
    return 0;
  }

  static int ozBotActionBackward( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_BACKWARD;
    return 0;
  }

  static int ozBotActionRight( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_RIGHT;
    return 0;
  }

  static int ozBotActionLeft( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_LEFT;
    return 0;
  }

  static int ozBotActionJump( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_JUMP;
    return 0;
  }

  static int ozBotActionCrouch( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_CROUCH;
    return 0;
  }

  static int ozBotActionUse( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_USE;
    return 0;
  }

  static int ozBotActionTake( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_TAKE;
    return 0;
  }

  static int ozBotActionGrab( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  static int ozBotActionThrow( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_THROW;
    return 0;
  }

  static int ozBotActionSuicide( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_SUICIDE;
    return 0;
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

  static int ozBotStateSetRunning( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    if( lua_toboolean( l, 1 ) ) {
      bot->state |= Bot::RUNNING_BIT;
    }
    else {
      bot->state &= ~Bot::RUNNING_BIT;
    }
    return 0;
  }

  static int ozBotStateToggleRunning( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );

    bot->state ^= Bot::RUNNING_BIT;
    return 0;
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

  static int ozBotSetStamina( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );
    BotClass *clazz = static_cast<BotClass*>( bot->type );

    bot->stamina = bound<double>( lua_tonumber( l, 1 ), 0, clazz->stamina );
    return 0;
  }

  static int ozBotAddStamina( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( ~lua.obj->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot *bot = static_cast<Bot*>( lua.obj );
    BotClass *clazz = static_cast<BotClass*>( bot->type );

    bot->stamina = bound<double>( bot->stamina + lua_tonumber( l, 1 ), 0, clazz->stamina );
    return 0;
  }

  static int ozWorldAddStr( lua_State *l )
  {
    const char *name = lua_tostring( l, 1 );
    Vec3 p = Vec3( lua_tonumber( l, 2 ), lua_tonumber( l, 3 ), lua_tonumber( l, 4 ) );
    Structure::Rotation rot = static_cast<Structure::Rotation>( lua_tonumber( l, 5 ) );
    lua.str = translator.createStruct( name, p, rot );

    if( collider.test( lua.str->toAABB() ) ) {
      int index = synapse.add( lua.str );
      lua_pushnumber( l, index );
    }
    else {
      delete lua.str;
      lua.str = null;
      lua_pushnumber( l, -1 );
    }
    return 1;
  }

  static int ozWorldAddObj( lua_State *l )
  {
    const char *name = lua_tostring( l, 1 );
    Vec3 p = Vec3( lua_tonumber( l, 2 ), lua_tonumber( l, 3 ), lua_tonumber( l, 4 ) );
    lua.obj = translator.createObject( name, p );

    if( collider.test( *lua.obj ) ) {
      int index = synapse.add( lua.obj );
      lua_pushnumber( l, index );
    }
    else {
      delete lua.obj;
      lua.obj = null;
      lua_pushnumber( l, -1 );
    }
    return 1;
  }

  static int ozWorldForceAddStr( lua_State *l )
  {
    const char *name = lua_tostring( l, 1 );
    Vec3 p = Vec3( lua_tonumber( l, 2 ), lua_tonumber( l, 3 ), lua_tonumber( l, 4 ) );
    Structure::Rotation rot = static_cast<Structure::Rotation>( lua_tonumber( l, 5 ) );

    int index = synapse.addStruct( name, p, rot );
    lua_pushnumber( l, index );
    return 1;
  }

  static int ozWorldForceAddObj( lua_State *l )
  {
    const char *name = lua_tostring( l, 1 );
    Vec3 p = Vec3( lua_tonumber( l, 2 ), lua_tonumber( l, 3 ), lua_tonumber( l, 4 ) );

    int index = synapse.addObject( name, p );
    lua_pushnumber( l, index );
    return 1;
  }

  static int ozWorldRemoveStr( lua_State *l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    synapse.remove( lua.str );
    lua.str = null;
    return 0;
  }

  static int ozWorldRemoveObj( lua_State *l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    if( lua.obj->cell == null ) {
      DynObject *dynObj = static_cast<DynObject*>( lua.obj );

      assert( dynObj->flags & Object::DYNAMIC_BIT );

      synapse.removeCut( dynObj );
    }
    else {
      synapse.remove( lua.obj );
    }

    lua.self = lua.self == lua.obj ? null : lua.self;
    lua.user = lua.user == lua.obj ? null : lua.user;
    lua.obj = null;
    return 0;
  }

  void Lua::callFunc( const char *functionName, int index )
  {
    assert( self != null );

    obj      = self;
    str      = null;

    objIndex = 0;
    strIndex = 0;

    lua_getglobal( l, "ozLocalData" );
    lua_getglobal( l, functionName );
    lua_rawgeti( l, -2, index );
    lua_pcall( l, 1, 0, 0 );
    lua_pop( l, 1 );

    if( lua_isstring( l, -1 ) ) {
      log.println( "M! %s", lua_tostring( l, -1 ) );
      lua_pop( l, 1 );
    }
  }

  void Lua::registerObject( int index )
  {
    // create object's local data
    lua_getglobal( l, "ozLocalData" );
    lua_newtable( l );
    lua_rawseti( l, -2, index );
    lua_pop( l, 1 );
  }

  void Lua::unregisterObject( int index )
  {
    // delete object's local data
    lua_getglobal( l, "ozLocalData" );
    lua_pushnil( l );
    lua_rawseti( l, -2, index );
    lua_pop( l, 1 );
  }

  void Lua::init()
  {
    log.println( "Initializing Matrix Lua {" );
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
    OZ_LUA_REGISTER( ozStrDamage );
    OZ_LUA_REGISTER( ozStrDestroy );

    OZ_LUA_REGISTER( ozObjBindIndex );
    OZ_LUA_REGISTER( ozObjBindSelf );
    OZ_LUA_REGISTER( ozObjBindUser );
    OZ_LUA_REGISTER( ozObjBindNext );

    OZ_LUA_REGISTER( ozObjIsNull );
    OZ_LUA_REGISTER( ozObjIsSelf );
    OZ_LUA_REGISTER( ozObjIsUser );
    OZ_LUA_REGISTER( ozObjIsPut );
    OZ_LUA_REGISTER( ozObjIsDynamic );
    OZ_LUA_REGISTER( ozObjIsBot );
    OZ_LUA_REGISTER( ozObjGetPos );
    OZ_LUA_REGISTER( ozObjSetPos );
    OZ_LUA_REGISTER( ozObjAddPos );
    OZ_LUA_REGISTER( ozObjGetDim );
    OZ_LUA_REGISTER( ozObjGetIndex );
    OZ_LUA_REGISTER( ozObjGetTypeName );
    OZ_LUA_REGISTER( ozObjGetLife );
    OZ_LUA_REGISTER( ozObjSetLife );
    OZ_LUA_REGISTER( ozObjAddLife );
    OZ_LUA_REGISTER( ozObjDamage );
    OZ_LUA_REGISTER( ozObjDestroy );

    OZ_LUA_REGISTER( ozObjHeadingToSelf );
    OZ_LUA_REGISTER( ozObjDistanceToSelf );

    OZ_LUA_REGISTER( ozDynGetVelocity );
    OZ_LUA_REGISTER( ozDynGetMomentum );
    OZ_LUA_REGISTER( ozDynSetMomentum );
    OZ_LUA_REGISTER( ozDynAddMomentum );
    OZ_LUA_REGISTER( ozDynGetMass );
    OZ_LUA_REGISTER( ozDynSetMass );
    OZ_LUA_REGISTER( ozDynAddMass );
    OZ_LUA_REGISTER( ozDynResetMass );
    OZ_LUA_REGISTER( ozDynGetLift );
    OZ_LUA_REGISTER( ozDynSetLift );
    OZ_LUA_REGISTER( ozDynAddLift );
    OZ_LUA_REGISTER( ozDynResetLift );

    OZ_LUA_REGISTER( ozBotGetH );
    OZ_LUA_REGISTER( ozBotSetH );
    OZ_LUA_REGISTER( ozBotAddH );
    OZ_LUA_REGISTER( ozBotGetV );
    OZ_LUA_REGISTER( ozBotSetV );
    OZ_LUA_REGISTER( ozBotAddV );
    OZ_LUA_REGISTER( ozBotActionForward );
    OZ_LUA_REGISTER( ozBotActionBackward );
    OZ_LUA_REGISTER( ozBotActionRight );
    OZ_LUA_REGISTER( ozBotActionLeft );
    OZ_LUA_REGISTER( ozBotActionJump );
    OZ_LUA_REGISTER( ozBotActionCrouch );
    OZ_LUA_REGISTER( ozBotActionUse );
    OZ_LUA_REGISTER( ozBotActionTake );
    OZ_LUA_REGISTER( ozBotActionGrab );
    OZ_LUA_REGISTER( ozBotActionThrow );
    OZ_LUA_REGISTER( ozBotActionSuicide );
    OZ_LUA_REGISTER( ozBotStateIsRunning );
    OZ_LUA_REGISTER( ozBotStateSetRunning );
    OZ_LUA_REGISTER( ozBotStateToggleRunning );
    OZ_LUA_REGISTER( ozBotGetStamina );
    OZ_LUA_REGISTER( ozBotSetStamina );
    OZ_LUA_REGISTER( ozBotAddStamina );

    OZ_LUA_REGISTER( ozWorldAddStr );
    OZ_LUA_REGISTER( ozWorldAddObj );
    OZ_LUA_REGISTER( ozWorldForceAddStr );
    OZ_LUA_REGISTER( ozWorldForceAddObj );
    OZ_LUA_REGISTER( ozWorldRemoveStr );
    OZ_LUA_REGISTER( ozWorldRemoveObj );

    lua_newtable( l );
    lua_setglobal( l, "ozLocalData" );

    for( int i = 0; i < translator.matrixScripts.length(); i++ ) {
      const Translator::Resource &res = translator.matrixScripts[i];

      log.print( "Processing '%s' ...", res.path.cstr() );

      if( luaL_dofile( l, res.path ) != 0 ) {
        log.printEnd( " Failed" );
        throw Exception( "Matrix Lua script error" );
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
    log.print( "Freeing Matrix Lua ..." );
    lua_close( l );
    log.printEnd( " OK" );
  }

}
