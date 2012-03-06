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
  ozObjBindUser()

  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )
  end
end

function healUser100_disposable( l )
  ozObjBindUser()

  if ozBotGetState( OZ_BOT_MECHANICAL_BIT ) then
    ozUseFailed()
  else
    ozObjAddLife( 100.0 )

    ozObjBindSelf()
    ozObjDestroy( true )
  end
end

function spawnSmallExplosion( l )
  ozOrbisAddObj( OZ_FORCE, "smallExplosion", ozObjGetPos() );
end

function spawnBigExplosion( l )
  ozOrbisAddObj( OZ_FORCE, "bigExplosion", ozObjGetPos() );
end
