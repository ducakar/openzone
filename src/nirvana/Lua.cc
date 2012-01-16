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

#include "matrix/Vehicle.hh"
#include "matrix/Library.hh"

#include "nirvana/Nirvana.hh"
#include "nirvana/Memo.hh"

#include "luamacros.hh"

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

Lua::Lua() : l( null )
{}

void Lua::mindCall( const char* functionName, Bot* self_ )
{
  self        = self_;
  obj         = self_;
  str         = null;
  objIndex    = 0;
  strIndex    = 0;
  forceUpdate = false;

  hard_assert( gettop() == 1 && self != null );

  getglobal( functionName );
  rawgeti( 1, self->index );
  lua_pcall( l, 1, 0, 0 );

  if( gettop() != 1 ) {
    throw Exception( "Nirvana Lua: %s(self = %d): %s", functionName, self->index, tostring( -1 ) );
  }
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
  log.println( "Initialising Nirvana Lua {" );
  log.indent();

  l = luaL_newstate();
  if( l == null ) {
    throw Exception( "Failed to create Lua state" );
  }

  hard_assert( gettop() == 0 );

  OZ_LUA_LOADLIBS();

  if( gettop() != 0 ) {
    throw Exception( "Failed to initialise Lua libraries" );
  }

  /*
   * General functions
   */

  OZ_LUA_FUNC( ozPrintln );
  OZ_LUA_FUNC( ozException );

  OZ_LUA_FUNC( ozForceUpdate );

  /*
   * Orbis
   */

  OZ_LUA_FUNC( ozBindAllOverlaps );
  OZ_LUA_FUNC( ozBindStrOverlaps );
  OZ_LUA_FUNC( ozBindObjOverlaps );

  /*
   * Terra
   */

  OZ_LUA_FUNC( ozTerraHeight );

  /*
   * Caelum
   */

  OZ_LUA_FUNC( ozCaelumGetHeading );
  OZ_LUA_FUNC( ozCaelumGetPeriod );
  OZ_LUA_FUNC( ozCaelumGetTime );

  /*
   * Structure
   */

  OZ_LUA_FUNC( ozStrBindIndex );
  OZ_LUA_FUNC( ozStrBindNext );

  OZ_LUA_FUNC( ozStrIsNull );

  OZ_LUA_FUNC( ozStrGetIndex );
  OZ_LUA_FUNC( ozStrGetBounds );
  OZ_LUA_FUNC( ozStrGetPos );
  OZ_LUA_FUNC( ozStrGetBSP );
  OZ_LUA_FUNC( ozStrGetHeading );
  OZ_LUA_FUNC( ozStrGetLife );

  OZ_LUA_FUNC( ozStrVectorFromSelf );
  OZ_LUA_FUNC( ozStrVectorFromSelfEye );
  OZ_LUA_FUNC( ozStrDirectionFromSelf );
  OZ_LUA_FUNC( ozStrDirectionFromSelfEye );
  OZ_LUA_FUNC( ozStrDistanceFromSelf );
  OZ_LUA_FUNC( ozStrDistanceFromSelfEye );
  OZ_LUA_FUNC( ozStrHeadingFromSelf );
  OZ_LUA_FUNC( ozStrRelativeHeadingFromSelf );
  OZ_LUA_FUNC( ozStrPitchFromSelf );
  OZ_LUA_FUNC( ozStrPitchFromSelfEye );

  OZ_LUA_FUNC( ozStrBindAllOverlaps );
  OZ_LUA_FUNC( ozStrBindStrOverlaps );
  OZ_LUA_FUNC( ozStrBindObjOverlaps );

  /*
   * Object
   */

  OZ_LUA_FUNC( ozObjBindIndex );
  OZ_LUA_FUNC( ozObjBindPilot );
  OZ_LUA_FUNC( ozObjBindSelf );
  OZ_LUA_FUNC( ozObjBindNext );

  OZ_LUA_FUNC( ozObjIsNull );
  OZ_LUA_FUNC( ozObjIsSelf );
  OZ_LUA_FUNC( ozObjIsCut );
  OZ_LUA_FUNC( ozObjIsBrowsable );
  OZ_LUA_FUNC( ozObjIsDynamic );
  OZ_LUA_FUNC( ozObjIsItem );
  OZ_LUA_FUNC( ozObjIsWeapon );
  OZ_LUA_FUNC( ozObjIsBot );
  OZ_LUA_FUNC( ozObjIsVehicle );

  OZ_LUA_FUNC( ozObjGetIndex );
  OZ_LUA_FUNC( ozObjGetPos );
  OZ_LUA_FUNC( ozObjGetDim );
  OZ_LUA_FUNC( ozObjGetFlags );
  OZ_LUA_FUNC( ozObjGetHeading );
  OZ_LUA_FUNC( ozObjGetClassName );
  OZ_LUA_FUNC( ozObjGetLife );

  OZ_LUA_FUNC( ozObjBindItems );

  OZ_LUA_FUNC( ozObjVectorFromSelf );
  OZ_LUA_FUNC( ozObjVectorFromSelfEye );
  OZ_LUA_FUNC( ozObjDirectionFromSelf );
  OZ_LUA_FUNC( ozObjDirectionFromSelfEye );
  OZ_LUA_FUNC( ozObjDistanceFromSelf );
  OZ_LUA_FUNC( ozObjDistanceFromSelfEye );
  OZ_LUA_FUNC( ozObjHeadingFromSelf );
  OZ_LUA_FUNC( ozObjRelativeHeadingFromSelf );
  OZ_LUA_FUNC( ozObjPitchFromSelf );
  OZ_LUA_FUNC( ozObjPitchFromSelfEye );
  OZ_LUA_FUNC( ozObjIsVisibleFromSelf );
  OZ_LUA_FUNC( ozObjIsVisibleFromSelfEye );

  OZ_LUA_FUNC( ozObjBindAllOverlaps );
  OZ_LUA_FUNC( ozObjBindStrOverlaps );
  OZ_LUA_FUNC( ozObjBindObjOverlaps );

  /*
   * Dynamic object
   */

  OZ_LUA_FUNC( ozDynBindParent );

  OZ_LUA_FUNC( ozDynGetVelocity );
  OZ_LUA_FUNC( ozDynGetMomentum );
  OZ_LUA_FUNC( ozDynGetMass );
  OZ_LUA_FUNC( ozDynGetLift );

  /*
   * Weapon
   */

  OZ_LUA_FUNC( ozWeaponGetDefaultRounds );
  OZ_LUA_FUNC( ozWeaponGetRounds );

  /*
   * Bot
   */

  OZ_LUA_FUNC( ozBotBindPilot );

  OZ_LUA_FUNC( ozBotGetName );

  OZ_LUA_FUNC( ozBotGetState );
  OZ_LUA_FUNC( ozBotGetEyePos );
  OZ_LUA_FUNC( ozBotGetH );
  OZ_LUA_FUNC( ozBotGetV );
  OZ_LUA_FUNC( ozBotGetDir );
  OZ_LUA_FUNC( ozBotGetStamina );

  OZ_LUA_FUNC( ozBotIsRunning );

  OZ_LUA_FUNC( ozBotIsVisibleFromSelfEyeToEye );

  /*
   * Vehicle
   */

  OZ_LUA_FUNC( ozVehicleGetH );
  OZ_LUA_FUNC( ozVehicleGetV );
  OZ_LUA_FUNC( ozVehicleGetDir );

  /*
   * Mind's bot
   */

  OZ_LUA_FUNC( ozSelfIsCut );

  OZ_LUA_FUNC( ozSelfGetIndex );
  OZ_LUA_FUNC( ozSelfGetPos );
  OZ_LUA_FUNC( ozSelfGetDim );
  OZ_LUA_FUNC( ozSelfGetFlags );
  OZ_LUA_FUNC( ozSelfGetTypeName );
  OZ_LUA_FUNC( ozSelfGetLife );

  OZ_LUA_FUNC( ozSelfGetVelocity );
  OZ_LUA_FUNC( ozSelfGetMomentum );
  OZ_LUA_FUNC( ozSelfGetMass );
  OZ_LUA_FUNC( ozSelfGetLift );

  OZ_LUA_FUNC( ozSelfGetName );

  OZ_LUA_FUNC( ozSelfGetState );
  OZ_LUA_FUNC( ozSelfGetEyePos );
  OZ_LUA_FUNC( ozSelfGetH );
  OZ_LUA_FUNC( ozSelfSetH );
  OZ_LUA_FUNC( ozSelfAddH );
  OZ_LUA_FUNC( ozSelfGetV );
  OZ_LUA_FUNC( ozSelfSetV );
  OZ_LUA_FUNC( ozSelfAddV );
  OZ_LUA_FUNC( ozSelfGetDir );
  OZ_LUA_FUNC( ozSelfGetStamina );

  OZ_LUA_FUNC( ozSelfActionForward );
  OZ_LUA_FUNC( ozSelfActionBackward );
  OZ_LUA_FUNC( ozSelfActionRight );
  OZ_LUA_FUNC( ozSelfActionLeft );
  OZ_LUA_FUNC( ozSelfActionJump );
  OZ_LUA_FUNC( ozSelfActionCrouch );
  OZ_LUA_FUNC( ozSelfActionUse );
  OZ_LUA_FUNC( ozSelfActionTake );
  OZ_LUA_FUNC( ozSelfActionGrab );
  OZ_LUA_FUNC( ozSelfActionThrow );
  OZ_LUA_FUNC( ozSelfActionAttack );
  OZ_LUA_FUNC( ozSelfActionExit );
  OZ_LUA_FUNC( ozSelfActionEject );
  OZ_LUA_FUNC( ozSelfActionSuicide );

  OZ_LUA_FUNC( ozSelfIsRunning );
  OZ_LUA_FUNC( ozSelfSetRunning );
  OZ_LUA_FUNC( ozSelfToggleRunning );

  OZ_LUA_FUNC( ozSelfSetGesture );

  OZ_LUA_FUNC( ozSelfBindItems );
  OZ_LUA_FUNC( ozSelfBindParent );

  OZ_LUA_FUNC( ozSelfBindAllOverlaps );
  OZ_LUA_FUNC( ozSelfBindStrOverlaps );
  OZ_LUA_FUNC( ozSelfBindObjOverlaps );

  /*
   * Nirvana
   */

  OZ_LUA_FUNC( ozNirvanaRemoveDevice );
  OZ_LUA_FUNC( ozNirvanaAddMemo );

  /*
   * Constants
   */

  OZ_LUA_CONST( "OZ_ORBIS_DIM",                   Orbis::DIM );

  OZ_LUA_CONST( "OZ_NORTH",                       NORTH );
  OZ_LUA_CONST( "OZ_WEST",                        WEST );
  OZ_LUA_CONST( "OZ_SOUTH",                       SOUTH );
  OZ_LUA_CONST( "OZ_EAST",                        EAST );

  OZ_LUA_CONST( "OZ_EVENT_CREATE",                Object::EVENT_CREATE );
  OZ_LUA_CONST( "OZ_EVENT_DESTROY",               Object::EVENT_DESTROY );
  OZ_LUA_CONST( "OZ_EVENT_USE",                   Object::EVENT_USE );
  OZ_LUA_CONST( "OZ_EVENT_DAMAGE",                Object::EVENT_DAMAGE );
  OZ_LUA_CONST( "OZ_EVENT_HIT",                   Object::EVENT_HIT );
  OZ_LUA_CONST( "OZ_EVENT_SPLASH",                Object::EVENT_SPLASH );
  OZ_LUA_CONST( "OZ_EVENT_FRICTING",              Object::EVENT_FRICTING );
  OZ_LUA_CONST( "OZ_EVENT_SHOT",                  Weapon::EVENT_SHOT );
  OZ_LUA_CONST( "OZ_EVENT_SHOT_EMPTY",            Weapon::EVENT_SHOT_EMPTY );
  OZ_LUA_CONST( "OZ_EVENT_HIT_HARD",              Bot::EVENT_HIT_HARD );
  OZ_LUA_CONST( "OZ_EVENT_LAND",                  Bot::EVENT_LAND );
  OZ_LUA_CONST( "OZ_EVENT_JUMP",                  Bot::EVENT_JUMP );
  OZ_LUA_CONST( "OZ_EVENT_FLIP",                  Bot::EVENT_FLIP );
  OZ_LUA_CONST( "OZ_EVENT_DEATH",                 Bot::EVENT_DEATH );
  OZ_LUA_CONST( "OZ_EVENT_ENGINE",                Vehicle::EVENT_ENGINE );
  OZ_LUA_CONST( "OZ_EVENT_NEXT_WEAPON",           Vehicle::EVENT_NEXT_WEAPON );
  OZ_LUA_CONST( "OZ_EVENT_SHOT0",                 Vehicle::EVENT_SHOT0 );
  OZ_LUA_CONST( "OZ_EVENT_SHOT1",                 Vehicle::EVENT_SHOT1 );
  OZ_LUA_CONST( "OZ_EVENT_SHOT2",                 Vehicle::EVENT_SHOT2 );
  OZ_LUA_CONST( "OZ_EVENT_SHOT3",                 Vehicle::EVENT_SHOT3 );
  OZ_LUA_CONST( "OZ_EVENT_SHOT_EMPTY",            Vehicle::EVENT_SHOT_EMPTY );

  OZ_LUA_CONST( "OZ_OBJECT_DYNAMIC_BIT",          Object::DYNAMIC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_WEAPON_BIT",           Object::WEAPON_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_BOT_BIT",              Object::BOT_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_VEHICLE_BIT",          Object::VEHICLE_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_ITEM_BIT",             Object::ITEM_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_BROWSABLE_BIT",        Object::BROWSABLE_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_NO_DRAW_BIT",          Object::NO_DRAW_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_WIDE_CULL_BIT",        Object::WIDE_CULL_BIT );

  OZ_LUA_CONST( "OZ_OBJECT_LUA_BIT",              Object::LUA_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_DESTROY_FUNC_BIT",     Object::DESTROY_FUNC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_DAMAGE_FUNC_BIT",      Object::DAMAGE_FUNC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_HIT_FUNC_BIT",         Object::HIT_FUNC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_USE_FUNC_BIT",         Object::USE_FUNC_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_UPDATE_FUNC_BIT",      Object::UPDATE_FUNC_BIT );

  OZ_LUA_CONST( "OZ_OBJECT_DEVICE_BIT",           Object::DEVICE_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_IMAGO_BIT",            Object::IMAGO_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_AUDIO_BIT",            Object::AUDIO_BIT );

  OZ_LUA_CONST( "OZ_OBJECT_DESTROYED_BIT",        Object::DESTROYED_BIT );

  OZ_LUA_CONST( "OZ_OBJECT_DISABLED_BIT",         Object::DISABLED_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_HIT_BIT",              Object::HIT_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_FRICTING_BIT",         Object::FRICTING_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_BELOW_BIT",            Object::BELOW_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_ON_FLOOR_BIT",         Object::ON_FLOOR_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_ON_SLICK_BIT",         Object::ON_SLICK_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_IN_WATER_BIT",         Object::IN_WATER_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_ON_LADDER_BIT",        Object::ON_LADDER_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_SOLID_BIT",            Object::SOLID_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_CYLINDER_BIT",         Object::CYLINDER_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_CLIMBER_BIT",          Object::CLIMBER_BIT );
  OZ_LUA_CONST( "OZ_OBJECT_PUSHER_BIT",           Object::PUSHER_BIT );

  OZ_LUA_CONST( "OZ_BOT_DEAD_BIT",                Bot::DEAD_BIT );
  OZ_LUA_CONST( "OZ_BOT_MECHANICAL_BIT",          Bot::MECHANICAL_BIT );
  OZ_LUA_CONST( "OZ_BOT_INCARNATABLE_BIT",        Bot::INCARNATABLE_BIT );
  OZ_LUA_CONST( "OZ_BOT_PLAYER_BIT",              Bot::PLAYER_BIT );

  OZ_LUA_CONST( "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );
  OZ_LUA_CONST( "OZ_BOT_RUNNING_BIT",             Bot::RUNNING_BIT );
  OZ_LUA_CONST( "OZ_BOT_SHOOTING_BIT",            Bot::SHOOTING_BIT );
  OZ_LUA_CONST( "OZ_BOT_MOVING_BIT",              Bot::MOVING_BIT );
  OZ_LUA_CONST( "OZ_BOT_CARGO_BIT",               Bot::CARGO_BIT );
  OZ_LUA_CONST( "OZ_BOT_CROUCHING_BIT",           Bot::CROUCHING_BIT );

  OZ_LUA_CONST( "OZ_BOT_GESTURE0_BIT",            Bot::GESTURE0_BIT );
  OZ_LUA_CONST( "OZ_BOT_GESTURE1_BIT",            Bot::GESTURE1_BIT );
  OZ_LUA_CONST( "OZ_BOT_GESTURE2_BIT",            Bot::GESTURE2_BIT );
  OZ_LUA_CONST( "OZ_BOT_GESTURE3_BIT",            Bot::GESTURE3_BIT );
  OZ_LUA_CONST( "OZ_BOT_GESTURE4_BIT",            Bot::GESTURE4_BIT );

  newtable();
  setglobal( "ozLocalData" );
  getglobal( "ozLocalData" );

  PhysFile luaDir( "lua/nirvana" );
  DArray<PhysFile> luaFiles = luaDir.ls();

  foreach( file, luaFiles.iter() ) {
    if( file->hasExtension( "lua" ) ) {
      log.print( "%s ...", file->name().cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to read Lua script '%s'", file->path().cstr() );
      }

      InputStream istream = file->inputStream();

      if( OZ_LUA_DOBUFFER( istream.begin(), istream.capacity(), file->path() ) != 0 ) {
        throw Exception( "Nirvana Lua script error" );
      }

      file->unmap();

      log.printEnd( " OK" );
    }
  }

  hard_assert( gettop() == 1 );

  log.unindent();
  log.println( "}" );
}

void Lua::free()
{
  if( l == null ) {
    return;
  }

  log.print( "Freeing Nirvana Lua ..." );

  objects.clear();
  objects.dealloc();
  structs.clear();
  structs.dealloc();

  lua_close( l );
  l = null;

  log.printEnd( " OK" );
}

/*
 * General functions
 */

int Lua::ozPrintln( lua_State* l )
{
  ARG( 1 );

  log.println( "N> %s", tostring( 1 ) );
  return 0;
}

int Lua::ozException( lua_State* l )
{
  ARG( 1 );

  const char* message = tostring( 1 );
  throw Exception( message );
}

int Lua::ozForceUpdate( lua_State* l )
{
  ARG( 0 );

  lua.forceUpdate = true;
  return 0;
}

/*
 * Orbis
 */

int Lua::ozBindAllOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  lua.objects.clear();
  lua.structs.clear();
  collider.getOverlaps( aabb, &lua.objects, &lua.structs );
  lua.objIndex = 0;
  lua.strIndex = 0;
  return 0;
}

int Lua::ozBindStrOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  lua.structs.clear();
  collider.getOverlaps( aabb, null, &lua.structs );
  lua.strIndex = 0;
  return 0;
}

int Lua::ozBindObjOverlaps( lua_State* l )
{
  ARG( 6 );

  AABB aabb = AABB( Point3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ),
                    Vec3( tofloat( 4 ), tofloat( 5 ), tofloat( 6 ) ) );

  lua.objects.clear();
  collider.getOverlaps( aabb, &lua.objects, null );
  lua.objIndex = 0;
  return 0;
}

