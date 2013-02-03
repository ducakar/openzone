/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file nirvana/Lua.hh
 *
 * Lua scripting engine for Nirvana
 */

#pragma once

#include <common/Lua.hh>
#include <nirvana/common.hh>

namespace oz
{
namespace nirvana
{

class Lua : public common::Lua
{
  public:

    bool mindCall( const char* functionName, Bot* self );

    void registerMind( int botIndex );
    void unregisterMind( int botIndex );

    void read( InputStream* istream );
    void write( OutputStream* ostream );

    void init();
    void destroy();

};

extern Lua lua;

}
}
