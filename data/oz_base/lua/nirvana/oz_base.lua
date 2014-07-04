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

-- lua/nirvana/oz_base.lua
--
-- Common Lua Nirvana functions.

--
-- Implementation of automaton-based mind.
--
-- On each `:mind()` call, one step of automaton is processed. Automaton should be given as a table:
--
-- local automaton = Automaton:new{
--   name = "automatonName",
--   initial = "state1",
--
--   state1 = {
--     onEnter = function(localData) executeThisOnceWhenEnteringState() end,
--     onUpdate = function(localData) executeThisEachStepThatStartsAndFinishesInThisState() end,
--     links = {
--       {
--         target = "state2",
--         condition = function(localData) return shouldJumpToState2() end
--       },
--       {
--         target = "state3",
--         condition = function(localData) return shouldJumpToState3() end
--       }
--     }
--   },
--   state2 = {
--     onUpdate = function(localData) doSomething() end,
--     links = {
--       {
--         target = "state1",
--         condition = function(localData) return true end
--       }
--     }
--   },
--   state3 = {
--     onEnter = function(localData) doSomething() end
--   }
-- }
--
-- someMind = automaton:mind()
--
-- - State names are custom, "name" and "initial" members are mandatory.
-- - Each automaton must have a unique name so this method can detect possible automaton switches.
-- - At the first step an automaton goes to "initial" state and executes its "onEnter" method
-- - On each subsequent step, all links are checked first, and then:
--   - if state change occurs "onEnter" of the target state is executed
--   - if state remains the same "onUpdate" of the current state is executed
--

Automaton = {}

function Automaton:new(definition)
  setmetatable(definition, self)
  self.__index = self
  return definition
end

function Automaton:mind()
  return function(localData)
    if localData.automatonName ~= self.name then
      localData.automatonName = self.name
      localData.automatonState = self.initial

      local state = self[self.initial]
      if state.onEnter then
        state.onEnter(localData)
      end
    else
      local state = self[localData.automatonState]

      for i = 1, #state.links do
        local link = state.links[i]

        if link.condition == nil or link.condition(localData) then
          localData.automatonState = link.target

          state = self[link.target]
          if state.onEnter then
            state.onEnter(localData)
          end
          return
        end
      end
      if state.onUpdate then
        state.onUpdate(localData)
      end
    end
  end
end
