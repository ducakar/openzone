/*
 *  common.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file common/common.hpp
 */

#include "oz/oz.hpp"

#include "configuration.hpp"

#include "common/Span.hpp"
#include "common/Bounds.hpp"
#include "common/AABB.hpp"
#include "common/Timer.hpp"

#include "libintl.h"

#define OZ_LUA_API( func ) static LuaAPI func

struct lua_State;

namespace oz
{

extern const float EPSILON;

typedef int ( LuaAPI )( lua_State* );

inline const char* gettext( const char* text )
{
  hard_assert( text != null );

  return text[0] == '\0' ? "" : ::gettext( text );
}

}
