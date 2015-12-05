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

#include "Lua.hh"

#include <cstring>
#include <lua.hpp>

#if LUA_VERSION_NUM < 502
# define luaL_requiref(l, name,  openFunc, global); \
  lua_pushcfunction(l, openFunc); \
  lua_pcall(l, 0, 0, 0);
#endif

namespace oz
{

Lua::Result::Result(lua_State* l) :
  l(l)
{}

void Lua::Result::read(int index, bool& value) const
{
  value = lua_toboolean(l, index);
}

void Lua::Result::read(int index, int& value) const
{
  value = int(lua_tointeger(l, index));
}

void Lua::Result::read(int index, float& value) const
{
  value = float(lua_tonumber(l, index));
}

void Lua::Result::read(int index, String& value) const
{
  const char* s = lua_tostring(l, index);
  value = s == nullptr ? String() : String(s);
}

void Lua::Result::read(int index, Function*& value) const
{
  value = lua_tocfunction(l, index);
}

void Lua::Result::read(int index, void*& value) const
{
  value = lua_touserdata(l, index);
}

Lua::Result::~Result()
{
  if (l != nullptr) {
    lua_settop(l, 0);
  }
}

Lua::Result::Result(Result&& r) :
  l(r.l)
{
  r.l = nullptr;
}

Lua::Result& Lua::Result::operator = (Lua::Result&& r)
{
  if (&r != this) {
    l = r.l;

    r.l = nullptr;
  }
  return *this;
}

Lua::Field::Field(lua_State* l, const Field* parent, const char* name) :
  l(l), parent(parent), name(name), index(0)
{}

Lua::Field::Field(lua_State* l, const Field* parent, int index) :
  l(l), parent(parent), name(nullptr), index(index)
{
  OZ_ASSERT(parent != nullptr);
}

void Lua::Field::push() const
{
  if (parent == nullptr) {
    lua_getglobal(l, name);
  }
  else {
    parent->push();

    if (name == nullptr) {
      lua_rawgeti(l, -1, index);
    }
    else {
      lua_getfield(l, -1, name);
    }

    lua_remove(l, -2);
  }
}

void Lua::Field::pushValue(nullptr_t) const
{
  lua_pushnil(l);
}

void Lua::Field::pushValue(bool value) const
{
  lua_pushboolean(l, value);
}

void Lua::Field::pushValue(int value) const
{
  lua_pushinteger(l, value);
}

void Lua::Field::pushValue(float value) const
{
  lua_pushnumber(l, value);
}

void Lua::Field::pushValue(const char* value) const
{
  lua_pushstring(l, value);
}

void Lua::Field::assign(void* data) const
{
  if (parent == nullptr) {
    lua_pushlightuserdata(l, data);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushlightuserdata(l, data);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
}

void Lua::Field::assign(const void* data, size_t size, const char* metatable) const
{
  if (parent == nullptr) {
    memcpy(lua_newuserdata(l, size), data, size);

    if (metatable != nullptr) {
      lua_getglobal(l, metatable);
      lua_setmetatable(l, -1);
    }

    lua_setglobal(l, name);
  }
  else {
    parent->push();
    memcpy(lua_newuserdata(l, size), data, size);

    if (metatable != nullptr) {
      lua_getglobal(l, metatable);
      lua_setmetatable(l, -1);
    }

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
}

Lua::Result Lua::Field::call(int nArgs) const
{
  if (lua_pcall(l, nArgs, LUA_MULTRET, 0)) {
    OZ_ERROR("oz::Lua: %s", lua_tostring(l, -1));
  }
  return Result(l);
}

Lua::Field& Lua::Field::operator = (const Lua::Field& s)
{
  if (parent == nullptr) {
    s.push();
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    s.push();

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (const Lua::Result&)
{
  if (parent == nullptr) {
    lua_pushvalue(l, 1);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushvalue(l, 1);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (Lua::Type type)
{
  if (parent != nullptr) {
    parent->push();
  }

  switch (type) {
    case NIL: {
      lua_pushnil(l);
      break;
    }
    case BOOLEAN: {
      lua_pushboolean(l, false);
      break;
    }
    case LIGHTUSERDATA: {
      lua_pushlightuserdata(l, nullptr);
      break;
    }
    case NUMBER: {
      lua_pushinteger(l, 0);
      break;
    }
    case STRING: {
      lua_pushstring(l, "");
      break;
    }
    case TABLE: {
      lua_newtable(l);
      break;
    }
    case FUNCTION:
    case USERDATA:
    case THREAD: {
      OZ_ERROR("oz::Lua: Assignment of an uninitialised function, user data or thread is not "
               "supported.");
    }
  }

  if (parent == nullptr) {
    lua_setglobal(l, name);
  }
  else {
    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (bool value)
{
  if (parent == nullptr) {
    lua_pushboolean(l, value);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushboolean(l, value);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (int value)
{
  if (parent == nullptr) {
    lua_pushinteger(l, value);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushinteger(l, value);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (float value)
{
  if (parent == nullptr) {
    lua_pushnumber(l, value);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushnumber(l, value);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (const char* value)
{
  if (parent == nullptr) {
    lua_pushstring(l, value);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushstring(l, value);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator = (Lua::Function* value)
{
  if (parent == nullptr) {
    lua_pushcfunction(l, value);
    lua_setglobal(l, name);
  }
  else {
    parent->push();
    lua_pushcfunction(l, value);

    if (name == nullptr) {
      lua_rawseti(l, -2, index);
    }
    else {
      lua_setfield(l, -2, name);
    }

    lua_pop(l, 1);
  }
  return *this;
}

Lua::Field Lua::Field::operator [] (const char* name) const
{
  return Field(l, this, name);
}

Lua::Field Lua::Field::operator [] (int index) const
{
  return Field(l, this, index);
}

Lua::Type Lua::Field::type() const
{
  push();

  Lua::Type t = Lua::Type(lua_type(l, -1));

  lua_pop(l, 1);
  return t;
}

bool Lua::Field::toBool() const
{
  push();

  bool value = lua_toboolean(l, -1);

  lua_pop(l, 1);
  return value;
}

int Lua::Field::toInt() const
{
  push();

  int value = int(lua_tointeger(l, -1));

  lua_pop(l, 1);
  return value;
}

float Lua::Field::toFloat() const
{
  push();

  float value = float(lua_tonumber(l, -1));

  lua_pop(l, 1);
  return value;
}

String Lua::Field::toString() const
{
  push();

  const char* s = lua_tostring(l, -1);
  String value = s == nullptr ? String() : String(s);

  lua_pop(l, 1);
  return value;
}

Lua::Function* Lua::Field::toFunction() const
{
  push();

  Function* value = lua_tocfunction(l, -1);

  lua_pop(l, 1);
  return value;
}

void* Lua::Field::toPointer() const
{
  push();

  void* value = lua_touserdata(l, -1);

  lua_pop(l, 1);
  return value;
}

void Lua::Field::setMetatable(const char* name)
{
  push();

  if (name == nullptr) {
    lua_pushnil(l);
  }
  else {
    lua_getglobal(l, name);
  }
  lua_setmetatable(l, -2);

  lua_pop(l, 1);
}

int Lua::randomSeed = 0;

Lua::Lua(const char* libs)
{
  init(libs);
}

Lua::~Lua()
{
  destroy();
}

Lua::Lua(Lua&& s) :
  l(s.l)
{
  s.l = nullptr;
}

Lua& Lua::operator = (Lua&& s)
{
  if (&s != this) {
    l = s.l;

    s.l = nullptr;
  }
  return *this;
}

Lua::Result Lua::operator () (const char* code) const
{
  if (luaL_dostring(l, code) != 0) {
    OZ_ERROR("oz::Lua: %s", lua_tostring(l, -1));
  }
  return Result(l);
}

Lua::Field Lua::operator [] (const char* name) const
{
  return Field(l, nullptr, name);
}

void Lua::readValue(lua_State* l, Stream* is)
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
    case 'i': {
      lua_pushinteger(l, lua_Integer(is->readLong64()));
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
      is->readChar(); // Skip final ']'.
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

void Lua::writeValue(lua_State* l, Stream* os)
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
#if LUA_VERSION_NUM >= 503
      if (lua_isinteger(l, -1)) {
        os->writeChar('i');
        os->writeLong64(lua_tointeger(l, -1));
        break;
      }
#endif

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

Lua::Result Lua::load(const File& file)
{
  Stream is = file.read();

  if (is.available() != 0) {
    if (luaL_loadbufferx(l, is.begin(), is.available(), file, "t") != 0 ||
        lua_pcall(l, 0, LUA_MULTRET, 0) != 0)
    {
      OZ_ERROR("oz::Lua: %s", lua_tostring(l, -1));
    }
  }

  return Result(l);
}

void Lua::loadDir(const File& dir) const
{
  for (const File& file : dir.list()) {
    if (!file.isFile() || !file.hasExtension("lua")) {
      continue;
    }

    Stream is = file.read();
    if (is.available() == 0) {
      continue;
    }

    if (luaL_loadbufferx(l, is.begin(), is.available(), file, "t") != 0 ||
        lua_pcall(l, 0, LUA_MULTRET, 0) != 0)
    {
      OZ_ERROR("oz::Lua: %s", lua_tostring(l, -1));
    }
  }
}

void Lua::init(const char* libs)
{
  destroy();

  libs = String::index(libs, 'A') >= 0 ? "ctiosmdp" : libs;

  l = luaL_newstate();
  if (l == nullptr) {
    OZ_ERROR("oz::Lua: Failed to create Lua state");
  }

  luaL_requiref(l, "", luaopen_base, true);

  if (String::index(libs, 'c') >= 0) {
#ifndef LUA_JITLIBNAME
    luaL_requiref(l, LUA_COLIBNAME, luaopen_coroutine, true);
#endif
  }
  if (String::index(libs, 't') >= 0) {
    luaL_requiref(l, LUA_TABLIBNAME, luaopen_table, true);
  }
  if (String::index(libs, 'i') >= 0) {
    luaL_requiref(l, LUA_IOLIBNAME, luaopen_io, true);
  }
  if (String::index(libs, 'o') >= 0) {
    luaL_requiref(l, LUA_OSLIBNAME, luaopen_os, true);
  }
  if (String::index(libs, 's') >= 0) {
    luaL_requiref(l, LUA_STRLIBNAME, luaopen_string, true);
  }
  if (String::index(libs, 'm') >= 0) {
    luaL_requiref(l, LUA_MATHLIBNAME, luaopen_math, true);
    luaL_dostring(l, String::format("math.random(%u)", randomSeed));
  }
  if (String::index(libs, 'd') >= 0) {
    luaL_requiref(l, LUA_DBLIBNAME, luaopen_debug, true);
  }
  if (String::index(libs, 'p') >= 0) {
    luaL_requiref(l, LUA_LOADLIBNAME, luaopen_package, true);
  }

  lua_settop(l, 0);
}

void Lua::destroy()
{
  if (l != nullptr) {
    OZ_ASSERT(lua_gettop(l) == 0);

    lua_close(l);
    l = nullptr;
  }
}

}
