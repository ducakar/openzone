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

function cvicek_onUse( l )
  ozBindUser()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 50 )
    ozBotAddStamina( 30 )

    ozBindSelf()
    ozObjDestroy( true )
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
  ozObjBindOverlaps( OZ_OBJECTS_BIT, 5 )
  while ozBindNextObj() do
    if ozObjHasFlag( OZ_VEHICLE_BIT ) then
      ozVehicleService()
    end
  end

  ozBindUser()
  ozBotRearm()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    ozBotHeal()
  end
end
