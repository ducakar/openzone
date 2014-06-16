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
 * @file common/LuaCommon.hh
 */

#pragma once

#include <common/common.hh>

// Forward declaration to prevent pollution from Lua headers.
struct lua_State;

namespace oz
{

/**
 * Lua base class.
 *
 * It provides common functions for derived Lua classes in matrix, nirvana and client layers.
 */
class LuaCommon
{
public:

  /**
   * Lua C API.
   */
  typedef int APIFunc(lua_State*);

  static int  randomSeed;       ///< Random seed for Lua environments.
  static bool isRandomSeedTime; ///< True iff `Time::time()` should be as a seed.

protected:

  lua_State* l;                 ///< Lua state descriptor.

protected:

  /**
   * Read serialised Lua value and push it on the stack (recursively for tables).
   *
   * @return Should always return true. This return value is only used for internal recursive calls
   *         to detect end of a table.
   */
  bool readValue(InputStream* is) const;

  /**
   * Read Lua value from a %JSON value and push it on the stack (recursively for tables).
   */
  void readValue(const JSON& json) const;

  /**
   * Serialise Lua value at the top of the stack (recursively for tables).
   */
  void writeValue(OutputStream* os) const;

  /**
   * Return Lua value at the top of the stack (recursively for tables) as a %JSON value.
   */
  JSON writeValue() const;

  /**
   * Load all `*.lua` files in a directory.
   */
  void loadDir(const File& dir) const;

  /**
   * Common initialisation for Lua classes.
   */
  void initCommon();

  /**
   * Common clean-up for Lua classes.
   */
  void freeCommon();

public:

  /**
   * Register Lua API function to the Lua state.
   */
  void registerFunction(const char* name, APIFunc func);

  /**
   * Import global variable into the Lua state.
   */
  void registerConstant(const char* name, bool value);

  /**
   * Import global variable into the Lua state.
   */
  void registerConstant(const char* name, int value);

  /**
   * Import global variable into the Lua state.
   */
  void registerConstant(const char* name, float value);

  /**
   * Import global variable into the Lua state.
   */
  void registerConstant(const char* name, const char* value);

  /**
   * Compile Lua source code to bytecode.
   */
  Buffer compile(const char* code, const char* name) const;

};

}
