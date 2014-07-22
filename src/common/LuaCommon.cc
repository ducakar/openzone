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
 * @file common/LuaCommon.cc
 */

#include <common/LuaCommon.hh>

#include <common/luabase.hh>

namespace oz
{

int  LuaCommon::randomSeed       = 0;
bool LuaCommon::isRandomSeedTime = true;

bool LuaCommon::readValue(lua_State* l, InputStream* is)
{
  char ch = is->readChar();

  switch (ch) {
    case 'N': {
      l_pushnil();
      return true;
    }
    case 'F': {
      l_pushbool(false);
      return true;
    }
    case 'T': {
      l_pushbool(true);
      return true;
    }
    case 'n': {
      l_pushdouble(is->readDouble());
      return true;
    }
    case 's': {
      l_pushstring(is->readString());
      return true;
    }
    case '[': {
      l_newtable();

      while (readValue(l, is)) { // key
        readValue(l, is); // value

        l_rawset(-3);
      }
      return true;
    }
    case ']': {
      return false;
    }
    default: {
      OZ_ERROR("Invalid type char '%c' in serialised Lua data", ch);
    }
  }
}

void LuaCommon::readValue(lua_State* l, const JSON& json)
{
  switch (json.type()) {
    case JSON::NIL: {
      l_pushnil();
      break;
    }
    case JSON::BOOLEAN: {
      l_pushbool(json.get(false));
      break;
    }
    case JSON::NUMBER: {
      l_pushdouble(json.get(0.0));
      break;
    }
    case JSON::STRING: {
      l_pushstring(json.get(""));
      break;
    }
    case JSON::ARRAY: {
      l_newtable();

      int index = 0;
      for (const JSON& i : json.arrayCIter()) {
        readValue(l, i);

        l_rawseti(-2, index);
        ++index;
      }
      break;
    }
    case JSON::OBJECT: {
      l_newtable();

      for (const auto& i : json.objectCIter()) {
        l_pushstring(i.key);
        readValue(l, i.value);

        l_rawset(-3);
      }
      break;
    }
  }
}

void LuaCommon::writeValue(lua_State* l, OutputStream* os)
{
  int type = l_type(-1);

  switch (type) {
    case LUA_TNIL: {
      os->writeChar('N');
      break;
    }
    case LUA_TBOOLEAN: {
      os->writeChar(l_tobool(-1) ? 'T' : 'F');
      break;
    }
    case LUA_TNUMBER: {
      os->writeChar('n');
      os->writeDouble(l_todouble(-1));
      break;
    }
    case LUA_TSTRING: {
      os->writeChar('s');
      os->writeString(l_tostring(-1));
      break;
    }
    case LUA_TTABLE: {
      os->writeChar('[');

      l_pushnil();
      while (l_next(-2) != 0) {
        // key
        l_pushvalue(-2);
        writeValue(l, os);
        l_pop(1);

        // value
        writeValue(l, os);

        l_pop(1);
      }

      os->writeChar(']');
      break;
    }
    default: {
      OZ_ERROR("Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
               " LUA_TSTRING and LUA_TTABLE data types");
    }
  }
}

JSON LuaCommon::writeValue(lua_State* l)
{
  int type = l_type(-1);

  switch (type) {
    case LUA_TNIL: {
      return JSON::NIL;
    }
    case LUA_TBOOLEAN: {
      return l_tobool(-1);
    }
    case LUA_TNUMBER: {
      return l_todouble(-1);
    }
    case LUA_TSTRING: {
      return l_tostring(-1);
    }
    case LUA_TTABLE: {
      int maxIndex  = -1;
      int nElements = 0;

      l_pushnil();
      while (l_next(-2) != 0) {
        l_pop(1);

        if (l_type(-1) != LUA_TNUMBER) {
          l_pop(1);
          goto object;
        }

        int index = l_toint(-1);
        if (index < 0) {
          l_pop(1);
          goto object;
        }

        maxIndex = max<int>(index, maxIndex);
        ++nElements;
      }

      if (maxIndex == nElements) {
        JSON json = JSON::ARRAY;

        l_pushnil();
        while (l_next(-2) != 0) {
          json.add(writeValue(l));
          l_pop(1);
        }

        return json;
      }
      else {
object:
        JSON json = JSON::OBJECT;

        l_pushnil();
        while (l_next(-2) != 0) {
          // key
          l_pushvalue(-2);
          String key = l_tostring(-1);
          l_pop(1);

          // value
          json.add(key, writeValue(l));
          l_pop(1);
        }

        return json;
      }
    }
    default: {
      OZ_ERROR("Serialisation is only supported for LUA_TNIL, LUA_TBOOLEAN, LUA_TNUMBER,"
               " LUA_TSTRING and LUA_TTABLE data types");
    }
  }
}

void LuaCommon::loadDir(const File& dir) const
{
  for (const File& file : dir.ls()) {
    if (file.type() != File::REGULAR || !file.hasExtension("lua")) {
      continue;
    }

    InputStream is = file.inputStream();

    if (!is.isAvailable()) {
      continue;
    }

    if (l_dobufferx(is.begin(), is.available(), file.path(), "t") != 0) {
      const char* errorMessage = l_tostring(-1);

      OZ_ERROR("Lua error: %s", errorMessage);
    }
  }
}

void LuaCommon::initCommon()
{
  l = luaL_newstate();
  if (l == nullptr) {
    OZ_ERROR("Failed to create Lua state");
  }

#if LUA_VERSION_NUM < 502
  lua_pushcfunction(l, luaopen_base);
  lua_pushcfunction(l, luaopen_table);
  lua_pushcfunction(l, luaopen_string);
  lua_pushcfunction(l, luaopen_math);
  lua_pcall(l, 0, 0, 0);
  lua_pcall(l, 0, 0, 0);
  lua_pcall(l, 0, 0, 0);
  lua_pcall(l, 0, 0, 0);
#else
  luaL_requiref(l, "",              luaopen_base,   true);
  luaL_requiref(l, LUA_TABLIBNAME,  luaopen_table,  true);
  luaL_requiref(l, LUA_STRLIBNAME,  luaopen_string, true);
  luaL_requiref(l, LUA_MATHLIBNAME, luaopen_math,   true);
  lua_settop(l, 0);
#endif

  if (l_gettop() != 0) {
    OZ_ERROR("Failed to initialise Lua libraries");
  }

  int seed = isRandomSeedTime ? int(Time::epoch()) : randomSeed;
  l_dostring(String::str("math.random(%u)", seed));
}

void LuaCommon::freeCommon()
{
  lua_close(l);
  l = nullptr;
}

void LuaCommon::registerFunction(const char* name, APIFunc func)
{
  l_register(name, func);
}

void LuaCommon::registerConstant(const char* name, bool value)
{
  l_pushbool(value);
  l_setglobal(name);
}

void LuaCommon::registerConstant(const char* name, int value)
{
  l_pushint(value);
  l_setglobal(name);
}

void LuaCommon::registerConstant(const char* name, float value)
{
  l_pushfloat(value);
  l_setglobal(name);
}

void LuaCommon::registerConstant(const char* name, const char* value)
{
  l_pushstring(value);
  l_setglobal(name);
}

}
