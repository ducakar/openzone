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
 * @file nirvana/Lua.hh
 *
 * Lua scripting engine for Nirvana
 */

#pragma once

#include "nirvana/common.hh"

namespace oz
{

class Module;

namespace nirvana
{

class Lua
{
  private:

    lua_State*      l;

    Bot*            self;

    Struct*         str;
    Object*         obj;

    int             strIndex;
    int             objIndex;

    Vector<Object*> objects;
    Vector<Struct*> structs;

    bool readVariable( InputStream* istream );
    void writeVariable( BufferStream* stream );

  public:

    bool forceUpdate;

    Lua();

    void mindCall( const char* functionName, Bot* self );

    void registerMind( int botIndex );
    void unregisterMind( int botIndex );

    void read( InputStream* istream );
    void write( BufferStream* ostream );

    void registerFunction( const char* name, LuaAPI func );
    void registerConstant( const char* name, bool value );
    void registerConstant( const char* name, int value );
    void registerConstant( const char* name, float value );
    void registerConstant( const char* name, const char* value );

    void init();
    void free();

  private:

    /*
     * General functions
     */

    OZ_LUA_API( ozPrintln );
    OZ_LUA_API( ozException );
    OZ_LUA_API( ozGettext );

    OZ_LUA_API( ozForceUpdate );

    /*
     * Orbis
     */

    OZ_LUA_API( ozBindAllOverlaps );
    OZ_LUA_API( ozBindStrOverlaps );
    OZ_LUA_API( ozBindObjOverlaps );

    /*
     * Terra
     */

    OZ_LUA_API( ozTerraHeight );

    /*
     * Caelum
     */

    OZ_LUA_API( ozCaelumGetHeading );
    OZ_LUA_API( ozCaelumGetPeriod );
    OZ_LUA_API( ozCaelumGetTime );

    /*
     * Structure
     */

    OZ_LUA_API( ozStrBindIndex );
    OZ_LUA_API( ozStrBindNext );

    OZ_LUA_API( ozStrIsNull );

    OZ_LUA_API( ozStrGetIndex );
    OZ_LUA_API( ozStrGetBounds );
    OZ_LUA_API( ozStrGetPos );
    OZ_LUA_API( ozStrGetBSP );
    OZ_LUA_API( ozStrGetHeading );
    OZ_LUA_API( ozStrGetLife );

    OZ_LUA_API( ozStrVectorFromSelf );
    OZ_LUA_API( ozStrVectorFromSelfEye );
    OZ_LUA_API( ozStrDirectionFromSelf );
    OZ_LUA_API( ozStrDirectionFromSelfEye );
    OZ_LUA_API( ozStrDistanceFromSelf );
    OZ_LUA_API( ozStrDistanceFromSelfEye );
    OZ_LUA_API( ozStrHeadingFromSelf );
    OZ_LUA_API( ozStrRelativeHeadingFromSelf );
    OZ_LUA_API( ozStrPitchFromSelf );
    OZ_LUA_API( ozStrPitchFromSelfEye );

    OZ_LUA_API( ozStrBindAllOverlaps );
    OZ_LUA_API( ozStrBindStrOverlaps );
    OZ_LUA_API( ozStrBindObjOverlaps );

    /*
     * Object
     */

    OZ_LUA_API( ozObjBindIndex );
    OZ_LUA_API( ozObjBindPilot );
    OZ_LUA_API( ozObjBindSelf );
    OZ_LUA_API( ozObjBindNext );

    OZ_LUA_API( ozObjIsNull );
    OZ_LUA_API( ozObjIsSelf );
    OZ_LUA_API( ozObjIsCut );
    OZ_LUA_API( ozObjIsBrowsable );
    OZ_LUA_API( ozObjIsDynamic );
    OZ_LUA_API( ozObjIsItem );
    OZ_LUA_API( ozObjIsWeapon );
    OZ_LUA_API( ozObjIsBot );
    OZ_LUA_API( ozObjIsVehicle );

    OZ_LUA_API( ozObjGetIndex );
    OZ_LUA_API( ozObjGetPos );
    OZ_LUA_API( ozObjGetDim );
    OZ_LUA_API( ozObjGetFlags );
    OZ_LUA_API( ozObjGetHeading );
    OZ_LUA_API( ozObjGetClassName );
    OZ_LUA_API( ozObjGetLife );

    OZ_LUA_API( ozObjBindItems );

