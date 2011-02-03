/*
 *  Lua.hpp
 *
 *  Lua scripting engine for Nirvana
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

#include "stable.hpp"

#include "nirvana/common.hpp"

struct lua_State;

namespace oz
{
namespace nirvana
{

  class Lua
  {
    private:

      typedef int ( LuaAPI )( lua_State* );

      lua_State* l;

      void callFunc( const char* functionName, int botIndex );

    public:

      Bot*            self;

      Struct*         str;
      Object*         obj;
      Particle*       part;

      int             strIndex;
      int             objIndex;

      Vector<Object*> objects;
      Vector<Struct*> structs;

      List<Object::Event>::CIterator event;

      bool            forceUpdate;

      void call( const char* functionName, Bot* self_ )
      {
        forceUpdate = false;
        self        = self_;

        callFunc( functionName, self->index );
      }

    public:

      // create a table for a mind that can act as mind's local storage, mind's local variables
      void registerMind( int botIndex );
      void unregisterMind( int botIndex );

      void init();
      void free();

    private:

      static LuaAPI ozPrintln;
      static LuaAPI ozException;
      static LuaAPI ozForceUpdate;

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

      static LuaAPI ozStrVectorFromSelf;
      static LuaAPI ozStrDirectionFromSelf;
      static LuaAPI ozStrDistanceFromSelf;
      static LuaAPI ozStrHeadingFromSelf;
      static LuaAPI ozStrPitchFromSelf;

      static LuaAPI ozObjBindIndex;
      static LuaAPI ozObjBindSelf;
      static LuaAPI ozObjBindNext;

      static LuaAPI ozObjIsNull;
      static LuaAPI ozObjIsSelf;
      static LuaAPI ozObjIsPut;
      static LuaAPI ozObjIsDynamic;
      static LuaAPI ozObjIsItem;
      static LuaAPI ozObjIsWeapon;
      static LuaAPI ozObjIsBot;
      static LuaAPI ozObjIsVehicle;
      static LuaAPI ozObjGetPos;
      static LuaAPI ozObjGetDim;
      static LuaAPI ozObjGetIndex;
      static LuaAPI ozObjGetFlags;
      static LuaAPI ozObjGetOldFlags;
      static LuaAPI ozObjGetTypeName;
      static LuaAPI ozObjGetLife;

      static LuaAPI ozObjVectorFromSelf;
      static LuaAPI ozObjDirectionFromSelf;
      static LuaAPI ozObjDistanceFromSelf;
      static LuaAPI ozObjHeadingFromSelf;
      static LuaAPI ozObjPitchFromSelf;
      static LuaAPI ozObjPitchFromSelfEye;

      static LuaAPI ozObjBindEvent;
      static LuaAPI ozEventBindNext;
      static LuaAPI ozEventGet;

      static LuaAPI ozDynGetVelocity;
      static LuaAPI ozDynGetMomentum;
      static LuaAPI ozDynGetMass;
      static LuaAPI ozDynGetLift;

      static LuaAPI ozBotGetEyePos;
      static LuaAPI ozBotGetH;
      static LuaAPI ozBotGetV;
      static LuaAPI ozBotGetDir;
      static LuaAPI ozBotStateIsRunning;
      static LuaAPI ozBotGetStamina;

      static LuaAPI ozSelfGetEyePos;
      static LuaAPI ozSelfGetH;
      static LuaAPI ozSelfSetH;
      static LuaAPI ozSelfAddH;
      static LuaAPI ozSelfGetV;
      static LuaAPI ozSelfSetV;
      static LuaAPI ozSelfAddV;
      static LuaAPI ozSelfActionForward;
      static LuaAPI ozSelfActionBackward;
      static LuaAPI ozSelfActionRight;
      static LuaAPI ozSelfActionLeft;
      static LuaAPI ozSelfActionJump;
      static LuaAPI ozSelfActionCrouch;
      static LuaAPI ozSelfActionUse;
      static LuaAPI ozSelfActionTake;
      static LuaAPI ozSelfActionGrab;
      static LuaAPI ozSelfActionThrow;
      static LuaAPI ozSelfActionAttack;
      static LuaAPI ozSelfActionExit;
      static LuaAPI ozSelfActionEject;
      static LuaAPI ozSelfActionSuicide;
      static LuaAPI ozSelfStateIsRunning;
      static LuaAPI ozSelfStateSetRunning;
      static LuaAPI ozSelfStateToggleRunning;

      static LuaAPI ozPartBindIndex;
      static LuaAPI ozPartIsNull;
      static LuaAPI ozPartGetPos;
      static LuaAPI ozPartGetIndex;
      static LuaAPI ozPartGetVelocity;
      static LuaAPI ozPartGetLife;

  };

  extern Lua lua;

}
}