/*
 * Terra
 */

int Lua::ozTerraHeight( lua_State* l )
{
  ARG( 2 );

  float x = tofloat( 1 );
  float y = tofloat( 2 );

  pushfloat( orbis.terra.height( x, y ) );
  return 1;
}

/*
 * Caelum
 */

int Lua::ozCaelumGetHeading( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.heading );
  return 1;
}

int Lua::ozCaelumGetPeriod( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.period );
  return 1;
}

int Lua::ozCaelumGetTime( lua_State* l )
{
  ARG( 0 );

  pushfloat( orbis.caelum.time );
  return 1;
}

/*
 * Structure
 */

int Lua::ozStrBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.structs.length() ) ) {
    ERROR( "invalid structure index" );
  }
  lua.str = orbis.structs[index];
  return 0;
}

int Lua::ozStrBindNext( lua_State* l )
{
  ARG( 0 );

  if( lua.strIndex < lua.structs.length() ) {
    lua.str = lua.structs[lua.strIndex];
    ++lua.strIndex;
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

int Lua::ozStrIsNull( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.str == null );
  return 1;
}

int Lua::ozStrGetIndex( lua_State* l )
{
  ARG( 0 );

  if( lua.str == null ) {
    pushint( -1 );
  }
  else {
    pushint( lua.str->index );
  }
  return 1;
}

int Lua::ozStrGetBounds( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( lua.str->mins.x );
  pushfloat( lua.str->mins.y );
  pushfloat( lua.str->mins.z );
  pushfloat( lua.str->maxs.x );
  pushfloat( lua.str->maxs.y );
  pushfloat( lua.str->maxs.z );
  return 6;
}

int Lua::ozStrGetPos( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( lua.str->p.x );
  pushfloat( lua.str->p.y );
  pushfloat( lua.str->p.z );
  return 3;
}

int Lua::ozStrGetBSP( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushstring( lua.str->bsp->name );
  return 1;
}

int Lua::ozStrGetHeading( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushint( lua.str->heading );
  return 1;
}

int Lua::ozStrGetLife( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( lua.str->life );
  return 1;
}

int Lua::ozStrVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  Vec3 vec = lua.str->p - lua.self->p;
  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

int Lua::ozStrVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  Vec3 vec = lua.str->p - eye;
  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

int Lua::ozStrDirectionFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  Vec3 dir = ~( lua.str->p - lua.self->p );
  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

int Lua::ozStrDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  Vec3 dir = ~( lua.str->p - eye );
  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

int Lua::ozStrDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  pushfloat( !( lua.str->p - lua.self->p ) );
  return 1;
}

