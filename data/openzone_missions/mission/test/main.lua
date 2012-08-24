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
 * mission/test.lua
 *
 * Test world.
]]--

function onUpdate()
  ozUIShowBuild( true )
end

function onCreate()
--   ozOrbisSetGravity( -6.0 )

  ozTerraLoad( "terra" )

  ozCaelumLoad( "earth" )
  ozCaelumSetHeading( math.pi / 8 )
  ozCaelumSetPeriod( 1440 )
--   ozCaelumSetRealTime()

  local me = ozOrbisAddPlayer( OZ_FORCE, 137, -6, 73, OZ_SOUTH )
--   local me = ozOrbisAddPlayer( OZ_FORCE, 2000, -2002, 100, OZ_SOUTH )
  ozCameraAddSwitchableUnit( me )
  ozCameraSwitchTo( me )

--   ozOrbisAddStr( OZ_FORCE, "testCastle", 2000, -2000, 100, OZ_NORTH )

  init_sample()

  ozFloraSeed()

  init_stressTest()
end

function init_sample()
  local lord = ozOrbisAddObj( OZ_FORCE, "bauul", 138, -11, 73, OZ_EAST )

  local memo = ozOrbisAddObj( OZ_FORCE, "datacube", 134, -6, 73 )
  ozNirvanaAddMemo( memo, "Simulations are like miniskirts, they show a lot but hide the " ..
                          "essentials.\n\n        -- Hubert Kirrman" )
  ozNirvanaAddMemo( lord, "Buahahahaha" )

  local droid  = ozOrbisAddObj( OZ_FORCE, "droid.OOM-9", 140, -11, 73, OZ_WEST )
  local cyborg = ozOrbisAddObj( OZ_FORCE, "cyborg", 140, -13, 73, OZ_WEST )
  ozBotSetMind( "" )

  ozCameraAddSwitchableUnit( cyborg )
  ozCameraAddSwitchableUnit( droid )

--   ozOrbisAddObj( "reaper", 145, -15, 73 )

  ozOrbisAddObj( OZ_FORCE, "musicPlayer", 135, -4, 73 )
  ozOrbisAddObj( OZ_FORCE, "galileo", 136, -4, 73 )
  ozOrbisAddObj( OZ_FORCE, "nvGoggles", 137, -4, 73 )
  ozOrbisAddObj( OZ_FORCE, "cvicek", 135, -2, 73 )
  ozOrbisAddObj( OZ_FORCE, "firstAid", 136, -2, 73 )
  ozOrbisAddObj( OZ_FORCE, "binoculars", 137, -2, 73 )
  ozOrbisAddObj( OZ_FORCE, "droid_weapon.hyperblaster", 135, -3, 73 )
  ozOrbisAddObj( OZ_FORCE, "droid_weapon.chaingun", 136, -3, 73 )
  ozOrbisAddObj( OZ_FORCE, "droid_weapon.grenadeLauncher", 137, -3, 73 )

  ozOrbisAddObj( OZ_FORCE, "key", 160, 3, 73 )

  ozOrbisAddObj( OZ_FORCE, "raptor", 155, -30, 71 )
  ozOrbisAddObj( OZ_FORCE, "hoverTank", 145, -30, 71 )

  ozOrbisAddObj( OZ_FORCE, "metalBarrel", 147, -11, 72 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", 147, -11, 74 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", 147, -11, 76 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 11, 72 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 11, 73 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 11, 74 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 11, 75 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 11, 76 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 12, 72 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 12, 73 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 12, 74 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 12, 75 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 147, 12, 76 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 11, 72 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 11, 73 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 11, 74 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 11, 75 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 11, 76 )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 12, 72 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 12, 73 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 12, 74 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 12, 75 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 148, 12, 76 )

  ozOrbisGenFrags( "ruins", 100, 146, 10, 79, 149, 13, 79, 0, 0, 2 )

  ozOrbisAddObj( OZ_FORCE, "box", 140, -2, 72 )
  ozOrbisAddObj( OZ_FORCE, "bigCrate", 140, -7, 72 )
  ozOrbisAddObj( OZ_FORCE, "metalCrate", 140, -4, 72 )
  ozOrbisAddObj( OZ_FORCE, "metalBarrel", 140, -5.5, 72 )

  ozOrbisAddStr( OZ_FORCE, "testCastle", 143, 0, 79, OZ_NORTH )
  ozOrbisAddStr( OZ_FORCE, "pool", 112, -7, 70 )
  ozOrbisAddStr( OZ_FORCE, "tower", 106, -7, 75, OZ_WEST )
  ozOrbisAddStr( OZ_FORCE, "atlantis", -160, 220, -5 )
  ozOrbisAddStr( OZ_FORCE, "house", 126, -30, 70.5 )

  ozOrbisAddObj( OZ_FORCE, "serviceStation", 110, -25, 69.6, OZ_WEST )
  ozOrbisAddObj( OZ_FORCE, "console", 107, -24, 68.0, OZ_EAST )

  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -6, 81 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -6, 82 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -6, 83 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -8, 81 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -8, 82 )
  ozOrbisAddObj( OZ_FORCE, "smallCrate", 108, -8, 83 )
end

function init_stressTest()
  for i = 1, 0 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "goblin", x, y, z + 0.75 )
    end
  end

  for i = 1, 0 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "knight", x, y, z + 1.10 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "beast", x, y, z + 1.10 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "explosiveBarrel", x, y, z + 0.70 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( OZ_TRY, "smallCrate", x, y, z + 0.40 )
    end
  end
end
