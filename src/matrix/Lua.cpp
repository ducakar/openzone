/*
 *  Lua.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3.0. See COPYING for details.
 */

#include "precompiled.h"

#include "Lua.h"

namespace oz
{

  Lua lua;

  void Lua::init()
  {
    state = lua_open();
    luaL_openlibs( state );
  }

  void Lua::free()
  {
    lua_close( state );
  }

  void Lua::loadAPI( const API *apis )
  {
    for( int i = 0; apis[i].name != null; i++ ) {
      lua_register( state, apis[i].name, apis[i].func );
    }
  }

  bool Lua::load( const char *file )
  {
    return luaL_dofile( state, file ) == 0;
  }

}