int Lua::ozStrDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  pushfloat( !( lua.str->p - eye ) );
  return 1;
}

int Lua::ozStrHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  float dx = lua.str->p.x - lua.self->p.x;
  float dy = lua.str->p.y - lua.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozStrRelativeHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  float dx = lua.str->p.x - lua.self->p.x;
  float dy = lua.str->p.y - lua.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) - lua.self->h ) + 720.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozStrPitchFromSelf( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();

  float dx = lua.str->p.x - lua.self->p.x;
  float dy = lua.str->p.y - lua.self->p.y;
  float dz = lua.str->p.z - lua.self->p.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozStrPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  STR_NOT_NULL();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  float dx = lua.str->p.x - eye.x;
  float dy = lua.str->p.y - eye.y;
  float dz = lua.str->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozStrBindAllOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( lua.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  lua.structs.clear();
  collider.getOverlaps( aabb, &lua.objects, &lua.structs );
  lua.objIndex = 0;
  lua.strIndex = 0;
  return 0;
}

int Lua::ozStrBindStrOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( lua.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.structs.clear();
  collider.getOverlaps( aabb, null, &lua.structs );
  lua.strIndex = 0;
  return 0;
}

int Lua::ozStrBindObjOverlaps( lua_State* l )
{
  ARG( 3 );
  STR_NOT_NULL();

  AABB aabb = AABB( lua.str->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  collider.getOverlaps( aabb, &lua.objects, null );
  lua.objIndex = 0;
  return 0;
}

/*
 * Object
 */

int Lua::ozObjBindIndex( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }
  lua.obj = orbis.objects[index];
  return 0;
}

int Lua::ozObjBindPilot( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  lua.obj = vehicle->pilot == -1 ? null : orbis.objects[vehicle->pilot];
  return 0;
}

int Lua::ozObjBindSelf( lua_State* l )
{
  ARG( 0 );

  lua.obj = lua.self;
  return 0;
}

int Lua::ozObjBindNext( lua_State* l )
{
  ARG( 0 );

  if( lua.objIndex < lua.objects.length() ) {
    lua.obj = lua.objects[lua.objIndex];
    ++lua.objIndex;
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

int Lua::ozObjIsNull( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj == null );
  return 1;
}

int Lua::ozObjIsSelf( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj == lua.self );
  return 1;
}

int Lua::ozObjIsCut( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && lua.obj->cell == null );
  return 1;
}

