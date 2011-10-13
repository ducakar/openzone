--[[
 *  handlers.lua
 *  Various handlers
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
]]--

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjRemove()
    end
  else
    l.ticks = 25

    local distance
    local dirX, dirY, dirZ

    ozObjBindObjOverlaps( 8, 8, 8 )
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 8 then
          distance = 8 - distance

          if ozObjIsVisibleFromSelf then
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
end

function bigExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjRemove()
    end
  else
    l.ticks = 25

    local distance
    local dirX, dirY, dirZ

    ozObjBindAllOverlaps( 20, 20, 20 )
    while ozStrBindNext() do
      ozStrDamage( 500 )
    end
    while ozObjBindNext() do
      if not ozObjIsSelf() then
        distance = ozObjDistanceFromSelf()
        if distance < 20 then
          distance = 20 - distance

          if ozObjIsVisibleFromSelf() then
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
end

function cvicek_onUse( l )
  ozObjQuietDestroy()
  ozObjBindUser()
  ozObjAddLife( 30 )
  ozBotAddStamina( 30 )
end

function bomb_onUse( l )
  if not l.ticks then
    l.ticks = 250 -- 5 s
    ozObjEnableUpdate( true )
  else
    l.ticks = nil
    ozObjEnableUpdate( false )
  end
end

function bomb_onUpdate( l )
  if l.ticks ~= 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

function shell_onUpdate( l )
  if not l.ticks then
    l.ticks = 300
  elseif l.ticks > 0 then
    l.ticks = l.ticks - 1
  else
    ozObjDestroy()
  end
end

function serviceStation_onUse( l )
  ozObjBindUser()
  ozBotRearm()
  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozBotHeal()
  end

  ozObjBindAllOverlaps( 5, 5, 2 )
  while ozObjBindNext() do
    if ozObjIsVehicle() then
      ozVehicleService()
    end
  end
end
