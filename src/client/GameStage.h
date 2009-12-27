/*
 *  GameStage.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#pragma once

#include "Stage.h"
#include "Proxy.h"

namespace oz
{
namespace client
{

  class GameStage : public Stage
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

      State state;
      Proxy *proxy;

      bool update();
      void render();

      virtual void load();
      virtual void unload();

      virtual void begin();
      virtual void end();

  };

  extern GameStage gameStage;

}
}
