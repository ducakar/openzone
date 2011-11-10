/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file client/Network.cpp
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
