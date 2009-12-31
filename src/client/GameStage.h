/*
 *  GameStage.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#pragma once

#include "Stage.h"
#include "Proxy.h"

namespace oz
{
namespace client
{

  struct GameStage : Stage
  {
    public:

      enum State
      {
        GAME,
        MENU
      };

    private:

      float moveStep;
      float runStep;

    public:

      State  state;
      Proxy* proxy;

      bool update();
      void render();

      virtual void init();
      virtual void free();

      virtual void load();
      virtual void unload();

  };

  extern GameStage gameStage;

}
}
