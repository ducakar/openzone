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
 * @file client/luaapi.hh
 *
 * Client Lua API implementation.
 */

#pragma once

#include "nirvana/luaapi.hh"

#include "client/QuestList.hh"
#include "client/Camera.hh"
#include "client/Profile.hh"

namespace oz
{
namespace client
{

struct ClientLuaState
{
  String mission;
  Lingua missionLingua;
};

static ClientLuaState cs;

/// @addtogroup luaapi
/// @{

/*
 * General functions
 */

static int ozGettext( lua_State* l )
{
  ARG( 1 );

  l_pushstring( cs.missionLingua.get( l_tostring( 1 ) ) );
  return 1;
}

/*
 * Orbis
 */

static int ozOrbisAddPlayer( lua_State* l )
{
  VARG( 4, 5 );

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point   p       = Point( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) );
  Heading heading = Heading( l_gettop() == 5 ? l_toint( 5 ) : Math::rand( 4 ) );

  if( mode != ADD_FORCE ) {
    AABB aabb = AABB( p, profile.clazz->dim );

    if( heading & WEST_EAST_MASK ) {
      swap( aabb.dim.x, aabb.dim.y );
    }

    if( collider.overlaps( aabb ) ) {
      ms.obj = null;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.obj = synapse.add( profile.clazz, p, heading );
  l_pushint( ms.obj == null ? -1 : ms.obj->index );

  if( ms.obj != null ) {
    Bot* player = static_cast<Bot*>( ms.obj );

    player->name = profile.name;
    player->mindFunc = "";

    foreach( i, player->items.citer() ) {
      synapse.removeObject( *i );
    }
    player->items.clear();
    player->weapon = -1;

    int iMax = min( player->clazz->nItems, profile.items.length() );

    for( int i = 0; i < iMax; ++i ) {
      Object*  obj  = synapse.add( profile.items[i], Point::ORIGIN, Heading( Math::rand( 4 ) ) );
      Dynamic* item = static_cast<Dynamic*>( obj );

      player->items.add( item->index );
      item->parent = player->index;
      synapse.cut( item );

      if( i == profile.weaponItem ) {
        player->weapon = item->index;
      }
    }
  }
  return 1;
}

/*
 * QuestList
 */

static int ozQuestAdd( lua_State* l )
{
  ARG( 5 );

  questList.quests.add( Quest( l_tostring( 1 ),
                               l_tostring( 2 ),
                               Point( l_tofloat( 3 ), l_tofloat( 4 ), l_tofloat( 5 ) ),
                               Quest::PENDING ) );

  l_pushint( questList.quests.length() - 1 );
  return 1;
}

static int ozQuestEnd( lua_State* l )
{
  ARG( 2 );

  int id = l_toint( 1 );
  if( uint( id ) >= uint( questList.quests.length() ) ) {
    ERROR( "Invalid quest id" );
  }

  questList.quests[id].state = l_tobool( 2 ) ? Quest::SUCCESSFUL : Quest::FAILED;
  return 0;
}

/*
 * Camera
 */

static int ozCameraMoveTo( lua_State* l )
{
  ARG( 5 );

  float h = Math::rad( l_tofloat( 4 ) );
  float v = Math::rad( l_tofloat( 5 ) );
  Point p = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );

  camera.strategic.h = h;
  camera.strategic.v = v;
  camera.strategic.desiredPos = p;

  return 0;
}

static int ozCameraWarpTo( lua_State* l )
{
  ARG( 5 );

  float h   = Math::rad( l_tofloat( 4 ) );
  float v   = Math::rad( l_tofloat( 5 ) );
  Quat  rot = Quat::rotationZXZ( h, v, 0.0f );
  Point p   = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );

  camera.rotateTo( rot );
  camera.moveTo( p );
  camera.strategic.h = h;
  camera.strategic.v = v;
  camera.strategic.desiredPos = p;

  return 0;
}

static int ozCameraAddSwitchableUnit( lua_State* l )
{
  ARG( 1 );
  BOT_INDEX( l_toint( 1 ) );

  camera.switchableUnits.add( bot->index );
  return 0;
}

static int ozCameraClearSwitchableUnits( lua_State* l )
{
  ARG( 1 );
  BOT_INDEX( l_toint( 1 ) );

  camera.switchableUnits.clear();
  camera.switchableUnits.dealloc();
  return 0;
}

static int ozCameraSwitchTo( lua_State* l )
{
  ARG( 1 );
  BOT_INDEX( l_toint( 1 ) );

  camera.setBot( bot );
  camera.setState( Camera::UNIT );
  return 0;
}

static int ozCameraAllowReincarnation( lua_State* l )
{
  ARG( 1 );

  camera.allowReincarnation = l_tobool( 1 );
  return 0;
}

static int ozCameraSetState( lua_State* l )
{
  ARG( 1 );

  camera.setState( Camera::State( l_toint( 1 ) ) );
  return 0;
}

static int ozCameraAddStateSwitch( lua_State* l )
{
  ARG( 1 );

  Camera::State state = Camera::State( l_toint( 1 ) );

  camera.cinematic.addStateSwitch( state );
  return 0;
}

static int ozCameraAddWait( lua_State* l )
{
  ARG( 1 );

  float time = l_tofloat( 1 );

  camera.cinematic.addWait( time );
  return 0;
}

static int ozCameraAddMove( lua_State* l )
{
  ARG( 6 );

  Point p    = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );
  Quat  rot  = Quat::rotationZXZ( Math::rad( l_tofloat( 4 ) ), Math::rad( l_tofloat( 5 ) ), 0.0f );
  float time = l_tofloat( 6 );

  camera.cinematic.addMove( rot, p, time );
  return 0;
}

/*
 * Profile
 */

static int ozProfileGetName( lua_State* l )
{
  ARG( 0 );

  l_pushstring( profile.name );
  return 1;
}

static int ozProfileGetBot( lua_State* l )
{
  ARG( 0 );

  l_pushstring( profile.clazz->name );
  return 1;
}

/// @}

/**
 * Register client-specific %Lua constants with the given %Lua VM.
 *
 * @ingroup client
 */
void importClientConstants( lua_State* l );

}
}
