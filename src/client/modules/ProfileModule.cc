/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file client/modules/ProfileModule.cc
 */

#include "stable.hh"

#include "client/modules/ProfileModule.hh"

#include "client/Lua.hh"

#include "luamacros.hh"

#ifndef __clang__
// GCC bug, issues false warning
# pragma GCC diagnostic ignored "-Wconversion"
#endif

namespace oz
{
namespace client
{

ProfileModule profileModule;

void ProfileModule::registerLua() const
{
  OZ_LUA_FUNC( ozProfileGetPlayerName );
}

void ProfileModule::init()
{
  const char* userName = SDL_getenv( "USER" );
  userName = userName == null ? "Player" : userName;

  char playerName[64];
  strncpy( playerName, userName, 64 );
  playerName[63] = '\0';

  if( 'a' <= playerName[0] && playerName[0] <= 'z' ) {
    playerName[0] += char( 'A' - 'a' );
  }

  this->playerName = config.getSet( "modules.profile.playerName", playerName );
}

int ProfileModule::ozProfileGetPlayerName( lua_State* l )
{
  ARG( 0 );

  pushstring( profileModule.playerName );
  return 1;
}

}
}
