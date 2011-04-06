--[[
 *  handlers.lua
 *  Various objects' handlers
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozOrbisRemoveObj()
    end
  else
    l.ticks = 25

    local distance
    local dirX, dirY, dirZ

    ozSelfBindObjOverlaps( 8, 8, 8 )
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 8 then
          distance = 8 - distance
          ozObjDamage( 100 + 10*distance )

          if distance < 7.9 and ozObjIsDynamic() then
            dirX, dirY, dirZ = ozObjDirectionFromSelf()
            distance = 2 * distance
            ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
          end
        end
      end
    end
  end
end

function bigExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozOrbisRemoveObj()
    end
  else
    l.ticks = 25

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
          ozObjDamage( 100 + 10*distance )

          if distance < 19.9 and ozObjIsDynamic() then
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
    l.time = 250 -- 5 s
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

function shell_onUpdate( l )
  if not l.time then
    l.time = 300
  elseif l.time > 0 then
    l.time = l.time - 1
  else
    ozObjDestroy()
  end
end

function serviceStation_onUpdate( l )
  if l.ticks and l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    -- check for vehicles every second
    l.ticks = 50

    ozSelfBindAllOverlaps( 6, 6, 2 )
    while ozObjBindNext() do
      if ozObjIsVehicle() then
        ozVehicleService()
      end
    end
  end
end

function serviceStation_onUse( l )
  ozObjBindUser()
  ozObjSetLife( 1.0 / 0.0 )
  ozBotSetStamina( 1.0 / 0.0 )
  ozBotRearm()
end
