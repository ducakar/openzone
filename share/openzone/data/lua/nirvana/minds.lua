--[[
 *  minds.lua
 *  Various AIs
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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
    ozSelfStateToggleRunning()
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
      ozSelfStateSetRunning( true )
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
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetTypeName() ~= "goblin" then
      distance = ozObjDistanceFromSelf()
      if distance < minDistance then
	minDistance = distance
	heading = ozObjHeadingFromSelf()
      end
    end
  end
  if heading then
    ozSelfSetH( heading + 180 )
    ozSelfStateSetRunning( true )
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
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetTypeName() == "goblin" then
      distance = ozObjDistanceFromSelf()
      if distance < minDistance then
	minDistance = distance
	heading = ozObjHeadingFromSelf()
      end
    end
  end
  if heading then
    ozSelfSetH( heading )
    ozSelfStateSetRunning( true )
  else
    ozSelfStateSetRunning( false )
    if math.random( 100 ) == 1 then
      ozSelfAddH( math.random( 120 ) - 60 )
    end
  end
  ozSelfActionForward()
end
