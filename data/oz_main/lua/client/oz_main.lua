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

-- lua/common/oz_main.lua
--
-- Flora module.

function floraSeed(density, spacing, treeDepth)
  local n = 4 * OZ_ORBIS_DIM * OZ_ORBIS_DIM * density
  for i = 1, n do
    local x = math.random() * 2*OZ_ORBIS_DIM - OZ_ORBIS_DIM
    local y = math.random() * 2*OZ_ORBIS_DIM - OZ_ORBIS_DIM
    local z = ozTerraHeight(x, y)

    if z < 100 and 2 < z then
      local type             = (z > 70 and "pine") or (z > 30 and "tree") or "palm"
      local dimX, dimY, dimZ = ozClassDim(type)

      if not ozOrbisOverlaps(0, x, y, z + dimZ + 2*OZ_EPSILON, dimX + spacing, dimY + spacing, dimZ)
      then
        ozOrbisAddObj(OZ_FORCE, type, x, y, z + dimZ - treeDepth)
      end
    end
  end
  ozPrintln("Flora seeded " .. n .. " trees")
end

-- Set up profile on first run.
if ozProfileGetClass() == "" then
  ozProfileSetClass("beast")
  ozProfileSetItems {
    "beast$plasmagun", "nvGoggles", "binoculars", "galileo", "musicPlayer", "cvicek", "cvicek"
  }
  ozProfileSetWeaponItem(0);
end
