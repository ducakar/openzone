--[[
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2012  Davorin Učakar
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
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
]]--

--[[
 * mission/cvicek.lua
 *
 * Sample mission.
]]--

oz_ticks = 0
oz_beasts = {}

function test_escapeQuest()
  if oz_me == -1 then
    ozQuestEnd( oz_escapeQuest, false )
    oz_escapeQuest = nil
    return
  end

  ozObjBindIndex( oz_me )

  if ozBotGetState( OZ_BOT_DEAD_BIT ) then
    ozQuestEnd( oz_escapeQuest, false )
    oz_escapeQuest = nil
    return
  end

  local x, y, z = ozObjGetPos()
  local dx, dy = x - 786.50, y - 999.60

  if dx*dx + dy*dy > 10000.0 then
    ozQuestEnd( oz_escapeQuest, true )
    oz_escapeQuest = nil
  end
end

function test_cvicekQuest()
  ozOrbisBindObjOverlaps( -1015, 608, 50, 50, 50, 50 )

  local nBottles = 0
  while ozObjBindNext() do
    if ozObjGetClassName() == "cvicek" then
      nBottles = nBottles + 1
    end
  end

  if nBottles >= 20 then
    ozQuestEnd( oz_cvicekQuest, true )
    oz_cvicekQuest = nil

    for i = 1, 5 do
      if oz_beasts[i] ~= -1 then
        ozObjBindIndex( oz_beasts[i] )
        ozBotSetMindFunc( "beast_happy" )
      end
    end

    for i = 1, 4 do
      if oz_beasts[i] ~= -1 then
        ozNirvanaRemoveDevice( oz_beasts[i] )
      end
    end

    if oz_beasts[1] ~= -1 then
      ozNirvanaAddMemo( oz_beasts[1],
        ozGettext( "Good job!" )
      )
    end

    if oz_beasts[2] ~= -1 then
      ozNirvanaAddMemo( oz_beasts[2],
        ozGettext( "Yeeha! Cvicek!" )
      )
    end

    if oz_beasts[3] ~= -1 then
      ozNirvanaAddMemo( oz_beasts[3],
        "Več kot spijemo, boljši smo\nin lepše zgledamo!"
      )
    end

    if oz_beasts[4] ~= -1 then
      ozNirvanaAddMemo( oz_beasts[4],
        ozGettext( "No more thirst!" )
      )
    end
  end
end

function test_farmQuest()
  ozOrbisBindStrOverlaps( 82.5, -25.0, 70.0, 5.0, 5.0, 5.0 )

  if not ozStrBindNext() then
    ozQuestEnd( oz_farmQuest, true )
    oz_farmQuest = nil

    if oz_beasts[5] ~= -1 then
      ozNirvanaRemoveDevice( oz_beasts[5] )

      ozNirvanaAddMemo( oz_beasts[5],
        ozGettext( "Good riddance! Finally some decent TV programme." )
      )
    end
  end
end

