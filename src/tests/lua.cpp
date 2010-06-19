/*
 *  lua.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "stable.hpp"

#include "matrix/io.hpp"

#include <lua.hpp>
#include <SDL_main.h>

using namespace oz;

static lua_State* l;
static float life = 100.0f;

static int print( lua_State* l )
{
  int n = lua_gettop( l );
  if( n != 1 ) {
    lua_pushstring( l, "ozPrint: incorrect parameters" );
    lua_error( l );
    return 0;
  }
  else if( !lua_isstring( l, 1 ) ) {
    lua_pushstring( l, "ozPrint: incorrect parameters" );
    lua_error( l );
    return 0;
  }
  printf( "%s", lua_tostring( l, 1 ) );
  return 0;
}

static int addLife( lua_State* l )
{
  int n = lua_gettop( l );
  if( n != 1 ) {
    lua_pushstring( l, "ozPrint: incorrect parameters" );
    lua_error( l );
    return 0;
  }
  else if( !lua_isnumber( l, 1 ) ) {
    lua_pushstring( l, "ozPrint: incorrect parameters" );
    lua_error( l );
    return 0;
  }
  life += float( lua_tonumber( l, 1 ) );
  return 0;
}

int main( int, char** )
{
  l = lua_open();
  luaL_openlibs( l );

  lua_register( l, "print", print );
  lua_register( l, "addLife", addLife );

  lua_pushnumber( l, 42 );
  lua_setglobal( l, "magic_number" );
  luaL_dofile( l, "script.lua" );
  lua_getglobal( l, "onUse" );
  lua_call( l, 0, 0 );
  if( lua_gettop( l ) != 0 && lua_isstring( l, 1 ) ) {
    printf( "Lua: %s\n", lua_tostring( l, 1 ) );
  }
  printf( "%f\n", life );

  lua_close( l );
  return 0;
}
