function randomWalk( localData )
  ozActionForward()
  
  if math.random( 3 ) == 1 then
    ozAddH( math.random() * 120.0 - 60.0 )
  end
  if math.random( 3 ) == 1 then
    ozActionJump()
  end
  if math.random( 3 ) == 1 then
    ozStateToggleRunning()
  end
end

function randomRampage( localData )
  ozActionForward()
  if math.random( 3 ) == 1 then
    ozAddH( math.random() * 120.0 - 60.0 )
  end
end

function beastie( localData )
  if math.random( 3 ) == 1 then
    if localData.run then
      localData.run = false
      ozAddH( math.random( 180 ) - 90 )
    else
      localData.run = true
      ozStateSetRunning( true )
      if math.random( 2 ) == 1 then
	ozActionJump()
      end
    end
  end
  if localData.run then
    ozActionForward()
  end
end

function prey( localData )
  local minDistance = 100
  local heading

  ozBindOverlaps( 20 )
  while ozBindNext() do
    if not ozIsSelf() and ozIsBot() and ozGetType() ~= "Goblin" then
      local distance = ozGetDistanceTo()
      if distance < minDistance then
	minDistance = distance
	heading = ozGetHeadingTo()
      end
    end
  end
  if heading then
    ozBindSelf()
    ozSetH( heading + 180 )
    ozStateSetRunning( true )
    ozActionForward()
  elseif math.random( 3 ) == 1 then
    ozBindSelf()
    ozAddH( math.random( 120 ) - 60 )
  end
end

function predator( localData )
  local minDistance = 100
  local heading

  ozBindOverlaps( 20 )
  while ozBindNext() do
    if not ozIsSelf() and ozIsBot() and ozGetType() == "Goblin" then
      local distance = ozGetDistanceTo()
      if distance < minDistance then
	minDistance = distance
	heading = ozGetHeadingTo()
      end
    end
  end
  ozBindSelf()
  if heading then
    ozSetH( heading )
    ozStateSetRunning( true )
  else
    ozStateSetRunning( false )
    if math.random( 100 ) == 1 then
      ozAddH( math.random( 3 ) - 60 )
    end
  end
  ozActionForward()
end
