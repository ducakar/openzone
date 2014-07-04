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

-- lua/nirvana/oz_missions.lua
--
-- AIs for cvicek mission.

--
-- Beast AI.
--

function beast_nervous(l)
  if math.random(100) == 1 then
    ozSelfAction(OZ_ACTION_FLIP)
  end
end

function beast_happy(l)
  if math.random(10) == 1 then
    ozSelfAction(OZ_ACTION_WAVE)
  end
end

beast_niceBeastie = Beast.explore

--
-- Droid AI.
--

Droid.guardAutomaton = Automaton:new {
  name = "Droid.guard",
  initial = "standGuard",

  standGuard = {
    onUpdate = standGuard,
    links = {
      {
        target = "shootTarget",
        condition = detectTargetFunc(100, 240, 5, "droid")
      }
    }
  },
  shootTarget = {
    onUpdate = shootTargetFunc(4.0),
    links = {
      {
        target = "standGuard",
        condition = isTargetLost
      }
    }
  }
}

Droid.sniperAutomaton = Automaton:new {
  name = "Droid.sniper",
  initial = "standGuard",

  standGuard = {
    onUpdate = standGuard,
    links = {
      {
        target = "shootTarget",
        condition = detectTargetFunc(200, 240, 5, "droid")
      }
    }
  },
  shootTarget = {
    onUpdate = shootTargetFunc(2.0),
    links = {
      {
        target = "standGuard",
        condition = isTargetLost
      }
    }
  }
}

Droid.patrolAutomaton = Automaton:new {
  name = "Droid.patrol",
  initial = "begin",

  begin = {
    onEnter = enterPatrol,
    links = {
      {
        target = "patrol"
      }
    }
  },
  patrol = {
    onUpdate = patrolFunc(50),
    links = {
      {
        target = "shootTarget",
        condition = detectTargetFunc(100, 240, 5, "droid")
      }
    }
  },
  shootTarget = {
    onUpdate = shootTargetFunc(4.0),
    links = {
      {
        target = "patrol",
        condition = isTargetLost
      }
    }
  }
}

Droid.armouredPatrolAutomaton = Automaton:new {
  name = "Droid.armouredPatrol",
  initial = "begin",

  begin = {
    onEnter = enterPatrol,
    links = {
      {
        target = "patrol"
      }
    }
  },
  patrol = {
    onUpdate = patrolFunc(150),
    links = {
      {
        target = "shootTarget",
        condition = detectTargetFunc(150, 240, 5, "droid")
      }
    }
  },
  shootTarget = {
    onUpdate = shootTargetFunc(1.0),
    links = {
      {
        target = "patrol",
        condition = isTargetLost
      }
    }
  }
}

droid_guard          = Droid.guardAutomaton:mind()
droid_sniper         = Droid.sniperAutomaton:mind()
droid_patrol         = Droid.patrolAutomaton:mind()
droid_armouredPatrol = Droid.armouredPatrolAutomaton:mind()

--
-- Goblin AI.
--

Goblin.guardAutomaton = Automaton:new {
  name = "Goblin.guard",
  initial = "standGuard",

  standGuard = {
    onUpdate = standGuard,
    links = {
      {
        target = "huntTarget",
        condition = detectTargetFunc(10, 180, 5, "goblin")
      }
    }
  },
  huntTarget = {
    onUpdate = huntTargetFunc(30, 1),
    links = {
      {
        target = "standGuard",
        condition = isTargetLost
      }
    }
  }
}

goblin_guard = Goblin.guardAutomaton:mind()
