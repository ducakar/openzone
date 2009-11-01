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

  Network network;

  bool Network::connect()
  {
//    const char *host = config.get( "net.server", "localhost" );
//    int port = config.get( "net.port", 6666 );
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

}
}
