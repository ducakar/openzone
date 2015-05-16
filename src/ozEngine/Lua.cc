/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2014 Davorin Učakar
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file ozEngine/Lua.cc
 */

#include "Lua.hh"

#include <lua.hpp>

namespace oz
{

int Lua::randomSeed = 0;

void Lua::readValue(lua_State* l, InputStream* is)
{
  char ch = is->readChar();

  switch (ch) {
    case 'N': {
      lua_pushnil(l);
      break;
    }
    case 'F': {
      lua_pushboolean(l, false);
      break;
    }
    case 'T': {
      lua_pushboolean(l, true);
      break;
    }
    case 'n': {
      lua_pushnumber(l, is->readDouble());
      break;
    }
    case 's': {
      lua_pushstring(l, is->readString());
      break;
    }
    case '[': {
      lua_newtable(l);

      while (is->available() != 0 && (*is)[is->tell()] != ']') {
        readValue(l, is); // Key.
        readValue(l, is); // Value.

        lua_rawset(l, -3);
      }
      is->skip(1); // Skip final ']'.
      break;
    }
    default: {
      OZ_ERROR("oz::Lua: Invalid type char '%c' in serialised Lua data", ch);
    }
  }
}

void Lua::readValue(lua_State* l, const Json& json)
{
  switch (json.type()) {
    case Json::NIL: {
      lua_pushnil(l);
      break;
    }
    case Json::BOOLEAN: {
      lua_pushboolean(l, json.get(false));
      break;
    }
    case Json::NUMBER: {
      lua_pushnumber(l, json.get(0.0));
      break;
    }
    case Json::STRING: {
      lua_pushstring(l, json.get(""));
      break;
    }
    case Json::ARRAY: {
      lua_newtable(l);

      int index = 0;
      for (const Json& i : json.arrayCIter()) {
        readValue(l, i);

        lua_rawseti(l, -2, index);
        ++index;
      }
      break;
    }
    case Json::OBJECT: {
      lua_newtable(l);

      for (const auto& i : json.objectCIter()) {
        lua_pushstring(l, i.key);
        readValue(l, i.value);

        lua_rawset(l, -3);
      }
      break;
    }
  }
}

void Lua::writeValue(lua_State* l, OutputStream* os)
{
  int type = lua_type(l, -1);

  switch (type) {
    case LUA_TNIL: {
      os->writeChar('N');
      break;
    }
    case LUA_TBOOLEAN: {
      os->writeChar(lua_toboolean(l, -1) ? 'T' : 'F');
      break;
    }
    case LUA_TNUMBER: {
      os->writeChar('n');
      os->writeDouble(lua_tonumber(l, -1));
      break;
    }
    case LUA_TSTRING: {
      os->writeChar('s');
      os->writeString(lua_tostring(l, -1));
      break;
    }
    case LUA_TTABLE: {
      os->writeChar('[');

      lua_pushnil(l);
      while (lua_next(l, -2) != 0) {
        // key
        lua_pushvalue(l, -2);
        writeValue(l, os);
        lua_pop(l, 1);

        // value
        writeValue(l, os);

        lua_pop(l, 1);
      }

      os->writeChar(']');
      break;
    }
    default: {
      OZ_ERROR("oz::Lua: Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
               " LUA_TSTRING and LUA_TTABLE data types");
    }
  }
}

Json Lua::writeValue(lua_State* l)
{
  int type = lua_type(l, -1);

  switch (type) {
    case LUA_TNIL: {
      return Json::NIL;
    }
    case LUA_TBOOLEAN: {
      return lua_toboolean(l, -1);
    }
    case LUA_TNUMBER: {
      return lua_tonumber(l, -1);
    }
    case LUA_TSTRING: {
      return lua_tostring(l, -1);
    }
    case LUA_TTABLE: {
      int maxIndex  = -1;
      int nElements = 0;

      lua_pushnil(l);
      while (lua_next(l, -2) != 0) {
        lua_pop(l, 1);

        if (lua_type(l, -1) != LUA_TNUMBER) {
          lua_pop(l, 1);
          goto object;
        }

        int index = int(lua_tointeger(l, -1));
        if (index < 0) {
          lua_pop(l, 1);
          goto object;
        }

        maxIndex = max<int>(index, maxIndex);
        ++nElements;
      }

      if (maxIndex == nElements) {
        Json json = Json::ARRAY;

        lua_pushnil(l);
        while (lua_next(l, -2) != 0) {
          json.add(writeValue(l));
          lua_pop(l, 1);
        }

        return json;
      }
      else {
object:
        Json json = Json::OBJECT;

        lua_pushnil(l);
        while (lua_next(l, -2) != 0) {
          // key
          lua_pushvalue(l, -2);
          String key = lua_tostring(l, -1);
          lua_pop(l, 1);

          // value
          json.add(key, writeValue(l));
          lua_pop(l, 1);
        }

        return json;
      }
    }
    default: {
      OZ_ERROR("oz::Lua: Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
               " LUA_TSTRING and LUA_TTABLE data types");
    }
  }
}

void Lua::loadDir(const File& dir) const
{
  for (const File& file : dir.ls()) {
    if (file.type() != File::REGULAR || !file.hasExtension("lua")) {
      continue;
    }

    InputStream is = file.inputStream();
    if (is.available() == 0) {
      continue;
    }

    if (luaL_loadbufferx(l, is.begin(), is.available(), file.path(), "t") != 0 ||
        lua_pcall(l, 0, LUA_MULTRET, 0) != 0)
    {
      const char* errorMessage = lua_tostring(l, -1);

      OZ_ERROR("oz::Lua: %s", errorMessage);
    }
  }
}

void Lua::init()
{
  l = luaL_newstate();
  if (l == nullptr) {
    OZ_ERROR("oz::Lua: Failed to create Lua state");
  }

#if LUA_VERSION_NUM < 502
  lua_pushcfunction(l, luaopen_base);
  lua_pcall(l, 0, 0, 0);
  lua_pushcfunction(l, luaopen_table);
  lua_pcall(l, 0, 0, 0);
  lua_pushcfunction(l, luaopen_string);
  lua_pcall(l, 0, 0, 0);
  lua_pushcfunction(l, luaopen_math);
  lua_pcall(l, 0, 0, 0);
#else
  luaL_requiref(l, "",              luaopen_base,   true);
  luaL_requiref(l, LUA_TABLIBNAME,  luaopen_table,  true);
  luaL_requiref(l, LUA_STRLIBNAME,  luaopen_string, true);
  luaL_requiref(l, LUA_MATHLIBNAME, luaopen_math,   true);
  lua_settop(l, 0);
#endif

  luaL_dostring(l, String::format("math.random(%u)", randomSeed));

  if (lua_gettop(l) != 0) {
    OZ_ERROR("oz::Lua: Failed to initialise Lua libraries");
  }
}

void Lua::destroy()
{
  lua_close(l);
  l = nullptr;
}

void Lua::import(const char* name, APIFunc func)
{
  lua_register(l, name, func);
}

void Lua::set(const char* name, bool value)
{
  lua_pushboolean(l, value);
  lua_setglobal(l, name);
}

void Lua::set(const char* name, int value)
{
  lua_pushnumber(l, value);
  lua_setglobal(l, name);
}

void Lua::set(const char* name, double value)
{
  lua_pushnumber(l, value);
  lua_setglobal(l, name);
}

void Lua::set(const char* name, const char* value)
{
  lua_pushstring(l, value);
  lua_setglobal(l, name);
}

}