int Lua::ozObjIsBrowsable( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && ( lua.obj->flags & Object::BROWSABLE_BIT ) );
  return 1;
}

int Lua::ozObjIsDynamic( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && ( lua.obj->flags & Object::DYNAMIC_BIT ) );
  return 1;
}

int Lua::ozObjIsItem( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && ( lua.obj->flags & Object::ITEM_BIT ) );
  return 1;
}

int Lua::ozObjIsWeapon( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && ( lua.obj->flags & Object::WEAPON_BIT ) );
  return 1;
}

int Lua::ozObjIsBot( lua_State* l )
{
  ARG( 0 );

  const Bot* bot = static_cast<const Bot*>( lua.obj );
  pushbool( bot != null && ( bot->flags & Object::BOT_BIT ) && !( bot->state & Bot::DEAD_BIT ) );
  return 1;
}

int Lua::ozObjIsVehicle( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.obj != null && ( lua.obj->flags & Object::VEHICLE_BIT ) );
  return 1;
}

int Lua::ozObjGetIndex( lua_State* l )
{
  ARG( 0 );

  if( lua.obj == null ) {
    pushint( -1 );
  }
  else {
    pushint( lua.obj->index );
  }
  return 1;
}

int Lua::ozObjGetPos( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  if( lua.obj->cell == null ) {
    hard_assert( lua.obj->flags & Object::DYNAMIC_BIT );

    const Dynamic* dyn = static_cast<const Dynamic*>( lua.obj );

    if( dyn->parent != -1 ) {
      Object* parent = orbis.objects[dyn->parent];

      if( parent != null ) {
        pushfloat( parent->p.x );
        pushfloat( parent->p.y );
        pushfloat( parent->p.z );
        return 3;
      }
    }
  }

  pushfloat( lua.obj->p.x );
  pushfloat( lua.obj->p.y );
  pushfloat( lua.obj->p.z );
  return 3;
}