    OZ_LUA_API( ozObjVectorFromSelf );
    OZ_LUA_API( ozObjVectorFromSelfEye );
    OZ_LUA_API( ozObjDirectionFromSelf );
    OZ_LUA_API( ozObjDirectionFromSelfEye );
    OZ_LUA_API( ozObjDistanceFromSelf );
    OZ_LUA_API( ozObjDistanceFromSelfEye );
    OZ_LUA_API( ozObjHeadingFromSelf );
    OZ_LUA_API( ozObjRelativeHeadingFromSelf );
    OZ_LUA_API( ozObjPitchFromSelf );
    OZ_LUA_API( ozObjPitchFromSelfEye );
    OZ_LUA_API( ozObjIsVisibleFromSelf );
    OZ_LUA_API( ozObjIsVisibleFromSelfEye );

    OZ_LUA_API( ozObjBindAllOverlaps );
    OZ_LUA_API( ozObjBindStrOverlaps );
    OZ_LUA_API( ozObjBindObjOverlaps );

    /*
     * Dynamic object
     */

    OZ_LUA_API( ozDynBindParent );

    OZ_LUA_API( ozDynGetVelocity );
    OZ_LUA_API( ozDynGetMomentum );
    OZ_LUA_API( ozDynGetMass );
    OZ_LUA_API( ozDynGetLift );

    /*
     * Weapon
     */

    OZ_LUA_API( ozWeaponGetDefaultRounds );
    OZ_LUA_API( ozWeaponGetRounds );

    /*
     * Bot
     */

    OZ_LUA_API( ozBotBindPilot );

    OZ_LUA_API( ozBotGetName );

    OZ_LUA_API( ozBotGetState );
    OZ_LUA_API( ozBotGetEyePos );
    OZ_LUA_API( ozBotGetH );
    OZ_LUA_API( ozBotGetV );
    OZ_LUA_API( ozBotGetDir );
    OZ_LUA_API( ozBotGetStamina );

    OZ_LUA_API( ozBotIsRunning );

    OZ_LUA_API( ozBotIsVisibleFromSelfEyeToEye );

    /*
     * Vehicle
     */

    OZ_LUA_API( ozVehicleGetH );
    OZ_LUA_API( ozVehicleGetV );
    OZ_LUA_API( ozVehicleGetDir );

    /*
     * Mind's bot
     */

    OZ_LUA_API( ozSelfIsCut );

    OZ_LUA_API( ozSelfGetIndex );
    OZ_LUA_API( ozSelfGetPos );
    OZ_LUA_API( ozSelfGetDim );
    OZ_LUA_API( ozSelfGetFlags );
    OZ_LUA_API( ozSelfGetTypeName );
    OZ_LUA_API( ozSelfGetLife );

    OZ_LUA_API( ozSelfGetVelocity );
    OZ_LUA_API( ozSelfGetMomentum );
    OZ_LUA_API( ozSelfGetMass );
    OZ_LUA_API( ozSelfGetLift );

    OZ_LUA_API( ozSelfGetName );

    OZ_LUA_API( ozSelfGetState );
    OZ_LUA_API( ozSelfGetEyePos );
    OZ_LUA_API( ozSelfGetH );
    OZ_LUA_API( ozSelfSetH );
    OZ_LUA_API( ozSelfAddH );
    OZ_LUA_API( ozSelfGetV );
    OZ_LUA_API( ozSelfSetV );
    OZ_LUA_API( ozSelfAddV );
    OZ_LUA_API( ozSelfGetDir );
    OZ_LUA_API( ozSelfGetStamina );

    OZ_LUA_API( ozSelfActionForward );
    OZ_LUA_API( ozSelfActionBackward );
    OZ_LUA_API( ozSelfActionRight );
    OZ_LUA_API( ozSelfActionLeft );
    OZ_LUA_API( ozSelfActionJump );
    OZ_LUA_API( ozSelfActionCrouch );
    OZ_LUA_API( ozSelfActionUse );
    OZ_LUA_API( ozSelfActionTake );
    OZ_LUA_API( ozSelfActionGrab );
    OZ_LUA_API( ozSelfActionThrow );
    OZ_LUA_API( ozSelfActionAttack );
    OZ_LUA_API( ozSelfActionExit );
    OZ_LUA_API( ozSelfActionEject );
    OZ_LUA_API( ozSelfActionSuicide );

    OZ_LUA_API( ozSelfIsRunning );
    OZ_LUA_API( ozSelfSetRunning );
    OZ_LUA_API( ozSelfToggleRunning );

    OZ_LUA_API( ozSelfSetGesture );

    OZ_LUA_API( ozSelfSetWeaponItem );

    OZ_LUA_API( ozSelfBindItems );
    OZ_LUA_API( ozSelfBindParent );

    OZ_LUA_API( ozSelfBindAllOverlaps );
    OZ_LUA_API( ozSelfBindStrOverlaps );
    OZ_LUA_API( ozSelfBindObjOverlaps );

    /*
     * Nirvana
     */

    OZ_LUA_API( ozNirvanaRemoveDevice );
    OZ_LUA_API( ozNirvanaAddMemo );

};

extern Lua lua;

}
}
