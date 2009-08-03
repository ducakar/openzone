/*
 *  Network.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#pragma once

#include <SDL_net.h>

namespace oz
{
namespace client
{

  class Network
  {
    private:

      TCPsocket socket;

    public:

      bool connect();
      void disconnect();

      void update();

  };

  extern Network network;

}
}
