function exception( l )
  ozException( "General exception" )
end

function heal50( l )
  ozWorldRemoveObj()
  ozObjBindUser()
  ozObjAddLife( 50.0 )
end

function spawnGoblin( l )
  x, y, z = ozObjGetPos()
  ozWorldAddObj( "Goblin", x, y, z + 2.0 )
end

function Bomb_onUse( l )
  if l.time then
    l.time = nil
  else
    l.time = 100
  end
end

function Bomb_onUpdate( l )
  if l.time then
    if l.time > 0 then
      l.time = l.time - 1
    else
      ozObjDestroy()
    end
  end
end

function Bomb_onDestroy( index )
  local distance

  ozSelfBindAllOverlaps( 20, 20, 20 )
  while ozStrBindNext() do
    ozStrDestroy()
  end
  while ozObjBindNext() do
    ozObjDestroy()
  end
end
