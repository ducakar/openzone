/*
 *  script.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2009, Davorin Učakar <davorin.ucakar@gmail.com>
 */

#include "precompiled.h"

#include "matrix/io.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using namespace oz;

static const char *scriptName = "/home/davorin/Projects/script.lua";
static lua_State *luaState;

static int ozPrint( lua_State* )
{
  int n = lua_gettop( luaState );
  if( n != 1 ) {
    lua_pushstring( luaState, "Lua: incorrect params" );
    lua_error( luaState );
  }

  return 0;
}

int main()
{
  luaState = lua_open();
  luaL_openlibs( luaState );

  lua_register( luaState, "ozPrint", ozPrint );

//   lua_getfield( luaState, LUA_GLOBALSINDEX, "ozPrint" );
//   lua_call( luaState, 0, 0 );

  luaL_dofile( luaState, scriptName );

  lua_close( luaState );
  return 0;
}
