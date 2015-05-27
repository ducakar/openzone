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

/**
 * @file client/LuaClient.cc
 *
 * Lua scripting engine for client
 */

#include <client/LuaClient.hh>

#include <matrix/Matrix.hh>
#include <client/luaapi.hh>

namespace oz
{
namespace client
{

static LuaClient& lua = luaClient;

void LuaClient::staticCall(const char* functionName)
{
  ms.obj      = nullptr;
  ms.str      = nullptr;
  ms.frag     = nullptr;
  ms.objIndex = 0;
  ms.strIndex = 0;

  hard_assert(l_gettop() == 0);

  l_getglobal(functionName);

  if (l_pcall(0, 0) != LUA_OK) {
    Log::println("Lua[C] in %s(): %s", functionName, l_tostring(-1));
    System::bell();

    l_settop(0);
  }
}

bool LuaClient::exec(const char* code) const
{
  hard_assert(l_gettop() == 0);

  ms.obj      = nullptr;
  ms.str      = nullptr;
  ms.frag     = nullptr;
  ms.objIndex = 0;
  ms.strIndex = 0;

  bool value = false;

  if (l_dostring(code) != LUA_OK) {
    Log::println("Lua[C] in '%s': %s", code, l_tostring(-1));
    System::bell();
  }
  else {
    value = l_tobool(-1);
  }

  l_settop(0);
  return value;
}

void LuaClient::update()
{
  staticCall("onUpdate");
}

void LuaClient::create(const char* mission)
{
  cs.mission = mission;

  Log::print("Importing mission catalogue '%s' ...", cs.mission.c());
  if (cs.missionLingua.initMission(cs.mission)) {
    Log::printEnd(" OK");
  }
  else {
    Log::printEnd(" Failed");
  }

  Log::println("Executing scripts for mission %s {", cs.mission.c());
  Log::indent();

  File missionDir = "@mission/" + cs.mission;
  List<File> files = missionDir.list();

  if (!missionDir.isDirectory()) {
    OZ_ERROR("Mission directory '%s' does not exist", missionDir.c());
  }
  if (files.isEmpty()) {
    OZ_ERROR("Mission directory '%s' contains no Lua scripts", missionDir.c());
  }

  File layoutFile = missionDir / "layout.json";

  if (layoutFile.isFile()) {
    Log::print("Loading layout from '%s' ...", layoutFile.c());

    Json json;
    if (!json.load(layoutFile)) {
      OZ_ERROR("Reading saved layout '%s' failed", layoutFile.c());
    }

    Log::printEnd(" OK");

    matrix.read(json["matrix"]);
    camera.read(json["camera"]);

    json.clear(true);
  }

  loadDir(missionDir);
  staticCall("onCreate");

  Log::unindent();
  Log::println("}");
}

void LuaClient::read(Stream* is)
{
  hard_assert(l_gettop() == 0);

  cs.mission = is->readString();

  Log::print("Importing mission catalogue '%s' ...", cs.mission.c());
  if (cs.missionLingua.initMission(cs.mission)) {
    Log::printEnd(" OK");
  }
  else {
    Log::printEnd(" Failed");
  }

  Log::print("Deserialising scripts for mission %s ...", cs.mission.c());

  File missionDir = "@mission/" + cs.mission;

  if (!missionDir.isDirectory()) {
    OZ_ERROR("Mission directory '%s' does not exist", missionDir.c());
  }

  for (const File& file : missionDir.list()) {
    if (!file.isFile() || !file.hasExtension("lua")) {
      continue;
    }

    Stream is = file.read();

    if (is.available() == 0 || l_dobufferx(is.begin(), is.available(), file, "t") != 0) {
      OZ_ERROR("Client Lua script error in %s", file.c());
    }
  }

  const char* name = is->readString();

  while (!String::isEmpty(name)) {
    readValue(l, is);

    l_setglobal(name);

    name = is->readString();
  }

  Log::printEnd(" OK");
}

void LuaClient::write(Stream* os)
{
  hard_assert(l_gettop() == 0);

  os->writeString(cs.mission);

#if LUA_VERSION_NUM >= 502
  l_pushglobaltable();
#endif
  l_pushnil();

#if LUA_VERSION_NUM >= 502
  while (l_next(-2) != 0) {
#else
  while (l_next(LUA_GLOBALSINDEX) != 0) {
#endif
    hard_assert(l_type(-2) == LUA_TSTRING);

    const char* name = l_tostring(-2);
    if (String::beginsWith(name, "oz_")) {
      os->writeString(name);
      writeValue(l, os);
    }

    l_pop(1);
  }

#if LUA_VERSION_NUM >= 502
  l_pop(1);
#endif

  os->writeString("");
}

void LuaClient::init()
{
  Log::print("Initialising Client Lua ...");

  Lua::init();

  ls.envName = "client";
  ms.structs.reserve(32);
  ms.objects.reserve(512);

  /*
   * General functions
   */

  IMPORT_FUNC(ozError);
  IMPORT_FUNC(ozPrintln);

  IMPORT_FUNC(ozGettext);

  IGNORE_FUNC(ozForceUpdate);

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

  IMPORT_FUNC(ozOrbisAddPlayer);

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

  IGNORE_FUNC(ozStrVectorFromSelf);
  IGNORE_FUNC(ozStrVectorFromSelfEye);
  IGNORE_FUNC(ozStrDirFromSelf);
  IGNORE_FUNC(ozStrDirFromSelfEye);
  IGNORE_FUNC(ozStrDistFromSelf);
  IGNORE_FUNC(ozStrDistFromSelfEye);
  IGNORE_FUNC(ozStrHeadingFromSelfEye);
  IGNORE_FUNC(ozStrRelHeadingFromSelfEye);
  IGNORE_FUNC(ozStrPitchFromSelfEye);
  IGNORE_FUNC(ozStrIsVisibleFromSelf);
  IGNORE_FUNC(ozStrIsVisibleFromSelfEye);

  /*
   * Entity
   */

  IMPORT_FUNC(ozEntGetState);
  IMPORT_FUNC(ozEntSetState);
  IMPORT_FUNC(ozEntGetLock);
  IMPORT_FUNC(ozEntSetLock);
  IMPORT_FUNC(ozEntTrigger);

  IMPORT_FUNC(ozEntOverlaps);
  IMPORT_FUNC(ozEntBindOverlaps);

  IGNORE_FUNC(ozEntVectorFromSelf);
  IGNORE_FUNC(ozEntVectorFromSelfEye);
  IGNORE_FUNC(ozEntDirFromSelf);
  IGNORE_FUNC(ozEntDirFromSelfEye);
  IGNORE_FUNC(ozEntDistFromSelf);
  IGNORE_FUNC(ozEntDistFromSelfEye);
  IGNORE_FUNC(ozEntHeadingFromSelfEye);
  IGNORE_FUNC(ozEntRelHeadingFromSelfEye);
  IGNORE_FUNC(ozEntPitchFromSelfEye);
  IGNORE_FUNC(ozEntIsVisibleFromSelf);
  IGNORE_FUNC(ozEntIsVisibleFromSelfEye);

  /*
   * Object
   */

  IMPORT_FUNC(ozClassDim);

  IMPORT_FUNC(ozBindObj);
  IGNORE_FUNC(ozBindSelf);
  IGNORE_FUNC(ozBindUser);
  IMPORT_FUNC(ozBindNextObj);

  IMPORT_FUNC(ozObjIsNull);
  IGNORE_FUNC(ozObjIsSelf);
  IGNORE_FUNC(ozObjIsUser);
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
  IGNORE_FUNC(ozObjReportStatus);
  IMPORT_FUNC(ozObjDamage);
  IMPORT_FUNC(ozObjDestroy);

  IMPORT_FUNC(ozObjOverlaps);
  IMPORT_FUNC(ozObjBindOverlaps);

  IGNORE_FUNC(ozObjVectorFromSelf);
  IGNORE_FUNC(ozObjVectorFromSelfEye);
  IGNORE_FUNC(ozObjDirFromSelf);
  IGNORE_FUNC(ozObjDirFromSelfEye);
  IGNORE_FUNC(ozObjDistFromSelf);
  IGNORE_FUNC(ozObjDistFromSelfEye);
  IGNORE_FUNC(ozObjHeadingFromSelfEye);
  IGNORE_FUNC(ozObjRelHeadingFromSelfEye);
  IGNORE_FUNC(ozObjPitchFromSelfEye);
  IGNORE_FUNC(ozObjIsVisibleFromSelf);
  IGNORE_FUNC(ozObjIsVisibleFromSelfEye);

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

  IMPORT_FUNC(ozBotMaxStamina);
  IMPORT_FUNC(ozBotGetStamina);
  IMPORT_FUNC(ozBotSetStamina);
  IMPORT_FUNC(ozBotAddStamina);

  IMPORT_FUNC(ozBotGetCargo);
  IMPORT_FUNC(ozBotGetWeaponItem);
  IMPORT_FUNC(ozBotSetWeaponItem);

  IMPORT_FUNC(ozBotCanReachEntity);
  IMPORT_FUNC(ozBotCanReachObj);

  IMPORT_FUNC(ozBotAction);
  IMPORT_FUNC(ozBotClearActions);

  IMPORT_FUNC(ozBotHeal);
  IMPORT_FUNC(ozBotRearm);
  IMPORT_FUNC(ozBotKill);

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

  IGNORE_FUNC(ozFragVectorFromSelf);
  IGNORE_FUNC(ozFragVectorFromSelfEye);
  IGNORE_FUNC(ozFragDirFromSelf);
  IGNORE_FUNC(ozFragDirFromSelfEye);
  IGNORE_FUNC(ozFragDistFromSelf);
  IGNORE_FUNC(ozFragDistFromSelfEye);
  IGNORE_FUNC(ozFragHeadingFromSelfEye);
  IGNORE_FUNC(ozFragRelHeadingFromSelfEye);
  IGNORE_FUNC(ozFragPitchFromSelfEye);
  IGNORE_FUNC(ozFragIsVisibleFromSelf);
  IGNORE_FUNC(ozFragIsVisibleFromSelfEye);

  /*
   * Mind's bot
   */

  IGNORE_FUNC(ozSelfIsCut);

  IGNORE_FUNC(ozSelfGetIndex);
  IGNORE_FUNC(ozSelfGetPos);
  IGNORE_FUNC(ozSelfGetDim);
  IGNORE_FUNC(ozSelfHasFlag);
  IGNORE_FUNC(ozSelfGetHeading);
  IGNORE_FUNC(ozSelfGetClassName);
  IGNORE_FUNC(ozSelfMaxLife);
  IGNORE_FUNC(ozSelfGetLife);
  IGNORE_FUNC(ozSelfDefaultResistance);
  IGNORE_FUNC(ozSelfGetResistance);

  IGNORE_FUNC(ozSelfBindItems);
  IGNORE_FUNC(ozSelfBindItem);

  IGNORE_FUNC(ozSelfGetParent);

  IGNORE_FUNC(ozSelfGetVelocity);
  IGNORE_FUNC(ozSelfGetMomentum);
  IGNORE_FUNC(ozSelfGetMass);
  IGNORE_FUNC(ozSelfGetLift);

  IGNORE_FUNC(ozSelfGetName);
  IGNORE_FUNC(ozSelfGetMind);
  IGNORE_FUNC(ozSelfHasState);
  IGNORE_FUNC(ozSelfGetEyePos);
  IGNORE_FUNC(ozSelfGetH);
  IGNORE_FUNC(ozSelfSetH);
  IGNORE_FUNC(ozSelfAddH);
  IGNORE_FUNC(ozSelfGetV);
  IGNORE_FUNC(ozSelfSetV);
  IGNORE_FUNC(ozSelfAddV);
  IGNORE_FUNC(ozSelfGetDir);

  IGNORE_FUNC(ozSelfGetStamina);
  IGNORE_FUNC(ozSelfMaxStamina);

  IGNORE_FUNC(ozSelfGetCargo);
  IGNORE_FUNC(ozSelfGetWeaponItem);
  IGNORE_FUNC(ozSelfSetWeaponItem);

  IGNORE_FUNC(ozSelfCanReachEntity);
  IGNORE_FUNC(ozSelfCanReachObj);

  IGNORE_FUNC(ozSelfAction);
  IGNORE_FUNC(ozSelfClearActions);

  IGNORE_FUNC(ozSelfBindItems);
  IGNORE_FUNC(ozSelfBindItem);

  IGNORE_FUNC(ozSelfOverlaps);
  IGNORE_FUNC(ozSelfBindOverlaps);

  /*
   * Mind
   */

  IGNORE_FUNC(ozMindGetSide);
  IGNORE_FUNC(ozMindSetSide);

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

  /*
   * Camera
   */

  IMPORT_FUNC(ozCameraMoveTo);
  IMPORT_FUNC(ozCameraWarpTo);

  IMPORT_FUNC(ozCameraGetBot);
  IMPORT_FUNC(ozCameraSetBot);
  IMPORT_FUNC(ozCameraAddSwitchableBot);
  IMPORT_FUNC(ozCameraClearSwitchableBots);
  IMPORT_FUNC(ozCameraAllowReincarnation);
  IMPORT_FUNC(ozCameraSetState);
  IMPORT_FUNC(ozCameraExecuteSequence);

  /*
   * Profile
   */

  IMPORT_FUNC(ozProfileGetName);
  IMPORT_FUNC(ozProfileSetName);
  IMPORT_FUNC(ozProfileGetClass);
  IMPORT_FUNC(ozProfileSetClass);
  IMPORT_FUNC(ozProfileGetItems);
  IMPORT_FUNC(ozProfileSetItems);
  IMPORT_FUNC(ozProfileGetWeaponItem);
  IMPORT_FUNC(ozProfileSetWeaponItem);

  /*
   * UI
   */

  IMPORT_FUNC(ozUIBell);
  IMPORT_FUNC(ozUIBuildFrame);

  importMatrixConstants(l);
  importNirvanaConstants(l);
  importClientConstants(l);

  // Import profile persistance.
  readValue(l, profile.persistent);
  l_setglobal("ozPersistent");

  loadDir("@lua/common");
  loadDir("@lua/client");

  hard_assert(l_gettop() == 0);

  Log::printEnd(" OK");
}

void LuaClient::destroy()
{
  if (l == nullptr) {
    return;
  }

  Log::print("Destroying Client Lua ...");

  l_getglobal("ozPersistent");
  profile.persistent = writeValue(l);

  ms.structs.clear();
  ms.structs.trim();

  ms.objects.clear();
  ms.objects.trim();

  cs.mission = "";
  cs.missionLingua.clear();

  Lua::destroy();

  Log::printEnd(" OK");
}

LuaClient luaClient;

}
}
