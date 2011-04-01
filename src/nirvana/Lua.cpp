/*
 *  Lua.cpp
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "nirvana/Lua.hpp"

#include "matrix/BotClass.hpp"
#include "matrix/Translator.hpp"

#include <lua.hpp>

#define OZ_LUA_ERROR( message ) luaL_error( l, "[%s] %s", __FUNCTION__, message );
#define OZ_LUA_FUNCTION( func ) lua_register( l, #func, func )
#define OZ_LUA_INT_CONST( name, value ) lua_pushinteger( l, value ); lua_setglobal( l, name )
#define OZ_LUA_FLOAT_CONST( name, value ) lua_pushnumber( l, value ); lua_setglobal( l, name )
#define OZ_LUA_STRING_CONST( name, value ) lua_pushstring( l, value ); lua_setglobal( l, name )

namespace oz
{
namespace nirvana
{

  Lua lua;

  Lua::Lua() : l( null )
  {}

  void Lua::call( const char* functionName, Bot* self_ )
  {
    hard_assert( self_ != null );
    hard_assert( lua_gettop( l ) == 1 && lua_istable( l, 1 ) );

    self  = self_;
    obj   = self_;
    str   = null;
    part  = null;
    event = List<Object::Event>::CIterator();

    objIndex = 0;
    strIndex = 0;

    forceUpdate = false;

    lua_getglobal( l, functionName );
    lua_rawgeti( l, 1, self->index );
    lua_pcall( l, 1, 0, 0 );

    if( lua_gettop( l ) != 1 ){
      if( lua_isstring( l, -1 ) ) {
        log.println( "N! %s", lua_tostring( l, -1 ) );
      }

      throw Exception( "Nirvana Lua function call finished with an error" );

      lua_settop( l, 1 );
    }
    hard_assert( lua_gettop( l ) == 1 && lua_istable( l, 1 ) );
  }

  void Lua::registerMind( int botIndex )
  {
    hard_assert( lua_istable( l, 1 ) );

    lua_newtable( l );
    lua_rawseti( l, 1, botIndex );
  }

  void Lua::unregisterMind( int botIndex )
  {
    hard_assert( lua_istable( l, 1 ) );

    lua_pushnil( l );
    lua_rawseti( l, 1, botIndex );
  }

  void Lua::init()
  {
    log.print( "Initialising Nirvana Lua ..." );

    l = lua_open();
    luaL_openlibs( l );

    OZ_LUA_FUNCTION( ozPrintln );
    OZ_LUA_FUNCTION( ozException );
    OZ_LUA_FUNCTION( ozForceUpdate );

    OZ_LUA_FUNCTION( ozBindAllOverlaps );
    OZ_LUA_FUNCTION( ozBindStrOverlaps );
    OZ_LUA_FUNCTION( ozBindObjOverlaps );
    OZ_LUA_FUNCTION( ozSelfBindAllOverlaps );
    OZ_LUA_FUNCTION( ozSelfBindStrOverlaps );
    OZ_LUA_FUNCTION( ozSelfBindObjOverlaps );

    OZ_LUA_FUNCTION( ozStrBindIndex );
    OZ_LUA_FUNCTION( ozStrBindNext );

    OZ_LUA_FUNCTION( ozStrIsNull );
    OZ_LUA_FUNCTION( ozStrGetBounds );
    OZ_LUA_FUNCTION( ozStrGetIndex );
    OZ_LUA_FUNCTION( ozStrGetPos );
    OZ_LUA_FUNCTION( ozStrGetBSP );

    OZ_LUA_FUNCTION( ozStrVectorFromSelf );
    OZ_LUA_FUNCTION( ozStrDirectionFromSelf );
    OZ_LUA_FUNCTION( ozStrDistanceFromSelf );
    OZ_LUA_FUNCTION( ozStrHeadingFromSelf );
    OZ_LUA_FUNCTION( ozStrPitchFromSelf );

    OZ_LUA_FUNCTION( ozObjBindIndex );
    OZ_LUA_FUNCTION( ozObjBindSelf );
    OZ_LUA_FUNCTION( ozObjBindNext );

    OZ_LUA_FUNCTION( ozObjIsNull );
    OZ_LUA_FUNCTION( ozObjIsSelf );
    OZ_LUA_FUNCTION( ozObjIsPut );
    OZ_LUA_FUNCTION( ozObjIsDynamic );
    OZ_LUA_FUNCTION( ozObjIsItem );
    OZ_LUA_FUNCTION( ozObjIsWeapon );
    OZ_LUA_FUNCTION( ozObjIsBot );
    OZ_LUA_FUNCTION( ozObjIsVehicle );
    OZ_LUA_FUNCTION( ozObjGetPos );
    OZ_LUA_FUNCTION( ozObjGetDim );
    OZ_LUA_FUNCTION( ozObjGetIndex );
    OZ_LUA_FUNCTION( ozObjGetFlags );
    OZ_LUA_FUNCTION( ozObjGetOldFlags );
    OZ_LUA_FUNCTION( ozObjGetTypeName );
    OZ_LUA_FUNCTION( ozObjGetLife );

    OZ_LUA_FUNCTION( ozObjVectorFromSelf );
    OZ_LUA_FUNCTION( ozObjDirectionFromSelf );
    OZ_LUA_FUNCTION( ozObjDistanceFromSelf );
    OZ_LUA_FUNCTION( ozObjHeadingFromSelf );
    OZ_LUA_FUNCTION( ozObjPitchFromSelf );
    OZ_LUA_FUNCTION( ozObjPitchFromSelfEye );

    OZ_LUA_FUNCTION( ozObjBindEvent );
    OZ_LUA_FUNCTION( ozEventBindNext );
    OZ_LUA_FUNCTION( ozEventGet );

    OZ_LUA_FUNCTION( ozDynGetVelocity );
    OZ_LUA_FUNCTION( ozDynGetMomentum );
    OZ_LUA_FUNCTION( ozDynGetMass );
    OZ_LUA_FUNCTION( ozDynGetLift );

    OZ_LUA_FUNCTION( ozBotGetEyePos );
    OZ_LUA_FUNCTION( ozBotGetH );
    OZ_LUA_FUNCTION( ozBotGetV );
    OZ_LUA_FUNCTION( ozBotGetDir );
    OZ_LUA_FUNCTION( ozBotStateIsRunning );
    OZ_LUA_FUNCTION( ozBotGetStamina );

    OZ_LUA_FUNCTION( ozSelfGetEyePos );
    OZ_LUA_FUNCTION( ozSelfGetH );
    OZ_LUA_FUNCTION( ozSelfSetH );
    OZ_LUA_FUNCTION( ozSelfAddH );
    OZ_LUA_FUNCTION( ozSelfGetV );
    OZ_LUA_FUNCTION( ozSelfSetV );
    OZ_LUA_FUNCTION( ozSelfAddV );
    OZ_LUA_FUNCTION( ozSelfActionForward );
    OZ_LUA_FUNCTION( ozSelfActionBackward );
    OZ_LUA_FUNCTION( ozSelfActionRight );
    OZ_LUA_FUNCTION( ozSelfActionLeft );
    OZ_LUA_FUNCTION( ozSelfActionJump );
    OZ_LUA_FUNCTION( ozSelfActionCrouch );
    OZ_LUA_FUNCTION( ozSelfActionUse );
    OZ_LUA_FUNCTION( ozSelfActionTake );
    OZ_LUA_FUNCTION( ozSelfActionGrab );
    OZ_LUA_FUNCTION( ozSelfActionThrow );
    OZ_LUA_FUNCTION( ozSelfActionAttack );
    OZ_LUA_FUNCTION( ozSelfActionExit );
    OZ_LUA_FUNCTION( ozSelfActionEject );
    OZ_LUA_FUNCTION( ozSelfActionSuicide );
    OZ_LUA_FUNCTION( ozSelfStateIsRunning );
    OZ_LUA_FUNCTION( ozSelfStateSetRunning );
    OZ_LUA_FUNCTION( ozSelfStateToggleRunning );

    OZ_LUA_FUNCTION( ozPartBindIndex );
    OZ_LUA_FUNCTION( ozPartIsNull );
    OZ_LUA_FUNCTION( ozPartGetPos );
    OZ_LUA_FUNCTION( ozPartGetIndex );
    OZ_LUA_FUNCTION( ozPartGetVelocity );
    OZ_LUA_FUNCTION( ozPartGetLife );

    OZ_LUA_INT_CONST( "OZ_OBJECT_DYNAMIC_BIT",          Object::DYNAMIC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_ITEM_BIT",             Object::ITEM_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_WEAPON_BIT",           Object::WEAPON_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_BOT_BIT",              Object::BOT_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_VEHICLE_BIT",          Object::VEHICLE_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_LUA_BIT",              Object::LUA_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_DESTROY_FUNC_BIT",     Object::DESTROY_FUNC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_DAMAGE_FUNC_BIT",      Object::DAMAGE_FUNC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_HIT_FUNC_BIT",         Object::HIT_FUNC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_USE_FUNC_BIT",         Object::USE_FUNC_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_USE_FUNC_BIT",         Object::USE_FUNC_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_MODEL_BIT",            Object::MODEL_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_AUDIO_BIT",            Object::AUDIO_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_CUT_BIT",              Object::CUT_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_DESTROYED_BIT",        Object::DESTROYED_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_DISABLED_BIT",         Object::DISABLED_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_HIT_BIT",              Object::HIT_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_FRICTING_BIT",         Object::FRICTING_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_ON_FLOOR_BIT",         Object::ON_FLOOR_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_ON_SLICK_BIT",         Object::ON_SLICK_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_IN_WATER_BIT",         Object::IN_WATER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_ON_LADDER_BIT",        Object::ON_LADDER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_SOLID_BIT",            Object::SOLID_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_CYLINDER_BIT",         Object::CYLINDER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_CLIMBER_BIT",          Object::CLIMBER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_PUSHER_BIT",           Object::PUSHER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_HOVER_BIT",            Object::HOVER_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_NO_DRAW_BIT",          Object::NO_DRAW_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_WIDE_CULL_BIT",        Object::WIDE_CULL_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_RANDOM_HEADING_BIT",   Object::RANDOM_HEADING_BIT );

    OZ_LUA_INT_CONST( "OZ_EVENT_DESTROY",               Object::EVENT_DESTROY );
    OZ_LUA_INT_CONST( "OZ_EVENT_DAMAGE",                Object::EVENT_DAMAGE );
    OZ_LUA_INT_CONST( "OZ_EVENT_HIT",                   Object::EVENT_HIT );
    OZ_LUA_INT_CONST( "OZ_EVENT_SPLASH",                Object::EVENT_SPLASH );
    OZ_LUA_INT_CONST( "OZ_EVENT_FRICTING",              Object::EVENT_FRICTING );
    OZ_LUA_INT_CONST( "OZ_EVENT_USE",                   Object::EVENT_USE );
    OZ_LUA_INT_CONST( "OZ_EVENT_SHOT",                  Weapon::EVENT_SHOT );
    OZ_LUA_INT_CONST( "OZ_EVENT_SHOT_EMPTY",            Weapon::EVENT_SHOT_EMPTY );
    OZ_LUA_INT_CONST( "OZ_EVENT_LAND",                  Bot::EVENT_LAND );
    OZ_LUA_INT_CONST( "OZ_EVENT_JUMP",                  Bot::EVENT_JUMP );
    OZ_LUA_INT_CONST( "OZ_EVENT_FLIP",                  Bot::EVENT_FLIP );
    OZ_LUA_INT_CONST( "OZ_EVENT_DEATH",                 Bot::EVENT_DEATH );

    lua_newtable( l );
    lua_setglobal( l, "ozLocalData" );
    lua_getglobal( l, "ozLocalData" );

    if( luaL_dofile( l, "lua/nirvana.luac" ) != 0 ) {
      log.printEnd( " Failed" );
      throw Exception( "Nirvana Lua script error" );
    }

    log.printEnd( " OK" );
  }

  void Lua::free()
  {
    log.print( "Freeing Nirvana Lua ..." );

    objects.clear();
    objects.dealloc();
    structs.clear();
    structs.dealloc();

    lua_close( l );

    log.printEnd( " OK" );
  }

  int Lua::ozPrintln( lua_State* l )
  {
    log.println( "N> %s", lua_tostring( l, 1 ) );
    return 0;
  }

  int Lua::ozException( lua_State* l )
  {
    const char* message = lua_tostring( l, 1 );
    throw Exception( message );
  }

  int Lua::ozForceUpdate( lua_State* )
  {
    lua.forceUpdate = true;
    return 0;
  }

  int Lua::ozBindAllOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Point3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.objects.clear();
    lua.structs.clear();
    collider.getOverlaps( aabb, &lua.objects, &lua.structs );
    lua.objIndex = 0;
    lua.strIndex = 0;
    return 0;
  }

  int Lua::ozBindStrOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Point3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  int Lua::ozBindObjOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Point3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  int Lua::ozSelfBindAllOverlaps( lua_State* l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ) );
    lua.objects.clear();
    lua.structs.clear();
    collider.getOverlaps( aabb, &lua.objects, &lua.structs );
    lua.objIndex = 0;
    lua.strIndex = 0;
    return 0;
  }

  int Lua::ozSelfBindStrOverlaps( lua_State* l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  int Lua::ozSelfBindObjOverlaps( lua_State* l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  int Lua::ozStrBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.structs.length() <= index ) {
      OZ_LUA_ERROR( "invalid index" );
    }
    lua.str = orbis.structs[index];
    return 0;
  }

  int Lua::ozStrBindNext( lua_State* l )
  {
    if( lua.strIndex < lua.structs.length() ) {
      lua.str = lua.structs[lua.strIndex];
      ++lua.strIndex;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  int Lua::ozStrIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.str == null );
    return 1;
  }

  int Lua::ozStrGetBounds( lua_State* l )
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

  int Lua::ozStrGetPos( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->p.x );
    lua_pushnumber( l, lua.str->p.y );
    lua_pushnumber( l, lua.str->p.z );
    return 3;
  }

  int Lua::ozStrGetIndex( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushinteger( l, lua.str->index );
    return 1;
  }

  int Lua::ozStrGetBSP( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushinteger( l, lua.str->bsp );
    return 1;
  }

  int Lua::ozStrVectorFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    Vec3 vec = lua.str->p - lua.self->p;
    lua_pushnumber( l, vec.x );
    lua_pushnumber( l, vec.y );
    lua_pushnumber( l, vec.z );
    return 3;
  }

  int Lua::ozStrDirectionFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    Vec3 dir = ~( lua.str->p - lua.self->p );
    lua_pushnumber( l, dir.x );
    lua_pushnumber( l, dir.y );
    lua_pushnumber( l, dir.z );
    return 1;
  }

  int Lua::ozStrDistanceFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    lua_pushnumber( l, !( lua.str->p - lua.self->p ) );
    return 1;
  }

  int Lua::ozStrHeadingFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    float dx = lua.str->p.x - lua.self->p.x;
    float dy = lua.str->p.y - lua.self->p.y;
    float angle = Math::deg( Math::atan2( -dx, dy ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  int Lua::ozStrPitchFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    float dx = lua.str->p.x - lua.self->p.x;
    float dy = lua.str->p.y - lua.self->p.y;
    float dz = lua.str->p.z - lua.self->p.z;
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

    lua_pushnumber( l, angle );
    return 1;
  }

  int Lua::ozObjBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.objects.length() <= index ) {
      OZ_LUA_ERROR( "invalid index" );
    }
    lua.obj = orbis.objects[index];
    return 0;
  }

  int Lua::ozObjBindSelf( lua_State* )
  {
    lua.obj = lua.self;
    return 0;
  }

  int Lua::ozObjBindNext( lua_State* l )
  {
    if( lua.objIndex < lua.objects.length() ) {
      lua.obj = lua.objects[lua.objIndex];
      ++lua.objIndex;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  int Lua::ozObjIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.obj == null );
    return 1;
  }

  int Lua::ozObjIsSelf( lua_State* l )
  {
    lua_pushboolean( l, lua.obj == lua.self );
    return 1;
  }

  int Lua::ozObjIsPut( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua_pushboolean( l, lua.obj->cell != null );
    return 1;
  }

  int Lua::ozObjIsDynamic( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::DYNAMIC_BIT ) );
    return 1;
  }

  int Lua::ozObjIsItem( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::ITEM_BIT ) );
    return 1;
  }

  int Lua::ozObjIsWeapon( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::WEAPON_BIT ) );
    return 1;
  }

  int Lua::ozObjIsBot( lua_State* l )
  {
    const Bot* bot = static_cast<const Bot*>( lua.obj );
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::BOT_BIT ) &&
                     !( bot->state & Bot::DEATH_BIT ) );
    return 1;
  }

  int Lua::ozObjIsVehicle( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::VEHICLE_BIT ) );
    return 1;
  }

  int Lua::ozObjGetPos( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->p.x );
    lua_pushnumber( l, lua.obj->p.y );
    lua_pushnumber( l, lua.obj->p.z );
    return 3;
  }

  int Lua::ozObjGetDim( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->dim.x );
    lua_pushnumber( l, lua.obj->dim.y );
    lua_pushnumber( l, lua.obj->dim.z );
    return 3;
  }

  int Lua::ozObjGetIndex( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->index );
    return 1;
  }

  int Lua::ozObjGetFlags( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->flags );
    return 1;
  }

  int Lua::ozObjGetOldFlags( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->oldFlags );
    return 1;
  }

  int Lua::ozObjGetTypeName( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushstring( l, lua.obj->clazz->name );
    return 1;
  }

  int Lua::ozObjGetLife( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->life );
    return 1;
  }

  int Lua::ozObjVectorFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    Vec3 vec = lua.obj->p - lua.self->p;
    lua_pushnumber( l, vec.x );
    lua_pushnumber( l, vec.y );
    lua_pushnumber( l, vec.z );
    return 3;
  }

  int Lua::ozObjDirectionFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    Vec3 dir = ~( lua.obj->p - lua.self->p );
    lua_pushnumber( l, dir.x );
    lua_pushnumber( l, dir.y );
    lua_pushnumber( l, dir.z );
    return 1;
  }

  int Lua::ozObjDistanceFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    lua_pushnumber( l, !( lua.obj->p - lua.self->p ) );
    return 1;
  }

  int Lua::ozObjHeadingFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.obj->p.x - lua.self->p.x;
    float dy = lua.obj->p.y - lua.self->p.y;
    float angle = Math::deg( Math::atan2( -dx, dy ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  int Lua::ozObjPitchFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    float dx = lua.obj->p.x - lua.self->p.x;
    float dy = lua.obj->p.y - lua.self->p.y;
    float dz = lua.obj->p.z - lua.self->p.z;
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

    lua_pushnumber( l, angle );
    return 1;
  }

  int Lua::ozObjPitchFromSelfEye( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }

    float dx = lua.obj->p.x - lua.self->p.x;
    float dy = lua.obj->p.y - lua.self->p.y;
    float dz = lua.obj->p.z - lua.self->p.z - lua.self->camZ;
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

    lua_pushnumber( l, angle );
    return 1;
  }

  int Lua::ozObjBindEvent( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua.event = lua.obj->events.citer();
    return 0;
  }

  int Lua::ozEventBindNext( lua_State* l )
  {
    if( lua.event.isValid() ) {
      ++lua.event;
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  int Lua::ozEventGet( lua_State* l )
  {
    if( !lua.event.isValid() ) {
      OZ_LUA_ERROR( "event is null" );
    }
    lua_pushinteger( l, lua.event->id );
    lua_pushnumber( l, lua.event->intensity );
    return 2;
  }

  int Lua::ozDynBindParent( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    const Dynamic* obj = static_cast<const Dynamic*>( lua.obj );

    if( obj->parent != -1 && orbis.objects[obj->parent] != null ) {
      lua.obj = orbis.objects[obj->parent];
      lua_pushboolean( l, true );
    }
    else {
      lua_pushboolean( l, false );
    }
    return 1;
  }

  int Lua::ozDynGetVelocity( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    const Dynamic* obj = static_cast<const Dynamic*>( lua.obj );

    lua_pushnumber( l, obj->velocity.x );
    lua_pushnumber( l, obj->velocity.y );
    lua_pushnumber( l, obj->velocity.z );
    return 3;
  }

  int Lua::ozDynGetMomentum( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    const Dynamic* obj = static_cast<const Dynamic*>( lua.obj );

    lua_pushnumber( l, obj->momentum.x );
    lua_pushnumber( l, obj->momentum.y );
    lua_pushnumber( l, obj->momentum.z );
    return 3;
  }

  int Lua::ozDynGetMass( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    const Dynamic* obj = static_cast<const Dynamic*>( lua.obj );

    lua_pushnumber( l, obj->mass );
    return 1;
  }

  int Lua::ozDynGetLift( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    const Dynamic* obj = static_cast<const Dynamic*>( lua.obj );

    lua_pushnumber( l, obj->lift );
    return 1;
  }

  int Lua::ozBotGetEyePos( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, bot->p.x );
    lua_pushnumber( l, bot->p.y );
    lua_pushnumber( l, bot->p.z + bot->camZ );
    return 3;
  }

  int Lua::ozBotGetH( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, Math::deg( bot->h ) );
    return 1;
  }

  int Lua::ozBotGetV( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, Math::deg( bot->v ) );
    return 1;
  }

  int Lua::ozBotGetDir( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
    float hvsc[6];

    Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
    Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[2] * hvsc[0];
    hvsc[5] = hvsc[2] * hvsc[1];

    lua_pushnumber( l, -hvsc[4] );
    lua_pushnumber( l,  hvsc[5] );
    lua_pushnumber( l, -hvsc[3] );

    return 3;
  }

  int Lua::ozBotStateIsRunning( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushboolean( l, bot->state & Bot::RUNNING_BIT );
    return 1;
  }

  int Lua::ozBotGetStamina( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, bot->stamina );
    return 1;
  }

  int Lua::ozSelfGetEyePos( lua_State* l )
  {
    lua_pushnumber( l, lua.self->p.x );
    lua_pushnumber( l, lua.self->p.y );
    lua_pushnumber( l, lua.self->p.z + lua.self->camZ );
    return 3;
  }

  int Lua::ozSelfGetH( lua_State* l )
  {
    lua_pushnumber( l, Math::deg( lua.self->h ) );
    return 1;
  }

  int Lua::ozSelfSetH( lua_State* l )
  {
    lua.self->h = Math::rad( float( lua_tonumber( l, 1 ) ) );
    lua.self->h = Math::mod( lua.self->h + Math::TAU, Math::TAU );
    return 1;
  }

  int Lua::ozSelfAddH( lua_State* l )
  {
    lua.self->h += Math::rad( float( lua_tonumber( l, 1 ) ) );
    lua.self->h = Math::mod( lua.self->h + Math::TAU, Math::TAU );
    return 1;
  }

  int Lua::ozSelfGetV( lua_State* l )
  {
    lua_pushnumber( l, Math::deg( lua.self->v ) );
    return 1;
  }

  int Lua::ozSelfSetV( lua_State* l )
  {
    lua.self->v = Math::rad( float( lua_tonumber( l, 1 ) ) );
    lua.self->v = clamp( lua.self->v, 0.0f, Math::TAU / 2.0f );
    return 1;
  }

  int Lua::ozSelfAddV( lua_State* l )
  {
    lua.self->v += Math::rad( float( lua_tonumber( l, 1 ) ) );
    lua.self->v = clamp( lua.self->v, 0.0f, Math::TAU / 2.0f );
    return 1;
  }

  int Lua::ozSelfActionForward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_FORWARD;
    return 0;
  }

  int Lua::ozSelfActionBackward( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_BACKWARD;
    return 0;
  }

  int Lua::ozSelfActionRight( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_RIGHT;
    return 0;
  }

  int Lua::ozSelfActionLeft( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_LEFT;
    return 0;
  }

  int Lua::ozSelfActionJump( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_JUMP;
    return 0;
  }

  int Lua::ozSelfActionCrouch( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_CROUCH;
    return 0;
  }

  int Lua::ozSelfActionUse( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_USE;
    return 0;
  }

  int Lua::ozSelfActionTake( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_TAKE;
    return 0;
  }

  int Lua::ozSelfActionGrab( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  int Lua::ozSelfActionThrow( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_THROW;
    return 0;
  }

  int Lua::ozSelfActionAttack( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_ATTACK;
    return 0;
  }

  int Lua::ozSelfActionExit( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_EXIT;
    return 0;
  }

  int Lua::ozSelfActionEject( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_EJECT;
    return 0;
  }

  int Lua::ozSelfActionSuicide( lua_State* )
  {
    lua.self->actions |= Bot::ACTION_SUICIDE;
    return 0;
  }

  int Lua::ozSelfStateIsRunning( lua_State* l )
  {
    lua_pushboolean( l, lua.self->state & Bot::RUNNING_BIT );
    return 1;
  }

  int Lua::ozSelfStateSetRunning( lua_State* l )
  {
    if( lua_toboolean( l, 1 ) ) {
      lua.self->state |= Bot::RUNNING_BIT;
    }
    else {
      lua.self->state &= ~Bot::RUNNING_BIT;
    }
    return 0;
  }

  int Lua::ozSelfStateToggleRunning( lua_State* )
  {
    lua.self->state ^= Bot::RUNNING_BIT;
    return 0;
  }

  int Lua::ozPartBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.parts.length() <= index ) {
      OZ_LUA_ERROR( "invalid particle index" );
    }
    lua.part = orbis.parts[index];
    return 0;
  }

  int Lua::ozPartIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.part == null );
    return 1;
  }

  int Lua::ozPartGetPos( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->p.x );
    lua_pushnumber( l, lua.part->p.y );
    lua_pushnumber( l, lua.part->p.z );
    return 3;
  }

  int Lua::ozPartGetIndex( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushinteger( l, lua.part->index );
    return 1;
  }

  int Lua::ozPartGetVelocity( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->velocity.x );
    lua_pushnumber( l, lua.part->velocity.y );
    lua_pushnumber( l, lua.part->velocity.z );
    return 3;
  }

  int Lua::ozPartGetLife( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->lifeTime );
    return 1;
  }

}
}
