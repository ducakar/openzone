function exception()
  ozException( "General exception" )
end

function heal50()
  ozRemoveObj()
  ozBindUser()
  ozAddLife( 50.0 )
end

function spawnGoblin()
  x, y, z = ozGetPos()
  ozAddObj( "Goblin", x, y, z + 2.0 )
end
