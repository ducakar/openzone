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
 * lua/matrix/oz_main.lua
 *
 * Handlers for oz_main objects.
]]--

--
-- Helper functions
--

function meleeAttack( aabbFwdTransl, aabbDim, minDamage, maxDamage )
  ozBindUser()

  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisBindOverlaps( OZ_OBJECTS_BIT,
                       pX + aabbFwdTransl * vX, pY + aabbFwdTransl * vY, pZ + aabbFwdTransl * vZ,
                       aabbDim, aabbDim, aabbDim )

  while ozBindNextObj() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( minDamage + math.random() * ( maxDamage - minDamage ) )
    end
  end

  return true
end

function gunAttack( bulletFragPool, velocity, stillDisp, movingDisp )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local disp = stillDisp
  if ozBotHasState( OZ_BOT_MOVING_BIT ) then
    disp = movingDisp
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = ozVec3.random( disp )

  ozOrbisAddFrag( OZ_FORCE, bulletFragPool,
                  pX, pY, pZ,
                  vX * velocity + dX, vY * velocity + dY, vZ * velocity + dZ )

  return true
end

function vehicleGunAttack( bulletFragPool, velocity, disp )
  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozVehicleGetDir()
  local dX, dY, dZ = ozVec3.random( disp )

  ozOrbisAddFrag( OZ_FORCE, bulletFragPool,
                  pX, pY, pZ,
                  vX * velocity + dX, vY * velocity + dY, vZ * velocity + dZ )
end

--
-- Miscellaneous
--

function firstAid_onUse( l )
  ozBindUser()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    return false
  else
    ozObjAddLife( 100.0 )

    ozBindSelf()
    ozObjDestroy( true )
  end

  return true
end

function cvicek_onUse( l )
  ozBindUser()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    return false
  else
    ozObjAddLife( 50 )
    ozBotAddStamina( 30 )

    ozBindSelf()
    ozObjDestroy( true )
  end

  return true
end

function bomb_onUse( l )
  if not l.ticks then
    l.ticks = 250 -- 5 s
    ozObjEnableUpdate( true )
  else
    l.ticks = nil
    ozObjEnableUpdate( false )
  end

  return true
end

function bomb_onUpdate( l )
  if l.ticks ~= 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

function serviceStation_onUse( l )
  local pX, pY, pZ = ozObjGetPos()

  ozBindUser()
  ozBotRearm()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    ozBotHeal()
  end

  ozOrbisBindOverlaps( OZ_OBJECTS_BIT, pX, pY, pZ, 5, 5, 2 )
  while ozBindNextObj() do
    if ozObjHasFlag( OZ_VEHICLE_BIT ) then
      ozVehicleService()
    end
  end

  return true
end

--
-- Explosions
--

function spawnSmallExplosion( l )
  ozObjAddEvent( OZ_EVENT_SHAKE, 0.5 )
  ozOrbisAddObj( OZ_FORCE, "smallExplosion", ozObjGetPos() )
end

function spawnBigExplosion( l )
  ozObjAddEvent( OZ_EVENT_FLASH, 0.5 )
  ozObjAddEvent( OZ_EVENT_SHAKE, 1.0 )
  ozOrbisAddObj( OZ_FORCE, "bigExplosion", ozObjGetPos() )
end

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjDestroy( true )
    end
  else
    l.ticks = 25

    ozObjAddEvent( OZ_EVENT_CREATE, 1 )
    ozObjBindOverlaps( OZ_OBJECTS_BIT, 8 )

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 8 then
          distance = 8 - distance

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 12*distance )

            if distance < 7.9 and ozObjHasFlag( OZ_DYNAMIC_BIT ) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()
              distance = 2 * distance

              ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
            end
          end
        end
      end
    end
  end
end

function bigExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjDestroy( true )
    end
  else
    l.ticks = 25

    ozObjAddEvent( OZ_EVENT_CREATE, 1 )
    ozObjBindOverlaps( OZ_STRUCTS_BIT + OZ_OBJECTS_BIT, 20 )

    while ozBindNextStr() do
      ozStrDamage( 1000 )
    end

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 20 then
          distance = 20 - distance

          ozObjDamage( 100 + 2*distance )

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 6*distance )

            if distance < 19.9 and ozObjHasFlag( OZ_DYNAMIC_BIT ) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()

              ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
            end
          end
        end
      end
    end
  end
end

--
-- Weapons
--

function melee_onShot( l )
  return meleeAttack( 0.4, 0.4, 100, 125 )
end

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

  return gunAttack( "plasmaBullet", 230, 2, 20 )
end

function blaster_onShot( l )
  return gunAttack( "blasterBullet", 220, 2, 20 )
end

function hyperblaster_onShot( l )
  return gunAttack( "hyperblasterBullet", 300, 1, 10 )
end

function chaingun_onShot( l )
  return gunAttack( "chaingunBullet", 250, 2, 20 )
end

function grenadeLauncher_onShot( l )
  ozBindUser()
  if ozBotHasState( OZ_BOT_SUBMERGED_BIT ) then
    return false
  end

  local disp;
  if ozBotHasState( OZ_BOT_MOVING_BIT ) then
    disp = 5
  else
    disp = 0.5
  end

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = ozVec3.random( disp )

  ozOrbisAddObj( OZ_FORCE, "grenade", pX, pY, pZ )
  ozDynSetMomentum( vX * 30 + dX, vY * 30 + dY, vZ * 30 + dZ )

  return true
end

function axe_onShot( l )
  return meleeAttack( 0.5, 0.3, 100, 150 )
end

function mace_onShot( l )
  return meleeAttack( 0.6, 0.4, 100, 200 )
end

function skull_onShot( l )
  return meleeAttack( 0.6, 0.4, 100, 300 )
end

--
-- Vehicle weapons
--

function vehicle_heavyBlaster_onShot( l )
  vehicleGunAttack( "heavyBlasterBullet", 300, 0 )
end

function vehicle_gatling_onShot( l )
  vehicleGunAttack( "gatlingBullet", 200, 5 )
end

function vehicle_cannon_onShot( l )
  local pX, pY, pZ = ozObjGetPos()
  local vX, vY, vZ = ozVehicleGetDir()

  ozOrbisAddObj( OZ_FORCE, "shell", pX, pY, pZ )
  ozDynSetMomentum( vX * 150, vY * 150, vZ * 150 )
end

function shell_onUpdate( l )
  if not l.ticks then
    l.ticks = 300
  elseif l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end
