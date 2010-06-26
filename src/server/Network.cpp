/*
 *  Network.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING file for details.
 */

#include "stable.hpp"

#include "Network.hpp"

namespace oz
{
namespace server
{

  Network network;

  bool Network::connect()
  {
    int port = config.getSet( "net.port", 6666 );

    log.print( "Opening socket on port %d ...", port );

    IPaddress ip;
    SDLNet_ResolveHost( &ip, null, port );
    serverSocket = SDLNet_TCP_Open( &ip );

    if( serverSocket == null ) {
      log.printEnd( " Failed" );
      return false;
    }

    log.printEnd( " OK" );
    return true;
  }

  void Network::disconnect()
  {
    SDLNet_TCP_Close( serverSocket );
    log.println( "Socket closed" );
  }

  void Network::update()
  {
    TCPsocket clientSocket = SDLNet_TCP_Accept( serverSocket );
    if( clientSocket != null ) {
      Client* client = new Client();

      client->socket = clientSocket;
      clients << client;

      log.println( "Client connected" );
    }

    foreach( client, clients.iterator() ) {
    }
  }

}
}
