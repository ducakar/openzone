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
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetTypeName() ~= "Goblin" then
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
    if not ozObjIsSelf() and ozObjIsBot() and ozObjGetTypeName() == "Goblin" then
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
      ozSelfAddH( math.random( 3 ) - 60 )
    end
  end
  ozSelfActionForward()
end

function droidDestroyer( l )
  if not l.target then
    print( "looking for target ..." )
    local minDistance = 100
    ozSelfBindObjOverlaps( 30, 30, 30 )
    while ozObjBindNext() do
      if ozObjIsBot() and not ozObjIsSelf() and ozObjGetTypeName() ~= "Droid" then
	local distance = ozObjDistanceFromSelf()
	if distance < minDistance then
	  l.target = ozObjGetIndex()
	  minDistance = distance

	  print( "new target:" .. ozObjGetTypeName() )
	end
      end
    end
  end
  if not l.target then
    if math.random( 10 ) == 1 then
      print( "changing direction" )
      ozSelfAddH( math.random( 120 ) - 60 )
    end
    ozSelfActionForward()
    ozSelfStateSetRunning( false )
  else
    print( "following target ..." )
    ozObjBindIndex( l.target )
    if not ozObjIsNull() and ozObjIsBot() then
      local distance = ozObjDistanceFromSelf()

      if distance < 40 then
        ozSelfSetH( ozObjHeadingFromSelf() )
        ozSelfSetV( ozObjPitchFromSelfEye() )

	if distance < 15 then
	  ozSelfActionAttack()
	else
	  ozSelfStateSetRunning( true )
	  ozSelfActionForward()
	end
      else
	l.target = nil
      end
    else
      l.target = nil
    end
  end
end
