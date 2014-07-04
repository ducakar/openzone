-- OpenZone - simple cross-platform FPS/RTS game engine.
--
-- Copyright © 2002-2014 Davorin Učakar
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- mission/01_cvicek/main.lua
--
-- Cvicek mission.

oz_ticks = 0
oz_beasts = {}

function test_escapeQuest()
  if oz_me == -1 then
    ozQuestEnd(oz_escapeQuest, false)
    oz_escapeQuest = nil
    return
  end

  ozBindObj(oz_me)

  if ozBotHasState(OZ_BOT_DEAD_BIT) then
    ozQuestEnd(oz_escapeQuest, false)
    oz_escapeQuest = nil
    return
  end

  local x, y, z = ozObjGetPos()
  local dx, dy = x - 786.50, y - 999.60

  if dx*dx + dy*dy > 10000.0 then
    ozQuestEnd(oz_escapeQuest, true)
    oz_escapeQuest = nil
  end
end

function test_cvicekQuest()
  ozOrbisBindOverlaps(OZ_OBJECTS_BIT, -1015, 608, 50, 50, 50, 50)

  local nBottles = 0
  while ozBindNextObj() do
    if ozObjGetClassName() == "cvicek" then
      nBottles = nBottles + 1
    end
  end

  if nBottles >= 20 then
    ozQuestEnd(oz_cvicekQuest, true)
    oz_cvicekQuest = nil

    for i = 1, 5 do
      if oz_beasts[i] ~= -1 then
        ozBindObj(oz_beasts[i])
        ozBotSetMind("beast_happy")
      end
    end

    for i = 1, 4 do
      if oz_beasts[i] ~= -1 then
        ozNirvanaRemoveDevice(oz_beasts[i])
      end
    end

    if oz_beasts[1] ~= -1 then
      ozNirvanaAddMemo(oz_beasts[1], ozGettext("Good job!"))
    end

    if oz_beasts[2] ~= -1 then
      ozNirvanaAddMemo(oz_beasts[2], ozGettext("Yeeha! Cviček!"))
    end

    if oz_beasts[3] ~= -1 then
      ozNirvanaAddMemo(oz_beasts[3], "Več kot spijemo, boljši smo\nin lepše zgledamo!")
    end

    if oz_beasts[4] ~= -1 then
      ozNirvanaAddMemo(oz_beasts[4], ozGettext("No more thirst!"))
    end
  end
end

function test_farmQuest()
  ozOrbisBindOverlaps(OZ_STRUCTS_BIT, 82.5, -25.0, 70.0, 5.0, 5.0, 5.0)

  if not ozBindNextStr() then
    ozQuestEnd(oz_farmQuest, true)
    oz_farmQuest = nil

    if oz_beasts[5] ~= -1 then
      ozNirvanaRemoveDevice(oz_beasts[5])

      ozNirvanaAddMemo(oz_beasts[5], ozGettext("Good riddance! Finally some decent TV programme."))
    end
  end
end

function onUpdate()
  -- clear references
  if oz_me ~= -1 then
    ozBindObj(oz_me)

    if ozObjIsNull() then
      oz_me = -1
    end
  end

  for i = 1, 5 do
    if oz_beasts[i] ~= -1 then
      ozBindObj(oz_beasts[i])

      if ozObjIsNull() then
        oz_beasts[i] = -1
      end
    end
  end

  if oz_escapeQuest and oz_ticks == 50 then
    test_escapeQuest()
  end

  if oz_cvicekQuest and oz_ticks == 100 then
    test_cvicekQuest()
  end

  if oz_farmQuest and oz_ticks == 150 then
    test_farmQuest()
  end

  if oz_ticks == 150 then
    oz_ticks = 0
  end
  oz_ticks = oz_ticks + 1
end

