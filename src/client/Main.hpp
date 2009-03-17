/*
 *  Main.hpp
 *
 *  Game initialization and main loop
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#define OZ_APP_NAME            "OpenZone"
#define OZ_WM_TITLE            OZ_APP_NAME
#define OZ_RC_DIR              ".openzone"
#define OZ_LOG_FILE            "client.log"
#define OZ_CONFIG_FILE         "client.xml"

namespace oz
{
namespace client
{

  class Main
  {
    private:

      static const int INIT_SDL = 0x01;
      static const int INIT_SDL_VIDEO = 0x02;
      static const int INIT_RENDER_INIT = 0x04;
      static const int INIT_SOUND = 0x08;
      static const int INIT_CLIENT_INIT = 0x10;
      static const int INIT_RENDER_LOAD = 0x20;
      static const int INIT_CLIENT_START = 0x40;

      int initFlags;

      void defaultConfig();
      void shutdown();

    public:

      void main();

  };

  extern Main main;

}
}

int main( int argc, char *argv[] );
