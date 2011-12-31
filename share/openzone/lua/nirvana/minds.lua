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
 * nirvana/minds.lua
 *
 * Various AIs.
]]--

function randomWalk( localData )
  ozSelfActionForward()

  if math.random( 3 ) == 1 then
    ozSelfAddH( math.random() * 120.0 - 60.0 )
  end
  if math.random( 3 ) == 1 then
    ozSelfActionJump()
  end
  if math.random( 3 ) == 1 then
    ozSelfToggleRunning()
  end
end

function randomRampage( localData )
  ozSelfActionForward()
  if math.random( 3 ) == 1 then
    ozSelfAddH( math.random() * 120.0 - 60.0 )
  end
end

function beastie( localData )
  if math.random( 3 ) == 1 then
    if localData.run then
      localData.run = false
      ozSelfAddH( math.random( 180 ) - 90 )
    else
      localData.run = true
      ozSelfSetRunning( true )
      if math.random( 2 ) == 1 then
        ozSelfActionJump()
      end
    end
  end
  if localData.run then
    ozSelfActionForward()
  end
end

function prey( localData )
  local minDistance = 100
  local heading
  local distance

  ozSelfBindObjOverlaps( 20, 20, 20 )
  while ozObjBindNext() do
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetClassName() ~= "goblin" then
      distance = ozObjDistanceFromSelf()
      if distance < minDistance then
        minDistance = distance
        heading = ozObjHeadingFromSelf()
      end
    end
  end
  if heading then
    ozSelfSetH( heading + 180 )
    ozSelfSetRunning( true )
    ozSelfActionForward()
  elseif math.random( 3 ) == 1 then
    ozSelfAddH( math.random( 120 ) - 60 )
  end
end

function predator( localData )
  local minDistance = 100
  local heading
  local distance

  ozSelfBindObjOverlaps( 20, 20, 20 )
  while ozObjBindNext() do
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetClassName() == "goblin" then
      distance = ozObjDistanceFromSelf()
      if distance < minDistance then
        minDistance = distance
        heading = ozObjHeadingFromSelf()
      end
    end
  end
  if heading then
    ozSelfSetH( heading )
    ozSelfSetRunning( true )
  else
    ozSelfSetRunning( false )
    if math.random( 100 ) == 1 then
      ozSelfAddH( math.random( 120 ) - 60 )
    end
  end
  ozSelfActionForward()
end
