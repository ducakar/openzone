--
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
--

-- lua/nirvana/oz_main.lua
--
-- AIs for oz_main bots.

--
-- Common functions for AI automata.
--

function randomWalkFunc( turnProbability, turnAngle )
  return function( l )
    ozSelfAction( OZ_ACTION_FORWARD )

    if not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
    if math.random( turnProbability ) == 1 then
      ozSelfAddH( math.random( 2.0 * turnAngle ) - turnAngle )
    end
  end
end

function detectLeaderFunc( maxDistance, leaderClass )
  return function( l )
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, maxDistance )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and isSubclassOf( leaderClass ) then
        l.leader = ozObjGetIndex()
        return true
      end
    end
    return false
  end
end

function isLeaderLost( l )
  return l.leader == nil
end

function followLeaderFunc( maxDistance, lowDistance, highDistance )
  return function( l )
    ozBindObj( l.leader )

    if ozObjIsNull() or ozBotHasState( OZ_BOT_DEAD_BIT ) then
      l.leader = nil
      return
    end

    local distance = ozObjDistFromSelf()

    if distance < lowDistance then
      l.nearLeader = true
    elseif distance > highDistance or not l.nearLeader then
      ozSelfSetH( ozObjHeadingFromSelfEye() )
      ozSelfAction( OZ_ACTION_FORWARD )

      if distance > maxDistance then
        l.leader = nil
        l.nearleader = nil
      elseif distance > highDistance then
        l.nearLeader = nil
        if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
          ozSelfAction( OZ_ACTION_WALK )
        end
      elseif not ozSelfHasState( OZ_BOT_WALKING_BIT ) then
        ozSelfAction( OZ_ACTION_WALK )
      end
    end
  end
end

function detectTargetFunc( visualDistance, visualAngle, hearDistance, excludeClass )
  return function( l )
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, visualDistance )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT + OZ_VEHICLE_BIT ) and
         ( ozObjDistFromSelf() <= hearDistance or
           math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 180.0 - visualAngle / 2.0 ) and
         ozObjIsVisibleFromSelfEye()
      then
        if ozObjHasFlag( OZ_BOT_BIT ) and not isSubclassOf( excludeClass ) then
          l.target = ozObjGetIndex()
          return true
        elseif ozObjHasFlag( OZ_VEHICLE_BIT ) then
          local vehicleIndex = ozObjGetIndex()
          ozBindObj( ozVehicleGetPilot() )

          if not ozObjIsNull() and not isSubclassOf( excludeClass ) then
            l.target = vehicleIndex
            return true
          end
        end
      end
    end
    return false
  end
end

function isTargetLost( l )
  return l.target == nil
end

function huntTargetFunc( maxDistance, shootDistance )
  return function( l )
    ozBindObj( l.target )

    if ozObjIsNull() or ( ozObjHasFlag( OZ_BOT_BIT ) and ozBotHasState( OZ_BOT_DEAD_BIT ) ) or
       not ozObjIsVisibleFromSelfEye()
    then
      l.target = nil
    else
      local distance = ozObjDistFromSelf()

      if distance > maxDistance then
        l.target = nil
      else
        ozSelfSetH( ozObjHeadingFromSelfEye() )
        ozSelfSetV( ozObjPitchFromSelfEye() )

        if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
          ozSelfAction( OZ_ACTION_WALK )
        end

        if distance > shootDistance then
          ozSelfAction( OZ_ACTION_FORWARD )
        else
          ozSelfAction( OZ_ACTION_ATTACK )
          ozForceUpdate()
        end
      end
    end
  end
end

function shootTargetFunc( inaccuracy )
  return function( l )
    ozBindObj( l.target )

    if ozObjIsNull() or ( ozObjHasFlag( OZ_BOT_BIT ) and ozBotHasState( OZ_BOT_DEAD_BIT ) ) or
       not ozObjIsVisibleFromSelfEye()
    then
      l.target = nil
      l.aimed = nil
    else
      local dX, dY, dZ = ozObjVectorFromSelfEye()
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistFromSelfEye()
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading + ( 0.5 - math.random() ) * inaccuracy )
      ozSelfSetV( ozObjPitchFromSelfEye() + ( 0.5 - math.random() ) * inaccuracy )

      if l.aimed then
        ozSelfAction( OZ_ACTION_ATTACK )
      else
        l.aimed = true
      end
    end
  end
end

function enterPatrol( l )
  l.pivotX, l.pivotY, l.pivotZ = ozSelfGetPos()

  if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
    ozSelfAction( OZ_ACTION_WALK )
  end
end

function patrolFunc( maxDistance )
  return function( l )
    local dX, dY, dZ = ozSelfGetPos()
    dX = l.pivotX - dX
    dY = l.pivotY - dY
    dZ = l.pivotZ - dZ
    local dist = math.sqrt( dX*dX + dY*dY + dZ*dZ )

    if dist > maxDistance then
      ozSelfSetH( math.atan2( -dX, dY ) * 180.0 / math.pi )
    elseif math.random( 20 ) == 1 then
      ozSelfAddH( math.random( -60, 60 ) )
    end

    ozSelfAction( OZ_ACTION_FORWARD )
  end
end

--
-- Misc AIs.
--

randomWalk = randomWalkFunc( 8, 60 )

function beastie( l )
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

function prey( l )
  local minDistance = 100
  local heading

  ozSelfBindOverlaps( OZ_OBJECTS_BIT, 20 )

  while ozBindNextObj() do
    if not ozObjIsSelf() and ozObjHasFlag( OZ_BOT_BIT ) and
       ozObjGetClassName() ~= "goblin"
    then
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

function predator( l )
  local self = ozObjGetIndex()
  local minDistance = 100
  local heading = 0

  ozSelfBindOverlaps( OZ_OBJECTS_BIT, 20 )

  while ozBindNextObj() do
    if not ozObjIsSelf() and ozObjHasFlag( OZ_BOT_BIT ) and
       ozObjGetClassName() == "goblin"
    then
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
-- Goblin AI.
--

Goblin = {}

--
-- Droid AI.
--

Droid = {}

Droid.basicAutomaton =
{
  name = "Droid.basic",
  initial = "randomWalk",

  randomWalk = {
    onUpdate = randomWalkFunc( 6, 60 ),
    links = {
      {
        target = "followLeader",
        condition = detectLeaderFunc( 50, "droid.OOM-9" )
      },
      {
        target = "huntTarget",
        condition = detectTargetFunc( 50, 120, 5, "droid" )
      }
    }
  },

  followLeader = {
    onUpdate = followLeaderFunc( 50, 8, 4 ),
    links = {
      {
        target = "randomWalk",
        condition = isLeaderLost
      },
      {
        target = "huntTarget",
        condition = detectTargetFunc( 50, 120, 5, "droid" )
      }
    }
  },

  huntTarget = {
    onUpdate = huntTargetFunc( 70, 30 ),
    links = {
      {
        target = "randomWalk",
        condition = isTargetLost
      }
    }
  }
}

droid_basic = automatonProcessor( Droid.basicAutomaton )
