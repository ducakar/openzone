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
 * @file client/Client.hh
 *
 * Game initialisation and main loop.
 */

#pragma once

#include "client/Stage.hh"

namespace oz
{
namespace client
{

class Client
{
  private:

    static const int INIT_SDL        = 0x0001;
    static const int INIT_PHYSFS     = 0x0002;
    static const int INIT_CONFIG     = 0x0010;
    static const int INIT_LINGUA     = 0x0020;
    static const int INIT_LIBRARY    = 0x0040;
    static const int INIT_CONTEXT    = 0x0100;
    static const int INIT_RENDER     = 0x0200;
    static const int INIT_AUDIO      = 0x0400;
    static const int INIT_STAGE_INIT = 0x1000;
    static const int INIT_STAGE_LOAD = 0x2000;
    static const int INIT_MAIN_LOOP  = 0x4000;

    Stage* stage;
    int    initFlags;

  public:

    void shutdown();
    void printUsage( const char* invocationName );

    int main( int argc, char** argv );

};

extern Client client;

}
}
