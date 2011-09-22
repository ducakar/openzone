/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Matrix
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "matrix/Object.hpp"

#define OZ_LUA_REGISTER_FUNC( func ) lua.registerFunction( #func, func )
#define OZ_LUA_REGISTER_CONST( name, value ) lua.registerConstant( #name, value )

struct lua_State;

namespace oz
{

  class Struct;
  class Bot;
  class Particle;

  typedef int ( LuaAPI )( lua_State* );

  class Lua
  {
    private:

      lua_State*      l;

      Object*         self;
      Bot*            user;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;

      Vector<Struct*> structs;
      Vector<Object*> objects;

      List<Object::Event>::Iterator event;

    public:

      float           damage;
      float           hitMomentum;

    private:

      bool readVariable( InputStream* istream );
      void writeVariable( OutputStream* stream );

    public:

      Lua();

      void staticCall( const char* functionName );
      void objectCall( const char* functionName, Object* self, Bot* user = null );

      void registerObject( int index );
      void unregisterObject( int index );

      void registerFunction( const char* name, LuaAPI func );
      void registerConstant( const char* name, int value );
      void registerConstant( const char* name, float value );
      void registerConstant( const char* name, const char* value );

      void read( InputStream* istream );
      void write( OutputStream* ostream );

      void init();
      void free();

    private:

      static LuaAPI ozPrintln;
      static LuaAPI ozException;

      static LuaAPI ozBindAllOverlaps;
      static LuaAPI ozBindStrOverlaps;
      static LuaAPI ozBindObjOverlaps;
      static LuaAPI ozSelfBindAllOverlaps;
      static LuaAPI ozSelfBindStrOverlaps;
      static LuaAPI ozSelfBindObjOverlaps;

      static LuaAPI ozStrBindIndex;
      static LuaAPI ozStrBindNext;

      static LuaAPI ozStrIsNull;
      static LuaAPI ozStrGetBounds;
      static LuaAPI ozStrGetIndex;
      static LuaAPI ozStrGetPos;
      static LuaAPI ozStrGetBSP;
      static LuaAPI ozStrDamage;
      static LuaAPI ozStrDestroy;

      static LuaAPI ozStrVectorFromSelf;
      static LuaAPI ozStrDirectionFromSelf;
      static LuaAPI ozStrDistanceFromSelf;
      static LuaAPI ozStrHeadingFromSelf;
      static LuaAPI ozStrPitchFromSelf;

      static LuaAPI ozObjBindIndex;
      static LuaAPI ozObjBindSelf;
      static LuaAPI ozObjBindUser;
      static LuaAPI ozObjBindNext;

      static LuaAPI ozObjIsNull;
      static LuaAPI ozObjIsSelf;
      static LuaAPI ozObjIsUser;
      static LuaAPI ozObjIsPut;
      static LuaAPI ozObjIsDynamic;
      static LuaAPI ozObjIsItem;
      static LuaAPI ozObjIsWeapon;
      static LuaAPI ozObjIsBot;
      static LuaAPI ozObjIsVehicle;
      static LuaAPI ozObjGetPos;
      static LuaAPI ozObjSetPos;
      static LuaAPI ozObjAddPos;
      static LuaAPI ozObjGetDim;
      static LuaAPI ozObjGetIndex;
      static LuaAPI ozObjGetFlags;
      static LuaAPI ozObjGetOldFlags;
      static LuaAPI ozObjGetTypeName;
      static LuaAPI ozObjGetLife;
      static LuaAPI ozObjSetLife;
      static LuaAPI ozObjAddLife;
      static LuaAPI ozObjAddEvent;
      static LuaAPI ozObjDamage;
      static LuaAPI ozObjDestroy;
      static LuaAPI ozObjQuietDestroy;

      static LuaAPI ozObjVectorFromSelf;
      static LuaAPI ozObjDirectionFromSelf;
      static LuaAPI ozObjDistanceFromSelf;
      static LuaAPI ozObjHeadingFromSelf;
      static LuaAPI ozObjPitchFromSelf;
      static LuaAPI ozObjPitchFromSelfEye;

