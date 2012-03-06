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
 * @file nirvana/Lua.cc
 *
 * Lua scripting engine for Nirvana
 */

#include "stable.hh"

#include "nirvana/Lua.hh"

#include "nirvana/luaapi.hh"

namespace oz
{
namespace nirvana
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
      throw Exception( "Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER, "
                       "LUA_TSTRING and LUA_TTABLE data types" );
    }
  }
}

Lua::Lua() :
  l( null )
{}

bool Lua::mindCall( const char* functionName, Bot* self_ )
{
  ms.self        = self_;
  ms.obj         = self_;
  ms.str         = null;
  ms.objIndex    = 0;
  ms.strIndex    = 0;
  ns.self        = self_;
  ns.forceUpdate = false;

  hard_assert( l_gettop() == 1 && ms.self != null );

  l_getglobal( functionName );
  l_rawgeti( 1, ms.self->index );
  l_pcall( 1, 0 );

  if( l_gettop() != 1 ) {
    throw Exception( "Nirvana Lua: %s(self = %d): %s", functionName, ms.self->index,
                     l_tostring( -1 ) );
  }

  return ns.forceUpdate;
}

void Lua::registerMind( int botIndex )
{
  hard_assert( l_gettop() == 1 );

  l_newtable();
  l_rawseti( 1, botIndex );
}

