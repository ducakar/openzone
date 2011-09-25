/*
 *  Network.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Network.hpp"

namespace oz
{
namespace client
{

  Network network;

#ifndef OZ_NETWORKING

  bool Network::connect()
  {
    return true;
  }

  void Network::disconnect()
  {}

  void Network::update()
  {}

#else

  bool Network::connect()
  {
//    const char* host = config.getSet( "net.server", "localhost" );
//    int port = config.getSet( "net.port", 6666 );
//
//    log.print( "Connecting to %s:%d ...", host, port );
//
//    IPaddress ip;
//    SDLNet_ResolveHost( &ip, host, port );
//    socket = SDLNet_TCP_Open( &ip );
//
//    if( socket == null ) {
//      log.printEnd( " Failed" );
//      return false;
//    }
//
//    log.printEnd( " OK" );
    return true;
  }

  void Network::disconnect()
  {
//    SDLNet_TCP_Close( socket );
//    socket = null;
  }

  void Network::update()
  {}

#endif

}
}
