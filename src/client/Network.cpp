/*
 *  Network.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "Network.h"

namespace oz
{
namespace client
{


  void Network::connect()
  {
    SDLNet_Init();
  }

}
}
