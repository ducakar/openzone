--[[
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2012  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
]]--

--[[
 * matrix/generic.lua
 *
 * Generic handlers.
]]--

function healUser100( l )
  ozBindUser()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )
  end
end

function healUser100_disposable( l )
  ozBindUser()

  if ozBotHasState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )

    ozBindSelf()
    ozObjDestroy( true )
  end
end

function spawnSmallExplosion( l )
  ozOrbisAddObj( OZ_FORCE, "smallExplosion", ozObjGetPos() );
end

function spawnBigExplosion( l )
  ozOrbisAddObj( OZ_FORCE, "bigExplosion", ozObjGetPos() );
end

function smallExplosion_onUpdate( l )
  if l.ticks then
    l.ticks = l.ticks - 1

    if l.ticks == 0 then
      ozObjDestroy( true )
    end
  else
    l.ticks = 25

    ozObjAddEvent( OZ_EVENT_CREATE, 1 )
    ozObjBindOverlaps( OZ_OBJECTS_BIT, 8 )

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 8 then
          distance = 8 - distance

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 12*distance )

            if distance < 7.9 and ozObjHasFlag( OZ_DYNAMIC_BIT ) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()
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
      ozObjDestroy( true )
    end
  else
    l.ticks = 25

    ozObjAddEvent( OZ_EVENT_CREATE, 1 )
    ozObjBindOverlaps( OZ_STRUCTS_BIT + OZ_OBJECTS_BIT, 20 )

    while ozBindNextStr() do
      ozStrDamage( 1000 )
    end

    while ozBindNextObj() do
      if not ozObjIsSelf() then
        local distance = ozObjDistFromSelf()

        if distance < 20 then
          distance = 20 - distance

          ozObjDamage( 100 + 4*distance )

          if ozObjIsVisibleFromSelf() then
            ozObjDamage( 100 + 4*distance )

            if distance < 19.9 and ozObjHasFlag( OZ_DYNAMIC_BIT ) then
              local dirX, dirY, dirZ = ozObjDirFromSelf()
              distance = 2 * distance

              ozDynAddMomentum( dirX * distance, dirY * distance, dirZ * distance )
            end
          end
        end
      end
    end
  end
end
