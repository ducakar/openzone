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

#include "client/QuestList.hh"
#include "client/Camera.hh"
#include "client/Profile.hh"

#include "nirvana/luaapi.hh"

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

/*
 * General functions
 */

static int ozGettext( lua_State* l )
{
  ARG( 1 );

  pushstring( cs.missionLingua.get( tostring( 1 ) ) );
  return 1;
}

/*
 * Orbis
 */

static int ozOrbisAddPlayer( lua_State* l )
{
  ARG_VAR( 3 );

  Point3  p       = Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) );
  Heading heading = Heading( gettop() == 4 ? toint( 4 ) : Math::rand( 4 ) );

  ms.obj = synapse.add( profile.clazz, p, heading );
  pushint( ms.obj == null ? -1 : ms.obj->index );

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
      Object*  obj  = synapse.add( profile.items[i], Point3::ORIGIN, Heading( Math::rand( 4 ) ) );
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

  questList.quests.add( Quest( tostring( 1 ),
                               tostring( 2 ),
                               Point3( tofloat( 3 ), tofloat( 4 ), tofloat( 5 ) ),
                               Quest::PENDING ) );

  pushint( questList.quests.length() - 1 );
  return 1;
}

static int ozQuestEnd( lua_State* l )
{
  ARG( 2 );

  int id = toint( 1 );
  if( uint( id ) >= uint( questList.quests.length() ) ) {
    ERROR( "invalid quest id" );
  }

  questList.quests[id].state = tobool( 2 ) ? Quest::SUCCESSFUL : Quest::FAILED;
  return 0;
}

/*
 * Camera
 */

static int ozCameraGetPos( lua_State* l )
{
  ARG( 0 );

  pushfloat( camera.p.x );
  pushfloat( camera.p.y );
  pushfloat( camera.p.z );

  return 3;
}

static int ozCameraGetDest( lua_State* l )
{
  ARG( 0 );

  pushfloat( camera.newP.x );
  pushfloat( camera.newP.y );
  pushfloat( camera.newP.z );

  return 3;
}

static int ozCameraGetH( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( camera.h ) );
  return 1;
}

static int ozCameraSetH( lua_State* l )
{
  ARG( 1 );

  camera.h = Math::rad( tofloat( 1 ) );
  return 0;
}

static int ozCameraGetV( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( camera.v ) );
  return 1;
}

static int ozCameraSetV( lua_State* l )
{
  ARG( 1 );

  camera.v = Math::rad( tofloat( 1 ) );
  return 0;
}

static int ozCameraMoveTo( lua_State* l )
{
  ARG( 3 );

  Point3 pos = Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) );
  camera.move( pos );

  return 3;
}

static int ozCameraWarpTo( lua_State* l )
{
  ARG( 3 );

  Point3 pos = Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) );
  camera.warp( pos );

  return 3;
}

static int ozCameraIncarnate( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }

  Bot* bot = static_cast<Bot*>( orbis.objects[index] );
  if( bot == null ) {
    ERROR( "object is null" );
  }
  else if( !( bot->flags & Object::BOT_BIT ) ) {
    ERROR( "object is not a bot" );
  }

  camera.setBot( bot );
  camera.setState( Camera::BOT );

  return 0;
}

static int ozCameraAllowReincarnation( lua_State* l )
{
  ARG( 1 );

  camera.allowReincarnation = tobool( 1 );
  return 0;
}

/*
 * Profile
 */

static int ozProfileGetName( lua_State* l )
{
  ARG( 0 );

  pushstring( profile.name );
  return 1;
}

static int ozProfileGetBot( lua_State* l )
{
  ARG( 0 );

  pushstring( profile.clazz->name );
  return 1;
}

}
}
