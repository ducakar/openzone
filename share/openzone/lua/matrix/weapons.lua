--[[
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
]]--

--[[
 * matrix/weapons.lua
 *
 * Weapon handlers.
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

  ozOrbisAddFrag( "plasmaBullet", pX, pY, pZ, vX * 230 + dX, vY * 230 + dY, vZ * 230 + dZ );
end

function blaster_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddFrag( "blasterBullet", pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ );
end

function hyperblaster_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 1 - math.random() * 2,
                     1 - math.random() * 2,
                     1 - math.random() * 2

  ozOrbisAddFrag( "hyperblasterBullet", pX, pY, pZ, vX * 300 + dX, vY * 300 + dY, vZ * 300 + dZ );
end

function chaingun_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 4 - math.random() * 8,
                     4 - math.random() * 8,
                     4 - math.random() * 8

  ozOrbisAddFrag( "chaingunBullet", pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ );
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

function skull_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozBindObjOverlaps( pX + 0.6*vX, pY + 0.6*vY, pZ + 0.6*vZ, 0.4, 0.4, 0.4 );
  while ozObjBindNext() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 200.0 * math.random() )
    end
  end
end

function vehicle_heavyBlaster_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  ozOrbisAddFrag( "heavyBlasterBullet", pX, pY, pZ, vX * 300, vY * 300, vZ * 300 );
end

function vehicle_gatling_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 5 - math.random() * 10,
                     5 - math.random() * 10,
                     5 - math.random() * 10

  ozOrbisAddFrag( "gatlingBullet", pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ );
end

function vehicle_cannon_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisAddObj( "shell", pX, pY, pZ )
  ozDynSetMomentum( vX * 200, vY * 200, vZ * 200 )
  ozDynSetVelocity( vX * 200, vY * 200, vZ * 200 )
end