int Lua::ozObjGetDim( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushfloat( lua.obj->dim.x );
  pushfloat( lua.obj->dim.y );
  pushfloat( lua.obj->dim.z );
  return 3;
}

int Lua::ozObjGetFlags( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  int mask = toint( 1 );
  pushbool( ( lua.obj->flags & mask ) != 0 );
  return 1;
}

int Lua::ozObjGetHeading( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushint( lua.obj->flags & Object::HEADING_MASK );
  return 1;
}

int Lua::ozObjGetClassName( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushstring( lua.obj->clazz->name );
  return 1;
}

int Lua::ozObjGetLife( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  pushfloat( lua.obj->life );
  return 1;
}

int Lua::ozObjBindItems( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();

  lua.objects.clear();
  foreach( item, lua.obj->items.citer() ) {
    lua.objects.add( orbis.objects[*item] );
  }
  lua.objIndex = 0;
  return 0;
}

int Lua::ozObjVectorFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Vec3 vec = lua.obj->p - lua.self->p;

  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

int Lua::ozObjVectorFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vec = lua.obj->p - eye;

  pushfloat( vec.x );
  pushfloat( vec.y );
  pushfloat( vec.z );
  return 3;
}

int Lua::ozObjDirectionFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Vec3 dir = ~( lua.obj->p - lua.self->p );

  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

