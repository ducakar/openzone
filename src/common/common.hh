/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file common/common.hh
 */

#pragma once

#include "oz/oz.hh"

#include "configuration.hh"

#include "common/Span.hh"
#include "common/Bounds.hh"
#include "common/AABB.hh"
#include "common/Timer.hh"

// We want to use C++ wrapped C headers, not vanilla ones that are included via SDL.
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define SDL_NO_COMPAT
#include <SDL/SDL.h>
#include <libintl.h>

#define OZ_LUA_API( func ) static LuaAPI func

struct lua_State;

namespace oz
{

// ensure epsilon is big enough for a 4 km x 4 km world (1 mm should do)
// EPSILON = Orbis::DIM * 4.0f * Math::EPSILON
constexpr float EPSILON = 2048.0f * 4.0f * Math::EPSILON;

typedef int ( LuaAPI )( lua_State* );

inline const char* gettext( const char* text )
{
  hard_assert( text != null );

  return text[0] == '\0' ? "" : ::gettext( text );
}

}
