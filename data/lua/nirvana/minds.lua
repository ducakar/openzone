function randomWalk()
  ozActionForward()
  
  if math.random( 1000 ) % 100 == 0 then
    ozAddH( math.random() * 120.0 - 60.0 )
  end
  if math.random( 1000 ) % 250 == 0 then
    ozActionJump()
  end
  if math.random( 1000 ) % 250 == 0 then
    ozStateToggleRunning()
  end
end