int Lua::ozObjDirectionFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   dir = ~( lua.obj->p - eye );

  pushfloat( dir.x );
  pushfloat( dir.y );
  pushfloat( dir.z );
  return 3;
}

int Lua::ozObjDistanceFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  pushfloat( !( lua.obj->p - lua.self->p ) );
  return 1;
}

int Lua::ozObjDistanceFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  pushfloat( !( lua.obj->p - eye ) );
  return 1;
}

int Lua::ozObjHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  float dx = lua.obj->p.x - lua.self->p.x;
  float dy = lua.obj->p.y - lua.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) ) + 360.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozObjRelativeHeadingFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  float dx = lua.obj->p.x - lua.self->p.x;
  float dy = lua.obj->p.y - lua.self->p.y;
  float angle = Math::fmod( Math::deg( Math::atan2( -dx, dy ) - lua.self->h ) + 720.0f, 360.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozObjPitchFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  float dx = lua.obj->p.x - lua.self->p.x;
  float dy = lua.obj->p.y - lua.self->p.y;
  float dz = lua.obj->p.z - lua.self->p.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozObjPitchFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye = Point3( self->p.x, self->p.y, self->p.z + self->camZ );

  float dx = lua.obj->p.x - eye.x;
  float dy = lua.obj->p.y - eye.y;
  float dz = lua.obj->p.z - eye.z;
  float angle = Math::deg( Math::atan2( dz, Math::sqrt( dx*dx + dy*dy ) ) + Math::TAU / 4.0f );

  pushfloat( angle );
  return 1;
}

