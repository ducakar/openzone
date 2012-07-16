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
 * @file matrix/Lua.cc
 *
 * Lua scripting engine for Matrix
 */

#include "stable.hh"

#include "matrix/Lua.hh"

#include "matrix/luaapi.hh"

namespace oz
{
namespace matrix
{

Lua lua;

bool Lua::readVariable( InputStream* istream )
{
  char ch = istream->readChar();

  switch( ch ) {
    case 'N': {
      l_pushnil();
      return true;
    }
    case 'b': {
      l_pushbool( istream->readBool() );
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

      while( readVariable( istream ) ) { // key
        readVariable( istream ); // value

        l_rawset( -3 );
      }
      return true;
    }
    case ']': {
      return false;
    }
    default: {
      throw Exception( "Invalid type char '%c' in serialised Lua data", ch );
    }
  }
}

void Lua::writeVariable( BufferStream* ostream )
{
  int type = l_type( -1 );

  switch( type ) {
    case LUA_TNIL: {
      ostream->writeChar( 'N' );
      break;
    }
    case LUA_TBOOLEAN: {
      ostream->writeChar( 'b' );
      ostream->writeBool( l_tobool( -1 ) != 0 );
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
        writeVariable( ostream );
        l_pop( 1 );

        // value
        writeVariable( ostream );

        l_pop( 1 );
      }

      ostream->writeChar( ']' );
      break;
    }
    default: {
      throw Exception( "Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
                       " LUA_TSTRING and LUA_TTABLE data types" );
    }
  }
}

Lua::Lua() :
  l( null )
{}

bool Lua::objectCall( const char* functionName, Object* self_, Bot* user_ )
{
  ms.self         = self_;
  ms.user         = user_;
  ms.obj          = self_;
  ms.str          = null;
  ms.frag         = null;
  ms.objIndex     = 0;
  ms.strIndex     = 0;
  ms.hasUseFailed = false;

  hard_assert( l_gettop() == 1 && ms.self != null );

  l_getglobal( functionName );
  l_rawgeti( 1, ms.self->index );
  l_pcall( 1, 0 );

  if( l_gettop() != 1 ) {
    Log::println( "Lua[M] in %s(self = %d, user = %d): %s", functionName, ms.self->index,
                  ms.user == null ? -1 : ms.user->index, l_tostring( -1 ) );
    System::bell();

    l_pop( 1 );
  }

  return !ms.hasUseFailed;
}

void Lua::registerObject( int index )
{
  // we cannot depend that ozLocalData exists at index 1 as this function can be called via a
  // script creating an object
  l_getglobal( "ozLocalData" );
  l_newtable();
  l_rawseti( -2, index );
  l_pop( 1 );
}

void Lua::unregisterObject( int index )
{
  // we cannot depend that ozLocalData exists at index 1 as this function can be called via a
  // script creating an object
  l_getglobal( "ozLocalData" );
  l_pushnil();
  l_rawseti( -2, index );
  l_pop( 1 );
}

void Lua::read( InputStream* istream )
{
  hard_assert( l_gettop() == 1 );

  char ch = istream->readChar();

  while( ch != '\0' ) {
    hard_assert( ch == 'i' );

    int index = istream->readInt();
    readVariable( istream );

    l_rawseti( 1, index );

    ch = istream->readChar();
  }
}

void Lua::write( BufferStream* ostream )
{
  hard_assert( l_gettop() == 1 );

  l_pushnil();
  while( l_next( 1 ) != 0 ) {
    hard_assert( l_type( -2 ) == LUA_TNUMBER );
    hard_assert( l_type( -1 ) == LUA_TTABLE );

    ostream->writeChar( 'i' );
    ostream->writeInt( l_toint( -2 ) );
    writeVariable( ostream );

    l_pop( 1 );
  }

  ostream->writeChar( '\0' );
}

void Lua::registerFunction( const char* name, LuaAPI func )
{
  l_register( name, func );
}

void Lua::registerConstant( const char* name, bool value )
{
  l_pushbool( value );
  l_setglobal( name );
}

void Lua::registerConstant( const char* name, int value )
{
  l_pushint( value );
  l_setglobal( name );
}

void Lua::registerConstant( const char* name, float value )
{
  l_pushfloat( value );
  l_setglobal( name );
}

void Lua::registerConstant( const char* name, const char* value )
{
  l_pushstring( value );
  l_setglobal( name );
}

void Lua::init()
{
  Log::print( "Initialising Matrix Lua ..." );

  l = luaL_newstate();
  if( l == null ) {
    throw Exception( "Failed to create Lua state" );
  }

  hard_assert( l_gettop() == 0 );

  IMPORT_LIBS();

  if( l_gettop() != 0 ) {
    throw Exception( "Failed to initialise Lua libraries" );
  }

  ls.envName = "matrix";
  ms.structs.alloc( 32 );
  ms.objects.alloc( 512 );

  /*
   * General functions
   */

  IMPORT_FUNC( ozException );
  IMPORT_FUNC( ozPrintln );

  IMPORT_FUNC( ozUseFailed );

  /*
   * Orbis
   */

  IMPORT_FUNC( ozOrbisGetGravity );
  IMPORT_FUNC( ozOrbisSetGravity );

  IMPORT_FUNC( ozOrbisAddStr );
  IMPORT_FUNC( ozOrbisAddObj );
  IMPORT_FUNC( ozOrbisAddFrag );
  IMPORT_FUNC( ozOrbisGenFrags );

  IMPORT_FUNC( ozOrbisOverlaps );
  IMPORT_FUNC( ozOrbisBindOverlaps );

  /*
   * Caelum
   */

  IMPORT_FUNC( ozCaelumLoad );

  IMPORT_FUNC( ozCaelumGetHeading );
  IMPORT_FUNC( ozCaelumSetHeading );
  IMPORT_FUNC( ozCaelumGetPeriod );
  IMPORT_FUNC( ozCaelumSetPeriod );
  IMPORT_FUNC( ozCaelumGetTime );
  IMPORT_FUNC( ozCaelumSetTime );
  IMPORT_FUNC( ozCaelumAddTime );
  IMPORT_FUNC( ozCaelumSetRealTime );

  /*
   * Terra
   */

  IMPORT_FUNC( ozTerraLoad );

  IMPORT_FUNC( ozTerraHeight );

  /*
   * Structure
   */

  IMPORT_FUNC( ozBindStr );
  IMPORT_FUNC( ozBindNextStr );

  IMPORT_FUNC( ozStrIsNull );

  IMPORT_FUNC( ozStrGetIndex );
  IMPORT_FUNC( ozStrGetBounds );
  IMPORT_FUNC( ozStrGetPos );
  IMPORT_FUNC( ozStrGetBSP );
  IMPORT_FUNC( ozStrGetHeading );

  IMPORT_FUNC( ozStrMaxLife );
  IMPORT_FUNC( ozStrGetLife );
  IMPORT_FUNC( ozStrSetLife );
  IMPORT_FUNC( ozStrAddLife );
  IMPORT_FUNC( ozStrDefaultResistance );
  IMPORT_FUNC( ozStrGetResistance );
  IMPORT_FUNC( ozStrSetResistance );

  IMPORT_FUNC( ozStrDamage );
  IMPORT_FUNC( ozStrDestroy );
  IMPORT_FUNC( ozStrRemove );

  IMPORT_FUNC( ozStrNumBoundObjs );
  IMPORT_FUNC( ozStrBindBoundObj );

  IMPORT_FUNC( ozStrNumEnts );
  IMPORT_FUNC( ozStrBindEnt );

  IMPORT_FUNC( ozStrOverlaps );
  IMPORT_FUNC( ozStrBindOverlaps );

  IMPORT_FUNC( ozStrVectorFromSelf );
  IMPORT_FUNC( ozStrVectorFromSelfEye );
  IMPORT_FUNC( ozStrDirFromSelf );
  IMPORT_FUNC( ozStrDirFromSelfEye );
  IMPORT_FUNC( ozStrDistFromSelf );
  IMPORT_FUNC( ozStrDistFromSelfEye );
  IMPORT_FUNC( ozStrHeadingFromSelfEye );
  IMPORT_FUNC( ozStrRelHeadingFromSelfEye );
  IMPORT_FUNC( ozStrPitchFromSelfEye );
  IMPORT_FUNC( ozStrIsVisibleFromSelf );
  IMPORT_FUNC( ozStrIsVisibleFromSelfEye );

  /*
   * Entity
   */

  IMPORT_FUNC( ozEntGetState );
  IMPORT_FUNC( ozEntSetState );
  IMPORT_FUNC( ozEntGetLock );
  IMPORT_FUNC( ozEntSetLock );
  IMPORT_FUNC( ozEntTrigger );

  IMPORT_FUNC( ozEntOverlaps );
  IMPORT_FUNC( ozEntBindOverlaps );

  IMPORT_FUNC( ozEntVectorFromSelf );
  IMPORT_FUNC( ozEntVectorFromSelfEye );
  IMPORT_FUNC( ozEntDirFromSelf );
  IMPORT_FUNC( ozEntDirFromSelfEye );
  IMPORT_FUNC( ozEntDistFromSelf );
  IMPORT_FUNC( ozEntDistFromSelfEye );
  IMPORT_FUNC( ozEntHeadingFromSelfEye );
  IMPORT_FUNC( ozEntRelHeadingFromSelfEye );
  IMPORT_FUNC( ozEntPitchFromSelfEye );
  IMPORT_FUNC( ozEntIsVisibleFromSelf );
  IMPORT_FUNC( ozEntIsVisibleFromSelfEye );

  /*
   * Object
   */

  IMPORT_FUNC( ozBindObj );
  IMPORT_FUNC( ozBindSelf );
  IMPORT_FUNC( ozBindUser );
  IMPORT_FUNC( ozBindNextObj );

  IMPORT_FUNC( ozObjIsNull );
  IMPORT_FUNC( ozObjIsSelf );
  IMPORT_FUNC( ozObjIsUser );
  IMPORT_FUNC( ozObjIsCut );

  IMPORT_FUNC( ozObjGetIndex );
  IMPORT_FUNC( ozObjGetPos );
  IMPORT_FUNC( ozObjWarpPos );
  IMPORT_FUNC( ozObjGetDim );
  IMPORT_FUNC( ozObjHasFlag );
  IMPORT_FUNC( ozObjGetHeading );
  IMPORT_FUNC( ozObjGetClassName );

  IMPORT_FUNC( ozObjMaxLife );
  IMPORT_FUNC( ozObjGetLife );
  IMPORT_FUNC( ozObjSetLife );
  IMPORT_FUNC( ozObjAddLife );
  IMPORT_FUNC( ozObjDefaultResistance );
  IMPORT_FUNC( ozObjGetResistance );
  IMPORT_FUNC( ozObjSetResistance );

  IMPORT_FUNC( ozObjAddEvent );

  IMPORT_FUNC( ozObjBindItems );
  IMPORT_FUNC( ozObjBindItem );
  IMPORT_FUNC( ozObjAddItem );
  IMPORT_FUNC( ozObjRemoveItem );
  IMPORT_FUNC( ozObjRemoveAllItems );

  IMPORT_FUNC( ozObjEnableUpdate );
  IMPORT_FUNC( ozObjDamage );
  IMPORT_FUNC( ozObjDestroy );

  IMPORT_FUNC( ozObjOverlaps );
  IMPORT_FUNC( ozObjBindOverlaps );

  IMPORT_FUNC( ozObjVectorFromSelf );
  IMPORT_FUNC( ozObjVectorFromSelfEye );
  IMPORT_FUNC( ozObjDirFromSelf );
  IMPORT_FUNC( ozObjDirFromSelfEye );
  IMPORT_FUNC( ozObjDistFromSelf );
  IMPORT_FUNC( ozObjDistFromSelfEye );
  IMPORT_FUNC( ozObjHeadingFromSelfEye );
  IMPORT_FUNC( ozObjRelHeadingFromSelfEye );
  IMPORT_FUNC( ozObjPitchFromSelfEye );
  IMPORT_FUNC( ozObjIsVisibleFromSelf );
  IMPORT_FUNC( ozObjIsVisibleFromSelfEye );

  /*
   * Dynamic object
   */

  IMPORT_FUNC( ozDynGetParent );

  IMPORT_FUNC( ozDynGetVelocity );
  IMPORT_FUNC( ozDynGetMomentum );
  IMPORT_FUNC( ozDynSetMomentum );
  IMPORT_FUNC( ozDynAddMomentum );
  IMPORT_FUNC( ozDynGetMass );
  IMPORT_FUNC( ozDynGetLift );

  /*
   * Weapon
   */

  IMPORT_FUNC( ozWeaponMaxRounds );
  IMPORT_FUNC( ozWeaponGetRounds );
  IMPORT_FUNC( ozWeaponSetRounds );
  IMPORT_FUNC( ozWeaponAddRounds );

  /*
   * Bot
   */

  IMPORT_FUNC( ozBotGetName );
  IMPORT_FUNC( ozBotSetName );
  IMPORT_FUNC( ozBotGetMind );
  IMPORT_FUNC( ozBotSetMind );

  IMPORT_FUNC( ozBotHasState );
  IMPORT_FUNC( ozBotGetEyePos );
  IMPORT_FUNC( ozBotGetH );
  IMPORT_FUNC( ozBotSetH );
  IMPORT_FUNC( ozBotAddH );
  IMPORT_FUNC( ozBotGetV );
  IMPORT_FUNC( ozBotSetV );
  IMPORT_FUNC( ozBotAddV );
  IMPORT_FUNC( ozBotGetDir );

  IMPORT_FUNC( ozBotGetCargo );
  IMPORT_FUNC( ozBotGetWeapon );
  IMPORT_FUNC( ozBotSetWeaponItem );

  IMPORT_FUNC( ozBotMaxStamina );
  IMPORT_FUNC( ozBotGetStamina );
  IMPORT_FUNC( ozBotSetStamina );
  IMPORT_FUNC( ozBotAddStamina );

  IMPORT_FUNC( ozBotAction );

  IMPORT_FUNC( ozBotHeal );
  IMPORT_FUNC( ozBotRearm );
  IMPORT_FUNC( ozBotKill );

  IMPORT_FUNC( ozBotCanReachEntity );
  IMPORT_FUNC( ozBotCanReachObj );

  /*
   * Vehicle
   */

  IMPORT_FUNC( ozVehicleGetPilot );

  IMPORT_FUNC( ozVehicleGetH );
  IMPORT_FUNC( ozVehicleSetH );
  IMPORT_FUNC( ozVehicleAddH );
  IMPORT_FUNC( ozVehicleGetV );
  IMPORT_FUNC( ozVehicleSetV );
  IMPORT_FUNC( ozVehicleAddV );
  IMPORT_FUNC( ozVehicleGetDir );

  IMPORT_FUNC( ozVehicleEmbarkBot );
  IMPORT_FUNC( ozVehicleDisembarkBot );

  IMPORT_FUNC( ozVehicleService );

  /*
   * Frag
   */

  IMPORT_FUNC( ozFragBindIndex );

  IMPORT_FUNC( ozFragIsNull );

  IMPORT_FUNC( ozFragGetPos );
  IMPORT_FUNC( ozFragWarpPos );
  IMPORT_FUNC( ozFragGetIndex );
  IMPORT_FUNC( ozFragGetVelocity );
  IMPORT_FUNC( ozFragSetVelocity );
  IMPORT_FUNC( ozFragAddVelocity );
  IMPORT_FUNC( ozFragGetLife );
  IMPORT_FUNC( ozFragSetLife );
  IMPORT_FUNC( ozFragAddLife );

  IMPORT_FUNC( ozFragRemove );

  IMPORT_FUNC( ozFragOverlaps );
  IMPORT_FUNC( ozFragBindOverlaps );

  IMPORT_FUNC( ozFragVectorFromSelf );
  IMPORT_FUNC( ozFragVectorFromSelfEye );
  IMPORT_FUNC( ozFragDirFromSelf );
  IMPORT_FUNC( ozFragDirFromSelfEye );
  IMPORT_FUNC( ozFragDistFromSelf );
  IMPORT_FUNC( ozFragDistFromSelfEye );
  IMPORT_FUNC( ozFragHeadingFromSelfEye );
  IMPORT_FUNC( ozFragRelHeadingFromSelfEye );
  IMPORT_FUNC( ozFragPitchFromSelfEye );
  IMPORT_FUNC( ozFragIsVisibleFromSelf );
  IMPORT_FUNC( ozFragIsVisibleFromSelfEye );

  importMatrixConstants( l );

  l_newtable();
  l_setglobal( "ozLocalData" );
  l_getglobal( "ozLocalData" );

  PFile luaDir( "lua/matrix" );
  DArray<PFile> luaFiles = luaDir.ls();

  foreach( file, luaFiles.iter() ) {
    if( file->hasExtension( "lua" ) ) {
      if( !file->map() ) {
        throw Exception( "Failed to read Lua script '%s'", file->path().cstr() );
      }

      InputStream istream = file->inputStream();

      if( IMPORT_BUFFER( istream.begin(), istream.capacity(), file->path() ) != 0 ) {
        throw Exception( "Matrix Lua script error" );
      }

      file->unmap();
    }
  }

  hard_assert( l_gettop() == 1 );

  Log::printEnd( " OK" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  Log::print( "Freeing Matrix Lua ..." );

  ms.structs.clear();
  ms.structs.dealloc();

  ms.objects.clear();
  ms.objects.dealloc();

  lua_close( l );
  l = null;

  Log::printEnd( " OK" );
}

}
}
