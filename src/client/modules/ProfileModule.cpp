/*
 *  ProfileModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/ProfileModule.hpp"

#include "luamacros.hpp"

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
    const char* userName = getenv( "USER" );
    userName = userName == null ? "Player" : userName;

    playerName = config.getSet( "modules.profile.playerName", userName );
  }

  int ProfileModule::ozProfileGetPlayerName( lua_State* l )
  {
    ARG( 0 );

    pushstring( profileModule.playerName );
    return 1;
  }

}
}
