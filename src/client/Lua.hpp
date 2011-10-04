/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"

namespace oz
{

  class Struct;
  class Bot;
  class Particle;
  class Module;

namespace client
{

  class Lua
  {
    private:

      lua_State*      l;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;
      bool            isFirstEvent;

      Vector<Struct*> structs;
      Vector<Object*> objects;

      List<Object::Event>::CIterator event;

    public:

      Lua();

      void staticCall( const char* functionName );

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

      /*
       * Orbis
       */

      OZ_LUA_API( ozOrbisAddStr );
      OZ_LUA_API( ozOrbisTryAddStr );
      OZ_LUA_API( ozOrbisAddObj );
      OZ_LUA_API( ozOrbisTryAddObj );
      OZ_LUA_API( ozOrbisAddPart );
      OZ_LUA_API( ozOrbisGenParts );

      OZ_LUA_API( ozOrbisBindAllOverlaps );
      OZ_LUA_API( ozOrbisBindStrOverlaps );
      OZ_LUA_API( ozOrbisBindObjOverlaps );

      /*
       * Terra
       */

      OZ_LUA_API( ozTerraLoad );

      OZ_LUA_API( ozTerraHeight );

      /*
       * Caelum
       */

      OZ_LUA_API( ozCaelumLoad );

      OZ_LUA_API( ozCaelumGetHeading );
      OZ_LUA_API( ozCaelumSetHeading );
      OZ_LUA_API( ozCaelumGetPeriod );
      OZ_LUA_API( ozCaelumSetPeriod );
      OZ_LUA_API( ozCaelumGetTime );
      OZ_LUA_API( ozCaelumSetTime );
      OZ_LUA_API( ozCaelumAddTime );

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
      OZ_LUA_API( ozStrGetRotation );
      OZ_LUA_API( ozStrGetLife );
      OZ_LUA_API( ozStrSetLife );
      OZ_LUA_API( ozStrAddLife );

      OZ_LUA_API( ozStrDamage );
      OZ_LUA_API( ozStrDestroy );
      OZ_LUA_API( ozStrRemove );

      OZ_LUA_API( ozStrBindAllOverlaps );
      OZ_LUA_API( ozStrBindStrOverlaps );
      OZ_LUA_API( ozStrBindObjOverlaps );

      /*
       * Object
       */

      OZ_LUA_API( ozEventBindNext );

      OZ_LUA_API( ozEventGet );

      OZ_LUA_API( ozObjBindIndex );
      OZ_LUA_API( ozObjBindNext );

      OZ_LUA_API( ozObjIsNull );
      OZ_LUA_API( ozObjIsCut );
      OZ_LUA_API( ozObjIsBrowsable );
      OZ_LUA_API( ozObjIsDynamic );
      OZ_LUA_API( ozObjIsItem );
      OZ_LUA_API( ozObjIsWeapon );
      OZ_LUA_API( ozObjIsBot );
      OZ_LUA_API( ozObjIsVehicle );

      OZ_LUA_API( ozObjGetIndex );
      OZ_LUA_API( ozObjGetPos );
      OZ_LUA_API( ozObjSetPos );
      OZ_LUA_API( ozObjAddPos );
      OZ_LUA_API( ozObjGetDim );
      OZ_LUA_API( ozObjGetFlags );
      OZ_LUA_API( ozObjGetOldFlags );
      OZ_LUA_API( ozObjGetClassName );
      OZ_LUA_API( ozObjGetLife );
      OZ_LUA_API( ozObjSetLife );
      OZ_LUA_API( ozObjAddLife );

      OZ_LUA_API( ozObjAddEvent );

      OZ_LUA_API( ozObjAddItem );
      OZ_LUA_API( ozObjRemoveItem );
      OZ_LUA_API( ozObjRemoveAllItems );

      OZ_LUA_API( ozObjEnableUpdate );
      OZ_LUA_API( ozObjDamage );
      OZ_LUA_API( ozObjDestroy );
      OZ_LUA_API( ozObjQuietDestroy );
      OZ_LUA_API( ozObjRemove );

      OZ_LUA_API( ozObjBindEvents );
      OZ_LUA_API( ozObjBindItems );

      OZ_LUA_API( ozObjBindAllOverlaps );
      OZ_LUA_API( ozObjBindStrOverlaps );
      OZ_LUA_API( ozObjBindObjOverlaps );

      /*
       * Dynamic object
       */

      OZ_LUA_API( ozDynBindParent );

