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
 * @file matrix/LuaMatrix.hh
 *
 * Lua scripting engine for Matrix
 */

#pragma once

#include <common/LuaCommon.hh>
#include <matrix/Object.hh>

namespace oz
{

class Bot;

class LuaMatrix : public LuaCommon
{
  public:

    float objectStatus;

  public:

    bool objectCall( const char* functionName, Object* self, Bot* user = nullptr );

    void registerObject( int index );
    void unregisterObject( int index );

    void read( InputStream* is );
    void write( OutputStream* os );

    void init();
    void destroy();

};

extern LuaMatrix luaMatrix;

}
