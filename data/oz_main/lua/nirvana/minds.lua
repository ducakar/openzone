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
  ozSelfAction( OZ_ACTION_FORWARD )

  if not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
    ozSelfAction( OZ_ACTION_WALK )
  end
  if math.random( 4 ) == 1 then
    ozSelfAddH( math.random() * 180 - 90 )
  end
end

function randomRampage( localData )
  ozSelfAction( OZ_ACTION_FORWARD )

  if math.random( 4 ) == 1 then
    ozSelfAddH( math.random() * 180 - 90 )
  end
  if math.random( 8 ) == 1 then
    ozSelfAction( OZ_ACTION_JUMP )
  end
  if math.random( 8 ) == 1 then
    ozSelfAction( OZ_ACTION_WALK )
  end
end

function beastie( localData )
  if not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
    ozSelfAction( OZ_ACTION_FORWARD )
  end
  if math.random( 3 ) == 1 then
    if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAddH( math.random( 180 ) - 90 )
    elseif math.random( 2 ) == 1 then
      ozSelfAction( OZ_ACTION_JUMP )
    end
    ozSelfAction( OZ_ACTION_WALK )
  end
end

function prey( localData )
  local minDistance = 100
  local heading

  ozSelfBindOverlaps( OZ_OBJECTS_BIT, 20 )

  while ozBindNextObj() do
    if not ozObjIsSelf() and ozObjHasFlag( OZ_BOT_BIT ) and ozObjGetClassName() ~= "goblin" then
      local distance = ozObjDistFromSelf()

      if distance < minDistance then
        minDistance = distance
        heading = ozObjHeadingFromSelfEye()
      end
    end
  end

  if heading then
    ozSelfSetH( heading + 180 )
    ozSelfAction( OZ_ACTION_FORWARD )

    if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
  elseif math.random( 3 ) == 1 then
    ozSelfAddH( math.random( 120 ) - 60 )
  end
end

function predator( localData )
  local self = ozObjGetIndex()
  local minDistance = 100
  local heading = 0

  ozSelfBindOverlaps( OZ_OBJECTS_BIT, 20 )

  while ozBindNextObj() do
    if not ozObjIsSelf() and ozObjHasFlag( OZ_BOT_BIT ) and ozObjGetClassName() == "goblin" then
      local distance = ozObjDistFromSelf()

      if distance < minDistance then
        minDistance = distance
        heading = ozObjHeadingFromSelfEye()
      end
    end
  end
  if minDistance ~= 100 then
    ozSelfSetH( heading )
    if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
  else
    if math.random( 8 ) == 1 then
      ozSelfAddH( math.random( 180 ) - 90 )
    end
    if not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
  end
  ozSelfAction( OZ_ACTION_FORWARD )
end
