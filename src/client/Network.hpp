/*
 *  Network.hpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#ifdef OZ_NETWORKING
# include <SDL_net.h>
#endif

namespace oz
{
namespace client
{

  class Network
  {
    private:

#ifdef OZ_NETWORKING
      TCPsocket socket;
#endif

    public:

      bool connect();
      void disconnect();

      void update();

  };

  extern Network network;

}
}
