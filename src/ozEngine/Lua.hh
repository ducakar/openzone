/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Lua.hh
 *
 * Wrapper for %Lua interpreter.
 */

#pragma once

#include "common.hh"

// Forward declaration to prevent pollution from Lua headers.
struct lua_State;

namespace oz
{

class Lua
{
public:

  /**
   * Lua C API.
   */
  typedef int APIFunc(lua_State*);

public:

  lua_State* l; ///< Lua state descriptor.

public:

  /**
   * Read serialised Lua value and push it on the stack (recursively for tables).
   */
  static void readValue(lua_State* l, InputStream* is);

  /**
   * Read Lua value from a %JSON value and push it on the stack (recursively for tables).
   */
  static void readValue(lua_State* l, const JSON& json);

  /**
   * Serialise Lua value at the top of the stack (recursively for tables).
   */
  static void writeValue(lua_State* l, OutputStream* os);

  /**
   * Return Lua value at the top of the stack (recursively for tables) as a %JSON value.
   */
  static JSON writeValue(lua_State* l);

  /**
   * Load all `*.lua` files in a directory.
   */
  void loadDir(const File& dir) const;

  bool exec(const char* code) const;

  /**
   * Common initialisation for Lua classes.
   */
  void init();

  /**
   * Common clean-up for Lua classes.
   */
  void destroy();

public:

  /**
   * Register Lua API function to the Lua state.
   */
  void import(const char* name, APIFunc func);

  /**
   * Import global variable into the Lua state.
   */
  void set(const char* name, bool value);

  /**
   * Import global variable into the Lua state.
   */
  void set(const char* name, int value);

  /**
   * Import global variable into the Lua state.
   */
  void set(const char* name, double value);

  /**
   * Import global variable into the Lua state.
   */
  void set(const char* name, const char* value);

};

}
