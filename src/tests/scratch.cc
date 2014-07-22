/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 */

/**
 * @file tests/scratch.cc
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/Lua.hh>
#include <cstdio>
#include <lua.hpp>

using namespace oz;

static Lua lua;

int main()
{
  System::init();
  char line[1024];

  lua.init();
  lua.import("J", [](lua_State* l)
  {
    Log() << Lua::writeValue(l);
    return 0;
  });
  lua.set("FAK", 42);

  fputs("> ", stdout);
  while (fgets(line, 1024, stdin)) {
    lua.exec(line);
    fputs("> ", stdout);
  }
  putchar('\n');

  lua.destroy();
  return 0;
}
