--
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
--

-- lua/common/oz_base.lua
--
-- Common Lua functions.

--
-- Common 3D vector operations.
--
Vec3 = {
  dot = function(ax, ay, az, bx, by, bz)
    return ax*bx + ay*by + az*bz
  end,

  cross = function(ax, ay, az, bx, by, bz)
    return ay*bz - az*by, az*bx - ax*bz, ax*by - ay*bx
  end,

  mix = function(ax, ay, az, bx, by, bz, t)
    return ax + t*(bx - ax), ay + t*(by - ay), az + t*(bz - az)
  end,

  length = function(x, y, z)
    return math.sqrt(Vec3.dot(x, y, z, x, y, z))
  end,

  normalize = function(x, y, z)
    local len = vec.length(x, y, z)
    return x / len, y / len, z / len
  end,

  distance = function(ax, ay, az, bx, by, bz)
    return length(ax - bx, ay - by, az - bz)
  end,

  reflect = function(ix, iy, iz, nx, ny, nz)
    local d = dot(ix, iy, iz, nx, ny, nz)
    return ix - d*nx, iy - d*ny, iz - d*nz
  end,

  random = function(d)
    local d2 = 2*d
    return d - d2*math.random(), d - d2*math.random(), d - d2*math.random()
  end
}

--
-- True iff the bound object's class begins with a given string.
--
function isSubclassOf(subclass)
  return ozObjGetClassName():sub(1, subclass:len()) == subclass
end
