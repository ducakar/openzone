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
 * @file nirvana/luaapi.hh
 *
 * Nirvana Lua API implementation.
 */

#pragma once

#include "matrix/luaapi.hh"

#include "nirvana/Memo.hh"
#include "nirvana/Nirvana.hh"

namespace oz
{
namespace nirvana
{

struct NirvanaLuaState
{
  Bot* self;

  bool forceUpdate;
};

static NirvanaLuaState ns;

/// @addtogroup luaapi
/// @{

/*
 * General functions
 */

static int ozForceUpdate( lua_State* l )
{
  ARG( 0 );

  ns.forceUpdate = true;
  return 0;
}

/*
 * Mind's bot
 */

static int ozSelfIsCut( lua_State* l )
{
  ARG( 0 );

  l_pushbool( ns.self->cell == null );
  return 1;
}

static int ozSelfGetIndex( lua_State* l )
{
  ARG( 0 );

  l_pushint( ns.self->index );
  return 1;
}

static int ozSelfGetPos( lua_State* l )
{
  ARG( 0 );

  if( ns.self->cell == null ) {
    if( ns.self->parent != -1 ) {
      Object* parent = orbis.objects[ns.self->parent];

      if( parent != null ) {
        l_pushfloat( parent->p.x );
        l_pushfloat( parent->p.y );
        l_pushfloat( parent->p.z );
        return 3;
      }
    }
  }

  l_pushfloat( ns.self->p.x );
  l_pushfloat( ns.self->p.y );
  l_pushfloat( ns.self->p.z );
  return 3;
}

static int ozSelfGetDim( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->dim.x );
  l_pushfloat( ns.self->dim.y );
  l_pushfloat( ns.self->dim.z );
  return 3;
}

static int ozSelfGetFlags( lua_State* l )
{
  ARG( 1 );

  int mask = l_toint( 1 );
  l_pushint( ns.self->flags & mask );
  return 1;
}

static int ozSelfGetTypeName( lua_State* l )
{
  ARG( 0 );

  l_pushstring( ns.self->clazz->name );
  return 1;
}

static int ozSelfGetLife( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->life );
  return 1;
}

static int ozSelfGetVelocity( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->velocity.x );
  l_pushfloat( ns.self->velocity.y );
  l_pushfloat( ns.self->velocity.z );
  return 3;
}

static int ozSelfGetMomentum( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->momentum.x );
  l_pushfloat( ns.self->momentum.y );
  l_pushfloat( ns.self->momentum.z );
  return 3;
}

static int ozSelfGetMass( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->mass );
  return 1;
}

static int ozSelfGetLift( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->lift );
  return 1;
}

static int ozSelfGetName( lua_State* l )
{
  ARG( 0 );

  l_pushstring( ns.self->name );
  return 1;
}

static int ozSelfGetState( lua_State* l )
{
  ARG( 1 );

  int mask = l_toint( 1 );
  l_pushbool( ns.self->state & mask );
  return 1;
}

static int ozSelfGetEyePos( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->p.x );
  l_pushfloat( ns.self->p.y );
  l_pushfloat( ns.self->p.z + ns.self->camZ );
  return 3;
}

static int ozSelfGetH( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( Math::deg( ns.self->h ) );
  return 1;
}

static int ozSelfSetH( lua_State* l )
{
  ARG( 1 );

  ns.self->h = Math::rad( l_tofloat( 1 ) );
  ns.self->h = angleWrap( ns.self->h );
  return 1;
}

static int ozSelfAddH( lua_State* l )
{
  ARG( 1 );

  ns.self->h += Math::rad( l_tofloat( 1 ) );
  ns.self->h  = angleWrap( ns.self->h );
  return 1;
}

static int ozSelfGetV( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( Math::deg( ns.self->v ) );
  return 1;
}

static int ozSelfSetV( lua_State* l )
{
  ARG( 1 );

  ns.self->v = Math::rad( l_tofloat( 1 ) );
  ns.self->v = clamp( ns.self->v, 0.0f, Math::TAU / 2.0f );
  return 1;
}

static int ozSelfAddV( lua_State* l )
{
  ARG( 1 );

  ns.self->v += Math::rad( l_tofloat( 1 ) );
  ns.self->v  = clamp( ns.self->v, 0.0f, Math::TAU / 2.0f );
  return 1;
}