int Lua::ozObjIsVisibleFromSelf( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();

  Point3 eye    = Point3( lua.self->p.x, lua.self->p.y, lua.self->p.z );
  Vec3   vector = lua.obj->p - eye;

  collider.translate( eye, vector, lua.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

int Lua::ozObjIsVisibleFromSelfEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  SELF_BOT();

  Point3 eye    = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vector = lua.obj->p - eye;

  collider.translate( eye, vector, lua.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

int Lua::ozObjBindAllOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( lua.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  lua.structs.clear();
  collider.getOverlaps( aabb, &lua.objects, &lua.structs );
  lua.objIndex = 0;
  lua.strIndex = 0;
  return 0;
}

int Lua::ozObjBindStrOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( lua.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.structs.clear();
  collider.getOverlaps( aabb, null, &lua.structs );
  lua.strIndex = 0;
  return 0;
}

int Lua::ozObjBindObjOverlaps( lua_State* l )
{
  ARG( 3 );
  OBJ_NOT_NULL();

  AABB aabb = AABB( lua.obj->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  collider.getOverlaps( aabb, &lua.objects, null );
  lua.objIndex = 0;
  return 0;
}

/*
 * Dynamic object
 */

int Lua::ozDynBindParent( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  if( dyn->parent != -1 && orbis.objects[dyn->parent] != null ) {
    lua.obj = orbis.objects[dyn->parent];
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

int Lua::ozDynGetVelocity( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->velocity.x );
  pushfloat( dyn->velocity.y );
  pushfloat( dyn->velocity.z );
  return 3;
}

int Lua::ozDynGetMomentum( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->momentum.x );
  pushfloat( dyn->momentum.y );
  pushfloat( dyn->momentum.z );
  return 3;
}

int Lua::ozDynGetMass( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->mass );
  return 1;
}

int Lua::ozDynGetLift( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_DYNAMIC();

  pushfloat( dyn->lift );
  return 1;
}

/*
 * Weapon
 */

int Lua::ozWeaponGetDefaultRounds( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  const WeaponClass* weaponClazz = static_cast<const WeaponClass*>( weapon->clazz );

  pushint( weaponClazz->nRounds );
  return 1;
}

int Lua::ozWeaponGetRounds( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_WEAPON();

  pushint( weapon->nRounds );
  return 1;
}

/*
 * Bot
 */

int Lua::ozBotBindPilot( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  lua.obj = vehicle->pilot == -1 ? null : orbis.objects[vehicle->pilot];
  return 0;
}

int Lua::ozBotGetName( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushstring( bot->name );
  return 1;
}

int Lua::ozBotGetState( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  int mask = toint( 1 );
  pushbool( ( bot->state & mask ) != 0 );
  return 1;
}

int Lua::ozBotGetEyePos( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( bot->p.x );
  pushfloat( bot->p.y );
  pushfloat( bot->p.z + bot->camZ );
  return 3;
}

int Lua::ozBotGetH( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( Math::deg( bot->h ) );
  return 1;
}

int Lua::ozBotGetV( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( Math::deg( bot->v ) );
  return 1;
}

int Lua::ozBotGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( bot->h, &hvsc[0], &hvsc[1] );
  Math::sincos( bot->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  pushfloat( -hvsc[4] );
  pushfloat(  hvsc[5] );
  pushfloat( -hvsc[3] );

  return 3;
}

int Lua::ozBotGetStamina( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushfloat( bot->stamina );
  return 1;
}

int Lua::ozBotIsRunning( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_BOT();

  pushbool( bot->state & Bot::RUNNING_BIT );
  return 1;
}

int Lua::ozBotIsVisibleFromSelfEyeToEye( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_NOT_SELF();
  OBJ_BOT();
  SELF_BOT();

  Point3 eye    = Point3( self->p.x, self->p.y, self->p.z + self->camZ );
  Vec3   vector = Point3( bot->p.x, bot->p.y, bot->p.z + bot->camZ ) - eye;

  collider.translate( eye, vector, lua.obj );
  pushbool( collider.hit.ratio == 1.0f );
  return 1;
}

/*
 * Vehicle
 */

int Lua::ozVehicleGetH( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  pushfloat( Math::deg( vehicle->h ) );
  return 1;
}

int Lua::ozVehicleGetV( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  pushfloat( Math::deg( vehicle->v ) );
  return 1;
}

int Lua::ozVehicleGetDir( lua_State* l )
{
  ARG( 0 );
  OBJ_NOT_NULL();
  OBJ_VEHICLE();

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( vehicle->h, &hvsc[0], &hvsc[1] );
  Math::sincos( vehicle->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  pushfloat( -hvsc[4] );
  pushfloat(  hvsc[5] );
  pushfloat( -hvsc[3] );

  return 3;
}

/*
 * Mind's bot
 */

int Lua::ozSelfIsCut( lua_State* l )
{
  ARG( 0 );

  pushbool( lua.self->cell == null );
  return 1;
}

int Lua::ozSelfGetIndex( lua_State* l )
{
  ARG( 0 );

  pushint( lua.self->index );
  return 1;
}

int Lua::ozSelfGetPos( lua_State* l )
{
  ARG( 0 );

  if( lua.self->cell == null ) {
    if( lua.self->parent != -1 ) {
      Object* parent = orbis.objects[lua.self->parent];

      if( parent != null ) {
        pushfloat( parent->p.x );
        pushfloat( parent->p.y );
        pushfloat( parent->p.z );
        return 3;
      }
    }
  }

  pushfloat( lua.self->p.x );
  pushfloat( lua.self->p.y );
  pushfloat( lua.self->p.z );
  return 3;
}

int Lua::ozSelfGetDim( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->dim.x );
  pushfloat( lua.self->dim.y );
  pushfloat( lua.self->dim.z );
  return 3;
}

int Lua::ozSelfGetFlags( lua_State* l )
{
  ARG( 1 );

  int mask = toint( 1 );
  pushint( lua.self->flags & mask );
  return 1;
}

int Lua::ozSelfGetTypeName( lua_State* l )
{
  ARG( 0 );

  pushstring( lua.self->clazz->name );
  return 1;
}

int Lua::ozSelfGetLife( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->life );
  return 1;
}

int Lua::ozSelfGetVelocity( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->velocity.x );
  pushfloat( lua.self->velocity.y );
  pushfloat( lua.self->velocity.z );
  return 3;
}

int Lua::ozSelfGetMomentum( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->momentum.x );
  pushfloat( lua.self->momentum.y );
  pushfloat( lua.self->momentum.z );
  return 3;
}

int Lua::ozSelfGetMass( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->mass );
  return 1;
}

int Lua::ozSelfGetLift( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->lift );
  return 1;
}

int Lua::ozSelfGetName( lua_State* l )
{
  ARG( 0 );

  pushstring( lua.self->name );
  return 1;
}

int Lua::ozSelfGetState( lua_State* l )
{
  ARG( 1 );

  int mask = toint( 1 );
  pushbool( ( lua.self->state & mask ) != 0 );
  return 1;
}

int Lua::ozSelfGetEyePos( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->p.x );
  pushfloat( lua.self->p.y );
  pushfloat( lua.self->p.z + lua.self->camZ );
  return 3;
}

int Lua::ozSelfGetH( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( lua.self->h ) );
  return 1;
}

int Lua::ozSelfSetH( lua_State* l )
{
  ARG( 1 );

  lua.self->h = Math::rad( tofloat( 1 ) );
  lua.self->h = Math::fmod( lua.self->h + Math::TAU, Math::TAU );
  return 1;
}

int Lua::ozSelfAddH( lua_State* l )
{
  ARG( 1 );

  lua.self->h += Math::rad( tofloat( 1 ) );
  lua.self->h = Math::fmod( lua.self->h + Math::TAU, Math::TAU );
  return 1;
}

int Lua::ozSelfGetV( lua_State* l )
{
  ARG( 0 );

  pushfloat( Math::deg( lua.self->v ) );
  return 1;
}

int Lua::ozSelfSetV( lua_State* l )
{
  ARG( 1 );

  lua.self->v = Math::rad( tofloat( 1 ) );
  lua.self->v = Math::fmod( lua.self->v + Math::TAU, Math::TAU );
  return 1;
}

int Lua::ozSelfAddV( lua_State* l )
{
  ARG( 1 );

  lua.self->v += Math::rad( tofloat( 1 ) );
  lua.self->v = Math::fmod( lua.self->v + Math::TAU, Math::TAU );
  return 1;
}

