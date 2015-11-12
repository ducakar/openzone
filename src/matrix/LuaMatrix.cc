/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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

#include <matrix/LuaMatrix.hh>

#include <matrix/luaapi.hh>

namespace oz
{

// For IMPORT_FUNC()/IGNORE_FUNC() macros.
static LuaMatrix& lua = luaMatrix;

String LuaMatrix::nameGenCall(const char* functionName)
{
  ms.self     = nullptr;
  ms.user     = nullptr;
  ms.obj      = nullptr;
  ms.str      = nullptr;
  ms.frag     = nullptr;
  ms.objIndex = 0;
  ms.strIndex = 0;

  OZ_ASSERT(l_gettop() == 1);

  String name = "";

  if (!String::isEmpty(functionName)) {
    l_getglobal(functionName);

    if (l_pcall(0, 1) != LUA_OK) {
      Log::println("Lua[M] in %s(): %s", functionName, l_tostring(-1));
      System::bell();
    }
    else {
      name = l_tostring(2);
    }

    l_settop(1);
  }

  return name;
}

bool LuaMatrix::objectCall(const char* functionName, Object* self, Bot* user)
{
  ms.self     = self;
  ms.user     = user;
  ms.obj      = self;
  ms.str      = nullptr;
  ms.frag     = nullptr;
  ms.objIndex = 0;
  ms.strIndex = 0;

  OZ_ASSERT(l_gettop() == 1 && self != nullptr);

  bool success = true;

  l_getglobal(functionName);
  l_rawgeti(1, self->index);

  if (l_pcall(1, 1) != LUA_OK) {
    Log::println("Lua[M] in %s(self = %d, user = %d): %s",
                 functionName, self->index, user == nullptr ? -1 : user->index, l_tostring(-1));
    System::bell();
  }
  else {
    success = l_tobool(2);
  }

  l_settop(1);

  objectStatus = ms.status;
  return success;
}

void LuaMatrix::registerObject(int index)
{
  // We cannot assume that `ozLocalData` exists at index 1 as this function may be called from a
  // handler script creating an object.
  l_getglobal("ozLocalData");
  l_newtable();
  l_rawseti(-2, index);
  l_pop(1);
}

void LuaMatrix::unregisterObject(int index)
{
  // We cannot assume that `ozLocalData` exists at index 1 as this function may be called from a
  // handler script creating an object.
  l_getglobal("ozLocalData");
  l_pushnil();
  l_rawseti(-2, index);
  l_pop(1);
}

void LuaMatrix::read(Stream* is)
{
  OZ_ASSERT(l_gettop() == 1);
  OZ_ASSERT((l_pushnil(), true));
  OZ_ASSERT(!l_next(1));

  int index = is->readInt();

  while (index >= 0) {
    readValue(l, is);

    l_rawseti(1, index);

    index = is->readInt();
  }
}

void LuaMatrix::write(Stream* os)
{
  OZ_ASSERT(l_gettop() == 1);

  l_pushnil();
  while (l_next(1)) {
    OZ_ASSERT(l_type(-2) == LUA_TNUMBER);
    OZ_ASSERT(l_type(-1) == LUA_TTABLE);

    os->writeInt(l_toint(-2));
    writeValue(l, os);

    l_pop(1);
  }

  os->writeInt(-1);
}

void LuaMatrix::init()
{
  Log::print("Initialising Matrix Lua ...");

  Lua::init("tsm");

  ls.envName = "matrix";
  ms.structs.reserve(32);
  ms.objects.reserve(512);

  /*
   * General functions
   */

  IMPORT_FUNC(ozError);
  IMPORT_FUNC(ozPrintln);

  /*
   * Orbis
   */

  IMPORT_FUNC(ozOrbisGetSize);
  IMPORT_FUNC(ozOrbisGetDim);

  IMPORT_FUNC(ozOrbisGetGravity);
  IMPORT_FUNC(ozOrbisSetGravity);

  IMPORT_FUNC(ozOrbisAddStr);
  IMPORT_FUNC(ozOrbisAddObj);
  IMPORT_FUNC(ozOrbisAddFrag);
  IMPORT_FUNC(ozOrbisGenFrags);

  IMPORT_FUNC(ozOrbisOverlaps);
  IMPORT_FUNC(ozOrbisBindOverlaps);

  /*
   * Caelum
   */

  IMPORT_FUNC(ozCaelumLoad);

  IMPORT_FUNC(ozCaelumGetHeading);
  IMPORT_FUNC(ozCaelumSetHeading);
  IMPORT_FUNC(ozCaelumGetPeriod);
  IMPORT_FUNC(ozCaelumSetPeriod);
  IMPORT_FUNC(ozCaelumGetTime);
  IMPORT_FUNC(ozCaelumSetTime);
  IMPORT_FUNC(ozCaelumAddTime);
  IMPORT_FUNC(ozCaelumSetRealTime);

  /*
   * Terra
   */

  IMPORT_FUNC(ozTerraLoad);
  IMPORT_FUNC(ozTerraHeight);

  /*
   * Structure
   */

  IMPORT_FUNC(ozBSPDim);

  IMPORT_FUNC(ozBindStr);
  IMPORT_FUNC(ozBindNextStr);

  IMPORT_FUNC(ozStrIsNull);

  IMPORT_FUNC(ozStrGetIndex);
  IMPORT_FUNC(ozStrGetBounds);
  IMPORT_FUNC(ozStrGetPos);
  IMPORT_FUNC(ozStrGetBSP);
  IMPORT_FUNC(ozStrGetHeading);

  IMPORT_FUNC(ozStrMaxLife);
  IMPORT_FUNC(ozStrGetLife);
  IMPORT_FUNC(ozStrSetLife);
  IMPORT_FUNC(ozStrAddLife);
  IMPORT_FUNC(ozStrDefaultResistance);
  IMPORT_FUNC(ozStrGetResistance);
  IMPORT_FUNC(ozStrSetResistance);

  IMPORT_FUNC(ozStrDamage);
  IMPORT_FUNC(ozStrDestroy);
  IMPORT_FUNC(ozStrRemove);

  IMPORT_FUNC(ozStrNumBoundObjs);
  IMPORT_FUNC(ozStrBindBoundObj);

  IMPORT_FUNC(ozStrNumEnts);
  IMPORT_FUNC(ozStrBindEnt);

  IMPORT_FUNC(ozStrOverlaps);
  IMPORT_FUNC(ozStrBindOverlaps);

  IMPORT_FUNC(ozStrVectorFromSelf);
  IMPORT_FUNC(ozStrVectorFromSelfEye);
  IMPORT_FUNC(ozStrDirFromSelf);
  IMPORT_FUNC(ozStrDirFromSelfEye);
  IMPORT_FUNC(ozStrDistFromSelf);
  IMPORT_FUNC(ozStrDistFromSelfEye);
  IMPORT_FUNC(ozStrHeadingFromSelfEye);
  IMPORT_FUNC(ozStrRelHeadingFromSelfEye);
  IMPORT_FUNC(ozStrPitchFromSelfEye);
  IMPORT_FUNC(ozStrIsVisibleFromSelf);
  IMPORT_FUNC(ozStrIsVisibleFromSelfEye);

  /*
   * Entity
   */

  IMPORT_FUNC(ozEntGetState);
  IMPORT_FUNC(ozEntGetLock);
  IMPORT_FUNC(ozEntSetLock);
  IMPORT_FUNC(ozEntTrigger);

  IMPORT_FUNC(ozEntOverlaps);
  IMPORT_FUNC(ozEntBindOverlaps);

  IMPORT_FUNC(ozEntVectorFromSelf);
  IMPORT_FUNC(ozEntVectorFromSelfEye);
  IMPORT_FUNC(ozEntDirFromSelf);
  IMPORT_FUNC(ozEntDirFromSelfEye);
  IMPORT_FUNC(ozEntDistFromSelf);
  IMPORT_FUNC(ozEntDistFromSelfEye);
  IMPORT_FUNC(ozEntHeadingFromSelfEye);
  IMPORT_FUNC(ozEntRelHeadingFromSelfEye);
  IMPORT_FUNC(ozEntPitchFromSelfEye);
  IMPORT_FUNC(ozEntIsVisibleFromSelf);
  IMPORT_FUNC(ozEntIsVisibleFromSelfEye);

  /*
   * Object
   */

  IMPORT_FUNC(ozClassDim);

  IMPORT_FUNC(ozBindObj);
  IMPORT_FUNC(ozBindSelf);
  IMPORT_FUNC(ozBindUser);
  IMPORT_FUNC(ozBindNextObj);

  IMPORT_FUNC(ozObjIsNull);
  IMPORT_FUNC(ozObjIsSelf);
  IMPORT_FUNC(ozObjIsUser);
  IMPORT_FUNC(ozObjIsCut);

  IMPORT_FUNC(ozObjGetIndex);
  IMPORT_FUNC(ozObjGetPos);
  IMPORT_FUNC(ozObjWarpPos);
  IMPORT_FUNC(ozObjGetDim);
  IMPORT_FUNC(ozObjHasFlag);
  IMPORT_FUNC(ozObjGetHeading);
  IMPORT_FUNC(ozObjGetClassName);

  IMPORT_FUNC(ozObjMaxLife);
  IMPORT_FUNC(ozObjGetLife);
  IMPORT_FUNC(ozObjSetLife);
  IMPORT_FUNC(ozObjAddLife);
  IMPORT_FUNC(ozObjDefaultResistance);
  IMPORT_FUNC(ozObjGetResistance);
  IMPORT_FUNC(ozObjSetResistance);

  IMPORT_FUNC(ozObjAddEvent);

  IMPORT_FUNC(ozObjBindItems);
  IMPORT_FUNC(ozObjBindItem);
  IMPORT_FUNC(ozObjAddItem);
  IMPORT_FUNC(ozObjRemoveItem);
  IMPORT_FUNC(ozObjRemoveAllItems);

  IMPORT_FUNC(ozObjEnableUpdate);
  IMPORT_FUNC(ozObjReportStatus);
  IMPORT_FUNC(ozObjDamage);
  IMPORT_FUNC(ozObjDestroy);

  IMPORT_FUNC(ozObjOverlaps);
  IMPORT_FUNC(ozObjBindOverlaps);

  IMPORT_FUNC(ozObjVectorFromSelf);
  IMPORT_FUNC(ozObjVectorFromSelfEye);
  IMPORT_FUNC(ozObjDirFromSelf);
  IMPORT_FUNC(ozObjDirFromSelfEye);
  IMPORT_FUNC(ozObjDistFromSelf);
  IMPORT_FUNC(ozObjDistFromSelfEye);
  IMPORT_FUNC(ozObjHeadingFromSelfEye);
  IMPORT_FUNC(ozObjRelHeadingFromSelfEye);
  IMPORT_FUNC(ozObjPitchFromSelfEye);
  IMPORT_FUNC(ozObjIsVisibleFromSelf);
  IMPORT_FUNC(ozObjIsVisibleFromSelfEye);

  /*
   * Dynamic object
   */

  IMPORT_FUNC(ozDynGetParent);

  IMPORT_FUNC(ozDynGetVelocity);
  IMPORT_FUNC(ozDynGetMomentum);
  IMPORT_FUNC(ozDynSetMomentum);
  IMPORT_FUNC(ozDynAddMomentum);
  IMPORT_FUNC(ozDynGetMass);
  IMPORT_FUNC(ozDynGetLift);

  /*
   * Weapon
   */

  IMPORT_FUNC(ozWeaponMaxRounds);
  IMPORT_FUNC(ozWeaponGetRounds);
  IMPORT_FUNC(ozWeaponSetRounds);
  IMPORT_FUNC(ozWeaponAddRounds);

  /*
   * Bot
   */

  IMPORT_FUNC(ozBotGetName);
  IMPORT_FUNC(ozBotSetName);
  IMPORT_FUNC(ozBotGetMind);
  IMPORT_FUNC(ozBotSetMind);

  IMPORT_FUNC(ozBotHasState);
  IMPORT_FUNC(ozBotGetEyePos);
  IMPORT_FUNC(ozBotGetH);
  IMPORT_FUNC(ozBotSetH);
  IMPORT_FUNC(ozBotAddH);
  IMPORT_FUNC(ozBotGetV);
  IMPORT_FUNC(ozBotSetV);
  IMPORT_FUNC(ozBotAddV);
  IMPORT_FUNC(ozBotGetDir);

  IMPORT_FUNC(ozBotGetCargo);
  IMPORT_FUNC(ozBotGetWeaponItem);
  IMPORT_FUNC(ozBotSetWeaponItem);

  IMPORT_FUNC(ozBotMaxStamina);
  IMPORT_FUNC(ozBotGetStamina);
  IMPORT_FUNC(ozBotSetStamina);
  IMPORT_FUNC(ozBotAddStamina);

  IMPORT_FUNC(ozBotAction);
  IMPORT_FUNC(ozBotClearActions);

  IMPORT_FUNC(ozBotHeal);
  IMPORT_FUNC(ozBotRearm);
  IMPORT_FUNC(ozBotKill);

  IMPORT_FUNC(ozBotCanReachEntity);
  IMPORT_FUNC(ozBotCanReachObj);

  /*
   * Vehicle
   */

  IMPORT_FUNC(ozVehicleGetPilot);

  IMPORT_FUNC(ozVehicleGetH);
  IMPORT_FUNC(ozVehicleSetH);
  IMPORT_FUNC(ozVehicleAddH);
  IMPORT_FUNC(ozVehicleGetV);
  IMPORT_FUNC(ozVehicleSetV);
  IMPORT_FUNC(ozVehicleAddV);
  IMPORT_FUNC(ozVehicleGetDir);

  IMPORT_FUNC(ozVehicleEmbarkBot);
  IMPORT_FUNC(ozVehicleDisembarkBot);

  IMPORT_FUNC(ozVehicleService);

  /*
   * Frag
   */

  IMPORT_FUNC(ozFragBindIndex);

  IMPORT_FUNC(ozFragIsNull);

  IMPORT_FUNC(ozFragGetPos);
  IMPORT_FUNC(ozFragWarpPos);
  IMPORT_FUNC(ozFragGetIndex);
  IMPORT_FUNC(ozFragGetVelocity);
  IMPORT_FUNC(ozFragSetVelocity);
  IMPORT_FUNC(ozFragAddVelocity);
  IMPORT_FUNC(ozFragGetLife);
  IMPORT_FUNC(ozFragSetLife);
  IMPORT_FUNC(ozFragAddLife);

  IMPORT_FUNC(ozFragRemove);

  IMPORT_FUNC(ozFragOverlaps);
  IMPORT_FUNC(ozFragBindOverlaps);

  IMPORT_FUNC(ozFragVectorFromSelf);
  IMPORT_FUNC(ozFragVectorFromSelfEye);
  IMPORT_FUNC(ozFragDirFromSelf);
  IMPORT_FUNC(ozFragDirFromSelfEye);
  IMPORT_FUNC(ozFragDistFromSelf);
  IMPORT_FUNC(ozFragDistFromSelfEye);
  IMPORT_FUNC(ozFragHeadingFromSelfEye);
  IMPORT_FUNC(ozFragRelHeadingFromSelfEye);
  IMPORT_FUNC(ozFragPitchFromSelfEye);
  IMPORT_FUNC(ozFragIsVisibleFromSelf);
  IMPORT_FUNC(ozFragIsVisibleFromSelfEye);

  importMatrixConstants(l);

  l_newtable();
  l_setglobal("ozLocalData");
  l_getglobal("ozLocalData");

  loadDir("@lua/common");
  loadDir("@lua/matrix");

  OZ_ASSERT(l_gettop() == 1);

  Log::printEnd(" OK");
}

void LuaMatrix::destroy()
{
  if (l == nullptr) {
    return;
  }

  Log::print("Destroying Matrix Lua ...");

  ms.structs.clear();
  ms.structs.trim();

  ms.objects.clear();
  ms.objects.trim();

  OZ_ASSERT(l_gettop() == 1);
  OZ_ASSERT((l_pushnil(), true));
  OZ_ASSERT(!l_next(1));

  l_settop(0);
  Lua::destroy();

  Log::printEnd(" OK");
}

LuaMatrix luaMatrix;

}
