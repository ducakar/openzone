/*
 *  common.hpp
 *
 *  Common matrix includes and definitions
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Span.hpp"
#include "matrix/Bounds.hpp"
#include "matrix/AABB.hpp"

#define OZ_LUA_API( func ) \
  static LuaAPI func

struct lua_State;

namespace oz
{

  extern const float EPSILON;

  enum Heading
  {
    NORTH = 0,
    WEST  = 1,
    SOUTH = 2,
    EAST  = 3
  };

  typedef int ( LuaAPI )( lua_State* );

  inline const char* gettext( const char* text )
  {
    hard_assert( text != null );

    return text[0] == '\0' ? "" : ::gettext( text );
  }

}