int Lua::ozSelfGetDir( lua_State* l )
{
  ARG( 0 );

  // { hsine, hcosine, vsine, vcosine, vsine * hsine, vsine * hcosine }
  float hvsc[6];

  Math::sincos( lua.self->h, &hvsc[0], &hvsc[1] );
  Math::sincos( lua.self->v, &hvsc[2], &hvsc[3] );

  hvsc[4] = hvsc[2] * hvsc[0];
  hvsc[5] = hvsc[2] * hvsc[1];

  pushfloat( -hvsc[4] );
  pushfloat(  hvsc[5] );
  pushfloat( -hvsc[3] );

  return 3;
}

int Lua::ozSelfGetStamina( lua_State* l )
{
  ARG( 0 );

  pushfloat( lua.self->stamina );
  return 1;
}

int Lua::ozSelfActionForward( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_FORWARD;
  return 0;
}

int Lua::ozSelfActionBackward( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_BACKWARD;
  return 0;
}

int Lua::ozSelfActionRight( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_RIGHT;
  return 0;
}

int Lua::ozSelfActionLeft( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_LEFT;
  return 0;
}

int Lua::ozSelfActionJump( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_JUMP;
  return 0;
}

int Lua::ozSelfActionCrouch( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_CROUCH;
  return 0;
}

int Lua::ozSelfActionUse( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_USE;
  return 0;
}

int Lua::ozSelfActionTake( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_TAKE;
  return 0;
}

int Lua::ozSelfActionGrab( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_GRAB;
  return 0;
}

int Lua::ozSelfActionThrow( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_THROW;
  return 0;
}

int Lua::ozSelfActionAttack( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_ATTACK;
  return 0;
}

int Lua::ozSelfActionExit( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_EXIT;
  return 0;
}

int Lua::ozSelfActionEject( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_EJECT;
  return 0;
}

int Lua::ozSelfActionSuicide( lua_State* l )
{
  ARG( 0 );

  lua.self->actions |= Bot::ACTION_SUICIDE;
  return 0;
}

int Lua::ozSelfIsRunning( lua_State* l )
{
  ARG( 0 );

  lua_pushboolean( l, lua.self->state & Bot::RUNNING_BIT );
  return 1;
}

int Lua::ozSelfSetRunning( lua_State* l )
{
  ARG( 1 );

  if( lua_toboolean( l, 1 ) ) {
    lua.self->state |= Bot::RUNNING_BIT;
  }
  else {
    lua.self->state &= ~Bot::RUNNING_BIT;
  }
  return 0;
}

int Lua::ozSelfToggleRunning( lua_State* l )
{
  ARG( 0 );

  lua.self->state ^= Bot::RUNNING_BIT;
  return 0;
}

int Lua::ozSelfSetGesture( lua_State* l )
{
  ARG( 1 );
  OBJ_NOT_NULL();

  lua.self->state &= ~( Bot::GESTURE0_BIT | Bot::GESTURE1_BIT | Bot::GESTURE2_BIT | Bot::GESTURE4_BIT );
  lua.self->state |= toint( 1 );
  return 0;
}

int Lua::ozSelfBindItems( lua_State* l )
{
  ARG( 0 );

  lua.objects.clear();
  foreach( item, lua.self->items.citer() ) {
    lua.objects.add( orbis.objects[*item] );
  }
  lua.objIndex = 0;
  return 0;
}

int Lua::ozSelfBindAllOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( lua.self->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  lua.structs.clear();
  collider.getOverlaps( aabb, &lua.objects, &lua.structs );
  lua.objIndex = 0;
  lua.strIndex = 0;
  return 0;
}

int Lua::ozSelfBindStrOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( lua.self->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.structs.clear();
  collider.getOverlaps( aabb, null, &lua.structs );
  lua.strIndex = 0;
  return 0;
}

int Lua::ozSelfBindObjOverlaps( lua_State* l )
{
  ARG( 3 );

  AABB aabb = AABB( lua.self->p,
                    Vec3( tofloat( 1 ), tofloat( 2 ), tofloat( 3 ) ) );

  lua.objects.clear();
  collider.getOverlaps( aabb, &lua.objects, null );
  lua.objIndex = 0;
  return 0;
}

int Lua::ozSelfBindParent( lua_State* l )
{
  ARG( 0 );

  if( lua.self->parent != -1 && orbis.objects[lua.self->parent] != null ) {
    lua.obj = orbis.objects[lua.self->parent];
    pushbool( true );
  }
  else {
    pushbool( false );
  }
  return 1;
}

/*
 * Nirvana
 */

int Lua::ozNirvanaRemoveDevice( lua_State* l )
{
  ARG( 1 );

  int index = toint( 1 );
  const Device* const* device = nirvana.devices.find( index );

  if( device == null ) {
    pushbool( false );
  }
  else {
    delete *device;
    nirvana.devices.exclude( index );
    pushbool( true );
  }
  return 1;
}

int Lua::ozNirvanaAddMemo( lua_State* l )
{
  ARG( 2 );

  int index = toint( 1 );
  if( uint( index ) >= uint( orbis.objects.length() ) ) {
    ERROR( "invalid object index" );
  }
  if( orbis.objects[index] == null ) {
    ERROR( "object is null" );
  }

  if( nirvana.devices.contains( index ) ) {
    ERROR( "object already has a device" );
  }

  nirvana.devices.add( index, new Memo( tostring( 2 ) ) );
  return 0;
}

}
}
