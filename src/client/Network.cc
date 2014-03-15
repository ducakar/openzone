/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <client/Network.hh>

#ifdef OZ_NET
# include <SDL_net.h>
#endif

namespace oz
{
namespace client
{

#ifndef OZ_NET

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
  Log::print( "Connecting to %s:%d ...", host.cstr(), port );

  IPaddress ip;
  SDLNet_ResolveHost( &ip, host, port );
  socket = SDLNet_TCP_Open( &ip );

  if( socket == nullptr ) {
    Log::printEnd( " Failed" );
    return false;
  }

  Log::printEnd( " OK" );
  return true;
}

void Network::disconnect()
{
  SDLNet_TCP_Close( socket );
  socket = nullptr;
}

void Network::update()
{}

#endif

void Network::init()
{
  host = config.include( "net.server", "localhost" ).get( "" );
  port = ushort( config.include( "net.port", 6666 ).get( 0 ) );
}

void Network::destroy()
{}

Network network;

}
}
