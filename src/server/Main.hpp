/*
 *  Main.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#define OZ_APP_NAME            "OpenZone server"
#define OZ_RC_DIR              ".openzone"
// #define OZ_LOG_FILE            "server.log"
#define OZ_CONFIG_FILE         "server.xml"

namespace oz
{
namespace server
{

  class Main
  {
    private:

      static const int INIT_SDL = 0x01;
      static const int INIT_GAME_INIT = 0x02;
      static const int INIT_GAME_START = 0x04;

      int initFlags;

    public:

      void main();
      void shutdown();

  };

  extern Main main;

}
}

#include <SDL/SDL_main.h>

int main( int argc, char* argv[] );
