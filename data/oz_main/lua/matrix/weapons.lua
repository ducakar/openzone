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

    if ozWeaponGetRounds() == ozWeaponMaxRounds() then
      ozObjEnableUpdate( false )
    end
  end
end

function plasmagun_onShot( l )
  ozObjEnableUpdate( true )

  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddFrag( OZ_FORCE, "plasmaBullet", pX, pY, pZ, vX * 230 + dX, vY * 230 + dY, vZ * 230 + dZ )

  return true
end

function blaster_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddFrag( OZ_FORCE, "blasterBullet", pX, pY, pZ, vX * 220 + dX, vY * 220 + dY, vZ * 220 + dZ )

  return true
end

function hyperblaster_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 1 - math.random() * 2,
                     1 - math.random() * 2,
                     1 - math.random() * 2

  ozOrbisAddFrag( OZ_FORCE, "hyperblasterBullet", pX, pY, pZ, vX * 300 + dX, vY * 300 + dY, vZ * 300 + dZ )

  return true
end

function chaingun_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 2 - math.random() * 4,
                     2 - math.random() * 4,
                     2 - math.random() * 4

  ozOrbisAddFrag( OZ_FORCE, "chaingunBullet", pX, pY, pZ, vX * 250 + dX, vY * 250 + dY, vZ * 250 + dZ )

  return true
end

function grenadeLauncher_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 0.5 - math.random() * 1,
                     0.5 - math.random() * 1,
                     1.0 - math.random() * 2

  ozOrbisAddObj( OZ_FORCE, "grenade", pX, pY, pZ )
  ozDynSetMomentum( vX * 30 + dX, vY * 30 + dY, vZ * 30 + dZ )

  return true
end

function axe_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisBindOverlaps( OZ_OBJECTS_BIT, pX + 0.5*vX, pY + 0.5*vY, pZ + 0.5*vZ, 0.3, 0.3, 0.3 )

  while ozBindNextObj() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 50.0 * math.random() )
    end
  end

  return true
end

function mace_onShot( l )
  ozBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisBindOverlaps( OZ_OBJECTS_BIT, pX + 0.6*vX, pY + 0.6*vY, pZ + 0.6*vZ, 0.4, 0.4, 0.4 )

  while ozBindNextObj() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 100.0 * math.random() )
    end
  end

  return true
end

function skull_onShot( l )
  ozBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisBindOverlaps( OZ_OBJECTS_BIT, pX + 0.6*vX, pY + 0.6*vY, pZ + 0.6*vZ, 0.4, 0.4, 0.4 )

  while ozBindNextObj() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 100.0 + 200.0 * math.random() )
    end
  end

  return true
end

function vehicle_heavyBlaster_onShot( l )
  local pX, pY, pZ = ozObjGetPos()

  ozBindUser()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisAddFrag( OZ_FORCE, "heavyBlasterBullet", pX, pY, pZ, vX * 300, vY * 300, vZ * 300 )
end

function vehicle_gatling_onShot( l )
  local pX, pY, pZ = ozObjGetPos()

  ozBindUser()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 5 - math.random() * 10,
                     5 - math.random() * 10,
                     5 - math.random() * 10

  ozOrbisAddFrag( OZ_FORCE, "gatlingBullet", pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ )
end

function vehicle_cannon_onShot( l )
  local pX, pY, pZ = ozObjGetPos()

  ozBindUser()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisAddObj( OZ_FORCE, "shell", pX, pY, pZ )
  ozDynSetMomentum( vX * 150, vY * 150, vZ * 150 )
end