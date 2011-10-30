/*
 *  Network.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include <SDL/SDL_net.h>

namespace oz
{
namespace server
{

class Network
{
  private:

    class Client
    {
      String    name;
      TCPsocket socket;
      Client*    next[1];
    };

    TCPsocket serverSocket;
    List<Client> clients;

  public:

    bool connect();
    void disconnect();

    void update();

};

extern Network network;

}
}