function onUpdate()
  -- clear references
  if oz_me ~= -1 then
    ozObjBindIndex( oz_me )

    if ozObjIsNull() then
      oz_me = -1
    end
  end

  for i = 1, 5 do
    if oz_beasts[i] ~= -1 then
      ozObjBindIndex( oz_beasts[i] )

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
  ozTerraLoad( "terra" )

  ozCaelumLoad( "earth" )
  ozCaelumSetHeading( math.pi / 8 )
  ozCaelumSetPeriod( 1440 )
  ozCaelumSetTime( 180 )

  oz_me = ozOrbisAdd( "beast", 786.50, 999.60, 124.23 )

  -- outpost
  ozOrbisAddStr( "bunker", 788.0, 1000.0, 124.5, OZ_NORTH )

  ozOrbisAddObj( "metalCrate", 790.56, 1002.59, 123.83 )
  ozOrbisAddObj( "smallCrate", 789.65, 1002.89, 123.53 )
  ozOrbisAddObj( "smallCrate", 790.35, 1002.69, 124.74 )

  ozOrbisAddObj( "bomb", 789.50, 1002.00, 127.41 )
  ozOrbisAddObj( "bomb", 789.50, 1001.00, 127.41 )
  ozOrbisAddObj( "bomb", 789.50, 1000.00, 127.41 )

  local items = { ozOrbisAddObj( "droid_weapon.chaingun", 0, 0, 0 ) }
  ozOrbisAddObj( "droid", 780.15, 1004.64, 123.39 )
  ozObjAddItem( items[1] )
  ozBotSetH( 180.0 )
  ozBotSetMindFunc( "droid_guard" )
  ozBotSetWeaponItem( 3 )

  ozOrbisAddObj( "droid", 798.43, 995.40, 124.80 )
  ozBotSetH( 90.0 )
  ozBotSetMindFunc( "droid_guard" )

  local items = {
    ozOrbisAddObj( "beast_weapon.plasmagun", 0, 0, 0 ),
    ozOrbisAddObj( "musicPlayer", 0, 0, 0 ),
    ozOrbisAddObj( "galileo", 0, 0, 0 ),
    ozOrbisAddObj( "firstAid", 0, 0, 0 ),
    ozOrbisAddObj( "firstAid", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", 810.41, 989.71, 124.33 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  local items = {
    ozOrbisAddObj( "droid_weapon.hyperblaster", 0, 0, 0 ),
    ozOrbisAddObj( "droid_weapon.hyperblaster", 0, 0, 0 ),
    ozOrbisAddObj( "galileo", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", 810.04, 991.14, 124.51 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  ozOrbisAddObj( "metalBarrel", 811.61, 990.85, 124.47 )
  ozOrbisAddObj( "hoverTank", 815.22, 991.62, 124.76 )

  -- military base
  ozOrbisAddStr( "tower", 2.0, -926.0, 145.0, OZ_NORTH )
  ozOrbisAddStr( "bunker", -35.0, -940.0, 138.5, OZ_SOUTH )
  ozOrbisAddStr( "bunker", -20.0, -940.0, 138.5, OZ_SOUTH )
  ozOrbisAddStr( "bunker", -20.0, -915.0, 138.0, OZ_NORTH )

  ozOrbisAddObj( "bigCrate", -33.32, -942.53, 137.83 )
  ozOrbisAddObj( "bigCrate", -34.59, -942.09, 137.83 )
  ozOrbisAddObj( "bigCrate", -34.14, -942.32, 139.04 )
  ozOrbisAddObj( "smallCrate", -33.20, -942.65, 138.74 )
  ozOrbisAddObj( "smallCrate", -32.26, -937.67, 137.53 )

  local items = { ozOrbisAddObj( "musicPlayer", 0, 0, 0 ) }
  ozOrbisAddObj( "metalCrate", -37.57, -939.23, 137.83 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  local items = {
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", -22.53, -941.39, 137.83 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  local items = {
    ozOrbisAddObj( "firstAid", 0, 0, 0 ),
    ozOrbisAddObj( "firstAid", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", -22.26, -940.07, 137.83 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  local items = {
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "cvicek", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", -17.61, -940.49, 137.83 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  local items = {
    ozOrbisAddObj( "galileo", 0, 0, 0 ),
    ozOrbisAddObj( "musicPlayer", 0, 0, 0 ),
    ozOrbisAddObj( "beast_weapon.plasmagun", 0, 0, 0 ),
    ozOrbisAddObj( "beast_weapon.plasmagun", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", -17.61, -942.49, 137.83 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  ozOrbisAddObj( "metalBarrel", -17.60, -914.44, 137.33 )
  ozOrbisAddObj( "metalBarrel", -17.55, -915.66, 137.33 )
  ozOrbisAddObj( "metalBarrel", -18.23, -915.13, 137.33 )

  local items = {
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 ),
    ozOrbisAddObj( "bomb", 0, 0, 0 )
  }
  ozOrbisAddObj( "metalCrate", -22.30, -912.60, 137.33 )
  for i = 1, #items do
    ozObjAddItem( items[i] )
  end

  ozOrbisAddObj( "serviceStation", -30.00, -916.00, 139.10 )

  ozOrbisAddObj( "raptor", -35.0, -925.0, 140.0 )
  ozVehicleSetH( 90.0 )

  ozOrbisAddObj( "droid", 2.00, -924.00, 150.59 )
  ozBotSetMindFunc( "droid_sniper" )

  ozOrbisAddObj( "droid", -36.83, -937.20, 142.17 )
  ozBotSetMindFunc( "droid_sniper" )

  local items = { ozOrbisAddObj( "droid_weapon.chaingun", 0, 0, 0 ) }
  ozOrbisAddObj( "droid", -24.16, -945.10, 138.04 )
  ozObjAddItem( items[1] )
  ozBotSetMindFunc( "droid_guard" )
  ozBotSetH( 0 )
  ozBotSetWeaponItem( 3 )

  ozOrbisAddObj( "droid", -58.43, -911.93, 135.16 )
  ozBotSetMindFunc( "droid_patrol" )
  ozBotSetH( 0 )

  ozOrbisAddObj( "droid", 54.03, -945.41, 145.58 )
  ozBotSetMindFunc( "droid_patrol" )
  ozBotSetH( 0 )

  local items = { ozOrbisAddObj( "droid_weapon.grenadeLauncher", 0, 0, 0 ) }
  ozOrbisAddObj( "droid", -22.47, -917.67, 137.83 )
  ozObjAddItem( items[1] )
  ozBotSetMindFunc( "droid_guard" )
  ozBotSetH( 270 )
  ozBotSetWeaponItem( 3 )

  ozOrbisAddObj( "droid.OOM-9", 0.44, -924.49, 148.03 )
  ozBotSetMindFunc( "droid_guard" )
  ozBotSetH( 90 )

  ozOrbisAddObj( "droid.OOM-9", -37.53, -940.69, 138.33 )
  ozBotSetMindFunc( "droid_guard" )
  ozBotSetH( 340 )

  local pilot = ozOrbisAddObj( "droid.OOM-9", 0, 0, 0 )
  ozBotSetMindFunc( "droid_armouredPatrol" )
  ozOrbisAddObj( "hoverTank", -40.00, -860.00, 138.00 )
  ozVehicleEmbarkPilot( pilot )

  -- kmetija
  ozOrbisAddStr( "house", 80.0, -20.0, 70.0, OZ_NORTH )
  ozOrbisAddStr( "house", 85.0, -30.0, 69.0, OZ_WEST )

  ozOrbisAddObj( "goblin", 80.0, -20.0, 70.0 )
  ozBotSetMindFunc( "goblin_defend" )

  ozOrbisAddObj( "goblin", 85.0, -10.0, 70.0 )
  ozBotSetMindFunc( "goblin_defend" )

  ozOrbisAddObj( "goblin", 82.0, -26.0, 70.0 )
  ozBotSetMindFunc( "goblin_defend" )
  ozBotSetH( 120 )

  ozOrbisAddObj( "goblin", 90.0, -43.0, 69.0 )
  ozBotSetMindFunc( "goblin_defend" )
  ozBotSetH( 230 )

  -- lair
  ozOrbisAddStr( "beastBunker", -1014.0, 620.0, 52.0, OZ_NORTH )
  ozOrbisAddStr( "beastBunker", -1029.0, 610.0, 51.5, OZ_WEST )
  ozOrbisAddStr( "beastBunker", -1010.0, 595.0, 52.3, OZ_SOUTH )

  local musicPlayer1 = ozOrbisAddObj( "musicPlayer", 0, 0, 0 )
  local musicPlayer2 = ozOrbisAddObj( "musicPlayer", 0, 0, 0 )
  local plasmagun1 = ozOrbisAddObj( "beast_weapon.plasmagun", 0, 0, 0 )
  local plasmagun2 = ozOrbisAddObj( "beast_weapon.plasmagun", 0, 0, 0 )
  local galileo = ozOrbisAddObj( "galileo", 0, 0, 0 )
  local firstAid1 = ozOrbisAddObj( "firstAid", 0, 0, 0 )
  local firstAid2 = ozOrbisAddObj( "firstAid", 0, 0, 0 )
  local firstAid3 = ozOrbisAddObj( "firstAid", 0, 0, 0 )
  local firstAid4 = ozOrbisAddObj( "firstAid", 0, 0, 0 )
  local bomb1 = ozOrbisAddObj( "bomb", 0, 0, 0 )
  local bomb2 = ozOrbisAddObj( "bomb", 0, 0, 0 )

  ozOrbisAddObj( "smallCrate", -1022.49, 596.83, 51.23 )
  ozOrbisAddObj( "smallCrate", -1021.96, 597.80, 51.16 )
  ozOrbisAddObj( "smallCrate", -1023.41, 596.91, 50.01 )
  ozOrbisAddObj( "bigCrate", -1022.41, 596.65, 50.32 )
  ozOrbisAddObj( "bigCrate", -1023.38, 597.93, 50.24 )
  ozOrbisAddObj( "metalCrate", -1021.95, 597.90, 50.24 )
  ozObjRemoveAllItems()
  ozObjAddItem( musicPlayer1 )
  ozObjAddItem( musicPlayer2 )
  ozObjAddItem( plasmagun1 )
  ozObjAddItem( plasmagun2 )
  ozObjAddItem( galileo )
  ozObjAddItem( firstAid1 )
  ozObjAddItem( firstAid2 )
  ozObjAddItem( firstAid3 )
  ozObjAddItem( firstAid4 )
  ozObjAddItem( bomb1 )
  ozObjAddItem( bomb2 )

  oz_beasts[1] = ozOrbisAddObj( "beast", -1005.96, 603.80, 50.40 )
  ozBotSetH( 20 )
  ozBotSetMindFunc( "beast_nervous" )

  oz_beasts[2] = ozOrbisAddObj( "beast", -1022.94, 600.36, 50.49 )
  ozObjRemoveAllItems()
  ozBotSetH( 280 )
  ozBotSetMindFunc( "beast_nervous" )

  oz_beasts[3] = ozOrbisAddObj( "beast", -1014.63, 618.34, 54.93 )
  ozObjRemoveAllItems()
  ozBotSetH( 220 )
  ozBotSetMindFunc( "beast_nervous" )

  oz_beasts[4] = ozOrbisAddObj( "beast", -1008.27, 596.49, 51.39 )
  ozObjRemoveAllItems()
  ozBotSetH( 10 )
  ozBotSetMindFunc( "beast_nervous" )

  oz_beasts[5] = ozOrbisAddObj( "beast", -1021.64, 601.85, 50.40 )
  ozObjRemoveAllItems()
  ozBotSetH( 200 )
  ozBotSetMindFunc( "beast_nervous" )

  ozNirvanaAddMemo( oz_beasts[1],
    ozGettext( "Well, is cvicek coming anytime soon?" )
  )

  ozNirvanaAddMemo( oz_beasts[2],
    ozGettext( "I'm sooo thirsty. I've been drinking only water for the past week." )
  )

  ozNirvanaAddMemo( oz_beasts[3],
    ozGettext( "Cvicek, cvicek, cvicek ... Where is cvicek?" )
  )

  ozNirvanaAddMemo( oz_beasts[4],
    ozGettext( "How's your mission going? I can't imagine how we will live without cvicek." )
  )

  ozNirvanaAddMemo( oz_beasts[5],
    ozGettext( "Is the Farm still standing? Please destroy it, maybe then there might be something better on TV." )
  )

  ozFloraSeed()

  oz_escapeQuest = ozQuestAdd(
    ozGettext( "Escape" ),
    ozGettext( "You have been caught by a Colonial patrol and they currently hold you in that Colonial outpost. Tomorrow you are being transferred to a Colonial detention camp for terrorist suspects. If you don't find a way to escape, say goodbye to freedom till the end of your life.\n\nIt would be extremely helpful if you can get a Galileo gadget, it will will show you destinations of your tasks and help you with navigation a lot." ),
    785, 1000, 124 )

  oz_cvicekQuest = ozQuestAdd(
    ozGettext( "Steal cvicek" ),
    ozGettext( "Steal at least 20 bottles of cvicek from Colonial military base and transport them back to Beast Lair.\n\nThe base is heavily defended so you will need some heavy armour to attack it.\n\nBeware not to use explosives there, cvicek bottles are very fragile and there is a explosives storage in that base.\n\nQuest will be complete one you unload 20 bottles of cvicek in the Lair." ),
    20, -940, 140 )

  oz_farmQuest = ozQuestAdd(
    ozGettext( "The Farm" ),
    ozGettext( "That farm is a stage of an annoying reality show called 'The Farm'. It wouldn't do much harm to blow it up.\n\nThe farmhouses are not very robust, so a dozen of bombs should be more then enough to demolish them. And don't expect any welcome from participants of the show." ),
    80, -20, 70 )

  local lairQuest = ozQuestAdd(
    ozGettext( "Beast Lair" ),
    ozGettext( "Beast Lair is your home base, hidden on an island, still safe from Colonial Army.\n\nHowever, supplies are low and there's a terrible shortage of beasts' favourite drink, cvicek.\n\nYou can visit the base anytime during the mission for any supplies you need." ),
    -1015, 608, 50 )
  ozQuestEnd( lairQuest, true )

  ozObjBindIndex( oz_me )
  ozBotSetName( ozProfileGetPlayerName() )
  ozBotSetMindFunc( "" )
  ozBotSetH( 270 )
  ozObjRemoveAllItems()

  ozCameraIncarnate( oz_me )
  ozCameraWarpTo( ozBotGetEyePos() )
  ozCameraAllowReincarnation( false )
end
