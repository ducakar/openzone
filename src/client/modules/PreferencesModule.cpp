/*
 *  PreferencesModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/PreferencesModule.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

  PreferencesModule preferencesModule;

  void PreferencesModule::init()
  {
    const char* userName = getenv( "USER" );
    userName = userName == null ? "Player" : userName;

    playerName = config.getSet( "modules.preferences.playerName", userName );

    OZ_LUA_FUNC( ozPreferencesGetPlayerName );
  }

  int PreferencesModule::ozPreferencesGetPlayerName( lua_State* l )
  {
    ARG( 0 );

    pushstring( preferencesModule.playerName );
    return 1;
  }

}
}
