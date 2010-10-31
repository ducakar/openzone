/*
 *  Network.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "./stable.hpp"

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