      static LuaAPI ozObjBindEvent;
      static LuaAPI ozEventBindNext;
      static LuaAPI ozEventGet;

      static LuaAPI ozDynBindParent;
      static LuaAPI ozDynGetVelocity;
      static LuaAPI ozDynSetVelocity;
      static LuaAPI ozDynAddVelocity;
      static LuaAPI ozDynGetMomentum;
      static LuaAPI ozDynSetMomentum;
      static LuaAPI ozDynAddMomentum;
      static LuaAPI ozDynGetMass;
      static LuaAPI ozDynSetMass;
      static LuaAPI ozDynAddMass;
      static LuaAPI ozDynResetMass;
      static LuaAPI ozDynGetLift;
      static LuaAPI ozDynSetLift;
      static LuaAPI ozDynAddLift;
      static LuaAPI ozDynResetLift;

      static LuaAPI ozBotGetName;
      static LuaAPI ozBotSetName;
      static LuaAPI ozBotGetEyePos;
      static LuaAPI ozBotGetH;
      static LuaAPI ozBotSetH;
      static LuaAPI ozBotAddH;
      static LuaAPI ozBotGetV;
      static LuaAPI ozBotSetV;
      static LuaAPI ozBotAddV;
      static LuaAPI ozBotGetDir;
      static LuaAPI ozBotActionForward;
      static LuaAPI ozBotActionBackward;
      static LuaAPI ozBotActionRight;
      static LuaAPI ozBotActionLeft;
      static LuaAPI ozBotActionJump;
      static LuaAPI ozBotActionCrouch;
      static LuaAPI ozBotActionUse;
      static LuaAPI ozBotActionTake;
      static LuaAPI ozBotActionGrab;
      static LuaAPI ozBotActionThrow;
      static LuaAPI ozBotActionAttack;
      static LuaAPI ozBotActionExit;
      static LuaAPI ozBotActionEject;
      static LuaAPI ozBotActionSuicide;
      static LuaAPI ozBotStateIsRunning;
      static LuaAPI ozBotStateSetRunning;
      static LuaAPI ozBotStateToggleRunning;
      static LuaAPI ozBotGetStamina;
      static LuaAPI ozBotSetStamina;
      static LuaAPI ozBotAddStamina;
      static LuaAPI ozBotRearm;

      static LuaAPI ozVehicleService;

      static LuaAPI ozPartBindIndex;
      static LuaAPI ozPartIsNull;
      static LuaAPI ozPartGetPos;
      static LuaAPI ozPartSetPos;
      static LuaAPI ozPartAddPos;
      static LuaAPI ozPartGetIndex;
      static LuaAPI ozPartGetVelocity;
      static LuaAPI ozPartSetVelocity;
      static LuaAPI ozPartAddVelocity;
      static LuaAPI ozPartGetLife;
      static LuaAPI ozPartSetLife;
      static LuaAPI ozPartAddLife;

      static LuaAPI ozOrbisAddStr;
      static LuaAPI ozOrbisForceAddStr;
      static LuaAPI ozOrbisAddObj;
      static LuaAPI ozOrbisForceAddObj;
      static LuaAPI ozOrbisAddPart;
      static LuaAPI ozOrbisRemoveStr;
      static LuaAPI ozOrbisRemoveObj;
      static LuaAPI ozOrbisRemovePart;
      static LuaAPI ozOrbisGenParts;

      static LuaAPI ozTerraHeight;

      static LuaAPI ozCaelumGetHeading;
      static LuaAPI ozCaelumSetHeading;
      static LuaAPI ozCaelumGetPeriod;
      static LuaAPI ozCaelumSetPeriod;
      static LuaAPI ozCaelumGetTime;
      static LuaAPI ozCaelumSetTime;
      static LuaAPI ozCaelumAddTime;

  };

  extern Lua lua;

}
