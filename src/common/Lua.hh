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
 * @file common/Lua.hh
 */

#pragma once

#include <common/common.hh>

// Forward declaration needed for Lua API declarations, to prevent pollution from Lua headers.
struct lua_State;

namespace oz
{
namespace common
{

/**
 * %Lua base class.
 *
 * It provides common functions for derived %Lua classes in matrix, nirvana and client layers.
 */
class Lua
{
  public:

    /**
     * Lua C API.
     */
    typedef int APIFunc( lua_State* );

    static int randomSeed;        ///< Random seed for %Lua environments.
    static bool isRandomSeedTime; ///< True iff current time (`Time::time()`) should be seed.

  protected:

    lua_State* l;                 ///< %Lua state descriptor.

  protected:

    /**
     * Default constructor, clears `l` to `nullptr`.
     */
    explicit Lua();

    /**
     * Read serialised %Lua variable and push it on global stack (recursively for tables).
     */
    bool readVariable( InputStream* istream );

    /**
     * Serialise %Lua variable at the top of the stack (recursively for tables).
     */
    void writeVariable( BufferStream* stream );

    /**
     * Common initialisation for %Lua classes.
     */
    void initCommon( const char* componentName );

    /**
     * Common clean-up for %Lua classes.
     */
    void freeCommon();

  public:

    /**
     * Register %Lua API function to the %Lua state.
     */
    void registerFunction( const char* name, APIFunc func );

    /**
     * Import global variable into the %Lua state.
     */
    void registerConstant( const char* name, bool value );

    /**
     * Import global variable into the %Lua state.
     */
    void registerConstant( const char* name, int value );

    /**
     * Import global variable into the %Lua state.
     */
    void registerConstant( const char* name, float value );

    /**
     * Import global variable into the %Lua state.
     */
    void registerConstant( const char* name, const char* value );

};

}
}
