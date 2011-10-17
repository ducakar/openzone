--[[
 *  weapons.lua
 *  Weapon handlers
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function plasmagun_onUpdate( l )
  if not l.tick then
    l.tick = 80
  elseif l.tick ~= 0 then
    l.tick = l.tick - 1
  else
    l.tick = 80

    ozWeaponAddRounds( 1 )

    if ozWeaponGetRounds() == ozWeaponGetDefaultRounds() then
      ozObjEnableUpdate( false )
    end
  end
end

function plasmagun_onShot( l )
  ozObjEnableUpdate( true )

  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddPart( pX, pY, pZ, vX * 230 + dX, vY * 230 + dY, vZ * 230 + dZ,
                  0.4, 0.8, 0.8, 1.9, 0.004, 5.0 );
end

function blaster_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
                  0.3, 1.0, 0.3, 1.9, 0.004, 5.0 );
end

function hyperblaster_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 1 - math.random() * 2,
                     1 - math.random() * 2,
                     1 - math.random() * 2

  ozOrbisAddPart( pX, pY, pZ, vX * 300 + dX, vY * 300 + dY, vZ * 300 + dZ,
                  1.0, 0.5, 1.0, 1.9, 0.005, 5.0 );
end

function chaingun_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 4 - math.random() * 8,
                     4 - math.random() * 8,
                     4 - math.random() * 8

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
                  1.0, 1.0, 0.0, 1.9, 0.004, 5.0 );
end

function grenadeLauncher_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 0.5 - math.random() * 1,
                     0.5 - math.random() * 1,
                     1.0 - math.random() * 2

  ozOrbisAddObj( "grenade", pX, pY, pZ )
  ozDynSetMomentum( vX * 30 + dX, vY * 30 + dY, vZ * 30 + dZ )
  ozDynSetVelocity( vX * 30 + dX, vY * 30 + dY, vZ * 30 + dZ )
end

function axe_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozBindObjOverlaps( pX + 0.5*vX, pY + 0.5*vY, pZ + 0.5*vZ, 0.3, 0.3, 0.3 );
  while ozObjBindNext() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 50.0 * math.random() )
    end
  end
end

function mace_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozBindObjOverlaps( pX + 0.6*vX, pY + 0.6*vY, pZ + 0.6*vZ, 0.4, 0.4, 0.4 );
  while ozObjBindNext() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 100.0 * math.random() )
    end
  end
end

function vehicle_heavyBlaster_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  ozOrbisAddPart( pX, pY, pZ, vX * 300, vY * 300, vZ * 300,
                  1.0, 0.2, 0.2, 1.9, 0.01, 3.0 );
end

function vehicle_chaingun_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 5 - math.random() * 10,
                     5 - math.random() * 10,
                     5 - math.random() * 10

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
                  1.0, 1.0, 0.0, 1.9, 0.008, 3.0 );
end

function vehicle_cannon_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisAddObj( "shell", pX, pY, pZ )
  ozDynSetMomentum( vX * 200, vY * 200, vZ * 200 )
  ozDynSetVelocity( vX * 200, vY * 200, vZ * 200 )
end
