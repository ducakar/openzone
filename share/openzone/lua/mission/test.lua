--[[
 *  test.lua
 *  Test world
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function onUpdate()
end

function onCreate()
  ozTerraLoad( "terra" )

  ozCaelumLoad( "earth" )
  ozCaelumSetHeading( math.pi / 8 )
  ozCaelumSetPeriod( 1440 )
  ozCaelumSetTime( 180 )

  ozCameraWarpTo( 141.0, -12.0, 84.75 )

  local me = ozOrbisAddObj( "beast", 137, -6, 73, OZ_SOUTH )
  ozBotSetName( ozProfileGetPlayerName() )
  ozBotSetMindFunc( "" )

  local lord = ozOrbisAddObj( "bauul", 138, -11, 73, OZ_EAST )

  local memo = ozOrbisAddObj( "datacube", 134, -6, 73 )
  ozNirvanaAddMemo( memo, "ds a sda dasd a dw eqfeqwikfp weiofhowue hfuh weuiwhfuw h" )
  ozNirvanaAddMemo( lord, "Buahahahaha" )

  ozOrbisAddObj( "droid.OOM-9", 140, -11, 73, OZ_WEST )

  ozOrbisAddObj( "reaper", 145, -15, 73 )

  ozOrbisGenParts( 1000, 136, -3, 71, 0, 0, 10, 15, 0.4, 0.4, 0.4, 0.2, 1.95, 0.1, 5.0 )

  ozOrbisAddObj( "musicPlayer", 135, -2, 73 )
  ozOrbisAddObj( "galileo", 136, -2, 73 )
  ozOrbisAddObj( "cvicek", 135, -3, 73 )
  ozOrbisAddObj( "firstAid", 136, -3, 73 )
  ozOrbisAddObj( "bomb", 137, -3, 73 )
  ozOrbisAddObj( "droid_weapon.hyperblaster", 135, -4, 73 )
  ozOrbisAddObj( "droid_weapon.chaingun", 136, -4, 73 )
  ozOrbisAddObj( "droid_weapon.grenadeLauncher", 137, -4, 73 )

  ozOrbisAddObj( "raptor", 155, -30, 70 )
  ozOrbisAddObj( "hoverTank", 145, -30, 71 )

  ozOrbisAddObj( "metalBarrel", 147, -11, 72 )
  ozOrbisAddObj( "metalBarrel", 147, -11, 74 )
  ozOrbisAddObj( "metalBarrel", 147, -11, 76 )

  ozOrbisAddObj( "smallCrate", 147, 11, 72 )
  ozOrbisAddObj( "smallCrate", 147, 11, 73 )
  ozOrbisAddObj( "smallCrate", 147, 11, 74 )
  ozOrbisAddObj( "smallCrate", 147, 11, 75 )
  ozOrbisAddObj( "smallCrate", 147, 11, 76 )

  ozOrbisAddObj( "smallCrate", 147, 12, 72 )
  ozOrbisAddObj( "smallCrate", 147, 12, 73 )
  ozOrbisAddObj( "smallCrate", 147, 12, 74 )
  ozOrbisAddObj( "smallCrate", 147, 12, 75 )
  ozOrbisAddObj( "smallCrate", 147, 12, 76 )

  ozOrbisAddObj( "smallCrate", 148, 11, 72 )
  ozOrbisAddObj( "smallCrate", 148, 11, 73 )
  ozOrbisAddObj( "smallCrate", 148, 11, 74 )
  ozOrbisAddObj( "smallCrate", 148, 11, 75 )
  ozOrbisAddObj( "smallCrate", 148, 11, 76 )

  ozOrbisAddObj( "smallCrate", 148, 12, 72 )
  ozOrbisAddObj( "smallCrate", 148, 12, 73 )
  ozOrbisAddObj( "smallCrate", 148, 12, 74 )
  ozOrbisAddObj( "smallCrate", 148, 12, 75 )
  ozOrbisAddObj( "smallCrate", 148, 12, 76 )

  ozOrbisAddObj( "bigCrate", 140, -7, 72 )
  ozOrbisAddObj( "metalCrate", 140, -4, 72 )
  ozOrbisAddObj( "metalBarrel", 140, -5.5, 72 )

  ozOrbisAddStr( "castle", 143, 0, 79, OZ_NORTH )
  ozOrbisAddStr( "pool", 112, -7, 70, OZ_NORTH )
  ozOrbisAddStr( "tower", 106, -7, 75, OZ_WEST )

  ozOrbisAddObj( "serviceStation", 110, -25, 68.8, OZ_WEST )

  ozOrbisAddObj( "smallCrate", 108, -6, 81 )
  ozOrbisAddObj( "smallCrate", 108, -6, 82 )
  ozOrbisAddObj( "smallCrate", 108, -6, 83 )
  ozOrbisAddObj( "smallCrate", 108, -8, 81 )
  ozOrbisAddObj( "smallCrate", 108, -8, 82 )
  ozOrbisAddObj( "smallCrate", 108, -8, 83 )

  ozFloraSeed()

  init_stressTest()
end

function init_stressTest()
  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisTryAddObj( "goblin", x, y, z + 0.75 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisTryAddObj( "knight", x, y, z + 1.10 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisTryAddObj( "beast", x, y, z + 1.10 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisTryAddObj( "metalBarrel", x, y, z + 0.70 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisTryAddObj( "smallCrate", x, y, z + 0.40 )
    end
  end
end
