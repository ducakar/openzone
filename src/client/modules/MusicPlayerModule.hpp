/*
 *  MusicPlayerModule.hpp
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

#include "client/modules/MusicPlayer.hpp"

namespace oz
{
namespace client
{

  class MusicPlayerModule : public Module
  {
    private:

      ui::MusicPlayer* musicPlayer;

    public:

      MusicPlayerModule();

      virtual void load();
      virtual void unload();

  };

  extern MusicPlayerModule musicPlayerModule;

}
}
