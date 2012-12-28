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
 * @file nirvana/Lua.cc
 *
 * Lua scripting engine for Nirvana
 */

#include <stable.hh>
#include <nirvana/Lua.hh>

#include <nirvana/luaapi.hh>

namespace oz
{
namespace nirvana
{

bool Lua::mindCall( const char* functionName, Bot* self_ )
{
  ms.self        = self_;
  ms.obj         = self_;
  ms.str         = nullptr;
  ms.objIndex    = 0;
  ms.strIndex    = 0;
  ns.self        = self_;
  ns.forceUpdate = false;

  hard_assert( l_gettop() == 1 && ms.self != nullptr );

  l_getglobal( functionName );
  l_rawgeti( 1, ms.self->index );
  l_pcall( 1, 0 );

  if( l_gettop() != 1 ) {
    Log::println( "Lua[N] in %s(self = %d): %s", functionName, ms.self->index, l_tostring( -1 ) );
    System::bell();

    l_pop( 1 );
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
  hard_assert( ( l_pushnil(), true ) );
  hard_assert( !l_next( 1 ) );

  int index = istream->readInt();

  while( index >= 0 ) {
    readVariable( istream );

    l_rawseti( 1, index );

    index = istream->readInt();
  }
}

void Lua::write( BufferStream* ostream )
{
  hard_assert( l_gettop() == 1 );

  l_pushnil();
  while( l_next( 1 ) ) {
    hard_assert( l_type( -2 ) == LUA_TNUMBER );
    hard_assert( l_type( -1 ) == LUA_TTABLE );

    ostream->writeInt( l_toint( -2 ) );
    writeVariable( ostream );

    l_pop( 1 );
  }

  ostream->writeInt( -1 );
}

void Lua::init()
{
  Log::print( "Initialising Nirvana Lua ..." );

  initCommon( "nirvana" );

  ms.structs.allocate( 32 );
  ms.objects.allocate( 512 );

  /*
   * General functions
   */

  IMPORT_FUNC( ozError );
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
  IGNORE_FUNC( ozCaelumSetRealTime );

  /*
   * Terra
   */

  IGNORE_FUNC( ozTerraLoad );

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
  IGNORE_FUNC( ozStrSetLife );
  IGNORE_FUNC( ozStrAddLife );
  IMPORT_FUNC( ozStrDefaultResistance );
  IMPORT_FUNC( ozStrGetResistance );
  IGNORE_FUNC( ozStrSetResistance );

  IGNORE_FUNC( ozStrDamage );
  IGNORE_FUNC( ozStrDestroy );
  IGNORE_FUNC( ozStrRemove );

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
  IGNORE_FUNC( ozEntSetState );
  IMPORT_FUNC( ozEntGetLock );
  IGNORE_FUNC( ozEntSetLock );
  IGNORE_FUNC( ozEntTrigger );

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
  IGNORE_FUNC( ozBindUser );
  IMPORT_FUNC( ozBindNextObj );

  IMPORT_FUNC( ozObjIsNull );
  IMPORT_FUNC( ozObjIsSelf );
  IGNORE_FUNC( ozObjIsUser );
  IMPORT_FUNC( ozObjIsCut );

  IMPORT_FUNC( ozObjGetIndex );
  IMPORT_FUNC( ozObjGetPos );
  IGNORE_FUNC( ozObjWarpPos );
  IMPORT_FUNC( ozObjGetDim );
  IMPORT_FUNC( ozObjHasFlag );
  IMPORT_FUNC( ozObjGetHeading );
  IMPORT_FUNC( ozObjGetClassName );

  IMPORT_FUNC( ozObjMaxLife );
  IMPORT_FUNC( ozObjGetLife );
  IGNORE_FUNC( ozObjSetLife );
  IGNORE_FUNC( ozObjAddLife );
  IMPORT_FUNC( ozObjDefaultResistance );
  IMPORT_FUNC( ozObjGetResistance );
  IGNORE_FUNC( ozObjSetResistance );

  IGNORE_FUNC( ozObjAddEvent );

  IMPORT_FUNC( ozObjBindItems );
  IMPORT_FUNC( ozObjBindItem );
  IGNORE_FUNC( ozObjAddItem );
  IGNORE_FUNC( ozObjRemoveItem );
  IGNORE_FUNC( ozObjRemoveAllItems );

  IGNORE_FUNC( ozObjEnableUpdate );
  IGNORE_FUNC( ozObjDamage );
  IGNORE_FUNC( ozObjDestroy );

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
  IGNORE_FUNC( ozDynSetMomentum );
  IGNORE_FUNC( ozDynAddMomentum );
  IMPORT_FUNC( ozDynGetMass );
  IMPORT_FUNC( ozDynGetLift );

  /*
   * Weapon
   */

  IMPORT_FUNC( ozWeaponMaxRounds );
  IMPORT_FUNC( ozWeaponGetRounds );
  IGNORE_FUNC( ozWeaponSetRounds );
  IGNORE_FUNC( ozWeaponAddRounds );

  /*
   * Bot
   */

  IMPORT_FUNC( ozBotGetName );
  IGNORE_FUNC( ozBotSetName );
  IMPORT_FUNC( ozBotGetMind );
  IGNORE_FUNC( ozBotSetMind );

  IMPORT_FUNC( ozBotHasState );
  IMPORT_FUNC( ozBotGetEyePos );
  IMPORT_FUNC( ozBotGetH );
  IGNORE_FUNC( ozBotSetH );
  IGNORE_FUNC( ozBotAddH );
  IMPORT_FUNC( ozBotGetV );
  IGNORE_FUNC( ozBotSetV );
  IGNORE_FUNC( ozBotAddV );
  IMPORT_FUNC( ozBotGetDir );

  IMPORT_FUNC( ozBotMaxStamina );
  IMPORT_FUNC( ozBotGetStamina );
  IGNORE_FUNC( ozBotSetStamina );
  IGNORE_FUNC( ozBotAddStamina );

  IMPORT_FUNC( ozBotGetCargo );
  IMPORT_FUNC( ozBotGetWeapon );
  IGNORE_FUNC( ozBotSetWeaponItem );

  IMPORT_FUNC( ozBotCanReachEntity );
  IMPORT_FUNC( ozBotCanReachObj );

  IGNORE_FUNC( ozBotAction );

  IGNORE_FUNC( ozBotHeal );
  IGNORE_FUNC( ozBotRearm );
  IGNORE_FUNC( ozBotKill );

  /*
   * Vehicle
   */

  IMPORT_FUNC( ozVehicleGetPilot );

  IMPORT_FUNC( ozVehicleGetH );
  IGNORE_FUNC( ozVehicleSetH );
  IGNORE_FUNC( ozVehicleAddH );
  IMPORT_FUNC( ozVehicleGetV );
  IGNORE_FUNC( ozVehicleSetV );
  IGNORE_FUNC( ozVehicleAddV );
  IMPORT_FUNC( ozVehicleGetDir );

  IGNORE_FUNC( ozVehicleEmbarkBot );
  IGNORE_FUNC( ozVehicleDisembarkBot );

  IGNORE_FUNC( ozVehicleService );

  /*
   * Frag
   */

  IMPORT_FUNC( ozFragBindIndex );

  IMPORT_FUNC( ozFragIsNull );

  IMPORT_FUNC( ozFragGetPos );
  IGNORE_FUNC( ozFragWarpPos );
  IMPORT_FUNC( ozFragGetIndex );
  IMPORT_FUNC( ozFragGetVelocity );
  IGNORE_FUNC( ozFragSetVelocity );
  IGNORE_FUNC( ozFragAddVelocity );
  IMPORT_FUNC( ozFragGetLife );
  IGNORE_FUNC( ozFragSetLife );
  IGNORE_FUNC( ozFragAddLife );

  IGNORE_FUNC( ozFragRemove );

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

  /*
   * Mind's bot
   */

  IMPORT_FUNC( ozSelfIsCut );

  IMPORT_FUNC( ozSelfGetIndex );
  IMPORT_FUNC( ozSelfGetPos );
  IMPORT_FUNC( ozSelfGetDim );
  IMPORT_FUNC( ozSelfHasFlag );
  IMPORT_FUNC( ozSelfGetHeading );
  IMPORT_FUNC( ozSelfGetClassName );
  IMPORT_FUNC( ozSelfMaxLife );
  IMPORT_FUNC( ozSelfGetLife );
  IMPORT_FUNC( ozSelfDefaultResistance );
  IMPORT_FUNC( ozSelfGetResistance );

  IMPORT_FUNC( ozSelfGetParent );
  IMPORT_FUNC( ozSelfGetVelocity );
  IMPORT_FUNC( ozSelfGetMomentum );
  IMPORT_FUNC( ozSelfGetMass );
  IMPORT_FUNC( ozSelfGetLift );

  IMPORT_FUNC( ozSelfGetName );
  IMPORT_FUNC( ozSelfGetMind );
  IMPORT_FUNC( ozSelfHasState );
  IMPORT_FUNC( ozSelfGetEyePos );
  IMPORT_FUNC( ozSelfGetH );
  IMPORT_FUNC( ozSelfSetH );
  IMPORT_FUNC( ozSelfAddH );
  IMPORT_FUNC( ozSelfGetV );
  IMPORT_FUNC( ozSelfSetV );
  IMPORT_FUNC( ozSelfAddV );
  IMPORT_FUNC( ozSelfGetDir );

  IMPORT_FUNC( ozSelfGetStamina );
  IMPORT_FUNC( ozSelfMaxStamina );

  IMPORT_FUNC( ozSelfGetCargo );
  IMPORT_FUNC( ozSelfGetWeapon );
  IMPORT_FUNC( ozSelfSetWeaponItem );

  IMPORT_FUNC( ozSelfCanReachEntity );
  IMPORT_FUNC( ozSelfCanReachObj );

  IMPORT_FUNC( ozSelfAction );

  IMPORT_FUNC( ozSelfBindItems );
  IMPORT_FUNC( ozSelfBindItem );

  IMPORT_FUNC( ozSelfOverlaps );
  IMPORT_FUNC( ozSelfBindOverlaps );

  /*
   * Nirvana
   */

  IMPORT_FUNC( ozNirvanaRemoveDevice );
  IMPORT_FUNC( ozNirvanaAddMemo );

  importMatrixConstants( l );

  l_newtable();
  l_setglobal( "ozLocalData" );
  l_getglobal( "ozLocalData" );

  PFile luaDir( "lua/nirvana" );
  DArray<PFile> luaFiles = luaDir.ls();

  foreach( file, luaFiles.iter() ) {
    if( file->hasExtension( "lua" ) ) {
      if( !file->map() ) {
        OZ_ERROR( "Failed to read Lua script '%s'", file->path().cstr() );
      }

      InputStream istream = file->inputStream();

      if( l_dobuffer( istream.begin(), istream.capacity(), file->path() ) != 0 ) {
        OZ_ERROR( "Nirvana Lua script error" );
      }

      file->unmap();
    }
  }

  hard_assert( l_gettop() == 1 );

  Log::printEnd( " OK" );
}

void Lua::destroy()
{
  if( l == nullptr ) {
    return;
  }

  Log::print( "Destroying Nirvana Lua ..." );

  ms.structs.clear();
  ms.structs.deallocate();

  ms.objects.clear();
  ms.objects.deallocate();

  hard_assert( l_gettop() == 1 );
  hard_assert( ( l_pushnil(), true ) );
  hard_assert( !l_next( 1 ) );

  freeCommon();

  Log::printEnd( " OK" );
}

Lua lua;

}
}
