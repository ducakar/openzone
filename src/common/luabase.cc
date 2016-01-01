/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

#include <common/luabase.hh>

namespace oz
{

void registerLuaConstant(lua_State* l, const char* name, bool value)
{
  l_pushbool(value);
  l_setglobal(name);
}

void registerLuaConstant(lua_State* l, const char* name, int value)
{
  l_pushint(value);
  l_setglobal(name);
}

void registerLuaConstant(lua_State* l, const char* name, float value)
{
  l_pushfloat(value);
  l_setglobal(name);
}

void registerLuaConstant(lua_State* l, const char* name, const char* value)
{
  l_pushstring(value);
  l_setglobal(name);
}

}
