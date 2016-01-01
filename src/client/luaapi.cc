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

#include <client/Camera.hh>

namespace oz
{
namespace client
{

void importClientConstants(lua_State* l);

void importClientConstants(lua_State* l)
{
  registerLuaConstant(l, "OZ_CAMERA_NONE",      Camera::NONE);
  registerLuaConstant(l, "OZ_CAMERA_STRATEGIC", Camera::STRATEGIC);
  registerLuaConstant(l, "OZ_CAMERA_UNIT",      Camera::UNIT);
  registerLuaConstant(l, "OZ_CAMERA_CINEMATIC", Camera::CINEMATIC);
}

}
}
