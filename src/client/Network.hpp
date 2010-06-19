/*
 *  Network.h
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
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
