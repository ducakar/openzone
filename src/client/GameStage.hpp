/*
 *  GameStage.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

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

      SDL_Thread*   auxThread;

      SDL_sem*      mainSemaphore;
      SDL_sem*      auxSemaphore;

      volatile bool isAlive;

      static int auxMain( void* );

      void run();

    public:

      State  state;
      Proxy* proxy;

      bool update();
      void present();

      virtual void load();
      virtual void unload();

      virtual void init();
      virtual void free();

  };

  extern GameStage gameStage;

}
}
