--[[
 *  droid.lua
 *  AI for droid and droid commander
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function droid_randomWalk( l )
  ozSelfStateSetRunning( false )
  ozSelfActionForward()
  if math.random( 10 ) == 1 then
    ozSelfAddH( math.random( 120 ) - 60 )
  end
end

function droid_followCommander( l )
  if not l.commander then
    ozSelfBindObjOverlaps( 30, 30, 30 )
    while ozObjBindNext() do
      if ozObjIsBot() and not ozObjIsSelf() and ozObjGetTypeName() == "droidCommander" then
	l.commander = ozObjGetIndex()
        break
      end
    end
  else
    ozObjBindIndex( l.commander )
    if ozObjIsNull() or not ozObjIsBot() or ozObjGetTypeName() ~= "droidCommander" then
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
	ozSelfStateSetRunning( true )
	l.nearCommander = false
      else
	ozSelfStateSetRunning( false )
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

    if distance < 100 then
      ozSelfSetH( ozObjHeadingFromSelf() )
      ozSelfSetV( ozObjPitchFromSelfEye() )

      if distance < 20 then
	ozSelfActionAttack()
	ozForceUpdate()
      else
	ozSelfStateSetRunning( true )
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
    ozSelfBindObjOverlaps( 100, 100, 100 )
    while ozObjBindNext() do
      local typeName = ozObjGetTypeName()
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
