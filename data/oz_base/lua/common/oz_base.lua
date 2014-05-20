-- OpenZone - simple cross-platform FPS/RTS game engine.
--
-- Copyright © 2002-2014 Davorin Učakar
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.


-- lua/common/oz_base.lua
--
-- 3D vector utilities and finite automaton processor.

vec3 = {
  mix = function( ax, ay, az, bx, by, bz, t )
    return ax + t*( bx - ax ), ay + t*( by - ay ), az + t*( bz - az )
  end,

  length = function( x, y, z )
    return math.sqrt( vec.dot( x, y, z, x, y, z ) )
  end,

  distance = function( ax, ay, az, bx, by, bz )
    return length( ax - bx, ay - by, az - bz )
  end,

  dot = function( ax, ay, az, bx, by, bz )
    return ax*bx + ay*by + az*bz
  end,

  cross = function( ax, ay, az, bx, by, bz )
    return ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx
  end,

  normalize = function( x, y, z )
    local len = vec.length( x, y, z )
    return x / len, y / len, z / len
  end,

  reflect = function( ix, iy, iz, nx, ny, nz )
    local d = dot( ix, iy, iz, nx, ny, nz )
    return ix - d*nx, iy - d*ny, iz - d*nz
  end,

  random = function( d )
    local d2 = 2*d
    return d - d2*math.random(), d - d2*math.random(), d - d2*math.random()
  end
}

function processAutomaton( automaton, localData )
  local state = localData.state

  if not state then
    state = automaton[automaton.initial]
    localData.state = state

    if state.onEnter then
      state.onEnter( localData )
    end
  end

  if state.onUpdate then
    state.onUpdate( localData )
  end
  for i = 1, #state.links do
    local link = state.links[i]

    if link.condition( localData ) then
      state = automaton[link.target]
      localData.state = state

      if state.onEnter then
        state.onEnter( localData )
      end
      break
    end
  end
end
