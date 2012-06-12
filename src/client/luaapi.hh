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

static int ozCameraGetPos( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( camera.p.x );
  l_pushfloat( camera.p.y );
  l_pushfloat( camera.p.z );

  return 3;
}

static int ozCameraGetH( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( Math::deg( camera.h ) );
  return 1;
}

static int ozCameraSetH( lua_State* l )
{
  ARG( 1 );

  camera.h = Math::rad( l_tofloat( 1 ) );
  return 0;
}

static int ozCameraGetV( lua_State* l )
{
  ARG( 0 );

  l_pushfloat( Math::deg( camera.v ) );
  return 1;
}

static int ozCameraSetV( lua_State* l )
{
  ARG( 1 );

  camera.v = Math::rad( l_tofloat( 1 ) );
  return 0;
}

static int ozCameraMoveTo( lua_State* l )
{
  ARG( 3 );

  Point pos = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );
  camera.move( pos );

  return 3;
}

static int ozCameraSmoothMoveTo( lua_State* l )
{
  ARG( 3 );

  Point pos = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );
  camera.smoothMove( pos );

  return 3;
}

static int ozCameraWarpTo( lua_State* l )
{
  ARG( 3 );

  Point pos = Point( l_tofloat( 1 ), l_tofloat( 2 ), l_tofloat( 3 ) );
  camera.warp( pos );

  return 3;
}

static int ozCameraIncarnate( lua_State* l )
{
  ARG( 1 );
  BOT_INDEX( l_toint( 1 ) );

  camera.setBot( bot );
  camera.setState( Camera::BOT );
  return 0;
}

static int ozCameraAllowReincarnation( lua_State* l )
{
  ARG( 1 );

  camera.allowReincarnation = l_tobool( 1 );
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

}
}
