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
 * nirvana/droid.lua
 *
 * AI for droid and droid commander.
]]--

function droid_randomWalk( l )
  ozSelfSetRunning( false )
  ozSelfActionForward()
  if math.random( 10 ) == 1 then
    ozSelfAddH( math.random( 120 ) - 60 )
  end
end

function droid_followCommander( l )
  if not l.commander then
    ozSelfBindObjOverlaps( 50, 50, 50 )
    while ozObjBindNext() do
      if ozObjIsBot() and not ozObjIsSelf() and ozObjGetClassName() == "droid.OOM-9" then
        l.commander = ozObjGetIndex()
        break
      end
    end
  else
    ozObjBindIndex( l.commander )
    if ozObjIsNull() or not ozObjIsBot() or ozObjGetClassName() ~= "droid.OOM-9" then
      l.commander = nil
    end
  end

  if l.commander then
    local distance = ozObjDistanceFromSelf()

    if distance < 4 then
      l.nearCommander = true
    elseif distance > 10 or not l.nearCommander then
      ozSelfSetH( ozObjHeadingFromSelf() )
      ozSelfActionForward()

      if distance > 100 then
        l.commander = nil
        l.nearCommander = false
      elseif distance > 10 then
        ozSelfSetRunning( true )
        l.nearCommander = false
      else
        ozSelfSetRunning( false )
      end
    end
    return true
  else
    return false
  end
end

function droid_huntTarget( l )
  ozObjBindIndex( l.target )
  if ozObjIsNull() or not ozObjIsBot() then
    l.target = nil
  else
    local distance = ozObjDistanceFromSelf()

    if distance < 60 then
      ozSelfSetH( ozObjHeadingFromSelf() )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      if distance < 30 then
        ozSelfActionAttack()
        ozForceUpdate()
      else
        ozSelfSetRunning( true )
        ozSelfActionForward()
      end
    else
      l.target = nil
    end
  end
end

function droid( l )
  if not l.target then
    local minDistance = 100
    ozSelfBindObjOverlaps( 50, 50, 50 )
    while ozObjBindNext() do
      local typeName = ozObjGetClassName()
      if ozObjIsBot() and not ozObjIsSelf() and string.sub( typeName, 1, 5 ) ~= "droid" then
        local distance = ozObjDistanceFromSelf()
        if distance < minDistance then
          l.target = ozObjGetIndex()
          minDistance = distance
        end
      end
    end
  end
  if l.target then
    droid_huntTarget( l )
  else
    local hasCommander = droid_followCommander( l )

    if not hasCommander then
      droid_randomWalk( l )
    end
  end
end
