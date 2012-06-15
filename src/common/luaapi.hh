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
 * @file common/luaapi.hh
 *
 * Common Lua API implementation.
 *
 * @defgroup luaapi Lua API
 */

#pragma once

#include "common/lua.hh"

namespace oz
{

/**
 * %Lua state structure for common layer %Lua API.
 *
 * @ingroup common
 */
struct LuaState
{
  const char* envName; ///< Name of %Lua component. Used when printing messages or errors.
};

/**
 * %Lua state for common layer %Lua API.
 *
 * @ingroup common
 */
static LuaState ls;

/// @addtogroup luaapi
/// @{

/**
 * Throw <tt>oz::Exception</tt>.
 *
 * @code void ozException( string description ) @endcode
 *
 * An instance of <tt>oz::Exception</tt> is thrown with the given description.
 */
static int ozException( lua_State* l )
{
  ARG( 1 );

  const char* message = l_tostring( 1 );
  throw Exception( "%s", message );
}

/**
 * Print string.
 *
 * @code void ozPrintln( string text ) @endcode
 *
 * Message is printed in form <tt>Lua:matrix> text</tt> (if called from matrix Lua).
 */
static int ozPrintln( lua_State* l )
{
  ARG( 1 );

  Log::println( "Lua:%s> %s", ls.envName, l_tostring( 1 ) );
  return 0;
}

/// @}

}
