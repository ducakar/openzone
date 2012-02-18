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

#include "matrix/Vehicle.hh"
#include "matrix/Library.hh"
#include "matrix/Physics.hh"
#include "matrix/Synapse.hh"
#include "matrix/Lua.hh"

#include "nirvana/Nirvana.hh"
#include "nirvana/Memo.hh"

#include "client/QuestList.hh"
#include "client/Camera.hh"
#include "client/Profile.hh"

#include "common/lua.hh"

namespace oz
{
namespace client
{

#include "matrix/luaapi.hh"
#include "nirvana/luaapi.hh"
#include "client/luaapi.hh"

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

void Lua::staticCall( const char* functionName )
{
  ms.obj      = null;
  ms.str      = null;
  ms.frag     = null;
  ms.objIndex = 0;
  ms.strIndex = 0;

  hard_assert( gettop() == 0 );

  getglobal( functionName );
  lua_pcall( l, 0, 0, 0 );

  if( gettop() != 0 ) {
    throw Exception( "Client Lua: %s(): %s", functionName, tostring( -1 ) );
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

  log.print( "Importing mission catalogue '%s' ...", cs.mission.cstr() );
  if( cs.missionLingua.initDomain( cs.mission ) ) {
    log.printEnd( " OK" );
  }
  else {
    log.printEnd( " Failed" );
  }

  log.println( "Executing mission script %s {", cs.mission.cstr() );
  log.indent();

  PhysFile missionFile( missionPath );
  if( !missionFile.map() ) {
    throw Exception( "Failed to read mission file '%s'", missionFile.path().cstr() );
  }

  InputStream istream = missionFile.inputStream();

  if( IMPORT_BUFFER( istream.begin(), istream.capacity(), missionPath ) != 0 ) {
    throw Exception( "Client Lua script error" );
  }

  missionFile.unmap();

  staticCall( "onCreate" );

  log.unindent();
  log.println( "}" );
}

void Lua::read( InputStream* istream )
{
  hard_assert( gettop() == 0 );

  cs.mission = istream->readString();
  String missionPath = "lua/mission/" + cs.mission + ".lua";

  log.print( "Importing mission catalogue '%s' ...", cs.mission.cstr() );
  if( cs.missionLingua.initDomain( cs.mission ) ) {
    log.printEnd( " OK" );
  }
  else {
    log.printEnd( " Failed" );
  }

  log.print( "Deserialising mission script %s ...", cs.mission.cstr() );

  PhysFile missionFile( missionPath );
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

    setglobal( name );

    ch = istream->readChar();
  }

  log.printEnd( " OK" );
}

void Lua::write( BufferStream* ostream )
{
  hard_assert( gettop() == 0 );

  ostream->writeString( cs.mission );

#if LUA_VERSION_NUM >= 502
  lua_pushglobaltable( l );
#endif

  pushnil();
#if LUA_VERSION_NUM >= 502
  while( next( -2 ) != 0 ) {
#else
  while( next( LUA_GLOBALSINDEX ) != 0 ) {
#endif
    hard_assert( type( -2 ) == LUA_TSTRING );

    const char* name = tostring( -2 );
    if( name[0] == 'o' && name[1] == 'z' && name[2] == '_' ) {
      ostream->writeChar( 's' );
      ostream->writeString( tostring( -2 ) );

      writeVariable( ostream );
    }

    pop( 1 );
  }

#if LUA_VERSION_NUM >= 502
  pop( 1 );
#endif

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
  log.print( "Initialising Client Lua ..." );

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
  IMPORT_FUNC( ozGettext );

  IGNORE_FUNC( ozUseFailed );
  IGNORE_FUNC( ozForceUpdate );

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

  IMPORT_FUNC( ozStrSetEntityLock );

  IGNORE_FUNC( ozStrVectorFromSelf );
  IGNORE_FUNC( ozStrVectorFromSelfEye );
  IGNORE_FUNC( ozStrDirectionFromSelf );
  IGNORE_FUNC( ozStrDirectionFromSelfEye );
  IGNORE_FUNC( ozStrDistanceFromSelf );
  IGNORE_FUNC( ozStrDistanceFromSelfEye );
  IGNORE_FUNC( ozStrHeadingFromSelf );
  IGNORE_FUNC( ozStrRelativeHeadingFromSelf );
  IGNORE_FUNC( ozStrPitchFromSelf );
  IGNORE_FUNC( ozStrPitchFromSelfEye );

  IMPORT_FUNC( ozStrBindAllOverlaps );
  IMPORT_FUNC( ozStrBindStrOverlaps );
  IMPORT_FUNC( ozStrBindObjOverlaps );

  /*
   * Object
   */

  IMPORT_FUNC( ozObjBindIndex );
  IMPORT_FUNC( ozObjBindPilot );
  IGNORE_FUNC( ozObjBindSelf );
  IGNORE_FUNC( ozObjBindUser );
  IMPORT_FUNC( ozObjBindNext );

  IMPORT_FUNC( ozObjIsNull );
  IGNORE_FUNC( ozObjIsSelf );
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

  IGNORE_FUNC( ozObjVectorFromSelf );
  IGNORE_FUNC( ozObjVectorFromSelfEye );
  IGNORE_FUNC( ozObjDirectionFromSelf );
  IGNORE_FUNC( ozObjDirectionFromSelfEye );
  IGNORE_FUNC( ozObjDistanceFromSelf );
  IGNORE_FUNC( ozObjDistanceFromSelfEye );
  IGNORE_FUNC( ozObjHeadingFromSelf );
  IGNORE_FUNC( ozObjRelativeHeadingFromSelf );
  IGNORE_FUNC( ozObjPitchFromSelf );
  IGNORE_FUNC( ozObjPitchFromSelfEye );
  IGNORE_FUNC( ozObjIsVisibleFromSelf );
  IGNORE_FUNC( ozObjIsVisibleFromSelfEye );

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

  IGNORE_FUNC( ozBotIsVisibleFromSelfEyeToEye );

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

  /*
   * Mind's bot
   */

  IGNORE_FUNC( ozSelfIsCut );

  IGNORE_FUNC( ozSelfGetIndex );
  IGNORE_FUNC( ozSelfGetPos );
  IGNORE_FUNC( ozSelfGetDim );
  IGNORE_FUNC( ozSelfGetFlags );
  IGNORE_FUNC( ozSelfGetTypeName );
  IGNORE_FUNC( ozSelfGetLife );

  IGNORE_FUNC( ozSelfGetVelocity );
  IGNORE_FUNC( ozSelfGetMomentum );
  IGNORE_FUNC( ozSelfGetMass );
  IGNORE_FUNC( ozSelfGetLift );

  IGNORE_FUNC( ozSelfGetName );

  IGNORE_FUNC( ozSelfGetState );
  IGNORE_FUNC( ozSelfGetEyePos );
  IGNORE_FUNC( ozSelfGetH );
  IGNORE_FUNC( ozSelfSetH );
  IGNORE_FUNC( ozSelfAddH );
  IGNORE_FUNC( ozSelfGetV );
  IGNORE_FUNC( ozSelfSetV );
  IGNORE_FUNC( ozSelfAddV );
  IGNORE_FUNC( ozSelfGetDir );
  IGNORE_FUNC( ozSelfGetStamina );

  IGNORE_FUNC( ozSelfActionForward );
  IGNORE_FUNC( ozSelfActionBackward );
  IGNORE_FUNC( ozSelfActionRight );
  IGNORE_FUNC( ozSelfActionLeft );
  IGNORE_FUNC( ozSelfActionJump );
  IGNORE_FUNC( ozSelfActionCrouch );
  IGNORE_FUNC( ozSelfActionUse );
  IGNORE_FUNC( ozSelfActionTake );
  IGNORE_FUNC( ozSelfActionGrab );
  IGNORE_FUNC( ozSelfActionThrow );
  IGNORE_FUNC( ozSelfActionAttack );
  IGNORE_FUNC( ozSelfActionExit );
  IGNORE_FUNC( ozSelfActionEject );
  IGNORE_FUNC( ozSelfActionSuicide );

  IGNORE_FUNC( ozSelfIsRunning );
  IGNORE_FUNC( ozSelfSetRunning );
  IGNORE_FUNC( ozSelfToggleRunning );

  IGNORE_FUNC( ozSelfSetGesture );

  IGNORE_FUNC( ozSelfBindItems );
  IGNORE_FUNC( ozSelfBindParent );

  IGNORE_FUNC( ozSelfBindAllOverlaps );
  IGNORE_FUNC( ozSelfBindStrOverlaps );
  IGNORE_FUNC( ozSelfBindObjOverlaps );

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

  IMPORT_FUNC( ozCameraGetPos );
  IMPORT_FUNC( ozCameraGetDest );
  IMPORT_FUNC( ozCameraGetH );
  IMPORT_FUNC( ozCameraSetH );
  IMPORT_FUNC( ozCameraGetV );
  IMPORT_FUNC( ozCameraSetV );
  IMPORT_FUNC( ozCameraMoveTo );
  IMPORT_FUNC( ozCameraWarpTo );

  IMPORT_FUNC( ozCameraAllowReincarnation );
  IMPORT_FUNC( ozCameraIncarnate );

  /*
   * Profile
   */

  IMPORT_FUNC( ozProfileGetPlayerName );

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

  hard_assert( gettop() == 0 );

  log.printEnd( " OK" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  log.print( "Freeing Client Lua ..." );

  ms.structs.clear();
  ms.structs.dealloc();

  ms.objects.clear();
  ms.objects.dealloc();

  cs.mission = "";
  cs.missionLingua.free();

  lua_close( l );
  l = null;

  log.printEnd( " OK" );
}

}
}
