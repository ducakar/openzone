/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <nirvana/LuaNirvana.hh>

#include <nirvana/luaapi.hh>

namespace oz
{

namespace
{

// For IMPORT_FUNC()/IGNORE_FUNC() macros.
LuaNirvana& lua = luaNirvana;

}

void LuaNirvana::mindCall(const char* functionName, Mind* mind, Bot* self)
{
  lua_State* l = l_;

  OZ_ASSERT(l_gettop() == 1 && mind != nullptr && self != nullptr);

  ms.self     = self;
  ms.obj      = self;
  ms.str      = nullptr;
  ms.objIndex = 0;
  ms.strIndex = 0;
  ns.self     = self;
  ns.mind     = mind;
  ns.device   = nullptr;

  l_getglobal(functionName);
  l_rawgeti(1, self->index);

  if (l_pcall(1, 0) != LUA_OK) {
    Log::println("Lua[N] in %s(self = %d): %s", functionName, self->index, l_tostring(-1));
    System::bell();

    l_pop(1);
  }

  OZ_ASSERT(l_gettop() == 1);
}

void LuaNirvana::registerMind(int botIndex)
{
  lua_State* l = l_;

  OZ_ASSERT(l_gettop() == 1);

  l_newtable();
  l_rawseti(1, botIndex);
}

void LuaNirvana::unregisterMind(int botIndex)
{
  lua_State* l = l_;

  OZ_ASSERT(l_gettop() == 1);

  l_pushnil();
  l_rawseti(1, botIndex);
}

void LuaNirvana::read(Stream* is)
{
  lua_State* l = l_;

  OZ_ASSERT(l_gettop() == 1);
  OZ_ASSERT((l_pushnil(), true));
  OZ_ASSERT(!l_next(1));

  int index = is->readInt();

  while (index != -1) {
    readValue(l_, is);

    l_rawseti(1, index);

    index = is->readInt();
  }
}

void LuaNirvana::write(Stream* os)
{
  lua_State* l = l_;

  OZ_ASSERT(l_gettop() == 1);

  l_pushnil();
  while (l_next(1)) {
    OZ_ASSERT(l_type(-2) == LUA_TNUMBER);
    OZ_ASSERT(l_type(-1) == LUA_TTABLE);

    os->writeInt(l_toint(-2));
    writeValue(l_, os);

    l_pop(1);
  }

  os->writeInt(-1);
}

void LuaNirvana::init()
{
  Log::print("Initialising Nirvana Lua ...");

  Lua::init("tsm");
  lua_State* l = l_;

  ls.envName = "nirvana";
  ms.structs.reserve(32);
  ms.objects.reserve(512);

  /*
   * General functions
   */

  IMPORT_FUNC(ozError);
  IMPORT_FUNC(ozPrintln);

  IMPORT_FUNC(ozForceUpdate);

  /*
   * Orbis
   */

  IMPORT_FUNC(ozOrbisGetSize);
  IMPORT_FUNC(ozOrbisGetDim);

  IMPORT_FUNC(ozOrbisGetGravity);
  IGNORE_FUNC(ozOrbisSetGravity);

  IGNORE_FUNC(ozOrbisAddStr);
  IGNORE_FUNC(ozOrbisAddObj);
  IGNORE_FUNC(ozOrbisAddFrag);
  IGNORE_FUNC(ozOrbisGenFrags);

  IMPORT_FUNC(ozOrbisOverlaps);
  IMPORT_FUNC(ozOrbisBindOverlaps);

  /*
   * Caelum
   */

  IGNORE_FUNC(ozCaelumLoad);

  IMPORT_FUNC(ozCaelumGetHeading);
  IGNORE_FUNC(ozCaelumSetHeading);
  IMPORT_FUNC(ozCaelumGetPeriod);
  IGNORE_FUNC(ozCaelumSetPeriod);
  IMPORT_FUNC(ozCaelumGetTime);
  IGNORE_FUNC(ozCaelumSetTime);
  IGNORE_FUNC(ozCaelumAddTime);
  IGNORE_FUNC(ozCaelumSetRealTime);

  /*
   * Terra
   */

  IGNORE_FUNC(ozTerraLoad);

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
  IGNORE_FUNC(ozStrSetLife);
  IGNORE_FUNC(ozStrAddLife);
  IMPORT_FUNC(ozStrDefaultResistance);
  IMPORT_FUNC(ozStrGetResistance);
  IGNORE_FUNC(ozStrSetResistance);

  IGNORE_FUNC(ozStrDamage);
  IGNORE_FUNC(ozStrDestroy);
  IGNORE_FUNC(ozStrRemove);

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
  IGNORE_FUNC(ozEntSetLock);
  IGNORE_FUNC(ozEntTrigger);

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
  IGNORE_FUNC(ozBindUser);
  IMPORT_FUNC(ozBindNextObj);

  IMPORT_FUNC(ozObjIsNull);
  IMPORT_FUNC(ozObjIsSelf);
  IGNORE_FUNC(ozObjIsUser);
  IMPORT_FUNC(ozObjIsCut);

  IMPORT_FUNC(ozObjGetIndex);
  IMPORT_FUNC(ozObjGetPos);
  IGNORE_FUNC(ozObjWarpPos);
  IMPORT_FUNC(ozObjGetDim);
  IMPORT_FUNC(ozObjHasFlag);
  IMPORT_FUNC(ozObjGetHeading);
  IMPORT_FUNC(ozObjGetClassName);

  IMPORT_FUNC(ozObjMaxLife);
  IMPORT_FUNC(ozObjGetLife);
  IGNORE_FUNC(ozObjSetLife);
  IGNORE_FUNC(ozObjAddLife);
  IMPORT_FUNC(ozObjDefaultResistance);
  IMPORT_FUNC(ozObjGetResistance);
  IGNORE_FUNC(ozObjSetResistance);

  IGNORE_FUNC(ozObjAddEvent);

  IMPORT_FUNC(ozObjBindItems);
  IMPORT_FUNC(ozObjBindItem);
  IGNORE_FUNC(ozObjAddItem);
  IGNORE_FUNC(ozObjRemoveItem);
  IGNORE_FUNC(ozObjRemoveAllItems);

  IGNORE_FUNC(ozObjEnableUpdate);
  IGNORE_FUNC(ozObjReportStatus);
  IGNORE_FUNC(ozObjDamage);
  IGNORE_FUNC(ozObjDestroy);

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
  IGNORE_FUNC(ozDynSetMomentum);
  IGNORE_FUNC(ozDynAddMomentum);
  IMPORT_FUNC(ozDynGetMass);
  IMPORT_FUNC(ozDynGetLift);

  /*
   * Weapon
   */

  IMPORT_FUNC(ozWeaponMaxRounds);
  IMPORT_FUNC(ozWeaponGetRounds);
  IGNORE_FUNC(ozWeaponSetRounds);
  IGNORE_FUNC(ozWeaponAddRounds);

  /*
   * Bot
   */

  IMPORT_FUNC(ozBotGetName);
  IGNORE_FUNC(ozBotSetName);
  IMPORT_FUNC(ozBotGetMind);
  IGNORE_FUNC(ozBotSetMind);

  IMPORT_FUNC(ozBotHasState);
  IMPORT_FUNC(ozBotGetEyePos);
  IMPORT_FUNC(ozBotGetH);
  IGNORE_FUNC(ozBotSetH);
  IGNORE_FUNC(ozBotAddH);
  IMPORT_FUNC(ozBotGetV);
  IGNORE_FUNC(ozBotSetV);
  IGNORE_FUNC(ozBotAddV);
  IMPORT_FUNC(ozBotGetDir);

  IMPORT_FUNC(ozBotMaxStamina);
  IMPORT_FUNC(ozBotGetStamina);
  IGNORE_FUNC(ozBotSetStamina);
  IGNORE_FUNC(ozBotAddStamina);

  IMPORT_FUNC(ozBotGetCargo);
  IMPORT_FUNC(ozBotGetWeaponItem);
  IGNORE_FUNC(ozBotSetWeaponItem);

  IMPORT_FUNC(ozBotCanReachEntity);
  IMPORT_FUNC(ozBotCanReachObj);

  IGNORE_FUNC(ozBotAction);
  IGNORE_FUNC(ozBotClearActions);

  IGNORE_FUNC(ozBotHeal);
  IGNORE_FUNC(ozBotRearm);
  IGNORE_FUNC(ozBotKill);

  /*
   * Vehicle
   */

  IMPORT_FUNC(ozVehicleGetPilot);

  IMPORT_FUNC(ozVehicleGetH);
  IGNORE_FUNC(ozVehicleSetH);
  IGNORE_FUNC(ozVehicleAddH);
  IMPORT_FUNC(ozVehicleGetV);
  IGNORE_FUNC(ozVehicleSetV);
  IGNORE_FUNC(ozVehicleAddV);
  IMPORT_FUNC(ozVehicleGetDir);

  IGNORE_FUNC(ozVehicleEmbarkBot);
  IGNORE_FUNC(ozVehicleDisembarkBot);

  IGNORE_FUNC(ozVehicleService);

  /*
   * Frag
   */

  IMPORT_FUNC(ozFragBindIndex);

  IMPORT_FUNC(ozFragIsNull);

  IMPORT_FUNC(ozFragGetPos);
  IGNORE_FUNC(ozFragWarpPos);
  IMPORT_FUNC(ozFragGetIndex);
  IMPORT_FUNC(ozFragGetVelocity);
  IGNORE_FUNC(ozFragSetVelocity);
  IGNORE_FUNC(ozFragAddVelocity);
  IMPORT_FUNC(ozFragGetLife);
  IGNORE_FUNC(ozFragSetLife);
  IGNORE_FUNC(ozFragAddLife);

  IGNORE_FUNC(ozFragRemove);

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

  /*
   * Mind's bot
   */

  IMPORT_FUNC(ozSelfIsCut);

  IMPORT_FUNC(ozSelfGetIndex);
  IMPORT_FUNC(ozSelfGetPos);
  IMPORT_FUNC(ozSelfGetDim);
  IMPORT_FUNC(ozSelfHasFlag);
  IMPORT_FUNC(ozSelfGetHeading);
  IMPORT_FUNC(ozSelfGetClassName);
  IMPORT_FUNC(ozSelfMaxLife);
  IMPORT_FUNC(ozSelfGetLife);
  IMPORT_FUNC(ozSelfDefaultResistance);
  IMPORT_FUNC(ozSelfGetResistance);

  IMPORT_FUNC(ozSelfGetParent);
  IMPORT_FUNC(ozSelfGetVelocity);
  IMPORT_FUNC(ozSelfGetMomentum);
  IMPORT_FUNC(ozSelfGetMass);
  IMPORT_FUNC(ozSelfGetLift);

  IMPORT_FUNC(ozSelfGetName);
  IMPORT_FUNC(ozSelfGetMind);
  IMPORT_FUNC(ozSelfHasState);
  IMPORT_FUNC(ozSelfGetEyePos);
  IMPORT_FUNC(ozSelfGetH);
  IMPORT_FUNC(ozSelfSetH);
  IMPORT_FUNC(ozSelfAddH);
  IMPORT_FUNC(ozSelfGetV);
  IMPORT_FUNC(ozSelfSetV);
  IMPORT_FUNC(ozSelfAddV);
  IMPORT_FUNC(ozSelfGetDir);

  IMPORT_FUNC(ozSelfGetStamina);
  IMPORT_FUNC(ozSelfMaxStamina);

  IMPORT_FUNC(ozSelfGetCargo);
  IMPORT_FUNC(ozSelfGetWeaponItem);
  IMPORT_FUNC(ozSelfSetWeaponItem);

  IMPORT_FUNC(ozSelfCanReachEntity);
  IMPORT_FUNC(ozSelfCanReachObj);

  IMPORT_FUNC(ozSelfAction);
  IMPORT_FUNC(ozSelfClearActions);

  IMPORT_FUNC(ozSelfBindItems);
  IMPORT_FUNC(ozSelfBindItem);

  IMPORT_FUNC(ozSelfOverlaps);
  IMPORT_FUNC(ozSelfBindOverlaps);

  /*
   * Mind
   */

  IMPORT_FUNC(ozMindGetSide);
  IMPORT_FUNC(ozMindSetSide);

  /*
   * QuestList
   */

  IMPORT_FUNC(ozQuestAdd);
  IMPORT_FUNC(ozQuestEnd);

  /*
   * TechGraph
   */

  IMPORT_FUNC(ozTechEnable);
  IMPORT_FUNC(ozTechDisable);
  IMPORT_FUNC(ozTechEnableAll);
  IMPORT_FUNC(ozTechDisableAll);

  /*
   * Nirvana
   */

  IMPORT_FUNC(ozNirvanaRemoveDevice);
  IMPORT_FUNC(ozNirvanaAddMemo);

  importMatrixConstants(l_);
  importNirvanaConstants(l_);

  l_newtable();
  l_setglobal("ozLocalData");
  l_getglobal("ozLocalData");

  loadDir("@lua/common");
  loadDir("@lua/nirvana");

  OZ_ASSERT(l_gettop() == 1);

  Log::printEnd(" OK");
}

void LuaNirvana::destroy()
{
  lua_State* l = l_;

  if (l_ == nullptr) {
    return;
  }

  Log::print("Destroying Nirvana Lua ...");

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

LuaNirvana luaNirvana;

}
