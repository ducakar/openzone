/*
 *  Lua.cpp
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Lua.hpp"

#include "matrix/BotClass.hpp"
#include "matrix/Bot.hpp"
#include "matrix/Collider.hpp"
#include "matrix/Synapse.hpp"

#include <lua.hpp>

#define OZ_LUA_ERROR( message ) luaL_error( l, "[%s] %s", __FUNCTION__, message );
#define OZ_LUA_FUNCTION( func ) lua_register( l, #func, func )
#define OZ_LUA_INT_CONST( name, value ) lua_pushinteger( l, value ); lua_setglobal( l, name )
#define OZ_LUA_FLOAT_CONST( name, value ) lua_pushnumber( l, value ); lua_setglobal( l, name )
#define OZ_LUA_STRING_CONST( name, value ) lua_pushstring( l, value ); lua_setglobal( l, name )

namespace oz
{

  Lua lua;

  static int ozPrintln( lua_State* l )
  {
    log.println( "M> %s", lua_tostring( l, 1 ) );
    return 0;
  }

  static int ozException( lua_State* l )
  {
    const char* message = lua_tostring( l, 1 );
    throw Exception( message );
  }

  static int ozBindAllOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.objects.clear();
    lua.structs.clear();
    collider.getOverlaps( aabb, &lua.objects, &lua.structs );
    lua.objIndex = 0;
    lua.strIndex = 0;
    return 0;
  }

  static int ozBindStrOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  static int ozBindObjOverlaps( lua_State* l )
  {
    AABB aabb = AABB( Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ),
                      Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  static int ozSelfBindAllOverlaps( lua_State* l )
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

  static int ozSelfBindStrOverlaps( lua_State* l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ) );
    lua.structs.clear();
    collider.getOverlaps( aabb, null, &lua.structs );
    lua.strIndex = 0;
    return 0;
  }

  static int ozSelfBindObjOverlaps( lua_State* l )
  {
    AABB aabb = AABB( lua.self->p,
                      Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) ) );
    lua.objects.clear();
    collider.getOverlaps( aabb, &lua.objects, null );
    lua.objIndex = 0;
    return 0;
  }

  static int ozStrBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.structs.length() <= index ) {
      OZ_LUA_ERROR( "invalid structure index" );
    }
    lua.str = orbis.structs[index];
    return 0;
  }

  static int ozStrBindNext( lua_State* l )
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

  static int ozStrIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.str == null );
    return 1;
  }

  static int ozStrGetBounds( lua_State* l )
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

  static int ozStrGetPos( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushnumber( l, lua.str->p.x );
    lua_pushnumber( l, lua.str->p.y );
    lua_pushnumber( l, lua.str->p.z );
    return 3;
  }

  static int ozStrGetIndex( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushinteger( l, lua.str->index );
    return 1;
  }

  static int ozStrGetBSP( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua_pushinteger( l, lua.str->bsp );
    return 1;
  }

  static int ozStrDamage( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua.str->damage( float( lua_tonumber( l, 1 ) ) );
    return 0;
  }

  static int ozStrDestroy( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    lua.str->destroy();
    return 0;
  }

  static int ozStrVectorFromSelf( lua_State* l )
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

  static int ozStrDirectionFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    Vec3 dir = ( lua.str->p - lua.self->p ).norm();
    lua_pushnumber( l, dir.x );
    lua_pushnumber( l, dir.y );
    lua_pushnumber( l, dir.z );
    return 3;
  }

  static int ozStrDistanceFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    lua_pushnumber( l, !( lua.str->p - lua.self->p ) );
    return 1;
  }

  static int ozStrHeadingFromSelf( lua_State* l )
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

  static int ozStrPitchFromSelf( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }
    float dx = lua.str->p.x - lua.self->p.x;
    float dy = lua.str->p.y - lua.self->p.y;
    float dz = lua.str->p.z - lua.self->p.z;
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozObjBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.objects.length() <= index ) {
      OZ_LUA_ERROR( "invalid object index" );
    }
    lua.obj = orbis.objects[index];
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

  static int ozObjBindNext( lua_State* l )
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

  static int ozObjIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.obj == null );
    return 1;
  }

  static int ozObjIsSelf( lua_State* l )
  {
    lua_pushboolean( l, lua.obj == lua.self );
    return 1;
  }

  static int ozObjIsUser( lua_State* l )
  {
    lua_pushboolean( l, lua.obj == lua.user );
    return 1;
  }

  static int ozObjIsPut( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua_pushboolean( l, lua.obj->cell != null );
    return 1;
  }

  static int ozObjIsDynamic( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::DYNAMIC_BIT ) );
    return 1;
  }

  static int ozObjIsItem( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::ITEM_BIT ) );
    return 1;
  }

  static int ozObjIsWeapon( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::WEAPON_BIT ) );
    return 1;
  }

  static int ozObjIsBot( lua_State* l )
  {
    const Bot* bot = static_cast<const Bot*>( lua.obj );
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::BOT_BIT ) &&
                     !( bot->state & Bot::DEATH_BIT ) );
    return 1;
  }

  static int ozObjIsVehicle( lua_State* l )
  {
    lua_pushboolean( l, lua.obj != null && ( lua.obj->flags & Object::VEHICLE_BIT ) );
    return 1;
  }

  static int ozObjGetPos( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    if( lua.obj->cell == null ) {
      assert( lua.obj->flags & Object::DYNAMIC_BIT );

      const Dynamic* dyn = static_cast<const Dynamic*>( lua.obj );

      if( dyn->parent != -1 ) {
        Object* parent = orbis.objects[dyn->parent];

        if( parent != null ) {
          lua_pushnumber( l, parent->p.x );
          lua_pushnumber( l, parent->p.y );
          lua_pushnumber( l, parent->p.z );
          return 3;
        }
      }
    }

    lua_pushnumber( l, lua.obj->p.x );
    lua_pushnumber( l, lua.obj->p.y );
    lua_pushnumber( l, lua.obj->p.z );
    return 3;
  }

  static int ozObjSetPos( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->p.x = float( lua_tonumber( l, 1 ) );
    lua.obj->p.y = float( lua_tonumber( l, 2 ) );
    lua.obj->p.z = float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozObjAddPos( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->p.x += float( lua_tonumber( l, 1 ) );
    lua.obj->p.y += float( lua_tonumber( l, 2 ) );
    lua.obj->p.z += float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozObjGetDim( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->dim.x );
    lua_pushnumber( l, lua.obj->dim.y );
    lua_pushnumber( l, lua.obj->dim.z );
    return 3;
  }

  static int ozObjGetIndex( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->index );
    return 1;
  }

  static int ozObjGetFlags( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->flags );
    return 1;
  }

  static int ozObjGetOldFlags( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushinteger( l, lua.obj->oldFlags );
    return 1;
  }

  static int ozObjGetTypeName( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushstring( l, lua.obj->type->name );
    return 1;
  }

  static int ozObjGetLife( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua_pushnumber( l, lua.obj->life );
    return 1;
  }

  static int ozObjSetLife( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = Math::bound( float( lua_tonumber( l, 1 ) ), 0.0f, lua.obj->type->life );
    return 0;
  }

  static int ozObjAddLife( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = Math::bound( lua.obj->life + float( lua_tonumber( l, 1 ) ),
                                 0.0f,
                                 lua.obj->type->life );
    return 0;
  }

  static int ozObjAddEvent( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    int   id        = int( lua_tointeger( l, 1 ) );
    float intensity = float( lua_tonumber( l, 2 ) );

    if( id >= 0 && intensity < 0.0f ) {
      OZ_LUA_ERROR( "event intensity for sounds (id >= 0) has to be > 0.0" );
    }
    lua.obj->addEvent( id, intensity );
    return 0;
  }

  static int ozObjDamage( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->damage( float( lua_tonumber( l, 1 ) ) );
    return 0;
  }

  static int ozObjDestroy( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    lua.obj->life = 0.0f;
    return 0;
  }

  static int ozObjVectorFromSelf( lua_State* l )
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

  static int ozObjDirectionFromSelf( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    Vec3 dir = ( lua.obj->p - lua.self->p ).norm();
    lua_pushnumber( l, dir.x );
    lua_pushnumber( l, dir.y );
    lua_pushnumber( l, dir.z );
    return 3;
  }

  static int ozObjDistanceFromSelf( lua_State* l )
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

  static int ozObjHeadingFromSelf( lua_State* l )
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

  static int ozObjPitchFromSelf( lua_State* l )
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
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozObjPitchFromSelfEye( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( lua.obj == lua.self ) {
      OZ_LUA_ERROR( "selected object is self" );
    }
    if( lua.self->flags & Object::BOT_BIT ) {
      OZ_LUA_ERROR( "self is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.self );

    float dx = lua.obj->p.x - bot->p.x;
    float dy = lua.obj->p.y - bot->p.y;
    float dz = lua.obj->p.z - bot->p.z - bot->camZ;
    float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) );

    lua_pushnumber( l, angle );
    return 1;
  }

  static int ozObjBindEvent( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    lua.event = lua.obj->events.iter();
    return 0;
  }

  static int ozEventBindNext( lua_State* l )
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

  static int ozEventGet( lua_State* l )
  {
    if( !lua.event.isValid() ) {
      OZ_LUA_ERROR( "event is null" );
    }
    lua_pushinteger( l, lua.event->id );
    lua_pushnumber( l, lua.event->intensity );
    return 2;
  }

  static int ozDynGetVelocity( lua_State* l )
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

  static int ozDynGetMomentum( lua_State* l )
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

  static int ozDynSetMomentum( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->flags &= ~Object::DISABLED_BIT;
    obj->momentum.x = float( lua_tonumber( l, 1 ) );
    obj->momentum.y = float( lua_tonumber( l, 2 ) );
    obj->momentum.z = float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozDynAddMomentum( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->flags &= ~Object::DISABLED_BIT;
    obj->momentum.x += float( lua_tonumber( l, 1 ) );
    obj->momentum.y += float( lua_tonumber( l, 2 ) );
    obj->momentum.z += float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozDynGetMass( lua_State* l )
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

  static int ozDynSetMass( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->mass = float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozDynAddMass( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->mass += float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozDynResetMass( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );
    const DynamicClass* clazz = static_cast<const DynamicClass*>( lua.obj->type );

    obj->mass = clazz->mass;
    return 0;
  }

  static int ozDynGetLift( lua_State* l )
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

  static int ozDynSetLift( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->lift = float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozDynAddLift( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );

    obj->lift += float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozDynResetLift( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::DYNAMIC_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not dynamic" );
    }

    Dynamic* obj = static_cast<Dynamic*>( lua.obj );
    const DynamicClass* clazz = static_cast<const DynamicClass*>( lua.obj->type );

    obj->lift = clazz->lift;
    return 0;
  }

  static int ozBotGetEyePos( lua_State* l )
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

  static int ozBotGetH( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, bot->h );
    return 1;
  }

  static int ozBotSetH( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->h = float( lua_tonumber( l, 1 ) );
    return 1;
  }

  static int ozBotAddH( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->h += float( lua_tonumber( l, 1 ) );
    return 1;
  }

  static int ozBotGetV( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    lua_pushnumber( l, bot->v );
    return 1;
  }

  static int ozBotSetV( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->v = float( lua_tonumber( l, 1 ) );
    return 1;
  }

  static int ozBotAddV( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->v += float( lua_tonumber( l, 1 ) );
    return 1;
  }

  static int ozBotGetDir( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    const Bot* bot = static_cast<const Bot*>( lua.obj );

    // { hsine, hcosine, vsine, vcosine, vcosine * hsine, vcosine * hcosine }
    float hvsc[6];

    Math::sincos( Math::rad( bot->h ), &hvsc[0], &hvsc[1] );
    Math::sincos( Math::rad( bot->v ), &hvsc[2], &hvsc[3] );

    hvsc[4] = hvsc[3] * hvsc[0];
    hvsc[5] = hvsc[3] * hvsc[1];

    lua_pushnumber( l, -hvsc[4] );
    lua_pushnumber( l,  hvsc[5] );
    lua_pushnumber( l,  hvsc[2] );

    return 3;
  }

  static int ozBotActionForward( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_FORWARD;
    return 0;
  }

  static int ozBotActionBackward( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_BACKWARD;
    return 0;
  }

  static int ozBotActionRight( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_RIGHT;
    return 0;
  }

  static int ozBotActionLeft( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_LEFT;
    return 0;
  }

  static int ozBotActionJump( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_JUMP;
    return 0;
  }

  static int ozBotActionCrouch( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_CROUCH;
    return 0;
  }

  static int ozBotActionUse( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_USE;
    return 0;
  }

  static int ozBotActionTake( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_TAKE;
    return 0;
  }

  static int ozBotActionGrab( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_GRAB;
    return 0;
  }

  static int ozBotActionThrow( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_THROW;
    return 0;
  }

  static int ozBotActionAttack( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_ATTACK;
    return 0;
  }

  static int ozBotActionExit( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_EXIT;
    return 0;
  }

  static int ozBotActionEject( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_EJECT;
    return 0;
  }

  static int ozBotActionSuicide( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->actions |= Bot::ACTION_SUICIDE;
    return 0;
  }

  static int ozBotStateIsRunning( lua_State* l )
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

  static int ozBotStateSetRunning( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    if( lua_toboolean( l, 1 ) ) {
      bot->state |= Bot::RUNNING_BIT;
    }
    else {
      bot->state &= ~Bot::RUNNING_BIT;
    }
    return 0;
  }

  static int ozBotStateToggleRunning( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );

    bot->state ^= Bot::RUNNING_BIT;
    return 0;
  }

  static int ozBotGetStamina( lua_State* l )
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

  static int ozBotSetStamina( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );
    const BotClass* clazz = static_cast<const BotClass*>( bot->type );

    bot->stamina = Math::bound( float( lua_tonumber( l, 1 ) ), 0.0f, clazz->stamina );
    return 0;
  }

  static int ozBotAddStamina( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }
    if( !( lua.obj->flags & Object::BOT_BIT ) ) {
      OZ_LUA_ERROR( "selected object is not a bot" );
    }

    Bot* bot = static_cast<Bot*>( lua.obj );
    const BotClass* clazz = static_cast<const BotClass*>( bot->type );

    bot->stamina = Math::bound( bot->stamina + float( lua_tonumber( l, 1 ) ), 0.0f, clazz->stamina );
    return 0;
  }

  static int ozPartBindIndex( lua_State* l )
  {
    int index = int( lua_tointeger( l, 1 ) );
    if( index < 0 || orbis.parts.length() <= index ) {
      OZ_LUA_ERROR( "invalid particle index" );
    }
    lua.part = orbis.parts[index];
    return 0;
  }

  static int ozPartIsNull( lua_State* l )
  {
    lua_pushboolean( l, lua.part == null );
    return 1;
  }

  static int ozPartGetPos( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->p.x );
    lua_pushnumber( l, lua.part->p.y );
    lua_pushnumber( l, lua.part->p.z );
    return 3;
  }

  static int ozPartSetPos( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->p.x = float( lua_tonumber( l, 1 ) );
    lua.part->p.y = float( lua_tonumber( l, 2 ) );
    lua.part->p.z = float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozPartAddPos( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->p.x += float( lua_tonumber( l, 1 ) );
    lua.part->p.y += float( lua_tonumber( l, 2 ) );
    lua.part->p.z += float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozPartGetIndex( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushinteger( l, lua.part->index );
    return 1;
  }

  static int ozPartGetVelocity( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->velocity.x );
    lua_pushnumber( l, lua.part->velocity.y );
    lua_pushnumber( l, lua.part->velocity.z );
    return 3;
  }

  static int ozPartSetVelocity( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->velocity.x = float( lua_tonumber( l, 1 ) );
    lua.part->velocity.y = float( lua_tonumber( l, 2 ) );
    lua.part->velocity.z = float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozPartAddVelocity( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->velocity.x += float( lua_tonumber( l, 1 ) );
    lua.part->velocity.y += float( lua_tonumber( l, 2 ) );
    lua.part->velocity.z += float( lua_tonumber( l, 3 ) );
    return 0;
  }

  static int ozPartGetLife( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua_pushnumber( l, lua.part->lifeTime );
    return 1;
  }

  static int ozPartSetLife( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->lifeTime = float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozPartAddLife( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    lua.part->lifeTime += float( lua_tonumber( l, 1 ) );
    return 0;
  }

  static int ozOrbisAddStr( lua_State* l )
  {
    const char* name = lua_tostring( l, 1 );
    Vec3 p = Vec3( float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ), float( lua_tonumber( l, 4 ) ) );
    Structure::Rotation rot = Structure::Rotation( lua_tointeger( l, 5 ) );

    int bsp = translator.bspIndex( name );
    if( bsp == -1 ) {
      OZ_LUA_ERROR( "invalid bsp name" );
    }

    orbis.requestBSP( bsp );
    Bounds bounds = Structure::rotate( *orbis.bsps[bsp], rot );

    if( !collider.overlaps( bounds.toAABB() + p ) ) {
      int index = synapse.addStruct( name, p, rot );
      lua.str = orbis.structs[index];
      lua_pushinteger( l, index );
    }
    else {
      lua.str = null;
      lua_pushinteger( l, -1 );
    }
    return 1;
  }

  static int ozOrbisForceAddStr( lua_State* l )
  {
    const char* name = lua_tostring( l, 1 );
    Vec3 p = Vec3( float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ), float( lua_tonumber( l, 4 ) ) );
    Structure::Rotation rot = Structure::Rotation( lua_tointeger( l, 5 ) );

    int index = synapse.addStruct( name, p, rot );
    lua.str = orbis.structs[index];
    lua_pushinteger( l, index );
    return 1;
  }

  static int ozOrbisAddObj( lua_State* l )
  {
    const char* name = lua_tostring( l, 1 );
    Vec3 p = Vec3( float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ), float( lua_tonumber( l, 4 ) ) );

    const ObjectClass* const* value = translator.classes.find( name );
    if( value == null ) {
      OZ_LUA_ERROR( "invalid object class" );
    }

    AABB aabb = AABB( p, ( *value )->dim );

    if( collider.overlaps( aabb ) ) {
      int index = synapse.addObject( name, p );
      lua.obj = orbis.objects[index];
      lua_pushinteger( l, index );
    }
    else {
      lua.obj = null;
      lua_pushinteger( l, -1 );
    }
    return 1;
  }

  static int ozOrbisForceAddObj( lua_State* l )
  {
    const char* name = lua_tostring( l, 1 );
    Vec3 p = Vec3( float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ), float( lua_tonumber( l, 4 ) ) );

    int index = synapse.addObject( name, p );
    lua.obj = orbis.objects[index];
    lua_pushinteger( l, index );
    return 1;
  }

  static int ozOrbisAddPart( lua_State* l )
  {
    Vec3  p           = Vec3( float( lua_tonumber( l, 1 ) ), float( lua_tonumber( l, 2 ) ), float( lua_tonumber( l, 3 ) ) );
    Vec3  velocity    = Vec3( float( lua_tonumber( l, 4 ) ), float( lua_tonumber( l, 5 ) ), float( lua_tonumber( l, 6 ) ) );
    Vec3  colour      = Vec3( float( lua_tonumber( l, 7 ) ), float( lua_tonumber( l, 8 ) ), float( lua_tonumber( l, 9 ) ) );
    float restitution = float( lua_tonumber( l, 10 ) );
    float mass        = float( lua_tonumber( l, 11 ) );
    float lifeTime    = float( lua_tonumber( l, 12 ) );

    int index = synapse.addPart( p, velocity, colour, restitution, mass, lifeTime );
    lua.part = orbis.parts[index];
    lua_pushinteger( l, index );
    return 1;
  }

  static int ozOrbisRemoveStr( lua_State* l )
  {
    if( lua.str == null ) {
      OZ_LUA_ERROR( "selected structure is null" );
    }

    synapse.remove( lua.str );
    lua.str = null;
    return 0;
  }

  static int ozOrbisRemoveObj( lua_State* l )
  {
    if( lua.obj == null ) {
      OZ_LUA_ERROR( "selected object is null" );
    }

    if( lua.obj->cell == null ) {
      Dynamic* dyn = static_cast<Dynamic*>( lua.obj );

      assert( dyn->flags & Object::DYNAMIC_BIT );

      synapse.removeCut( dyn );
    }
    else {
      synapse.remove( lua.obj );
    }

    lua.self = lua.self == lua.obj ? null : lua.self;
    lua.user = lua.user == lua.obj ? null : lua.user;
    lua.obj = null;
    return 0;
  }

  static int ozOrbisRemovePart( lua_State* l )
  {
    if( lua.part == null ) {
      OZ_LUA_ERROR( "selected particle is null" );
    }

    synapse.remove( lua.part );
    lua.part = null;
    return 0;
  }

  void Lua::callFunc( const char* functionName, int index )
  {
    assert( self != null );

    obj   = self;
    str   = null;
    part  = null;
    event = List<Object::Event>::Iterator();

    objIndex = 0;
    strIndex = 0;

    lua_getglobal( l, "ozLocalData" );
    lua_getglobal( l, functionName );
    lua_rawgeti( l, -2, index );
    lua_pcall( l, 1, 0, 0 );

    if( lua_isstring( l, -1 ) ) {
      log.println( "M! %s", lua_tostring( l, -1 ) );
      lua_pop( l, 1 );
    }
    lua_pop( l, 1 );
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
    log.println( "Initialising Matrix Lua {" );
    log.indent();

    l = lua_open();
    luaL_openlibs( l );

    OZ_LUA_FUNCTION( ozPrintln );
    OZ_LUA_FUNCTION( ozException );

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
    OZ_LUA_FUNCTION( ozStrDamage );
    OZ_LUA_FUNCTION( ozStrDestroy );

    OZ_LUA_FUNCTION( ozStrVectorFromSelf );
    OZ_LUA_FUNCTION( ozStrDirectionFromSelf );
    OZ_LUA_FUNCTION( ozStrDistanceFromSelf );
    OZ_LUA_FUNCTION( ozStrHeadingFromSelf );
    OZ_LUA_FUNCTION( ozStrPitchFromSelf );

    OZ_LUA_FUNCTION( ozObjBindIndex );
    OZ_LUA_FUNCTION( ozObjBindSelf );
    OZ_LUA_FUNCTION( ozObjBindUser );
    OZ_LUA_FUNCTION( ozObjBindNext );

    OZ_LUA_FUNCTION( ozObjIsNull );
    OZ_LUA_FUNCTION( ozObjIsSelf );
    OZ_LUA_FUNCTION( ozObjIsUser );
    OZ_LUA_FUNCTION( ozObjIsPut );
    OZ_LUA_FUNCTION( ozObjIsDynamic );
    OZ_LUA_FUNCTION( ozObjIsItem );
    OZ_LUA_FUNCTION( ozObjIsWeapon );
    OZ_LUA_FUNCTION( ozObjIsBot );
    OZ_LUA_FUNCTION( ozObjIsVehicle );
    OZ_LUA_FUNCTION( ozObjGetPos );
    OZ_LUA_FUNCTION( ozObjSetPos );
    OZ_LUA_FUNCTION( ozObjAddPos );
    OZ_LUA_FUNCTION( ozObjGetDim );
    OZ_LUA_FUNCTION( ozObjGetIndex );
    OZ_LUA_FUNCTION( ozObjGetFlags );
    OZ_LUA_FUNCTION( ozObjGetOldFlags );
    OZ_LUA_FUNCTION( ozObjGetTypeName );
    OZ_LUA_FUNCTION( ozObjGetLife );
    OZ_LUA_FUNCTION( ozObjSetLife );
    OZ_LUA_FUNCTION( ozObjAddLife );
    OZ_LUA_FUNCTION( ozObjAddEvent );
    OZ_LUA_FUNCTION( ozObjDamage );
    OZ_LUA_FUNCTION( ozObjDestroy );

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
    OZ_LUA_FUNCTION( ozDynSetMomentum );
    OZ_LUA_FUNCTION( ozDynAddMomentum );
    OZ_LUA_FUNCTION( ozDynGetMass );
    OZ_LUA_FUNCTION( ozDynSetMass );
    OZ_LUA_FUNCTION( ozDynAddMass );
    OZ_LUA_FUNCTION( ozDynResetMass );
    OZ_LUA_FUNCTION( ozDynGetLift );
    OZ_LUA_FUNCTION( ozDynSetLift );
    OZ_LUA_FUNCTION( ozDynAddLift );
    OZ_LUA_FUNCTION( ozDynResetLift );

    OZ_LUA_FUNCTION( ozBotGetEyePos );
    OZ_LUA_FUNCTION( ozBotGetH );
    OZ_LUA_FUNCTION( ozBotSetH );
    OZ_LUA_FUNCTION( ozBotAddH );
    OZ_LUA_FUNCTION( ozBotGetV );
    OZ_LUA_FUNCTION( ozBotSetV );
    OZ_LUA_FUNCTION( ozBotAddV );
    OZ_LUA_FUNCTION( ozBotGetDir );
    OZ_LUA_FUNCTION( ozBotActionForward );
    OZ_LUA_FUNCTION( ozBotActionBackward );
    OZ_LUA_FUNCTION( ozBotActionRight );
    OZ_LUA_FUNCTION( ozBotActionLeft );
    OZ_LUA_FUNCTION( ozBotActionJump );
    OZ_LUA_FUNCTION( ozBotActionCrouch );
    OZ_LUA_FUNCTION( ozBotActionUse );
    OZ_LUA_FUNCTION( ozBotActionTake );
    OZ_LUA_FUNCTION( ozBotActionGrab );
    OZ_LUA_FUNCTION( ozBotActionThrow );
    OZ_LUA_FUNCTION( ozBotActionAttack );
    OZ_LUA_FUNCTION( ozBotActionExit );
    OZ_LUA_FUNCTION( ozBotActionEject );
    OZ_LUA_FUNCTION( ozBotActionSuicide );
    OZ_LUA_FUNCTION( ozBotStateIsRunning );
    OZ_LUA_FUNCTION( ozBotStateSetRunning );
    OZ_LUA_FUNCTION( ozBotStateToggleRunning );
    OZ_LUA_FUNCTION( ozBotGetStamina );
    OZ_LUA_FUNCTION( ozBotSetStamina );
    OZ_LUA_FUNCTION( ozBotAddStamina );

    OZ_LUA_FUNCTION( ozPartBindIndex );
    OZ_LUA_FUNCTION( ozPartIsNull );
    OZ_LUA_FUNCTION( ozPartGetPos );
    OZ_LUA_FUNCTION( ozPartSetPos );
    OZ_LUA_FUNCTION( ozPartAddPos );
    OZ_LUA_FUNCTION( ozPartGetIndex );
    OZ_LUA_FUNCTION( ozPartGetVelocity );
    OZ_LUA_FUNCTION( ozPartSetVelocity );
    OZ_LUA_FUNCTION( ozPartAddVelocity );
    OZ_LUA_FUNCTION( ozPartGetLife );
    OZ_LUA_FUNCTION( ozPartSetLife );
    OZ_LUA_FUNCTION( ozPartAddLife );

    OZ_LUA_FUNCTION( ozOrbisAddStr );
    OZ_LUA_FUNCTION( ozOrbisForceAddStr );
    OZ_LUA_FUNCTION( ozOrbisAddObj );
    OZ_LUA_FUNCTION( ozOrbisForceAddObj );
    OZ_LUA_FUNCTION( ozOrbisAddPart );
    OZ_LUA_FUNCTION( ozOrbisRemoveStr );
    OZ_LUA_FUNCTION( ozOrbisRemoveObj );
    OZ_LUA_FUNCTION( ozOrbisRemovePart );

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
    OZ_LUA_INT_CONST( "OZ_OBJECT_DETECT_BIT",           Object::DETECT_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_CLIMBER_BIT",          Object::CLIMBER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_PUSHER_BIT",           Object::PUSHER_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_HOVER_BIT",            Object::HOVER_BIT );

    OZ_LUA_INT_CONST( "OZ_OBJECT_NO_DRAW_BIT",          Object::NO_DRAW_BIT );
    OZ_LUA_INT_CONST( "OZ_OBJECT_WIDE_CULL_BIT",        Object::WIDE_CULL_BIT );

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

    for( int i = 0; i < translator.matrixScripts.length(); ++i ) {
      const Translator::Resource& res = translator.matrixScripts[i];

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
