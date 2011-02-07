/*
 *  Main.hpp
 *
 *  Game initialisation and main loop
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "client/Stage.hpp"

namespace oz
{
namespace client
{

  class Main
  {
    private:

      static const int INIT_CONFIG       = 0x0001;
      static const int INIT_SDL          = 0x0002;
      static const int INIT_SDL_VIDEO    = 0x0004;
      static const int INIT_TRANSLATOR   = 0x0010;
      static const int INIT_GAME_INIT    = 0x0020;
      static const int INIT_RENDER_INIT  = 0x0100;
      static const int INIT_CONTEXT_INIT = 0x0200;
      static const int INIT_AUDIO        = 0x0400;
      static const int INIT_CONTEXT_LOAD = 0x1000;
      static const int INIT_RENDER_LOAD  = 0x2000;
      static const int QUICK_SHUTDOWN    = 0x80000000;

      Stage* stage;
      int    initFlags;

      float  allTime;
      uint   inactiveMillis;
      uint   droppedMillis;
      float  loadingTime;

    public:

      void shutdown();
      void printUsage();

      void main( int* argc, char* argv[] );

  };

  extern Main main;

}
}
