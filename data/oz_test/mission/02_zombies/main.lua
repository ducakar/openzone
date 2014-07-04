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

-- mission/02_zombies/main.lua
--
-- Zombies! mission.

function onUpdate()
end

function onCreate()
  ozTerraLoad("sample")

  ozCaelumLoad("earth")
  ozCaelumSetHeading(math.pi / 8)
  ozCaelumSetPeriod(3600)
  ozCaelumSetTime(450)

  oz_me = ozOrbisAddObj(OZ_FORCE, "beast", -1028, 610, 50.60, OZ_EAST, true)
  ozBotSetName(ozProfileGetName())
  ozBotSetMind("")

  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1014.0, 620.0, 52.0, OZ_SOUTH)
  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1029.0, 610.0, 51.5, OZ_EAST)
  ozOrbisAddStr(OZ_FORCE, "beastBunker", -1010.0, 595.0, 53.0, OZ_NORTH)

  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1022.30, 596.59, 51.54)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1021.96, 597.80, 51.45)
  ozOrbisAddObj(OZ_FORCE, "smallCrate", -1023.41, 596.91, 50.29)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", -1022.48, 596.59, 50.63)
  ozOrbisAddObj(OZ_FORCE, "bigCrate", -1023.38, 597.93, 50.52)
  ozOrbisAddObj(OZ_FORCE, "metalCrate", -1021.95, 597.90, 50.54)
  ozObjAddItem("musicPlayer")
  ozObjAddItem("musicPlayer")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("beast$plasmagun")
  ozObjAddItem("nvGoggles")
  ozObjAddItem("galileo")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("firstAid")
  ozObjAddItem("bomb")
  ozObjAddItem("bomb")

  floraSeed(0.0015, 12, 0.75)

  local bounds = { -1100, -900, 500, 700 }

  for i = 1, 200 do
    local x = -1050 + 100 * math.random()
    local y = 550 + 100 * math.random()
    local z = ozTerraHeight(x, y)

    if z > 0 then
      ozOrbisAddObj(OZ_TRY, "zombie", x, y, z + 1.10)
    end
  end

  ozCameraAllowReincarnation(false)
  ozCameraAddSwitchableBot(oz_me)
  ozCameraSetBot(oz_me)

  ozUIBuildFrame(true)
end
