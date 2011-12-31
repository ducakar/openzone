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
 * matrix/handlers.lua
 *
 * Various handlers.
]]--

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjRemove()
    end
  else
    ozObjAddEvent( OZ_EVENT_CREATE, 1.0 )

    l.ticks = 25

    local distance
    local dirX, dirY, dirZ

    ozObjBindObjOverlaps( 8, 8, 8 )
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 8 then
          distance = 8 - distance

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 10*distance )

            if distance < 7.9 and ozObjIsDynamic() then
              dirX, dirY, dirZ = ozObjDirectionFromSelf()
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
      ozObjRemove()
    end
  else
    ozObjAddEvent( OZ_EVENT_CREATE, 1.0 )

    l.ticks = 25

    local distance
    local dirX, dirY, dirZ

    ozObjBindAllOverlaps( 20, 20, 20 )
    while ozStrBindNext() do
      ozStrDamage( 2000 )
    end
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 20 then
          distance = 20 - distance

          ozObjDamage( 10*distance )

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 10*distance )

            if distance < 19.9 and ozObjIsDynamic() then
              dirX, dirY, dirZ = ozObjDirectionFromSelf()
              distance = 2 * distance
              ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
            end
          end
        end
      end
    end
  end
end

function cvicek_onUse( l )
  ozObjBindUser()

  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 50 )
    ozBotAddStamina( 30 )

    ozObjBindSelf()
    ozObjQuietDestroy()
  end
end

function bomb_onUse( l )
  if not l.ticks then
    l.ticks = 250 -- 5 s
    ozObjEnableUpdate( true )
  else
    l.ticks = nil
    ozObjEnableUpdate( false )
  end
end

function bomb_onUpdate( l )
  if l.ticks ~= 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
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

function serviceStation_onUse( l )
  ozObjBindUser()
  ozBotRearm()
  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozBotHeal()
  end

  ozObjBindAllOverlaps( 5, 5, 2 )
  while ozObjBindNext() do
    if ozObjIsVehicle() then
      ozVehicleService()
    end
  end
end
