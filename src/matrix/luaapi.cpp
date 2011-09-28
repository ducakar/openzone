/*
 *  luamacros.h
 *
 *  Utility Lua macros for API functions. They don't include prefixes, so are not safe to be
 *  included in general.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/luaapi.hpp"

namespace oz
{

  void registerLuaFunction( lua_State* l, const char* name, LuaAPI func )
  {
    lua_register( l, name, func );
  }

  void registerLuaConstant( lua_State* l, const char* name, bool value )
  {
    pushbool( value );
    setglobal( name );
  }

  void registerLuaConstant( lua_State* l, const char* name, int value )
  {
    pushint( value );
    setglobal( name );
  }

  void registerLuaConstant( lua_State* l, const char* name, float value )
  {
    pushfloat( value );
    setglobal( name );
  }

  void registerLuaConstant( lua_State* l, const char* name, const char* value )
  {
    pushstring( value );
    setglobal( name );
  }

}