void Lua::unregisterMind( int botIndex )
{
  hard_assert( l_gettop() == 1 );

  l_pushnil();
  l_rawseti( 1, botIndex );
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
  log.print( "Initialising Nirvana Lua ..." );

  l = luaL_newstate();
  if( l == null ) {
    throw Exception( "Failed to create Lua state" );
  }

  hard_assert( l_gettop() == 0 );

  IMPORT_LIBS();

  if( l_gettop() != 0 ) {
    throw Exception( "Failed to initialise Lua libraries" );
  }

  ls.envName = "nirvana";

  /*
   * General functions
   */

  IMPORT_FUNC( ozException );
  IMPORT_FUNC( ozPrintln );

  IGNORE_FUNC( ozUseFailed );
  IMPORT_FUNC( ozForceUpdate );

  /*
   * Orbis
   */

  IMPORT_FUNC( ozOrbisGetGravity );
  IGNORE_FUNC( ozOrbisSetGravity );

  IGNORE_FUNC( ozOrbisAddStr );
  IGNORE_FUNC( ozOrbisAddObj );
  IGNORE_FUNC( ozOrbisAddFrag );
  IGNORE_FUNC( ozOrbisGenFrags );

  IMPORT_FUNC( ozOrbisOverlaps );
  IMPORT_FUNC( ozOrbisBindOverlaps );

  /*
   * Caelum
   */

  IGNORE_FUNC( ozCaelumLoad );

  IMPORT_FUNC( ozCaelumGetHeading );
  IGNORE_FUNC( ozCaelumSetHeading );
  IMPORT_FUNC( ozCaelumGetPeriod );
  IGNORE_FUNC( ozCaelumSetPeriod );
  IMPORT_FUNC( ozCaelumGetTime );
  IGNORE_FUNC( ozCaelumSetTime );
  IGNORE_FUNC( ozCaelumAddTime );

  /*
   * Terra
   */

  IGNORE_FUNC( ozTerraLoad );

  IMPORT_FUNC( ozTerraHeight );

  /*
   * Structure
   */

  IMPORT_FUNC( ozStrBindIndex );
  IMPORT_FUNC( ozStrBindNext );

  IMPORT_FUNC( ozStrIsNull );

  IMPORT_FUNC( ozStrGetIndex );
  IMPORT_FUNC( ozStrGetBounds );
  IMPORT_FUNC( ozStrGetPos );
  IMPORT_FUNC( ozStrGetBSP );
  IMPORT_FUNC( ozStrGetHeading );
  IMPORT_FUNC( ozStrGetLife );
  IGNORE_FUNC( ozStrSetLife );
  IGNORE_FUNC( ozStrAddLife );

  IGNORE_FUNC( ozStrDamage );
  IGNORE_FUNC( ozStrDestroy );
  IGNORE_FUNC( ozStrRemove );

  IMPORT_FUNC( ozStrGetEntityState );
  IGNORE_FUNC( ozStrSetEntityState );

  IMPORT_FUNC( ozStrGetEntityLock );
  IGNORE_FUNC( ozStrSetEntityLock );

  IMPORT_FUNC( ozStrVectorFromSelf );
  IMPORT_FUNC( ozStrVectorFromSelfEye );
  IMPORT_FUNC( ozStrDirectionFromSelf );
  IMPORT_FUNC( ozStrDirectionFromSelfEye );
  IMPORT_FUNC( ozStrDistanceFromSelf );
  IMPORT_FUNC( ozStrDistanceFromSelfEye );
  IMPORT_FUNC( ozStrHeadingFromSelf );
  IMPORT_FUNC( ozStrRelativeHeadingFromSelf );
  IMPORT_FUNC( ozStrPitchFromSelf );
  IMPORT_FUNC( ozStrPitchFromSelfEye );

  IMPORT_FUNC( ozStrBindAllOverlaps );
  IMPORT_FUNC( ozStrBindStrOverlaps );
  IMPORT_FUNC( ozStrBindObjOverlaps );

  /*
   * Object
   */

  IMPORT_FUNC( ozObjBindIndex );
  IMPORT_FUNC( ozObjBindPilot );
  IMPORT_FUNC( ozObjBindSelf );
  IGNORE_FUNC( ozObjBindUser );
  IMPORT_FUNC( ozObjBindNext );

  IMPORT_FUNC( ozObjIsNull );
  IMPORT_FUNC( ozObjIsSelf );
  IGNORE_FUNC( ozObjIsUser );
  IMPORT_FUNC( ozObjIsCut );
  IMPORT_FUNC( ozObjIsBrowsable );
  IMPORT_FUNC( ozObjIsDynamic );
  IMPORT_FUNC( ozObjIsItem );
  IMPORT_FUNC( ozObjIsWeapon );
  IMPORT_FUNC( ozObjIsBot );
  IMPORT_FUNC( ozObjIsVehicle );

  IMPORT_FUNC( ozObjGetIndex );
  IMPORT_FUNC( ozObjGetPos );
  IGNORE_FUNC( ozObjSetPos );
  IGNORE_FUNC( ozObjAddPos );
  IMPORT_FUNC( ozObjGetDim );
  IMPORT_FUNC( ozObjGetFlags );
  IMPORT_FUNC( ozObjGetHeading );
  IMPORT_FUNC( ozObjGetClassName );
  IMPORT_FUNC( ozObjGetLife );
  IGNORE_FUNC( ozObjSetLife );
  IGNORE_FUNC( ozObjAddLife );

  IGNORE_FUNC( ozObjAddEvent );

  IMPORT_FUNC( ozObjBindItems );
  IGNORE_FUNC( ozObjAddItem );
  IGNORE_FUNC( ozObjRemoveItem );
  IGNORE_FUNC( ozObjRemoveAllItems );

  IGNORE_FUNC( ozObjEnableUpdate );
  IGNORE_FUNC( ozObjDamage );
  IGNORE_FUNC( ozObjDestroy );
  IGNORE_FUNC( ozObjQuietDestroy );

  IMPORT_FUNC( ozObjVectorFromSelf );
  IMPORT_FUNC( ozObjVectorFromSelfEye );
  IMPORT_FUNC( ozObjDirectionFromSelf );
  IMPORT_FUNC( ozObjDirectionFromSelfEye );
  IMPORT_FUNC( ozObjDistanceFromSelf );
  IMPORT_FUNC( ozObjDistanceFromSelfEye );
  IMPORT_FUNC( ozObjHeadingFromSelf );
  IMPORT_FUNC( ozObjRelativeHeadingFromSelf );
  IMPORT_FUNC( ozObjPitchFromSelf );
  IMPORT_FUNC( ozObjPitchFromSelfEye );
  IMPORT_FUNC( ozObjIsVisibleFromSelf );
  IMPORT_FUNC( ozObjIsVisibleFromSelfEye );

  IMPORT_FUNC( ozObjBindAllOverlaps );
  IMPORT_FUNC( ozObjBindStrOverlaps );
  IMPORT_FUNC( ozObjBindObjOverlaps );

  /*
   * Dynamic object
   */

  IMPORT_FUNC( ozDynBindParent );

  IMPORT_FUNC( ozDynGetVelocity );
  IMPORT_FUNC( ozDynGetMomentum );
  IGNORE_FUNC( ozDynSetMomentum );
  IGNORE_FUNC( ozDynAddMomentum );
  IMPORT_FUNC( ozDynGetMass );
  IMPORT_FUNC( ozDynGetLift );

  /*
   * Weapon
   */

  IMPORT_FUNC( ozWeaponGetDefaultRounds );
  IMPORT_FUNC( ozWeaponGetRounds );
  IGNORE_FUNC( ozWeaponSetRounds );
  IGNORE_FUNC( ozWeaponAddRounds );
  IGNORE_FUNC( ozWeaponReload );

  /*
   * Bot
   */

  IMPORT_FUNC( ozBotBindPilot );

  IMPORT_FUNC( ozBotGetName );
  IGNORE_FUNC( ozBotSetName );
  IMPORT_FUNC( ozBotGetMindFunc );
  IGNORE_FUNC( ozBotSetMindFunc );

  IMPORT_FUNC( ozBotGetState );
  IMPORT_FUNC( ozBotGetEyePos );
  IMPORT_FUNC( ozBotGetH );
  IGNORE_FUNC( ozBotSetH );
  IGNORE_FUNC( ozBotAddH );
  IMPORT_FUNC( ozBotGetV );
  IGNORE_FUNC( ozBotSetV );
  IGNORE_FUNC( ozBotAddV );
  IMPORT_FUNC( ozBotGetDir );
  IMPORT_FUNC( ozBotGetStamina );
  IGNORE_FUNC( ozBotSetStamina );
  IGNORE_FUNC( ozBotAddStamina );

  IMPORT_FUNC( ozBotIsRunning );
  IGNORE_FUNC( ozBotSetRunning );
  IGNORE_FUNC( ozBotToggleRunning );

  IGNORE_FUNC( ozBotSetWeaponItem );

  IGNORE_FUNC( ozBotHeal );
  IGNORE_FUNC( ozBotRearm );
  IGNORE_FUNC( ozBotKill );

  IMPORT_FUNC( ozBotIsVisibleFromSelfEyeToEye );

  /*
   * Vehicle
   */

  IMPORT_FUNC( ozVehicleGetH );
  IGNORE_FUNC( ozVehicleSetH );
  IGNORE_FUNC( ozVehicleAddH );
  IMPORT_FUNC( ozVehicleGetV );
  IGNORE_FUNC( ozVehicleSetV );
  IGNORE_FUNC( ozVehicleAddV );
  IMPORT_FUNC( ozVehicleGetDir );

  IGNORE_FUNC( ozVehicleEmbarkPilot );

  IGNORE_FUNC( ozVehicleService );

  /*
   * Frag
   */

  IGNORE_FUNC( ozFragBindIndex );

  IGNORE_FUNC( ozFragIsNull );

  IGNORE_FUNC( ozFragGetPos );
  IGNORE_FUNC( ozFragSetPos );
  IGNORE_FUNC( ozFragAddPos );
  IGNORE_FUNC( ozFragGetIndex );
  IGNORE_FUNC( ozFragGetVelocity );
  IGNORE_FUNC( ozFragSetVelocity );
  IGNORE_FUNC( ozFragAddVelocity );
  IGNORE_FUNC( ozFragGetLife );
  IGNORE_FUNC( ozFragSetLife );
  IGNORE_FUNC( ozFragAddLife );

  IGNORE_FUNC( ozFragRemove );

  /*
   * Mind's bot
   */

  IMPORT_FUNC( ozSelfIsCut );

  IMPORT_FUNC( ozSelfGetIndex );
  IMPORT_FUNC( ozSelfGetPos );
  IMPORT_FUNC( ozSelfGetDim );
  IMPORT_FUNC( ozSelfGetFlags );
  IMPORT_FUNC( ozSelfGetTypeName );
  IMPORT_FUNC( ozSelfGetLife );

  IMPORT_FUNC( ozSelfGetVelocity );
  IMPORT_FUNC( ozSelfGetMomentum );
  IMPORT_FUNC( ozSelfGetMass );
  IMPORT_FUNC( ozSelfGetLift );

  IMPORT_FUNC( ozSelfGetName );

  IMPORT_FUNC( ozSelfGetState );
  IMPORT_FUNC( ozSelfGetEyePos );
  IMPORT_FUNC( ozSelfGetH );
  IMPORT_FUNC( ozSelfSetH );
  IMPORT_FUNC( ozSelfAddH );
  IMPORT_FUNC( ozSelfGetV );
  IMPORT_FUNC( ozSelfSetV );
  IMPORT_FUNC( ozSelfAddV );
  IMPORT_FUNC( ozSelfGetDir );
  IMPORT_FUNC( ozSelfGetStamina );

  IMPORT_FUNC( ozSelfActionForward );
  IMPORT_FUNC( ozSelfActionBackward );
  IMPORT_FUNC( ozSelfActionRight );
  IMPORT_FUNC( ozSelfActionLeft );
  IMPORT_FUNC( ozSelfActionJump );
  IMPORT_FUNC( ozSelfActionCrouch );
  IMPORT_FUNC( ozSelfActionUse );
  IMPORT_FUNC( ozSelfActionTake );
  IMPORT_FUNC( ozSelfActionGrab );
  IMPORT_FUNC( ozSelfActionThrow );
  IMPORT_FUNC( ozSelfActionAttack );
  IMPORT_FUNC( ozSelfActionExit );
  IMPORT_FUNC( ozSelfActionEject );
  IMPORT_FUNC( ozSelfActionSuicide );

  IMPORT_FUNC( ozSelfIsRunning );
  IMPORT_FUNC( ozSelfSetRunning );
  IMPORT_FUNC( ozSelfToggleRunning );

  IMPORT_FUNC( ozSelfSetGesture );

  IMPORT_FUNC( ozSelfBindItems );
  IMPORT_FUNC( ozSelfBindParent );

  IMPORT_FUNC( ozSelfBindAllOverlaps );
  IMPORT_FUNC( ozSelfBindStrOverlaps );
  IMPORT_FUNC( ozSelfBindObjOverlaps );

  /*
   * Nirvana
   */

  IMPORT_FUNC( ozNirvanaRemoveDevice );
  IMPORT_FUNC( ozNirvanaAddMemo );

  importLuaConstants( l );

  l_newtable();
  l_setglobal( "ozLocalData" );
  l_getglobal( "ozLocalData" );

  PhysFile luaDir( "lua/nirvana" );
  DArray<PhysFile> luaFiles = luaDir.ls();

  foreach( file, luaFiles.iter() ) {
    if( file->hasExtension( "lua" ) ) {
      if( !file->map() ) {
        throw Exception( "Failed to read Lua script '%s'", file->path().cstr() );
      }

      InputStream istream = file->inputStream();

      if( IMPORT_BUFFER( istream.begin(), istream.capacity(), file->path() ) != 0 ) {
        throw Exception( "Nirvana Lua script error" );
      }

      file->unmap();
    }
  }

  hard_assert( l_gettop() == 1 );

  log.printEnd( " OK" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  log.print( "Freeing Nirvana Lua ..." );

  ms.structs.clear();
  ms.structs.dealloc();

  ms.objects.clear();
  ms.objects.dealloc();

  lua_close( l );
  l = null;

  log.printEnd( " OK" );
}

}
}
