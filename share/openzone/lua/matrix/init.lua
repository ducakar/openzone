function matrix_onCreate()
  ozCaelumSetHeading( math.pi / 8 )
  ozCaelumSetPeriod( 1440 )
  ozCaelumSetTime( 180 )

  init_sample()

  ozFloraSeed()

  init_stressTest()
end

function init_sample()
  ozOrbisForceAddObj( "lord", 138, -11, 73 )
  ozBotSetH( 270 )
  ozBotSetName( "Davorin" )

  ozOrbisForceAddObj( "droid.OOM-9", 140, -11, 73 )
  ozBotSetH( 90 )

  ozOrbisGenParts( 1000, 136, -3, 71, 0, 0, 10, 15, 0.4, 0.4, 0.4, 0.2, 1.95, 0.1, 5.0 )

  ozOrbisForceAddObj( "cvicek", 135, -3, 73 )
  ozOrbisForceAddObj( "firstAid", 136, -3, 73 )
  ozOrbisForceAddObj( "bomb", 137, -3, 73 )
  ozOrbisForceAddObj( "droid_weapon.hyperblaster", 135, -4, 73 )
  ozOrbisForceAddObj( "droid_weapon.chaingun", 136, -4, 73 )
  ozOrbisForceAddObj( "droid_weapon.grenadeLauncher", 137, -4, 73 )

  ozOrbisForceAddObj( "raptor", 155, -30, 70 )
  ozOrbisForceAddObj( "tank", 145, -30, 71 )

  ozOrbisForceAddObj( "metalBarrel", 147, -11, 72 )
  ozOrbisForceAddObj( "metalBarrel", 147, -11, 74 )
  ozOrbisForceAddObj( "metalBarrel", 147, -11, 76 )

  ozOrbisForceAddObj( "smallCrate", 147, 11, 72 )
  ozOrbisForceAddObj( "smallCrate", 147, 11, 73 )
  ozOrbisForceAddObj( "smallCrate", 147, 11, 74 )
  ozOrbisForceAddObj( "smallCrate", 147, 11, 75 )
  ozOrbisForceAddObj( "smallCrate", 147, 11, 76 )

  ozOrbisForceAddObj( "smallCrate", 147, 12, 72 )
  ozOrbisForceAddObj( "smallCrate", 147, 12, 73 )
  ozOrbisForceAddObj( "smallCrate", 147, 12, 74 )
  ozOrbisForceAddObj( "smallCrate", 147, 12, 75 )
  ozOrbisForceAddObj( "smallCrate", 147, 12, 76 )

  ozOrbisForceAddObj( "smallCrate", 148, 11, 72 )
  ozOrbisForceAddObj( "smallCrate", 148, 11, 73 )
  ozOrbisForceAddObj( "smallCrate", 148, 11, 74 )
  ozOrbisForceAddObj( "smallCrate", 148, 11, 75 )
  ozOrbisForceAddObj( "smallCrate", 148, 11, 76 )

  ozOrbisForceAddObj( "smallCrate", 148, 12, 72 )
  ozOrbisForceAddObj( "smallCrate", 148, 12, 73 )
  ozOrbisForceAddObj( "smallCrate", 148, 12, 74 )
  ozOrbisForceAddObj( "smallCrate", 148, 12, 75 )
  ozOrbisForceAddObj( "smallCrate", 148, 12, 76 )

  ozOrbisForceAddObj( "bigCrate", 140, -7, 72 )
  ozOrbisForceAddObj( "crate4", 140, -4, 72 )
  ozOrbisForceAddObj( "barrel2", 140, -6, 72 )

  ozOrbisForceAddStr( "castle", 143, 0, 79, OZ_STRUCT_R0 )
  ozOrbisForceAddStr( "pool", 112, -7, 70, OZ_STRUCT_R0 )
  ozOrbisForceAddStr( "bunker", 106, -7, 75, OZ_STRUCT_R90 )

  ozOrbisForceAddObj( "serviceStation", 110, -25, 68.8 )

  ozOrbisForceAddObj( "smallCrate", 108, -6, 81 )
  ozOrbisForceAddObj( "smallCrate", 108, -6, 82 )
  ozOrbisForceAddObj( "smallCrate", 108, -6, 83 )
  ozOrbisForceAddObj( "smallCrate", 108, -8, 81 )
  ozOrbisForceAddObj( "smallCrate", 108, -8, 82 )
  ozOrbisForceAddObj( "smallCrate", 108, -8, 83 )
end

function init_stressTest()
  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( "goblin", x, y, z + 0.75 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( "knight", x, y, z + 1.10 )
    end
  end

  for i = 1, 1000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( "beast", x, y, z + 1.10 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( "metalBarrel", x, y, z + 0.70 )
    end
  end

  for i = 1, 10000 do
    local x = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local y = -OZ_ORBIS_DIM + 2 * OZ_ORBIS_DIM * math.random()
    local z = ozTerraHeight( x, y )

    if z > 0 then
      ozOrbisAddObj( "smallCrate", x, y, z + 0.40 )
    end
  end
end
