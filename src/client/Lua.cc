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
 * @file client/Lua.cc
 *
 * Lua scripting engine for client
 */

#include "stable.hh"

#include "client/Lua.hh"

#include "client/luaapi.hh"

using namespace oz::nirvana;

namespace oz
{
namespace client
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

void Lua::staticCall( const char* functionName )
{
  ms.obj      = null;
  ms.str      = null;
  ms.frag     = null;
  ms.objIndex = 0;
  ms.strIndex = 0;

  hard_assert( l_gettop() == 0 );

  l_getglobal( functionName );
  l_pcall( 0, 0 );

  if( l_gettop() != 0 ) {
    Log::println( "Lua[C] in %s(): %s", functionName, l_tostring( -1 ) );
    System::bell();

    l_pop( 1 );
  }
}

void Lua::update()
{
  staticCall( "onUpdate" );
}

void Lua::create( const char* mission_ )
{
  cs.mission = mission_;
  String missionPath = "lua/mission/" + cs.mission + ".lua";

  Log::print( "Importing mission catalogue '%s' ...", cs.mission.cstr() );
  if( cs.missionLingua.initDomain( cs.mission ) ) {
    Log::printEnd( " OK" );
  }
  else {
    Log::printEnd( " Failed" );
  }

  Log::println( "Executing mission script %s {", cs.mission.cstr() );
  Log::indent();

  PFile missionFile( missionPath );
  if( !missionFile.map() ) {
    throw Exception( "Failed to read mission file '%s'", missionFile.path().cstr() );
  }

  InputStream istream = missionFile.inputStream();

  if( IMPORT_BUFFER( istream.begin(), istream.capacity(), missionPath ) != 0 ) {
    throw Exception( "Client Lua script error" );
  }

  missionFile.unmap();

  staticCall( "onCreate" );

  Log::unindent();
  Log::println( "}" );
}

void Lua::read( InputStream* istream )
{
  hard_assert( l_gettop() == 0 );

  cs.mission = istream->readString();
  String missionPath = "lua/mission/" + cs.mission + ".lua";

  Log::print( "Importing mission catalogue '%s' ...", cs.mission.cstr() );
  if( cs.missionLingua.initDomain( cs.mission ) ) {
    Log::printEnd( " OK" );
  }
  else {
    Log::printEnd( " Failed" );
  }

  Log::print( "Deserialising mission script %s ...", cs.mission.cstr() );

  PFile missionFile( missionPath );
  if( !missionFile.map() ) {
    throw Exception( "Failed to read mission script '%s'", missionFile.path().cstr() );
  }

  InputStream is = missionFile.inputStream();

  if( IMPORT_BUFFER( is.begin(), is.capacity(), missionPath ) != 0 ) {
    throw Exception( "Client Lua script error" );
  }

  missionFile.unmap();

  char ch = istream->readChar();

  while( ch != '\0' ) {
    hard_assert( ch == 's' );

    String name = istream->readString();
    readVariable( istream );

    l_setglobal( name );

    ch = istream->readChar();
  }

  Log::printEnd( " OK" );
}

void Lua::write( BufferStream* ostream )
{
  hard_assert( l_gettop() == 0 );

  ostream->writeString( cs.mission );

#if LUA_VERSION_NUM >= 502
  l_pushglobaltable();
#endif

  l_pushnil();
#if LUA_VERSION_NUM >= 502
  while( l_next( -2 ) != 0 ) {
#else
  while( l_next( LUA_GLOBALSINDEX ) != 0 ) {
#endif
    hard_assert( l_type( -2 ) == LUA_TSTRING );

    const char* name = l_tostring( -2 );
    if( name[0] == 'o' && name[1] == 'z' && name[2] == '_' ) {
      ostream->writeChar( 's' );
      ostream->writeString( l_tostring( -2 ) );

      writeVariable( ostream );
    }

    l_pop( 1 );
  }

#if LUA_VERSION_NUM >= 502
  l_pop( 1 );
#endif

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
  Log::print( "Initialising Client Lua ..." );

  l = luaL_newstate();
  if( l == null ) {
    throw Exception( "Failed to create Lua state" );
  }

  hard_assert( l_gettop() == 0 );

  IMPORT_LIBS();

  if( l_gettop() != 0 ) {
    throw Exception( "Failed to initialise Lua libraries" );
  }

  ls.envName = "client";
  ms.structs.alloc( 32 );
  ms.objects.alloc( 512 );

  /*
   * General functions
   */

  IMPORT_FUNC( ozException );
  IMPORT_FUNC( ozPrintln );

  IMPORT_FUNC( ozGettext );

  IGNORE_FUNC( ozUseFailed );
  IGNORE_FUNC( ozForceUpdate );

  /*
   * Orbis
   */

  IMPORT_FUNC( ozOrbisGetGravity );
  IMPORT_FUNC( ozOrbisSetGravity );

  IMPORT_FUNC( ozOrbisAddStr );
  IMPORT_FUNC( ozOrbisAddObj );
  IMPORT_FUNC( ozOrbisAddFrag );
  IMPORT_FUNC( ozOrbisGenFrags );

  IMPORT_FUNC( ozOrbisAddPlayer );

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

  IGNORE_FUNC( ozStrVectorFromSelf );
  IGNORE_FUNC( ozStrVectorFromSelfEye );
  IGNORE_FUNC( ozStrDirFromSelf );
  IGNORE_FUNC( ozStrDirFromSelfEye );
  IGNORE_FUNC( ozStrDistFromSelf );
  IGNORE_FUNC( ozStrDistFromSelfEye );
  IGNORE_FUNC( ozStrHeadingFromSelfEye );
  IGNORE_FUNC( ozStrRelHeadingFromSelfEye );
  IGNORE_FUNC( ozStrPitchFromSelfEye );
  IGNORE_FUNC( ozStrIsVisibleFromSelf );
  IGNORE_FUNC( ozStrIsVisibleFromSelfEye );

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

  IGNORE_FUNC( ozEntVectorFromSelf );
  IGNORE_FUNC( ozEntVectorFromSelfEye );
  IGNORE_FUNC( ozEntDirFromSelf );
  IGNORE_FUNC( ozEntDirFromSelfEye );
  IGNORE_FUNC( ozEntDistFromSelf );
  IGNORE_FUNC( ozEntDistFromSelfEye );
  IGNORE_FUNC( ozEntHeadingFromSelfEye );
  IGNORE_FUNC( ozEntRelHeadingFromSelfEye );
  IGNORE_FUNC( ozEntPitchFromSelfEye );
  IGNORE_FUNC( ozEntIsVisibleFromSelf );
  IGNORE_FUNC( ozEntIsVisibleFromSelfEye );

  /*
   * Object
   */

  IMPORT_FUNC( ozBindObj );
  IGNORE_FUNC( ozBindSelf );
  IGNORE_FUNC( ozBindUser );
  IMPORT_FUNC( ozBindNextObj );

  IMPORT_FUNC( ozObjIsNull );
  IGNORE_FUNC( ozObjIsSelf );
  IGNORE_FUNC( ozObjIsUser );
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

  IGNORE_FUNC( ozObjVectorFromSelf );
  IGNORE_FUNC( ozObjVectorFromSelfEye );
  IGNORE_FUNC( ozObjDirFromSelf );
  IGNORE_FUNC( ozObjDirFromSelfEye );
  IGNORE_FUNC( ozObjDistFromSelf );
  IGNORE_FUNC( ozObjDistFromSelfEye );
  IGNORE_FUNC( ozObjHeadingFromSelfEye );
  IGNORE_FUNC( ozObjRelHeadingFromSelfEye );
  IGNORE_FUNC( ozObjPitchFromSelfEye );
  IGNORE_FUNC( ozObjIsVisibleFromSelf );
  IGNORE_FUNC( ozObjIsVisibleFromSelfEye );

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

  IMPORT_FUNC( ozBotMaxStamina );
  IMPORT_FUNC( ozBotGetStamina );
  IMPORT_FUNC( ozBotSetStamina );
  IMPORT_FUNC( ozBotAddStamina );

  IMPORT_FUNC( ozBotGetCargo );
  IMPORT_FUNC( ozBotGetWeapon );
  IMPORT_FUNC( ozBotSetWeaponItem );

  IMPORT_FUNC( ozBotCanReachEntity );
  IMPORT_FUNC( ozBotCanReachObj );

  IMPORT_FUNC( ozBotAction );

  IMPORT_FUNC( ozBotHeal );
  IMPORT_FUNC( ozBotRearm );
  IMPORT_FUNC( ozBotKill );

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

  IGNORE_FUNC( ozFragVectorFromSelf );
  IGNORE_FUNC( ozFragVectorFromSelfEye );
  IGNORE_FUNC( ozFragDirFromSelf );
  IGNORE_FUNC( ozFragDirFromSelfEye );
  IGNORE_FUNC( ozFragDistFromSelf );
  IGNORE_FUNC( ozFragDistFromSelfEye );
  IGNORE_FUNC( ozFragHeadingFromSelfEye );
  IGNORE_FUNC( ozFragRelHeadingFromSelfEye );
  IGNORE_FUNC( ozFragPitchFromSelfEye );
  IGNORE_FUNC( ozFragIsVisibleFromSelf );
  IGNORE_FUNC( ozFragIsVisibleFromSelfEye );

  /*
   * Mind's bot
   */

  IGNORE_FUNC( ozSelfIsCut );

  IGNORE_FUNC( ozSelfGetIndex );
  IGNORE_FUNC( ozSelfGetPos );
  IGNORE_FUNC( ozSelfGetDim );
  IGNORE_FUNC( ozSelfHasFlag );
  IGNORE_FUNC( ozSelfGetHeading );
  IGNORE_FUNC( ozSelfGetClassName );
  IGNORE_FUNC( ozSelfMaxLife );
  IGNORE_FUNC( ozSelfGetLife );
  IGNORE_FUNC( ozSelfDefaultResistance );
  IGNORE_FUNC( ozSelfGetResistance );

  IGNORE_FUNC( ozSelfBindItems );
  IGNORE_FUNC( ozSelfBindItem );

  IGNORE_FUNC( ozSelfGetParent );

  IGNORE_FUNC( ozSelfGetVelocity );
  IGNORE_FUNC( ozSelfGetMomentum );
  IGNORE_FUNC( ozSelfGetMass );
  IGNORE_FUNC( ozSelfGetLift );

  IGNORE_FUNC( ozSelfGetName );
  IGNORE_FUNC( ozSelfGetMind );
  IGNORE_FUNC( ozSelfHasState );
  IGNORE_FUNC( ozSelfGetEyePos );
  IGNORE_FUNC( ozSelfGetH );
  IGNORE_FUNC( ozSelfSetH );
  IGNORE_FUNC( ozSelfAddH );
  IGNORE_FUNC( ozSelfGetV );
  IGNORE_FUNC( ozSelfSetV );
  IGNORE_FUNC( ozSelfAddV );
  IGNORE_FUNC( ozSelfGetDir );

  IGNORE_FUNC( ozSelfGetStamina );
  IGNORE_FUNC( ozSelfMaxStamina );

  IGNORE_FUNC( ozSelfGetCargo );
  IGNORE_FUNC( ozSelfGetWeapon );
  IGNORE_FUNC( ozSelfSetWeaponItem );

  IGNORE_FUNC( ozSelfCanReachEntity );
  IGNORE_FUNC( ozSelfCanReachObj );

  IGNORE_FUNC( ozSelfAction );

  IGNORE_FUNC( ozSelfBindItems );
  IGNORE_FUNC( ozSelfBindItem );

  IGNORE_FUNC( ozSelfOverlaps );
  IGNORE_FUNC( ozSelfBindOverlaps );

  /*
   * Nirvana
   */

  IMPORT_FUNC( ozNirvanaRemoveDevice );
  IMPORT_FUNC( ozNirvanaAddMemo );

  /*
   * QuestList
   */

  IMPORT_FUNC( ozQuestAdd );
  IMPORT_FUNC( ozQuestEnd );

  /*
   * Camera
   */

  IMPORT_FUNC( ozCameraMoveTo );
  IMPORT_FUNC( ozCameraWarpTo );

  IMPORT_FUNC( ozCameraAddSwitchableUnit );
  IMPORT_FUNC( ozCameraClearSwitchableUnits );
  IMPORT_FUNC( ozCameraSwitchTo );
  IMPORT_FUNC( ozCameraAllowReincarnation );
  IMPORT_FUNC( ozCameraSetState );

  IMPORT_FUNC( ozCameraAddStateSwitch );
  IMPORT_FUNC( ozCameraAddWait );
  IMPORT_FUNC( ozCameraAddMove );

  /*
   * Profile
   */

  IMPORT_FUNC( ozProfileGetName );
  IMPORT_FUNC( ozProfileGetBot );

  importMatrixConstants( l );
  importClientConstants( l );

  hard_assert( l_gettop() == 0 );

  Log::printEnd( " OK" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  Log::print( "Freeing Client Lua ..." );

  ms.structs.clear();
  ms.structs.dealloc();

  ms.objects.clear();
  ms.objects.dealloc();

  cs.mission = "";
  cs.missionLingua.free();

  lua_close( l );
  l = null;

  Log::printEnd( " OK" );
}

}
}
