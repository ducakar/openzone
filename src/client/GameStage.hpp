/*
 *  GameStage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#pragma once

#include "client/Stage.hpp"
#include "client/Proxy.hpp"

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
