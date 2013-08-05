/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include <nirvana/luaapi.hh>

#include <client/Camera.hh>
#include <client/Profile.hh>
#include <client/ui/BuildMenu.hh>
#include <client/ui/UI.hh>

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
  VARG( 4, 6 );

  AddMode mode    = AddMode( l_toint( 1 ) );
  Point   p       = Point( l_tofloat( 2 ), l_tofloat( 3 ), l_tofloat( 4 ) );
  Heading heading = Heading( Math::rand( 4 ) );
  bool    empty   = false;

  int nParams = l_gettop();
  for( int i = 5; i <= nParams; ++i ) {
    if( l_type( i ) == LUA_TBOOLEAN ) {
      empty = l_tobool( i );
    }
    else {
      heading = Heading( l_toint( i ) );
    }
  }

  if( mode != ADD_FORCE ) {
    AABB aabb = AABB( p, profile.clazz->dim );

    if( heading & WEST_EAST_MASK ) {
      swap( aabb.dim.x, aabb.dim.y );
    }

    if( collider.overlaps( aabb ) ) {
      ms.obj = nullptr;
      l_pushint( -1 );
      return 1;
    }
  }

  ms.obj = synapse.add( profile.clazz, p, heading, true );
  l_pushint( ms.obj == nullptr ? -1 : ms.obj->index );

  if( ms.obj != nullptr ) {
    Bot* player = static_cast<Bot*>( ms.obj );

    player->name = profile.name;
    player->mindFunc = "";

    if( !empty ) {
      int invMax = min( player->clazz->nItems, profile.items.length() );

      for( int i = 0; i < invMax; ++i ) {
        Object*  obj  = synapse.add( profile.items[i], Point::ORIGIN, Heading( Math::rand( 4 ) ),
                                    true );
        Dynamic* item = static_cast<Dynamic*>( obj );

        player->items.add( item->index );
        item->parent = player->index;
        synapse.cut( item );

        if( i == profile.weaponItem ) {
          player->weapon = item->index;
        }
      }
    }
  }
  return 1;
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
  camera.switchableUnits.deallocate();
  return 0;
}

static int ozCameraSwitchTo( lua_State* l )
{
  ARG( 1 );
  BOT_INDEX( l_toint( 1 ) );

  camera.rotateTo( Quat::rotationZXZ( bot->h, bot->v, 0.0f ) );
  camera.moveTo( Point( bot->p.x, bot->p.y, bot->p.z + bot->camZ ) );
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

static int ozCameraExecuteSequence( lua_State* l )
{
  ARG( 1 );

  String file = String::str( "@mission/%s/%s.sequence.json", cs.mission.cstr(), l_tostring( 1 ) );

  camera.setState( Camera::CINEMATIC );
  camera.cinematic.executeSequence( file, &cs.missionLingua );
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

/*
 * UI
 */

static int ozUIBuildMenu( lua_State* l )
{
  ARG( 1 );

  camera.strategic.hasBuildMenu = l_tobool( 1 );
  return 0;
}

/// @}

/**
 * Register client-specific Lua constants with a given Lua VM.
 */
void importClientConstants( lua_State* l );

}
}
