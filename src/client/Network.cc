/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file client/Network.cc
 */

#include "stable.hh"

#include "client/Network.hh"

#ifdef __native_client__
#else
# include <SDL_net.h>
#endif

namespace oz
{
namespace client
{

Network network;

#ifdef __native_client__

bool Network::connect()
{
  return true;
}

void Network::disconnect()
{}

void Network::update()
{}

#else

static TCPsocket socket;

bool Network::connect()
{
  const char* host = config.getSet( "net.server", "localhost" );
  ushort port = ushort( config.getSet( "net.port", 6666 ) );

  Log::print( "Connecting to %s:%d ...", host, port );

  IPaddress ip;
  SDLNet_ResolveHost( &ip, host, port );
  socket = SDLNet_TCP_Open( &ip );

  if( socket == null ) {
    Log::printEnd( " Failed" );
    return false;
  }

  Log::printEnd( " OK" );
  return true;
}

void Network::disconnect()
{
  SDLNet_TCP_Close( socket );
  socket = null;
}

void Network::update()
{}

#endif

}
}
