/*
 *  PreferencesModule.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Module.hpp"
#include "client/Lua.hpp"

namespace oz
{
namespace client
{

  class PreferencesModule : public Module
  {
    private:

      String playerName;

    public:

      virtual void init();

    private:

      OZ_LUA_API( ozPreferencesGetPlayerName );

  };

  extern PreferencesModule preferencesModule;

}
}
