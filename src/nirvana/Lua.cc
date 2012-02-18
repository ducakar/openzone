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

#include "matrix/Library.hh"
#include "matrix/Vehicle.hh"
#include "matrix/Collider.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"

#include "nirvana/Nirvana.hh"
#include "nirvana/Memo.hh"

#include "common/lua.hh"

namespace oz
{
namespace nirvana
{

#include "matrix/luaapi.hh"
#include "nirvana/luaapi.hh"

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

bool Lua::mindCall( const char* functionName, Bot* self_ )
{
  ms.self        = self_;
  ms.obj         = self_;
  ms.str         = null;
  ms.objIndex    = 0;
  ms.strIndex    = 0;
  ns.self        = self_;
  ns.forceUpdate = false;

  hard_assert( gettop() == 1 && ms.self != null );

  getglobal( functionName );
  rawgeti( 1, ms.self->index );
  lua_pcall( l, 1, 0, 0 );

  if( gettop() != 1 ) {
    throw Exception( "Nirvana Lua: %s(self = %d): %s", functionName, ms.self->index,
                     tostring( -1 ) );
  }

  return ns.forceUpdate;
}

void Lua::registerMind( int botIndex )
{
  hard_assert( gettop() == 1 );

  newtable();
  rawseti( 1, botIndex );
}

void Lua::unregisterMind( int botIndex )
{
  hard_assert( gettop() == 1 );

  pushnil();
  rawseti( 1, botIndex );
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
  log.print( "Initialising Nirvana Lua ..." );

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

  IGNORE_FUNC( ozUseFailed );
  IMPORT_FUNC( ozForceUpdate );

  /*
   * Orbis
   */

  IMPORT_FUNC( ozOrbisGetGravity );
  IGNORE_FUNC( ozOrbisSetGravity );

  IGNORE_FUNC( ozOrbisAddStr );
  IGNORE_FUNC( ozOrbisTryAddStr );
  IGNORE_FUNC( ozOrbisAddObj );
  IGNORE_FUNC( ozOrbisTryAddObj );
  IGNORE_FUNC( ozOrbisAddFrag );
  IGNORE_FUNC( ozOrbisGenFrags );

  IMPORT_FUNC( ozOrbisBindAllOverlaps );
  IMPORT_FUNC( ozOrbisBindStrOverlaps );
  IMPORT_FUNC( ozOrbisBindObjOverlaps );

  /*
   * Terra
   */

  IGNORE_FUNC( ozTerraLoad );

  IMPORT_FUNC( ozTerraHeight );

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

  IGNORE_FUNC( ozBotActionForward );
  IGNORE_FUNC( ozBotActionBackward );
  IGNORE_FUNC( ozBotActionRight );
  IGNORE_FUNC( ozBotActionLeft );
  IGNORE_FUNC( ozBotActionJump );
  IGNORE_FUNC( ozBotActionCrouch );
  IGNORE_FUNC( ozBotActionUse );
  IGNORE_FUNC( ozBotActionTake );
  IGNORE_FUNC( ozBotActionGrab );
  IGNORE_FUNC( ozBotActionThrow );
  IGNORE_FUNC( ozBotActionAttack );
  IGNORE_FUNC( ozBotActionExit );
  IGNORE_FUNC( ozBotActionEject );
  IGNORE_FUNC( ozBotActionSuicide );

  IMPORT_FUNC( ozBotIsRunning );
  IGNORE_FUNC( ozBotSetRunning );
  IGNORE_FUNC( ozBotToggleRunning );

  IGNORE_FUNC( ozBotSetGesture );

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

  /*
   * Constants
   */

  IMPORT_CONST( "OZ_ORBIS_DIM",                   Orbis::DIM );

  IMPORT_CONST( "OZ_NORTH",                       NORTH );
  IMPORT_CONST( "OZ_WEST",                        WEST );
  IMPORT_CONST( "OZ_SOUTH",                       SOUTH );
  IMPORT_CONST( "OZ_EAST",                        EAST );

  IMPORT_CONST( "OZ_EVENT_CREATE",                Object::EVENT_CREATE );
  IMPORT_CONST( "OZ_EVENT_DESTROY",               Object::EVENT_DESTROY );
  IMPORT_CONST( "OZ_EVENT_USE",                   Object::EVENT_USE );
  IMPORT_CONST( "OZ_EVENT_DAMAGE",                Object::EVENT_DAMAGE );
  IMPORT_CONST( "OZ_EVENT_HIT",                   Object::EVENT_HIT );
  IMPORT_CONST( "OZ_EVENT_SPLASH",                Object::EVENT_SPLASH );
  IMPORT_CONST( "OZ_EVENT_FRICTING",              Object::EVENT_FRICTING );
  IMPORT_CONST( "OZ_EVENT_SHOT",                  Weapon::EVENT_SHOT );
  IMPORT_CONST( "OZ_EVENT_SHOT_EMPTY",            Weapon::EVENT_SHOT_EMPTY );
  IMPORT_CONST( "OZ_EVENT_HIT_HARD",              Bot::EVENT_HIT_HARD );
  IMPORT_CONST( "OZ_EVENT_LAND",                  Bot::EVENT_LAND );
  IMPORT_CONST( "OZ_EVENT_JUMP",                  Bot::EVENT_JUMP );
  IMPORT_CONST( "OZ_EVENT_FLIP",                  Bot::EVENT_FLIP );
  IMPORT_CONST( "OZ_EVENT_DEATH",                 Bot::EVENT_DEATH );
  IMPORT_CONST( "OZ_EVENT_ENGINE",                Vehicle::EVENT_ENGINE );
  IMPORT_CONST( "OZ_EVENT_NEXT_WEAPON",           Vehicle::EVENT_NEXT_WEAPON );
  IMPORT_CONST( "OZ_EVENT_SHOT0",                 Vehicle::EVENT_SHOT0 );
  IMPORT_CONST( "OZ_EVENT_SHOT1",                 Vehicle::EVENT_SHOT1 );
  IMPORT_CONST( "OZ_EVENT_SHOT2",                 Vehicle::EVENT_SHOT2 );
  IMPORT_CONST( "OZ_EVENT_SHOT3",                 Vehicle::EVENT_SHOT3 );
  IMPORT_CONST( "OZ_EVENT_SHOT_EMPTY",            Vehicle::EVENT_SHOT_EMPTY );

  IMPORT_CONST( "OZ_OBJECT_DYNAMIC_BIT",          Object::DYNAMIC_BIT );
  IMPORT_CONST( "OZ_OBJECT_WEAPON_BIT",           Object::WEAPON_BIT );
  IMPORT_CONST( "OZ_OBJECT_BOT_BIT",              Object::BOT_BIT );
  IMPORT_CONST( "OZ_OBJECT_VEHICLE_BIT",          Object::VEHICLE_BIT );
  IMPORT_CONST( "OZ_OBJECT_ITEM_BIT",             Object::ITEM_BIT );
  IMPORT_CONST( "OZ_OBJECT_BROWSABLE_BIT",        Object::BROWSABLE_BIT );
  IMPORT_CONST( "OZ_OBJECT_NO_DRAW_BIT",          Object::NO_DRAW_BIT );
  IMPORT_CONST( "OZ_OBJECT_WIDE_CULL_BIT",        Object::WIDE_CULL_BIT );

  IMPORT_CONST( "OZ_OBJECT_LUA_BIT",              Object::LUA_BIT );
  IMPORT_CONST( "OZ_OBJECT_DESTROY_FUNC_BIT",     Object::DESTROY_FUNC_BIT );
  IMPORT_CONST( "OZ_OBJECT_DAMAGE_FUNC_BIT",      Object::DAMAGE_FUNC_BIT );
  IMPORT_CONST( "OZ_OBJECT_HIT_FUNC_BIT",         Object::HIT_FUNC_BIT );
  IMPORT_CONST( "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );
  IMPORT_CONST( "OZ_OBJECT_USE_FUNC_BIT",         Object::USE_FUNC_BIT );
  IMPORT_CONST( "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );

  IMPORT_CONST( "OZ_OBJECT_IMAGO_BIT",            Object::IMAGO_BIT );
  IMPORT_CONST( "OZ_OBJECT_AUDIO_BIT",            Object::AUDIO_BIT );

  IMPORT_CONST( "OZ_OBJECT_DESTROYED_BIT",        Object::DESTROYED_BIT );

  IMPORT_CONST( "OZ_OBJECT_DISABLED_BIT",         Object::DISABLED_BIT );
  IMPORT_CONST( "OZ_OBJECT_FRICTING_BIT",         Object::FRICTING_BIT );
  IMPORT_CONST( "OZ_OBJECT_BELOW_BIT",            Object::BELOW_BIT );
  IMPORT_CONST( "OZ_OBJECT_ON_FLOOR_BIT",         Object::ON_FLOOR_BIT );
  IMPORT_CONST( "OZ_OBJECT_ON_SLICK_BIT",         Object::ON_SLICK_BIT );
  IMPORT_CONST( "OZ_OBJECT_IN_LIQUID_BIT",        Object::IN_LIQUID_BIT );
  IMPORT_CONST( "OZ_OBJECT_ON_LADDER_BIT",        Object::ON_LADDER_BIT );
  IMPORT_CONST( "OZ_OBJECT_SOLID_BIT",            Object::SOLID_BIT );
  IMPORT_CONST( "OZ_OBJECT_CYLINDER_BIT",         Object::CYLINDER_BIT );
  IMPORT_CONST( "OZ_OBJECT_CLIMBER_BIT",          Object::CLIMBER_BIT );
  IMPORT_CONST( "OZ_OBJECT_PUSHER_BIT",           Object::PUSHER_BIT );

  IMPORT_CONST( "OZ_BOT_DEAD_BIT",                Bot::DEAD_BIT );
  IMPORT_CONST( "OZ_BOT_MECHANICAL_BIT",          Bot::MECHANICAL_BIT );
  IMPORT_CONST( "OZ_BOT_INCARNATABLE_BIT",        Bot::INCARNATABLE_BIT );
  IMPORT_CONST( "OZ_BOT_PLAYER_BIT",              Bot::PLAYER_BIT );

  IMPORT_CONST( "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );
  IMPORT_CONST( "OZ_BOT_RUNNING_BIT",             Bot::RUNNING_BIT );
  IMPORT_CONST( "OZ_BOT_SHOOTING_BIT",            Bot::SHOOTING_BIT );
  IMPORT_CONST( "OZ_BOT_MOVING_BIT",              Bot::MOVING_BIT );
  IMPORT_CONST( "OZ_BOT_CARGO_BIT",               Bot::CARGO_BIT );
  IMPORT_CONST( "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );

  IMPORT_CONST( "OZ_BOT_GESTURE0_BIT",            Bot::GESTURE0_BIT );
  IMPORT_CONST( "OZ_BOT_GESTURE1_BIT",            Bot::GESTURE1_BIT );
  IMPORT_CONST( "OZ_BOT_GESTURE2_BIT",            Bot::GESTURE2_BIT );
  IMPORT_CONST( "OZ_BOT_GESTURE3_BIT",            Bot::GESTURE3_BIT );
  IMPORT_CONST( "OZ_BOT_GESTURE4_BIT",            Bot::GESTURE4_BIT );

  newtable();
  setglobal( "ozLocalData" );
  getglobal( "ozLocalData" );

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

  hard_assert( gettop() == 1 );

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
