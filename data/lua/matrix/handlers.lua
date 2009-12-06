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

function Explosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozWorldRemoveObj()
    end
  else
    l.ticks = 50

    local distance
    local dirX, dirY, dirZ

    ozSelfBindAllOverlaps( 20, 20, 20 )
    while ozStrBindNext() do
      ozStrDamage( 500 )
    end
    while ozObjBindNext() do
      if not ozObjIsSelf() then
	distance = ozObjDistanceFromSelf()
	if 0 < distance and distance < 20 then
	  distance = 20 - distance
	  ozObjDamage( distance*distance )

	  if ozObjIsDynamic() then
	    dirX, dirY, dirZ = ozObjDirectionFromSelf()
	    ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
	  end
	end
      end
    end
  end
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
  ozWorldForceAddObj( "Explosion", ozObjGetPos() )
end