function onCreate()
  ozTerraLoad("sample")

  ozCaelumLoad("earth")
  ozCaelumSetHeading(math.pi / 8)
  ozCaelumSetPeriod(2400)
  ozCaelumSetTime(450)

  oz_me = ozOrbisAddObj(OZ_FORCE, "beast", 786.50, 999.60, 124.73, true)
  ozBotSetName(ozProfileGetName())
  ozBotSetMind("")
  ozBotSetH(270)

  -- outpost
  ozOrbisAddStr(OZ_FORCE, "bunker", 788.0, 1000.0, 125, OZ_SOUTH)

  ozOrbisAddObj(OZ_FORCE, "metalCrate", 790.56, 1002.59, 124.33)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", 789.65, 1002.89, 124.03)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", 790.35, 1002.69, 125.24)

  ozOrbisAddObj(OZ_FORCE, "bomb", 789.50, 1002.00, 127.91)
  ozOrbisAddObj(OZ_FORCE, "bomb", 789.50, 1001.00, 127.91)

  ozOrbisAddObj(OZ_FORCE, "droid", 780.15, 1004.64, 123.39)
  ozObjAddItem("droid$chaingun")
  ozBotSetH(180.0)
  ozBotSetMind("droid_guard")
  ozBotSetWeaponItem(3)

  ozOrbisAddObj(OZ_FORCE, "droid", 798.43, 995.40, 124.80)
  ozBotSetH(90.0)
  ozBotSetMind("droid_guard")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", 810.41, 989.71, 124.33)
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("musicPlayer")
  ozObjAddItem("musicPlayer")
  ozObjAddItem("galileo")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", 810.04, 991.14, 124.51)
  ozObjAddItem("binoculars")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  ozOrbisAddObj(OZ_FORCE, "metalBarrel", 811.61, 990.85, 124.47)
  ozOrbisAddObj(OZ_FORCE, "hoverTank", 815.22, 991.62, 124.76)

  -- military base
  ozOrbisAddStr(OZ_FORCE, "tower", 2.0, -926.0, 145.7, OZ_WEST)
  ozOrbisAddStr(OZ_FORCE, "bunker", -35.0, -940.0, 138.6, OZ_NORTH)
  ozOrbisAddStr(OZ_FORCE, "bunker", -20.0, -940.0, 138.6, OZ_NORTH)
  ozOrbisAddStr(OZ_FORCE, "bunker", -20.0, -915.0, 138.5, OZ_SOUTH)

  ozOrbisAddObj(OZ_FORCE, "bigCrate", -33.32, -942.53, 137.93)
  ozObjAddItem("droid$chaingun")
  ozObjAddItem("droid$chaingun")
  ozObjAddItem("droid$chaingun")
  ozObjAddItem("droid$grenadeLauncher")
  ozObjAddItem("droid$grenadeLauncher")

  ozOrbisAddObj(OZ_FORCE, "bigCrate", -34.59, -942.09, 137.93)
  ozObjAddItem("droid$blaster")
  ozObjAddItem("droid$chaingun")
  ozObjAddItem("droid$chaingun")

  ozOrbisAddObj(OZ_FORCE, "bigCrate", -34.14, -942.32, 139.14)
  ozObjAddItem("droid$hyperblaster")
  ozObjAddItem("droid$hyperblaster")

  ozOrbisAddObj(OZ_FORCE, "smallCrate", -33.20, -942.65, 138.84)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", -32.26, -937.67, 137.63)

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -37.57, -939.23, 137.93)
  ozObjAddItem("musicPlayer")
  ozObjAddItem("nvGoggles")
  ozObjAddItem("binoculars")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -22.53, -941.39, 137.93)
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -22.26, -940.07, 137.93)
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")

  ozOrbisAddObj(OZ_FORCE, "cvicek", -17.27, -940.64, 138.79)
  ozOrbisAddObj(OZ_FORCE, "cvicek", -22.79, -941.41, 138.79)
  ozOrbisAddObj(OZ_FORCE, "cvicek", -22.45, -939.19, 137.58)

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -17.61, -940.49, 137.93)
  ozObjAddItem("cvicek")
  ozObjAddItem("cvicek")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -17.61, -942.49, 137.93)
  ozObjAddItem("galileo")
  ozObjAddItem("musicPlayer")
  ozObjAddItem("nvGoggles")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  ozOrbisAddObj(OZ_FORCE, "metalBarrel", -17.60, -914.44, 137.83)
  ozOrbisAddObj(OZ_FORCE, "metalBarrel", -17.55, -915.66, 137.83)
  ozOrbisAddObj(OZ_FORCE, "metalBarrel", -18.23, -915.13, 137.83)

  ozOrbisAddObj(OZ_FORCE, "metalCrate", -22.30, -912.60, 137.83)
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  ozOrbisAddObj(OZ_FORCE, "serviceStation", -30.00, -916.00, 139.30)

  ozOrbisAddObj(OZ_FORCE, "raptor", -35.0, -925.0, 140.0)
  ozVehicleSetH(90.0)

  ozOrbisAddObj(OZ_FORCE, "droid", 2.00, -924.00, 151.29)
  ozBotSetMind("droid_sniper")

  ozOrbisAddObj(OZ_FORCE, "droid", -36.83, -937.20, 142.27)
  ozBotSetMind("droid_sniper")

  ozOrbisAddObj(OZ_FORCE, "droid", -24.16, -945.10, 138.14)
  ozObjAddItem("droid$chaingun")
  ozBotSetMind("droid_guard")
  ozBotSetH(0)
  ozBotSetWeaponItem(3)

  ozOrbisAddObj(OZ_FORCE, "droid", -58.43, -911.93, 135.26)
  ozBotSetMind("droid_patrol")
  ozBotSetH(0)

  ozOrbisAddObj(OZ_FORCE, "droid", 54.03, -945.41, 145.68)
  ozBotSetMind("droid_patrol")
  ozBotSetH(0)

  ozOrbisAddObj(OZ_FORCE,  "droid", -22.47, -917.67, 138.33)
  ozObjAddItem("droid$grenadeLauncher")
  ozBotSetMind("droid_guard")
  ozBotSetH(270)
  ozBotSetWeaponItem(3)

  ozOrbisAddObj(OZ_FORCE, "droid.OOM-9", 0.44, -924.49, 148.73)
  ozBotSetMind("droid_guard")
  ozBotSetH(90)

  ozOrbisAddObj(OZ_FORCE, "droid.OOM-9", -37.53, -940.69, 138.43)
  ozBotSetMind("droid_guard")
  ozBotSetH(340)

  local pilot = ozOrbisAddObj(OZ_FORCE, "droid.OOM-9", 0, 0, 0)
  ozBotSetMind("droid_armouredPatrol")
  ozOrbisAddObj(OZ_FORCE, "hoverTank", -40.00, -860.00, 138.00)
  ozVehicleEmbarkBot(pilot)

  -- kmetija
  ozOrbisAddStr(OZ_FORCE, "house", 80.0, -20.0, 70.2, OZ_EAST)
  ozOrbisAddStr(OZ_FORCE, "house", 85.0, -30.0, 69.4, OZ_NORTH)

  ozOrbisAddObj(OZ_FORCE, "goblin", 80.0, -20.0, 70.0)
  ozBotSetMind("goblin_guard")

  ozOrbisAddObj(OZ_FORCE, "goblin", 85.0, -10.0, 70.0)
  ozBotSetMind("goblin_guard")

  ozOrbisAddObj(OZ_FORCE, "goblin", 82.0, -26.0, 70.0)
  ozBotSetMind("goblin_guard")
  ozBotSetH(120)

  ozOrbisAddObj(OZ_FORCE, "goblin", 90.0, -43.0, 69.0)
  ozBotSetMind("goblin_guard")
  ozBotSetH(230)

  -- lair
  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1014.0, 620.0, 52.0, OZ_SOUTH)
  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1029.0, 610.0, 51.5, OZ_EAST)
  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1010.0, 595.0, 53.0, OZ_NORTH)

  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1022.30, 596.59, 51.54)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1021.96, 597.80, 51.45)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1023.41, 596.91, 50.29)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", -1022.48, 596.59, 50.63)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", -1023.38, 597.93, 50.52)
  ozOrbisAddObj(OZ_FORCE, "metalCrate", -1021.95, 597.90, 50.54)
  ozObjAddItem("musicPlayer")
  ozObjAddItem("musicPlayer")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("nvGoggles")
  ozObjAddItem("galileo")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  oz_beasts[1] = ozOrbisAddObj(OZ_FORCE, "beast", -1005.96, 603.80, 51.10)
  ozBotSetH(20)
  ozBotSetMind("beast_nervous")

  oz_beasts[2] = ozOrbisAddObj(OZ_FORCE, "beast", -1022.94, 600.36, 50.76, true)
  ozBotSetH(280)
  ozBotSetMind("beast_nervous")

  oz_beasts[3] = ozOrbisAddObj(OZ_FORCE, "beast", -1014.63, 618.34, 54.93, true)
  ozBotSetH(220)
  ozBotSetMind("beast_nervous")

  oz_beasts[4] = ozOrbisAddObj(OZ_FORCE, "beast", -1008.27, 596.49, 52.09, true)
  ozBotSetH(10)
  ozBotSetMind("beast_nervous")

  oz_beasts[5] = ozOrbisAddObj(OZ_FORCE, "beast", -1021.64, 601.85, 50.76, true)
  ozBotSetH(200)
  ozBotSetMind("beast_nervous")

  ozNirvanaAddMemo(oz_beasts[1], ozGettext("Well, is cviček coming any time soon?"))

  ozNirvanaAddMemo(oz_beasts[2], ozGettext("I'm sooo thirsty. I've been drinking only water " ..
                                           "for the past week."))

  ozNirvanaAddMemo(oz_beasts[3], ozGettext("Cviček, cviček, cviček ... Where is cviček?"))

  ozNirvanaAddMemo(oz_beasts[4], ozGettext("How's your mission going? I can't imagine how we " ..
                                           "will live without cviček."))

  ozNirvanaAddMemo(oz_beasts[5], ozGettext("Is the Farm still standing? Please destroy it, " ..
                                           "maybe then there might be something better on TV."))

  oz_escapeQuest = ozQuestAdd(ozGettext("Escape"), ozGettext(
    "You have been caught by a Colonial patrol and they currently hold you in that Colonial " ..
    "outpost. Tomorrow you are being transferred to a Colonial detention camp for terrorist " ..
    "suspects. If you don't find a way to escape, say goodbye to freedom till the end of your " ..
    "life.\n\n" ..
    "It would be extremely helpful if you can get a Galileo gadget, it will will show you " ..
    "destinations of your tasks and help you with navigation a lot."),
    785, 1000, 124,
    OZ_QUEST_PENDING)

  oz_cvicekQuest = ozQuestAdd(ozGettext("Steal cviček"), ozGettext(
    "Steal at least 20 bottles of cviček from Colonial military base and transport them back " ..
    "to Beast Lair.\n\n" ..
    "The base is heavily defended so you will need some heavy armour to attack it.\n\n" ..
    "Beware not to use explosives there, cviček bottles are very fragile and there is a " ..
    "explosives storage in that base.\n\nQuest will be complete one you unload 20 bottles of " ..
    "cviček in the Lair (bottles inside an inventory, a box or a vehicle don't count)."),
    20, -940, 140,
    OZ_QUEST_PENDING)

  oz_farmQuest = ozQuestAdd(ozGettext("The Farm"), ozGettext(
    "That farm is a stage of an annoying reality show called 'The Farm'. It wouldn't do much " ..
    "harm to blow it up.\n\nThe farmhouses are not very robust, so a few bombs or a heavier " ..
    "weapon should do the trick. And don't expect any welcome from participants of the " ..
    "show."),
    80, -20, 70, OZ_QUEST_PENDING)

  ozQuestAdd(ozGettext("Beast Lair"), ozGettext(
    "Beast Lair is your home base, hidden on an island, still safe from Colonial Army.\n\n" ..
    "However, supplies are low and there's a terrible shortage of beasts' favourite drink, " ..
    "cviček.\n\n" ..
    "You can visit the base anytime during the mission for any supplies you need."),
    -1015, 608, 50,
    OZ_QUEST_NONE)

  floraSeed(0.0015, 12, 0.75)

  ozCameraAllowReincarnation(false)
  ozCameraAddSwitchableBot(oz_me)
  ozCameraSetBot(oz_me)

  ozCameraWarpTo(-600, 608, 80, 90.0, 90.0)
  ozCameraExecuteSequence("intro")
end
