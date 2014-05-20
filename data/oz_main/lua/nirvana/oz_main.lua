-- OpenZone - simple cross-platform FPS/RTS game engine.
--
-- Copyright © 2002-2014 Davorin Učakar
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

-- lua/nirvana/oz_main.lua
--
-- AIs for oz_main bots.

--
-- Misc AI
--

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

--
-- Droid AI
--

function droid_randomWalk( l )
  if not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
    ozSelfAction( OZ_ACTION_WALK )
  end
  ozSelfAction( OZ_ACTION_FORWARD )

  if math.random( 10 ) == 1 then
    ozSelfAddH( math.random( 120 ) - 60 )
  end
end

function droid_followCommander( l )
  if not l.commander then
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 50 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjGetClassName() == "droid.OOM-9" then
        l.commander = ozObjGetIndex()
        break
      end
    end
  else
    ozBindObj( l.commander )

    if ozObjIsNull() or ozBotHasState( OZ_BOT_DEAD_BIT ) then
      l.commander = nil
    end
  end

  if l.commander then
    local distance = ozObjDistFromSelf()

    if distance < 4 then
      l.nearCommander = true
    elseif distance > 10 or not l.nearCommander then
      ozSelfSetH( ozObjHeadingFromSelfEye() )
      ozSelfAction( OZ_ACTION_FORWARD )

      if distance > 100 then
        l.commander = nil
        l.nearCommander = false
      elseif distance > 10 then
        l.nearCommander = false
        if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
          ozSelfAction( OZ_ACTION_WALK )
        end
      elseif not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
        ozSelfAction( OZ_ACTION_WALK )
      end
    end
    return true
  else
    return false
  end
end

function droid_huntTarget( l )
  ozBindObj( l.target )

  if ozObjIsNull() or ozBotHasState( OZ_BOT_DEAD_BIT ) then
    l.target = nil
  else
    local distance = ozObjDistFromSelf()

    if distance < 60 then
      ozSelfSetH( ozObjHeadingFromSelfEye() )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      if distance < 30 then
        ozSelfAction( OZ_ACTION_ATTACK )
        ozForceUpdate()
      else
        ozSelfAction( OZ_ACTION_FORWARD )
      end
    else
      l.target = nil
    end
  end
end

function droid( l )
  if not l.target then
    local minDistance = 100

    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 50 )

    while ozBindNextObj() do
      local typeName = ozObjGetClassName()
      if ozObjHasFlag( OZ_BOT_BIT ) and string.sub( typeName, 1, 5 ) ~= "droid" then
        local distance = ozObjDistFromSelf()

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
