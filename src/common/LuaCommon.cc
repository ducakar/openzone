/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 * @file common/LuaCommon.cc
 */

#include <common/LuaCommon.hh>

#include <common/luaapi.hh>

namespace oz
{

int  LuaCommon::randomSeed       = 0;
bool LuaCommon::isRandomSeedTime = true;

bool LuaCommon::readValue( InputStream* istream )
{
  char ch = istream->readChar();

  switch( ch ) {
    case 'N': {
      l_pushnil();
      return true;
    }
    case 'F': {
      l_pushbool( false );
      return true;
    }
    case 'T': {
      l_pushbool( true );
      return true;
    }
    case 'n': {
      l_pushdouble( istream->readDouble() );
      return true;
    }
    case 's': {
      l_pushstring( istream->readString() );
      return true;
    }
    case '[': {
      l_newtable();

      while( readValue( istream ) ) { // key
        readValue( istream ); // value

        l_rawset( -3 );
      }
      return true;
    }
    case ']': {
      return false;
    }
    default: {
      OZ_ERROR( "Invalid type char '%c' in serialised Lua data", ch );
    }
  }
}

void LuaCommon::readValue( const JSON& json )
{
  switch( json.type() ) {
    case JSON::NIL: {
      l_pushnil();
      break;
    }
    case JSON::BOOLEAN: {
      l_pushbool( json.asBool() );
      break;
    }
    case JSON::NUMBER: {
      l_pushdouble( json.asDouble() );
      break;
    }
    case JSON::STRING: {
      l_pushstring( json.asString() );
      break;
    }
    case JSON::ARRAY: {
      l_newtable();

      int index = 0;
      foreach( i, json.arrayCIter() ) {
        readValue( *i );

        l_rawseti( -2, index );
        ++index;
      }
      break;
    }
    case JSON::OBJECT: {
      l_newtable();

      foreach( i, json.objectCIter() ) {
        l_pushstring( i->key );
        readValue( i->value );

        l_rawset( -3 );
      }
      break;
    }
  }
}

void LuaCommon::writeValue( OutputStream* ostream )
{
  int type = l_type( -1 );

  switch( type ) {
    case LUA_TNIL: {
      ostream->writeChar( 'N' );
      break;
    }
    case LUA_TBOOLEAN: {
      ostream->writeChar( l_tobool( -1 ) ? 'T' : 'F' );
      break;
    }
    case LUA_TNUMBER: {
      ostream->writeChar( 'n' );
      ostream->writeDouble( l_todouble( -1 ) );
      break;
    }
    case LUA_TSTRING: {
      ostream->writeChar( 's' );
      ostream->writeString( l_tostring( -1 ) );
      break;
    }
    case LUA_TTABLE: {
      ostream->writeChar( '[' );

      l_pushnil();
      while( l_next( -2 ) != 0 ) {
        // key
        l_pushvalue( -2 );
        writeValue( ostream );
        l_pop( 1 );

        // value
        writeValue( ostream );

        l_pop( 1 );
      }

      ostream->writeChar( ']' );
      break;
    }
    default: {
      OZ_ERROR( "Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
                " LUA_TSTRING and LUA_TTABLE data types" );
    }
  }
}

JSON LuaCommon::writeValue()
{
  int type = l_type( -1 );

  switch( type ) {
    case LUA_TNIL: {
      return JSON( JSON::NIL );
    }
    case LUA_TBOOLEAN: {
      return JSON( l_tobool( -1 ) );
    }
    case LUA_TNUMBER: {
      return JSON( l_todouble( -1 ) );
    }
    case LUA_TSTRING: {
      return JSON( l_tostring( -1 ) );
    }
    case LUA_TTABLE: {
      JSON json( JSON::OBJECT );

      l_pushnil();
      while( l_next( -2 ) != 0 ) {
        // key
        l_pushvalue( -2 );
        String key = l_tostring( -1 );
        l_pop( 1 );

        // value
        json.add( key, writeValue() );
        l_pop( 1 );
      }

      return json;
    }
    default: {
      OZ_ERROR( "Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
                " LUA_TSTRING and LUA_TTABLE data types" );
    }
  }
}

void LuaCommon::loadDir( const File& dir )
{
  DArray<File> luaFiles = dir.ls();

  foreach( file, luaFiles.citer() ) {
    if( file->type() != File::REGULAR || !file->hasExtension( "lua" ) ) {
      continue;
    }

    InputStream is = file->inputStream();

    if( !is.isAvailable() ) {
      continue;
    }

    if( l_dobuffer( is.begin(), is.available(), file->path() ) != 0 ) {
      const char* errorMessage = l_tostring( -1 );

      OZ_ERROR( "Lua error: %s", errorMessage );
    }
  }
}

void LuaCommon::initCommon( const char* componentName )
{
  ls.envName = componentName;

  l = luaL_newstate();
  if( l == nullptr ) {
    OZ_ERROR( "Failed to create Lua state" );
  }

#if LUA_VERSION_NUM < 502
  lua_pushcfunction( l, luaopen_base );
  lua_pushcfunction( l, luaopen_table );
  lua_pushcfunction( l, luaopen_string );
  lua_pushcfunction( l, luaopen_math );
  lua_pcall( l, 0, 0, 0 );
  lua_pcall( l, 0, 0, 0 );
  lua_pcall( l, 0, 0, 0 );
  lua_pcall( l, 0, 0, 0 );
#else
  luaL_requiref( l, "",              luaopen_base,   true );
  luaL_requiref( l, LUA_TABLIBNAME,  luaopen_table,  true );
  luaL_requiref( l, LUA_STRLIBNAME,  luaopen_string, true );
  luaL_requiref( l, LUA_MATHLIBNAME, luaopen_math,   true );
  lua_settop( l, 0 );
#endif

  if( l_gettop() != 0 ) {
    OZ_ERROR( "Failed to initialise Lua libraries" );
  }

  int seed = isRandomSeedTime ? int( Time::time() ) : randomSeed;
  l_dostring( String::str( "math.random( %u )", seed ) );

  IGNORE_FUNC( ozError );
  IGNORE_FUNC( ozPrintln );

  loadDir( "@lua/common" );
}

void LuaCommon::freeCommon()
{
  lua_close( l );
  l = nullptr;
}

void LuaCommon::registerFunction( const char* name, APIFunc func )
{
  l_register( name, func );
}

void LuaCommon::registerConstant( const char* name, bool value )
{
  l_pushbool( value );
  l_setglobal( name );
}

void LuaCommon::registerConstant( const char* name, int value )
{
  l_pushint( value );
  l_setglobal( name );
}

void LuaCommon::registerConstant( const char* name, float value )
{
  l_pushfloat( value );
  l_setglobal( name );
}

void LuaCommon::registerConstant( const char* name, const char* value )
{
  l_pushstring( value );
  l_setglobal( name );
}

}
