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
namespace server
{

  class Network
  {
    private:

      struct Client
      {
        String    name;
        TCPsocket socket;
        Client    *next[1];
      };

      TCPsocket serverSocket;
      List<Client, 0> clients;

    public:

      bool connect();
      void disconnect();

      void update();

  };

  extern Network network;

}
}
