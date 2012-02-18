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

#include "matrix/Library.hh"
#include "matrix/Vehicle.hh"
#include "matrix/Collider.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

#include "common/lua.hh"

namespace oz
{
namespace matrix
{

#include "matrix/luaapi.hh"

Lua lua;

bool Lua::readVariable( InputStream* istream )
{
  char ch = istream->readChar();

  switch( ch ) {
    case 'N': {
      pushnil();
      return true;
    }
    case 'b': {
      pushbool( istream->readBool() );
      return true;
    }
    case 'n': {
      pushdouble( istream->readDouble() );
      return true;
    }
    case 's': {
      pushstring( istream->readString() );
      return true;
    }
    case '[': {
      newtable();

      while( readVariable( istream ) ) { // key
        readVariable( istream ); // value

        rawset( -3 );
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
  int type = type( -1 );

  switch( type ) {
    case LUA_TNIL: {
      ostream->writeChar( 'N' );
      break;
    }
    case LUA_TBOOLEAN: {
      ostream->writeChar( 'b' );
      ostream->writeBool( tobool( -1 ) != 0 );
      break;
    }
    case LUA_TNUMBER: {
      ostream->writeChar( 'n' );
      ostream->writeDouble( todouble( -1 ) );
      break;
    }
    case LUA_TSTRING: {
      ostream->writeChar( 's' );
      ostream->writeString( tostring( -1 ) );
      break;
    }
    case LUA_TTABLE: {
      ostream->writeChar( '[' );

      pushnil();
      while( next( -2 ) != 0 ) {
        // key
        pushvalue( -2 );
        writeVariable( ostream );
        pop( 1 );

        // value
        writeVariable( ostream );

        pop( 1 );
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

  hard_assert( gettop() == 1 && ms.self != null );

  getglobal( functionName );
  rawgeti( 1, ms.self->index );
  lua_pcall( l, 1, 0, 0 );

  if( gettop() != 1 ) {
    throw Exception( "Matrix Lua: %s(self = %d, user = %d): %s", functionName, ms.self->index,
                     ms.user == null ? -1 : ms.user->index, tostring( -1 ) );
  }

  return !ms.hasUseFailed;
}

void Lua::registerObject( int index )
{
  // we cannot depend that ozLocalData exists at index 1 as this function can be called via a
  // script creating an object
  getglobal( "ozLocalData" );
  newtable();
  rawseti( -2, index );
  pop( 1 );
}

void Lua::unregisterObject( int index )
{
  // we cannot depend that ozLocalData exists at index 1 as this function can be called via a
  // script creating an object
  getglobal( "ozLocalData" );
  pushnil();
  rawseti( -2, index );
  pop( 1 );
}

void Lua::read( InputStream* istream )
{
  hard_assert( gettop() == 1 );
  hard_assert( ( pushnil(), next( 1 ) == 0 ) );

  char ch = istream->readChar();

  while( ch != '\0' ) {
    hard_assert( ch == 'i' );

    int index = istream->readInt();
    readVariable( istream );

    rawseti( 1, index );

    ch = istream->readChar();
  }
}

void Lua::write( BufferStream* ostream )
{
  hard_assert( gettop() == 1 );

  pushnil();
  while( next( 1 ) != 0 ) {
    hard_assert( type( -2 ) == LUA_TNUMBER );
    hard_assert( type( -1 ) == LUA_TTABLE );

    ostream->writeChar( 'i' );
    ostream->writeInt( toint( -2 ) );
    writeVariable( ostream );

    pop( 1 );
  }

  ostream->writeChar( '\0' );
}

void Lua::registerFunction( const char* name, LuaAPI func )
{
  lua_register( l, name, func );
}

void Lua::registerConstant( const char* name, bool value )
{
  pushbool( value );
  setglobal( name );
}

void Lua::registerConstant( const char* name, int value )
{
  pushint( value );
  setglobal( name );
}

void Lua::registerConstant( const char* name, float value )
{
  pushfloat( value );
  setglobal( name );
}

void Lua::registerConstant( const char* name, const char* value )
{
  pushstring( value );
  setglobal( name );
}

void Lua::init()
{
  log.print( "Initialising Matrix Lua ..." );

  l = luaL_newstate();
  if( l == null ) {
    throw Exception( "Failed to create Lua state" );
  }

  hard_assert( gettop() == 0 );

  IMPORT_LIBS();

  if( gettop() != 0 ) {
    throw Exception( "Failed to initialise Lua libraries" );
  }

  /*
   * General functions
   */

  IMPORT_FUNC( ozPrintln );
  IMPORT_FUNC( ozException );

  IMPORT_FUNC( ozUseFailed );

  /*
   * Orbis
   */

  IMPORT_FUNC( ozOrbisGetGravity );
  IMPORT_FUNC( ozOrbisSetGravity );

  IMPORT_FUNC( ozOrbisAddStr );
  IMPORT_FUNC( ozOrbisTryAddStr );
  IMPORT_FUNC( ozOrbisAddObj );
  IMPORT_FUNC( ozOrbisTryAddObj );
  IMPORT_FUNC( ozOrbisAddFrag );
  IMPORT_FUNC( ozOrbisGenFrags );

  IMPORT_FUNC( ozOrbisBindAllOverlaps );
  IMPORT_FUNC( ozOrbisBindStrOverlaps );
  IMPORT_FUNC( ozOrbisBindObjOverlaps );

  /*
   * Terra
   */

  IMPORT_FUNC( ozTerraLoad );

  IMPORT_FUNC( ozTerraHeight );

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
  IMPORT_FUNC( ozStrSetLife );
  IMPORT_FUNC( ozStrAddLife );

  IMPORT_FUNC( ozStrDamage );
  IMPORT_FUNC( ozStrDestroy );
  IMPORT_FUNC( ozStrRemove );

  IMPORT_FUNC( ozStrGetEntityLock );
  IMPORT_FUNC( ozStrSetEntityLock );

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
  IMPORT_FUNC( ozObjBindUser );
  IMPORT_FUNC( ozObjBindNext );

  IMPORT_FUNC( ozObjIsNull );
  IMPORT_FUNC( ozObjIsSelf );
  IMPORT_FUNC( ozObjIsUser );
  IMPORT_FUNC( ozObjIsCut );
  IMPORT_FUNC( ozObjIsBrowsable );
  IMPORT_FUNC( ozObjIsDynamic );
  IMPORT_FUNC( ozObjIsItem );
  IMPORT_FUNC( ozObjIsWeapon );
  IMPORT_FUNC( ozObjIsBot );
  IMPORT_FUNC( ozObjIsVehicle );

  IMPORT_FUNC( ozObjGetIndex );
  IMPORT_FUNC( ozObjGetPos );
  IMPORT_FUNC( ozObjSetPos );
  IMPORT_FUNC( ozObjAddPos );
  IMPORT_FUNC( ozObjGetDim );
  IMPORT_FUNC( ozObjGetFlags );
  IMPORT_FUNC( ozObjGetHeading );
  IMPORT_FUNC( ozObjGetClassName );
  IMPORT_FUNC( ozObjGetLife );
  IMPORT_FUNC( ozObjSetLife );
  IMPORT_FUNC( ozObjAddLife );

  IMPORT_FUNC( ozObjAddEvent );

  IMPORT_FUNC( ozObjBindItems );
  IMPORT_FUNC( ozObjAddItem );
  IMPORT_FUNC( ozObjRemoveItem );
  IMPORT_FUNC( ozObjRemoveAllItems );

  IMPORT_FUNC( ozObjEnableUpdate );
  IMPORT_FUNC( ozObjDamage );
  IMPORT_FUNC( ozObjDestroy );
  IMPORT_FUNC( ozObjQuietDestroy );

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
  IMPORT_FUNC( ozDynSetMomentum );
  IMPORT_FUNC( ozDynAddMomentum );
  IMPORT_FUNC( ozDynGetMass );
  IMPORT_FUNC( ozDynGetLift );

  /*
   * Weapon
   */

  IMPORT_FUNC( ozWeaponGetDefaultRounds );
  IMPORT_FUNC( ozWeaponGetRounds );
  IMPORT_FUNC( ozWeaponSetRounds );
  IMPORT_FUNC( ozWeaponAddRounds );
  IMPORT_FUNC( ozWeaponReload );

  /*
   * Bot
   */

  IMPORT_FUNC( ozBotBindPilot );

  IMPORT_FUNC( ozBotGetName );
  IMPORT_FUNC( ozBotSetName );
  IMPORT_FUNC( ozBotGetMindFunc );
  IMPORT_FUNC( ozBotSetMindFunc );

  IMPORT_FUNC( ozBotGetState );
  IMPORT_FUNC( ozBotGetEyePos );
  IMPORT_FUNC( ozBotGetH );
  IMPORT_FUNC( ozBotSetH );
  IMPORT_FUNC( ozBotAddH );
  IMPORT_FUNC( ozBotGetV );
  IMPORT_FUNC( ozBotSetV );
  IMPORT_FUNC( ozBotAddV );
  IMPORT_FUNC( ozBotGetDir );

  IMPORT_FUNC( ozBotGetStamina );
  IMPORT_FUNC( ozBotSetStamina );
  IMPORT_FUNC( ozBotAddStamina );

  IMPORT_FUNC( ozBotActionForward );
  IMPORT_FUNC( ozBotActionBackward );
  IMPORT_FUNC( ozBotActionRight );
  IMPORT_FUNC( ozBotActionLeft );
  IMPORT_FUNC( ozBotActionJump );
  IMPORT_FUNC( ozBotActionCrouch );
  IMPORT_FUNC( ozBotActionUse );
  IMPORT_FUNC( ozBotActionTake );
  IMPORT_FUNC( ozBotActionGrab );
  IMPORT_FUNC( ozBotActionThrow );
  IMPORT_FUNC( ozBotActionAttack );
  IMPORT_FUNC( ozBotActionExit );
  IMPORT_FUNC( ozBotActionEject );
  IMPORT_FUNC( ozBotActionSuicide );

  IMPORT_FUNC( ozBotIsRunning );
  IMPORT_FUNC( ozBotSetRunning );
  IMPORT_FUNC( ozBotToggleRunning );

  IMPORT_FUNC( ozBotSetGesture );

  IMPORT_FUNC( ozBotSetWeaponItem );

  IMPORT_FUNC( ozBotHeal );
  IMPORT_FUNC( ozBotRearm );
  IMPORT_FUNC( ozBotKill );

  IMPORT_FUNC( ozBotIsVisibleFromSelfEyeToEye );

  /*
   * Vehicle
   */

  IMPORT_FUNC( ozVehicleGetH );
  IMPORT_FUNC( ozVehicleSetH );
  IMPORT_FUNC( ozVehicleAddH );
  IMPORT_FUNC( ozVehicleGetV );
  IMPORT_FUNC( ozVehicleSetV );
  IMPORT_FUNC( ozVehicleAddV );
  IMPORT_FUNC( ozVehicleGetDir );

  IMPORT_FUNC( ozVehicleEmbarkPilot );

  IMPORT_FUNC( ozVehicleService );

  /*
   * Frag
   */

  IMPORT_FUNC( ozFragBindIndex );

  IMPORT_FUNC( ozFragIsNull );

  IMPORT_FUNC( ozFragGetPos );
  IMPORT_FUNC( ozFragSetPos );
  IMPORT_FUNC( ozFragAddPos );
  IMPORT_FUNC( ozFragGetIndex );
  IMPORT_FUNC( ozFragGetVelocity );
  IMPORT_FUNC( ozFragSetVelocity );
  IMPORT_FUNC( ozFragAddVelocity );
  IMPORT_FUNC( ozFragGetLife );
  IMPORT_FUNC( ozFragSetLife );
  IMPORT_FUNC( ozFragAddLife );

  IMPORT_FUNC( ozFragRemove );

#include "matrix/luaconst.hh"

  newtable();
  setglobal( "ozLocalData" );
  getglobal( "ozLocalData" );

  PhysFile luaDir( "lua/matrix" );
  DArray<PhysFile> luaFiles = luaDir.ls();

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

  hard_assert( gettop() == 1 );

  log.printEnd( " OK" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  log.print( "Freeing Matrix Lua ..." );

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