static int ozSelfGetDir( lua_State* l )
{
  ARG( 0 );

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( ns.self->h, &hvsc[0], &hvsc[1] );
  Math::sincos( ns.self->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  l_pushfloat( -hvsc[4] );
  l_pushfloat(  hvsc[5] );
  l_pushfloat( -hvsc[3] );

  return 3;
}

static int ozSelfGetStamina( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( ns.self->stamina );
  return 1;
}

static int ozSelfActionForward( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_FORWARD;
  return 0;
}

static int ozSelfActionBackward( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_BACKWARD;
  return 0;
}

static int ozSelfActionRight( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_RIGHT;
  return 0;
}

static int ozSelfActionLeft( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_LEFT;
  return 0;
}

static int ozSelfActionJump( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_JUMP;
  return 0;
}

static int ozSelfActionCrouch( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_CROUCH;
  return 0;
}

static int ozSelfActionUse( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_USE;
  return 0;
}

static int ozSelfActionTake( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_TAKE;
  return 0;
}

static int ozSelfActionGrab( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_GRAB;
  return 0;
}

static int ozSelfActionThrow( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_THROW;
  return 0;
}

static int ozSelfActionAttack( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_ATTACK;
  return 0;
}

static int ozSelfActionExit( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_EXIT;
  return 0;
}

static int ozSelfActionEject( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_EJECT;
  return 0;
}

static int ozSelfActionSuicide( lua_State* l )
{
  ARG( 0 );

  ns.self->actions |= Bot::ACTION_SUICIDE;
  return 0;
}

static int ozSelfIsRunning( lua_State* l )
{
  ARG( 0 );

  lua_pushboolean( l, ns.self->state & Bot::RUNNING_BIT );
  return 1;
}

static int ozSelfSetRunning( lua_State* l )
{
  ARG( 1 );

  if( lua_toboolean( l, 1 ) ) {
    ns.self->state |= Bot::RUNNING_BIT;
  }
  else {
    ns.self->state &= ~Bot::RUNNING_BIT;
  }
  return 0;
}

static int ozSelfToggleRunning( lua_State* l )
{
  ARG( 0 );

  ns.self->state ^= Bot::RUNNING_BIT;
  return 0;
}

static int ozSelfSetGesture( lua_State* l )
{
  ARG( 1 );
  OBJ();

  ns.self->state &= ~Bot::GESTURE_MASK;
  ns.self->state |= l_toint( 1 );
  return 0;
}

static int ozSelfBindItems( lua_State* l )
{
  ARG( 0 );

  ms.objects.clear();
  foreach( item, ns.self->items.citer() ) {
    ms.objects.add( orbis.objects[*item] );
  }
  ms.objIndex = 0;
  return 0;
}

static int ozSelfBindAllOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( ns.self->p,
                    Vec3( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) ) );

  ms.objects.clear();
  ms.structs.clear();
  collider.getOverlaps( aabb, &ms.structs, &ms.objects );
  ms.objIndex = 0;
  ms.strIndex = 0;
  return 0;
}

static int ozSelfBindStrOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( ns.self->p,
                    Vec3( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) ) );

  ms.structs.clear();
  collider.getOverlaps( aabb, &ms.structs, null );
  ms.strIndex = 0;
  return 0;
}

static int ozSelfBindObjOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( ns.self->p,
                    Vec3( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) ) );

  ms.objects.clear();
  collider.getOverlaps( aabb, null, &ms.objects );
  ms.objIndex = 0;
  return 0;
}

static int ozSelfBindParent( lua_State* l )
{
  ARG( 0 );

  if( ns.self->parent != -1 && orbis.objects[ns.self->parent] != null ) {
    ms.obj = orbis.objects[ns.self->parent];
    l_pushbool( true );
  }
  else {
    l_pushbool( false );
  }
  return 1;
}

/*
 * Nirvana
 */

static int ozNirvanaRemoveDevice( lua_State* l )
{
  ARG( 1 );

  int index = l_toint( 1 );
  const nirvana::Device* const* device = nirvana::nirvana.devices.find( index );

  if( device == null ) {
    l_pushbool( false );
  }
  else {
    delete *device;
    nirvana::nirvana.devices.exclude( index );
    l_pushbool( true );
  }
  return 1;
}

static int ozNirvanaAddMemo( lua_State* l )
{
  ARG( 2 );

  int index = l_toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }
  if( orbis.objects[index] == null ) {
    ERROR( "object is null" );
  }

  if( nirvana::nirvana.devices.contains( index ) ) {
    ERROR( "object already has a device" );
  }

  nirvana::nirvana.devices.add( index, new nirvana::Memo( l_tostring( 2 ) ) );
  return 0;
}

/// @}

}
}
