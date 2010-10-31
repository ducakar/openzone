/*
 *  Main.hpp
 *
 *  Game initialisation and main loop
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

#define OZ_APP_NAME            "OpenZone"
#define OZ_WM_TITLE            OZ_APP_NAME
#define OZ_RC_DIR              ".openzone"
//#define OZ_LOG_FILE            "client.log"
#define OZ_CONFIG_FILE         "client.rc"

#include "client/Stage.hpp"

namespace oz
{
namespace client
{

  class Main
  {
    private:

      static const int INIT_CONFIG      = 0x001;
      static const int INIT_SDL         = 0x002;
      static const int INIT_SDL_VIDEO   = 0x004;
      static const int INIT_RENDER_INIT = 0x008;
      static const int INIT_AUDIO       = 0x010;
      static const int INIT_CONTEXT     = 0x020;
      static const int INIT_GAME_INIT   = 0x040;
      static const int INIT_RENDER_LOAD = 0x080;

      Stage* stage;
      int    initFlags;

    public:

      void main( int* argc, char* argv[] );
      void shutdown();

  };

  extern Main main;

}
}