      OZ_LUA_API( ozDynGetVelocity );
      OZ_LUA_API( ozDynSetVelocity );
      OZ_LUA_API( ozDynAddVelocity );
      OZ_LUA_API( ozDynGetMomentum );
      OZ_LUA_API( ozDynSetMomentum );
      OZ_LUA_API( ozDynAddMomentum );
      OZ_LUA_API( ozDynGetMass );
      OZ_LUA_API( ozDynGetLift );

      /*
       * Weapon
       */

      OZ_LUA_API( ozWeaponGetDefaultRounds );
      OZ_LUA_API( ozWeaponGetRounds );
      OZ_LUA_API( ozWeaponSetRounds );
      OZ_LUA_API( ozWeaponAddRounds );
      OZ_LUA_API( ozWeaponReload );

      /*
       * Bot
       */

      OZ_LUA_API( ozBotGetName );
      OZ_LUA_API( ozBotSetName );
      OZ_LUA_API( ozBotGetMindFunc );
      OZ_LUA_API( ozBotSetMindFunc );

      OZ_LUA_API( ozBotGetState );
      OZ_LUA_API( ozBotGetEyePos );
      OZ_LUA_API( ozBotGetH );
      OZ_LUA_API( ozBotSetH );
      OZ_LUA_API( ozBotAddH );
      OZ_LUA_API( ozBotGetV );
      OZ_LUA_API( ozBotSetV );
      OZ_LUA_API( ozBotAddV );
      OZ_LUA_API( ozBotGetDir );
      OZ_LUA_API( ozBotGetStamina );
      OZ_LUA_API( ozBotSetStamina );
      OZ_LUA_API( ozBotAddStamina );

      OZ_LUA_API( ozBotActionForward );
      OZ_LUA_API( ozBotActionBackward );
      OZ_LUA_API( ozBotActionRight );
      OZ_LUA_API( ozBotActionLeft );
      OZ_LUA_API( ozBotActionJump );
      OZ_LUA_API( ozBotActionCrouch );
      OZ_LUA_API( ozBotActionUse );
      OZ_LUA_API( ozBotActionTake );
      OZ_LUA_API( ozBotActionGrab );
      OZ_LUA_API( ozBotActionThrow );
      OZ_LUA_API( ozBotActionAttack );
      OZ_LUA_API( ozBotActionExit );
      OZ_LUA_API( ozBotActionEject );
      OZ_LUA_API( ozBotActionSuicide );

      OZ_LUA_API( ozBotIsRunning );
      OZ_LUA_API( ozBotSetRunning );
      OZ_LUA_API( ozBotToggleRunning );

      OZ_LUA_API( ozBotSetWeaponItem );

      OZ_LUA_API( ozBotHeal );
      OZ_LUA_API( ozBotRearm );
      OZ_LUA_API( ozBotKill );

      OZ_LUA_API( ozBotVectorFromSelfEye );
      OZ_LUA_API( ozBotDirectionFromSelfEye );
      OZ_LUA_API( ozBotDistanceFromSelfEye );
      OZ_LUA_API( ozBotHeadingFromSelfEye );
      OZ_LUA_API( ozBotPitchFromSelfEye );

      /*
       * Vehicle
       */

      OZ_LUA_API( ozVehicleService );

      /*
       * Particle
       */

      OZ_LUA_API( ozPartBindIndex );

      OZ_LUA_API( ozPartIsNull );

      OZ_LUA_API( ozPartGetPos );
      OZ_LUA_API( ozPartSetPos );
      OZ_LUA_API( ozPartAddPos );
      OZ_LUA_API( ozPartGetIndex );
      OZ_LUA_API( ozPartGetVelocity );
      OZ_LUA_API( ozPartSetVelocity );
      OZ_LUA_API( ozPartAddVelocity );
      OZ_LUA_API( ozPartGetLife );
      OZ_LUA_API( ozPartSetLife );
      OZ_LUA_API( ozPartAddLife );

      OZ_LUA_API( ozPartRemove );

      /*
       * Nirvana
       */

      OZ_LUA_API( ozNirvanaRemoveDevice );
      OZ_LUA_API( ozNirvanaAddMemo );

      /*
       * Camera
       */

      OZ_LUA_API( ozCameraGetPos );
      OZ_LUA_API( ozCameraGetDest );
      OZ_LUA_API( ozCameraMoveTo );
      OZ_LUA_API( ozCameraWarpTo );

      OZ_LUA_API( ozCameraAllowReincarnation );
      OZ_LUA_API( ozCameraIncarnate );

  };

  extern Lua lua;

}
}
