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
  if l.ticks then
    if l.ticks == 0 then
      ozSelfSetGesture( 0 )
      l.ticks = nil
    else
      l.ticks = l.ticks - 1
    end
  elseif math.random( 200 ) == 1 then
    ozSelfSetGesture( OZ_BOT_GESTURE4_BIT )
    l.ticks = 2
  end
end

function beast_happy( l )
  if l.ticks then
    if l.ticks == 0 then
      ozSelfSetGesture( 0 )
      l.ticks = nil
    else
      l.ticks = l.ticks - 1
    end
  elseif math.random( 10 ) == 1 then
    ozSelfSetGesture( OZ_BOT_GESTURE1_BIT )
    l.ticks = 2
  end
end

function droid_guard( l )
  local self = ozObjGetIndex()

  if l.target then
    ozBindObj( l.target )
    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjIsVisibleFromEye( self ) then
      local dX, dY, dZ = ozObjVectorFromEye( self )
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistanceFromEye( self )
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading + ( 0.5 - math.random() ) * 4.0 )
      ozSelfSetV( ozObjPitchFromEye( self ) + ( 0.5 - math.random() ) * 4.0 )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfActionAttack()
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindObjOverlaps( 100, 100, 100 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistanceFromEye( self ) < 4 or math.abs( 180.0 - ozObjRelativeHeadingFromSelf() ) > 60.0 ) and
         ozObjIsVisibleFromEye( self )
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
  local self = ozObjGetIndex()

  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjIsVisibleFromEye( self ) then
      local dX, dY, dZ = ozObjVectorFromEye( self )
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistanceFromEye( self )
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading )
      ozSelfSetV( ozObjPitchFromEye( self ) )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfActionAttack()
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindObjOverlaps( 200, 200, 200 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistanceFromEye( self ) < 4 or math.abs( 180.0 - ozObjRelativeHeadingFromSelf() ) > 60.0 ) and
         ozObjIsVisibleFromEye( self )
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
  local self = ozObjGetIndex()

  if not l.pivotX then
    l.pivotX, l.pivotY, l.pivotZ = ozSelfGetPos()
    ozSelfSetRunning( false )
  end

  if l.target then
    ozBindObj( l.target )
    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjIsVisibleFromEye( self ) then
      local dX, dY, dZ = ozObjVectorFromEye( self )
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistanceFromEye( self )
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading + ( 0.5 - math.random() ) * 4.0 )
      ozSelfSetV( ozObjPitchFromEye( self ) + ( 0.5 - math.random() ) * 4.0 )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfActionAttack()
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindObjOverlaps( 100, 100, 100 )

    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistanceFromEye( self ) < 4 or math.abs( 180.0 - ozObjRelativeHeadingFromSelf() ) > 60.0 ) and
         ozObjIsVisibleFromEye( self )
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

  ozSelfActionForward()
end

function droid_armouredPatrol( l )
  local self = ozObjGetIndex()

  if not l.pivotX then
    l.pivotX, l.pivotY, l.pivotZ = ozSelfGetPos()
    ozSelfSetRunning( false )
  end

  if l.target then
    ozBindObj( l.target )

    if ( ozObjHasFlag( OZ_BOT_BIT ) and ozObjIsVisibleFromEye( self ) ) or
       ( ozObjHasFlag( OZ_VEHICLE_BIT ) and ozObjIsVisibleFromEye( self ) )
    then
      local dX, dY, dZ = ozObjVectorFromEye( self )
      local vX, vY, xZ = ozDynGetVelocity()
      local dist = ozObjDistanceFromEye( self )
      local estimatedBulletTime = dist / 300.0

      dX = dX + estimatedBulletTime * vX
      dY = dY + estimatedBulletTime * vY

      local heading = math.atan2( -dX, dY ) * 180.0 / math.pi

      ozSelfSetH( heading )
      ozSelfSetV( ozObjPitchFromEye( self ) )

      if not l.aimed then
        l.aimed = true
      else
        ozSelfActionAttack()
      end
    else
      l.target = nil
    end

    return
  else
    ozSelfBindObjOverlaps( 200, 200, 200 )
    while ozBindNextObj() do
      if ozObjIsBot() and string.sub( ozObjGetClassName(), 1, 5 ) ~= "droid" and
         ( ozObjDistanceFromSelf() < 100 or math.abs( 180.0 - ozObjRelativeHeadingFromSelf() ) > 60.0 ) and
         ozBotIsVisibleFromSelfEyeToEye()
      then
        l.target = ozObjGetIndex()
        l.aimed = nil
        return
      elseif ozObjHasFlag( OZ_VEHICLE_BIT ) and
          ( ozObjDistanceFromSelf() < 100 or math.abs( 180.0 - ozObjRelativeHeadingFromSelf() ) > 60.0 ) and
          ozObjIsVisibleFromSelfEye()
      then
        local vehicleIndex = ozObjGetIndex()
        ozBindPilot()

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
    ozSelfActionForward()
    l.forward = true
  else
    l.forward = nil
  end
end

function goblin_defend( l )
  local self = ozObjGetIndex()

  if l.target then
    ozBindObj( l.target )

    if ozObjHasFlag( OZ_BOT_BIT ) and ozObjIsVisibleFromEye( self ) then
      ozSelfSetH( ozObjHeadingFromEye( self ) )
      ozSelfSetV( ozObjPitchFromEye( self ) )

      local dist = ozObjDistanceFromEye( self )

      if dist > 30 then
        l.target = nil
      elseif dist > 2 then
        ozSelfSetRunning( true )
        ozSelfActionForward()
      else
        ozSelfActionAttack()
      end
    else
      l.target = nil
    end
  else
    ozSelfBindObjOverlaps( 10, 10, 10 )
    while ozBindNextObj() do
      if ozObjHasFlag( OZ_BOT_BIT ) and string.sub( ozObjGetClassName(), 1, 7 ) ~= "goblin" and
         ozObjIsVisibleFromEye( self )
      then
        l.target = ozObjGetIndex()
      end
    end
  end
end
