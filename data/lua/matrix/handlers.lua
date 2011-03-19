--[[
 *  handlers.lua
 *  Various handler functions
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function heal100( l )
  ozObjBindUser()
  ozObjAddLife( 100.0 )
end

function disposableHeal100( l )
  ozObjQuietDestroy()
  ozObjBindUser()
  ozObjAddLife( 100.0 )
end

function spawnGoblin( l )
  x, y, z = ozObjGetPos()
  ozOrbisAddObj( "Goblin", x, y, z + 2.0 )
end

function Explosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozOrbisRemoveObj()
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
	if distance < 20 then
	  distance = 20 - distance
	  ozObjDamage( distance*distance )

	  if distance < 20 and ozObjIsDynamic() then
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
    l.time = 150 -- 3s
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

function Bomb_onDestroy( l )
  ozOrbisForceAddObj( "Explosion", ozObjGetPos() )
end

function Rifle_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 3 - math.random() * 6,
		     3 - math.random() * 6,
		     3 - math.random() * 6

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
		  1.0, 1.0, 0.0, 1.9, 0.005, 5.0 );
end

function Axe_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()

  ozBindObjOverlaps( pX + 0.5*vX, pY + 0.5*vY, pZ + 0.5*vZ, 0.3, 0.3, 0.3 );
  while ozObjBindNext() do
    if not ( ozObjIsSelf() or ozObjIsUser() ) then
      ozObjDamage( 120.0 )
    end
  end
end
