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
#include "matrix/Sphere.hpp"

#define OZ_LUA_API( func ) \
  static int func( lua_State* l )

#define OZ_LUA_FUNC( func ) \
  lua.registerFunction( #func, func )

#define OZ_LUA_CONST( name, value ) \
  lua.registerConstant( name, value )

namespace oz
{

  extern const float EPSILON;

  inline const char* gettext( const char* text )
  {
    hard_assert( text != null );

    return text[0] == '\0' ? "" : ::gettext( text );
  }

}
