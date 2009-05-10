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
namespace server
{

  Network network;

  bool Network::connect()
  {
    int port = config.get( "net.port", 6666 );

    logFile.print( "Opening socket on port %d ...", port );

    IPaddress ip;
    SDLNet_ResolveHost( &ip, null, port );
    serverSocket = SDLNet_TCP_Open( &ip );

    if( serverSocket == null ) {
      logFile.printEnd( " Failed" );
      return false;
    }

    logFile.printEnd( " OK" );
    return true;
  }

  void Network::disconnect()
  {
    SDLNet_TCP_Close( serverSocket );
    logFile.println( "Socket closed" );
  }

  void Network::update()
  {
    TCPsocket clientSocket = SDLNet_TCP_Accept( serverSocket );
    if( clientSocket != null ) {
      Client *client = new Client();

      client->socket = clientSocket;
      clients << client;

      logFile.println( "Client connected" );
    }

    foreach( client, clients.iterator() ) {
    }
  }

}
}
