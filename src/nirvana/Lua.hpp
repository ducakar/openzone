/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/common.hpp"

namespace oz
{

  class Module;

namespace nirvana
{

  class Lua
  {
    private:

      lua_State* l;

      Bot*            self;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;
      bool            isFirstEvent;

      Vector<Object*> objects;
      Vector<Struct*> structs;

      List<Object::Event>::CIterator event;

      bool readVariable( InputStream* istream );
      void writeVariable( OutputStream* stream );

    public:

      bool forceUpdate;

      Lua();

      void mindCall( const char* functionName, Bot* self );

      void registerMind( int botIndex );
      void unregisterMind( int botIndex );

      void read( InputStream* istream );
      void write( OutputStream* ostream );

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
      OZ_LUA_API( ozStrGetRotation );
      OZ_LUA_API( ozStrGetLife );

      OZ_LUA_API( ozStrVectorFromSelf );
      OZ_LUA_API( ozStrVectorFromSelfEye );
      OZ_LUA_API( ozStrDirectionFromSelf );
      OZ_LUA_API( ozStrDirectionFromSelfEye );
      OZ_LUA_API( ozStrDistanceFromSelf );
      OZ_LUA_API( ozStrDistanceFromSelfEye );
      OZ_LUA_API( ozStrHeadingFromSelf );
      OZ_LUA_API( ozStrPitchFromSelf );
      OZ_LUA_API( ozStrPitchFromSelfEye );

      OZ_LUA_API( ozStrBindAllOverlaps );
      OZ_LUA_API( ozStrBindStrOverlaps );
      OZ_LUA_API( ozStrBindObjOverlaps );

      /*
       * Object
       */

      OZ_LUA_API( ozEventBindNext );

      OZ_LUA_API( ozEventGet );

      OZ_LUA_API( ozObjBindIndex );
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
      OZ_LUA_API( ozObjGetOldFlags );
      OZ_LUA_API( ozObjGetClassName );
      OZ_LUA_API( ozObjGetLife );

      OZ_LUA_API( ozObjVectorFromSelf );
      OZ_LUA_API( ozObjVectorFromSelfEye );
      OZ_LUA_API( ozObjDirectionFromSelf );
      OZ_LUA_API( ozObjDirectionFromSelfEye );
      OZ_LUA_API( ozObjDistanceFromSelf );
      OZ_LUA_API( ozObjDistanceFromSelfEye );
      OZ_LUA_API( ozObjHeadingFromSelf );
      OZ_LUA_API( ozObjPitchFromSelf );
      OZ_LUA_API( ozObjPitchFromSelfEye );

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

      OZ_LUA_API( ozBotGetName );

      OZ_LUA_API( ozBotGetState );
      OZ_LUA_API( ozBotGetEyePos );
      OZ_LUA_API( ozBotGetH );
      OZ_LUA_API( ozBotGetV );
      OZ_LUA_API( ozBotGetDir );
      OZ_LUA_API( ozBotGetStamina );

      OZ_LUA_API( ozBotIsRunning );

      OZ_LUA_API( ozBotVectorFromSelfEye );
      OZ_LUA_API( ozBotDirectionFromSelfEye );
      OZ_LUA_API( ozBotDistanceFromSelfEye );
      OZ_LUA_API( ozBotHeadingFromSelfEye );
      OZ_LUA_API( ozBotPitchFromSelfEye );

      /*
       * Vehicle
       */

      /*
       * Mind's bot
       */

      OZ_LUA_API( ozSelfIsCut );

      OZ_LUA_API( ozSelfGetIndex );
      OZ_LUA_API( ozSelfGetPos );
      OZ_LUA_API( ozSelfGetDim );
      OZ_LUA_API( ozSelfGetFlags );
      OZ_LUA_API( ozSelfGetOldFlags );
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

      OZ_LUA_API( ozSelfBindEvents );
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
