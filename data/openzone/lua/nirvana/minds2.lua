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
 * nirvana/minds2.lua
 *
 * AIs for cvicek mission.
]]--

function test()
  print( ozLocalData[69].ticks )
end

function beast_nervous( l )
  if math.random( 100 ) == 1 then
    ozSelfAction( OZ_ACTION_FLIP )
  end
end

function beast_happy( l )
  if math.random( 10 ) == 1 then
    ozSelfAction( OZ_ACTION_WAVE )
  end
end

function droid_guard( l )
  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and ozObjIsVisibleFromSelfEye() then
      local dX, dY, dZ = ozObjVectorFromSelfEye()
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistFromSelfEye()
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading + ( 0.5 - math.random() ) * 4.0 )
      ozSelfSetV( ozObjPitchFromSelfEye() + ( 0.5 - math.random() ) * 4.0 )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfAction( OZ_ACTION_ATTACK )
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 100 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and
         string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistFromSelfEye() < 4 or math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 60.0 ) and
         ozObjIsVisibleFromSelfEye()
      then
        l.target = ozObjGetIndex()
        l.aimed = nil
        return
      end
    end
  end

  if math.random( 20 ) == 1 then
    ozSelfAddH( math.random( -60, 60 ) )
  end
end

function droid_sniper( l )
  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and ozObjIsVisibleFromSelfEye() then
      local dX, dY, dZ = ozObjVectorFromSelfEye()
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistFromSelfEye()
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfAction( OZ_ACTION_ATTACK )
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 200 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and
         string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistFromSelfEye() < 4 or math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 60.0 ) and
         ozObjIsVisibleFromSelfEye()
      then
        l.target = ozObjGetIndex()
        l.aimed = nil
        return
      end
    end
  end

  if math.random( 20 ) == 1 then
    ozSelfAddH( math.random( -60, 60 ) )
  end
end

function droid_patrol( l )
  if not l.pivotX then
    l.pivotX, l.pivotY, l.pivotZ = ozSelfGetPos()

    if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
  end

  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and ozObjIsVisibleFromSelfEye() then
      local dX, dY, dZ = ozObjVectorFromSelfEye()
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistFromSelfEye()
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading + ( 0.5 - math.random() ) * 4.0 )
      ozSelfSetV( ozObjPitchFromSelfEye() + ( 0.5 - math.random() ) * 4.0 )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfAction( OZ_ACTION_ATTACK )
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 100 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and
         string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistFromSelfEye() < 4 or math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 60.0 ) and
         ozObjIsVisibleFromSelfEye()
      then
        l.target = ozObjGetIndex()
        l.aimed = nil
        return
      end
    end
  end

  local dX, dY, dZ = ozSelfGetPos()
  dX = l.pivotX - dX
  dY = l.pivotY - dY
  dZ = l.pivotZ - dZ
  local dist = math.sqrt( dX*dX + dY*dY + dZ*dZ )

  if dist > 50 then
    ozSelfSetH( math.atan2( -dX, dY ) * 180.0 / math.pi )
  elseif math.random( 20 ) == 1 then
    ozSelfAddH( math.random( -60, 60 ) )
  end

  ozSelfAction( OZ_ACTION_FORWARD )
end

function droid_armouredPatrol( l )
  if not l.pivotX then
    l.pivotX, l.pivotY, l.pivotZ = ozSelfGetPos()

    if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
      ozSelfAction( OZ_ACTION_WALK )
    end
  end

  if l.target then
    ozBindObj( l.target )

    if ( ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and ozObjIsVisibleFromSelfEye() ) or
       ( ozObjHasFlag( OZ_VEHICLE_BIT ) and ozObjIsVisibleFromSelfEye() )
    then
      local dX, dY, dZ = ozObjVectorFromSelfEye()
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistFromSelfEye()
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfAction( OZ_ACTION_ATTACK )
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 200 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and
         string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistFromSelf() < 100 or math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 60.0 ) and
         ozObjIsVisibleFromSelfEye()
      then
        l.target = ozObjGetIndex()
        l.aimed = nil
        return
      elseif ozObjHasFlag( OZ_VEHICLE_BIT ) and
          ( ozObjDistFromSelf() < 100 or math.abs( 180.0 - ozObjRelHeadingFromSelfEye() ) > 60.0 ) and
          ozObjIsVisibleFromSelfEye()
      then
        local vehicleIndex = ozObjGetIndex()
        ozBindObj( ozVehicleGetPilot() )

        if not ozObjIsNull() and string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" then
          l.target = vehicleIndex
          l.aimed = nil
          return
        end
      end
    end
  end

  local dX, dY, dZ = ozSelfGetPos()
  dX = l.pivotX - dX
  dY = l.pivotY - dY
  dZ = l.pivotZ - dZ
  local dist = math.sqrt( dX*dX + dY*dY + dZ*dZ )

  if dist > 150 then
    ozSelfSetH( math.atan2( -dX, dY ) * 180.0 / math.pi )
  elseif math.random( 5 ) == 1 then
    ozSelfAddH( math.random( -60, 60 ) )
  end

  if not l.forward then
    ozSelfAction( OZ_ACTION_FORWARD )
    l.forward = true
  else
    l.forward = nil
  end
end

function goblin_defend( l )
  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and ozObjIsVisibleFromSelfEye() then
      ozSelfSetH( ozObjHeadingFromSelfEye() )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      local dist = ozObjDistFromSelfEye()

      if dist > 30 then
        l.target = nil
      elseif dist > 2 then
        if ozSelfHasState( OZ_BOT_WALKING_BIT ) then
          ozSelfAction( OZ_ACTION_WALK )
        end
        ozSelfActionForward( OZ_ACTION_FORWARD )
      else
        ozSelfAction( OZ_ACTION_ATTACK )
      end
    else
      l.target = nil
    end
  else
    ozSelfBindOverlaps( OZ_OBJECTS_BIT, 10 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and ozObjHasFlag( OZ_OBJ_SOLID_BIT ) and
         string.sub( ozObjGetClassName(), 1, 7 ) ~= "goblin" and
         ozObjIsVisibleFromSelfEye()
      then
        l.target = ozObjGetIndex()
      end
    end
  end
end
