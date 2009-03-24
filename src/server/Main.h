/*
 *  Main.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#define OZ_APP_NAME            "Darkness server"
#define OZ_RC_DIR              ".dark/"
#define OZ_LOG_FILE            "server.log"
#define OZ_CONFIG_FILE         "server.conf"

namespace oz
{
namespace server
{

  class Main
  {
    protected:

      static const int INIT_SDL = 0x01;

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
