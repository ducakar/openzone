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
 * @file client/Lua.hh
 *
 * Lua scripting engine for client
 */

#pragma once

#include <common/Lua.hh>

#include <client/common.hh>

namespace oz
{
namespace client
{

class Lua : public common::Lua
{
  public:

    void staticCall( const char* functionName );

    void update();
    void create( const char* missionPath );

    void read( InputStream* istream );
    void write( BufferStream* ostream );

    void init();
    void free();

};

extern Lua lua;

}
}
