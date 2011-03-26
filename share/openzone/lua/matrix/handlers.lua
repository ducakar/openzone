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
  ozOrbisAddObj( "goblin", x, y, z + 2.0 )
end

function bigExplosion_onUpdate( l )
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
	  ozObjDamage( 100 + distance*distance )

	  if distance < 19.9 and ozObjIsDynamic() then
	    dirX, dirY, dirZ = ozObjDirectionFromSelf()
	    ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
	  end
	end
      end
    end
  end
end

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozOrbisRemoveObj()
    end
  else
    l.ticks = 50

    local distance
    local dirX, dirY, dirZ

    ozSelfBindObjOverlaps( 8, 8, 8 )
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 10 then
          distance = 10 - distance
          ozObjDamage( 100 + 4*distance*distance )

          if distance < 9.9 and ozObjIsDynamic() then
            dirX, dirY, dirZ = ozObjDirectionFromSelf()
            distance = 2 * distance
            ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
          end
        end
      end
    end
  end
end

function bomb_onUse( l )
  if l.time then
    l.time = nil
  else
    l.time = 150 -- 3s
  end
end

function bomb_onUpdate( l )
  if l.time then
    if l.time > 0 then
      l.time = l.time - 1
    else
      ozObjDestroy()
    end
  end
end

function bomb_onDestroy( l )
  ozOrbisForceAddObj( "bigExplosion", ozObjGetPos() )
end

function shell_onDestroy( l )
  ozOrbisForceAddObj( "smallExplosion", ozObjGetPos() );
end

function shell_onUpdate( l )
  if not l.ticks then
    l.ticks = 2 * 50
  elseif l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

function rifle_onShot( l )
  ozObjBindUser()

  local pX, pY, pZ = ozBotGetEyePos()
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 3 - math.random() * 6,
		     3 - math.random() * 6,
		     3 - math.random() * 6

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
		  1.0, 1.0, 0.0, 1.9, 0.005, 5.0 );
end

function tank_onShot0( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  ozOrbisAddPart( pX, pY, pZ, vX * 300, vY * 300, vZ * 300,
                  1.0, 0.2, 0.2, 1.9, 0.01, 3.0 );
end

function tank_onShot1( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()
  local dX, dY, dZ = 5 - math.random() * 10,
                     5 - math.random() * 10,
                     5 - math.random() * 10

  ozOrbisAddPart( pX, pY, pZ, vX * 200 + dX, vY * 200 + dY, vZ * 200 + dZ,
                  1.0, 1.0, 0.0, 1.9, 0.008, 3.0 );
end

function tank_onShot2( l )
  local pX, pY, pZ = ozObjGetPos()
  ozObjBindUser();
  local vX, vY, vZ = ozBotGetDir()

  ozOrbisForceAddObj( "shell", pX, pY, pZ )
  ozDynSetMomentum( vX * 400, vY * 400, vZ * 400 )
  ozDynSetVelocity( vX * 400, vY * 400, vZ * 400 )
end

function axe_onShot( l )
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

function serviceStation_onUpdate( l )
  if l.ticks and l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    -- check for vehicles every second
    l.ticks = 50

    ozSelfBindAllOverlaps( 4, 4, 4 )
    while ozObjBindNext() do
      if ozObjIsVehicle() then
        ozVehicleService()
      end
    end
  end
end
