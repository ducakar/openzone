/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2016 Davorin Učakar
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

Lua::Result::Result(lua_State* l)
  : l_(l)
{}

void Lua::Result::read(int index, bool* value) const
{
  *value = lua_toboolean(l_, index);
}

void Lua::Result::read(int index, int* value) const
{
  *value = int(lua_tointeger(l_, index));
}

void Lua::Result::read(int index, float* value) const
{
  *value = float(lua_tonumber(l_, index));
}

void Lua::Result::read(int index, String* value) const
{
  const char* s = lua_tostring(l_, index);
  *value = s == nullptr ? String() : String(s);
}

void Lua::Result::read(int index, Function** value) const
{
  *value = lua_tocfunction(l_, index);
}

void Lua::Result::read(int index, void** value) const
{
  *value = lua_touserdata(l_, index);
}

Lua::Result::~Result()
{
  if (l_ != nullptr) {
    lua_settop(l_, 0);
  }
}

Lua::Result::Result(Result&& other) noexcept
  : l_(other.l_)
{
  other.l_ = nullptr;
}

Lua::Result& Lua::Result::operator=(Lua::Result&& other) noexcept
{
  if (&other != this) {
    l_ = other.l_;

    other.l_ = nullptr;
  }
  return *this;
}

Lua::Field::Field(lua_State* l, const Field* parent, const char* name)
  : l_(l), parent_(parent), name_(name), index_(0)
{}

Lua::Field::Field(lua_State* l, const Field* parent, int index)
  : l_(l), parent_(parent), name_(nullptr), index_(index)
{
  OZ_ASSERT(parent != nullptr);
}

void Lua::Field::push() const
{
  if (parent_ == nullptr) {
    lua_getglobal(l_, name_);
  }
  else {
    parent_->push();

    if (name_ == nullptr) {
      lua_rawgeti(l_, -1, index_);
    }
    else {
      lua_getfield(l_, -1, name_);
    }

    lua_remove(l_, -2);
  }
}

void Lua::Field::pushValue(nullptr_t) const
{
  lua_pushnil(l_);
}

void Lua::Field::pushValue(bool value) const
{
  lua_pushboolean(l_, value);
}

void Lua::Field::pushValue(int value) const
{
  lua_pushinteger(l_, value);
}

void Lua::Field::pushValue(float value) const
{
  lua_pushnumber(l_, value);
}

void Lua::Field::pushValue(const char* value) const
{
  lua_pushstring(l_, value);
}

void Lua::Field::assign(void* data) const
{
  if (parent_ == nullptr) {
    lua_pushlightuserdata(l_, data);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushlightuserdata(l_, data);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
}

void Lua::Field::assign(const void* data, size_t size, const char* metatable) const
{
  if (parent_ == nullptr) {
    memcpy(lua_newuserdata(l_, size), data, size);

    if (metatable != nullptr) {
      lua_getglobal(l_, metatable);
      lua_setmetatable(l_, -1);
    }

    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    memcpy(lua_newuserdata(l_, size), data, size);

    if (metatable != nullptr) {
      lua_getglobal(l_, metatable);
      lua_setmetatable(l_, -1);
    }

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
}

Lua::Result Lua::Field::call(int nArgs) const
{
  if (lua_pcall(l_, nArgs, LUA_MULTRET, 0)) {
    OZ_ERROR("oz::Lua: %s", lua_tostring(l_, -1));
  }
  return Result(l_);
}

Lua::Field& Lua::Field::operator=(const Lua::Field& other)
{
  if (parent_ == nullptr) {
    other.push();
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    other.push();

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(const Lua::Result&)
{
  if (parent_ == nullptr) {
    lua_pushvalue(l_, 1);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushvalue(l_, 1);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(Lua::Type type)
{
  if (parent_ != nullptr) {
    parent_->push();
  }

  switch (type) {
    case NIL: {
      lua_pushnil(l_);
      break;
    }
    case BOOLEAN: {
      lua_pushboolean(l_, false);
      break;
    }
    case LIGHTUSERDATA: {
      lua_pushlightuserdata(l_, nullptr);
      break;
    }
    case NUMBER: {
      lua_pushinteger(l_, 0);
      break;
    }
    case STRING: {
      lua_pushstring(l_, "");
      break;
    }
    case TABLE: {
      lua_newtable(l_);
      break;
    }
    case FUNCTION:
    case USERDATA:
    case THREAD: {
      OZ_ERROR("oz::Lua: Assignment of an uninitialised function, user data or thread is not "
               "supported.");
    }
  }

  if (parent_ == nullptr) {
    lua_setglobal(l_, name_);
  }
  else {
    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(bool value)
{
  if (parent_ == nullptr) {
    lua_pushboolean(l_, value);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushboolean(l_, value);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(int value)
{
  if (parent_ == nullptr) {
    lua_pushinteger(l_, value);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushinteger(l_, value);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(float value)
{
  if (parent_ == nullptr) {
    lua_pushnumber(l_, value);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushnumber(l_, value);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(const char* value)
{
  if (parent_ == nullptr) {
    lua_pushstring(l_, value);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushstring(l_, value);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field& Lua::Field::operator=(Lua::Function* func)
{
  if (parent_ == nullptr) {
    lua_pushcfunction(l_, func);
    lua_setglobal(l_, name_);
  }
  else {
    parent_->push();
    lua_pushcfunction(l_, func);

    if (name_ == nullptr) {
      lua_rawseti(l_, -2, index_);
    }
    else {
      lua_setfield(l_, -2, name_);
    }

    lua_pop(l_, 1);
  }
  return *this;
}

Lua::Field Lua::Field::operator[](const char* name) const
{
  return Field(l_, this, name);
}

Lua::Field Lua::Field::operator[](int index) const
{
  return Field(l_, this, index);
}

Lua::Type Lua::Field::type() const
{
  push();

  Lua::Type t = Lua::Type(lua_type(l_, -1));

  lua_pop(l_, 1);
  return t;
}

bool Lua::Field::toBool() const
{
  push();

  bool value = lua_toboolean(l_, -1);

  lua_pop(l_, 1);
  return value;
}

int Lua::Field::toInt() const
{
  push();

  int value = int(lua_tointeger(l_, -1));

  lua_pop(l_, 1);
  return value;
}

float Lua::Field::toFloat() const
{
  push();

  float value = float(lua_tonumber(l_, -1));

  lua_pop(l_, 1);
  return value;
}

String Lua::Field::toString() const
{
  push();

  const char* s = lua_tostring(l_, -1);
  String value = s == nullptr ? String() : String(s);

  lua_pop(l_, 1);
  return value;
}

Lua::Function* Lua::Field::toFunction() const
{
  push();

  Function* value = lua_tocfunction(l_, -1);

  lua_pop(l_, 1);
  return value;
}

void* Lua::Field::toPointer() const
{
  push();

  void* value = lua_touserdata(l_, -1);

  lua_pop(l_, 1);
  return value;
}

void Lua::Field::setMetatable(const char* name)
{
  push();

  if (name == nullptr) {
    lua_pushnil(l_);
  }
  else {
    lua_getglobal(l_, name);
  }
  lua_setmetatable(l_, -2);

  lua_pop(l_, 1);
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

Lua::Lua(Lua&& other) noexcept
  : l_(other.l_)
{
  other.l_ = nullptr;
}

Lua& Lua::operator=(Lua&& other) noexcept
{
  if (&other != this) {
    l_ = other.l_;

    other.l_ = nullptr;
  }
  return *this;
}

Lua::Result Lua::operator()(const char* code) const
{
  if (luaL_dostring(l_, code) != 0) {
    OZ_ERROR("oz::Lua: %s", lua_tostring(l_, -1));
  }
  return Result(l_);
}

Lua::Field Lua::operator[](const char* name) const
{
  return Field(l_, nullptr, name);
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
      lua_pushinteger(l, lua_Integer(is->readInt64()));
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

      while (is->available() != 0 && *is->pos() != ']') {
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
      for (const Json& i : json.arrayCRange()) {
        readValue(l, i);

        lua_rawseti(l, -2, index);
        ++index;
      }
      break;
    }
    case Json::OBJECT: {
      lua_newtable(l);

      for (const auto& i : json.objectCRange()) {
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
        os->writeInt64(lua_tointeger(l, -1));
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
  Stream is(0);

  if (file.read(&is)) {
    if (luaL_loadbufferx(l_, is.begin(), is.available(), file, "t") != 0 ||
        lua_pcall(l_, 0, LUA_MULTRET, 0) != 0)
    {
      OZ_ERROR("oz::Lua: %s", lua_tostring(l_, -1));
    }
  }

  return Result(l_);
}

void Lua::loadDir(const File& dir) const
{
  for (const File& file : dir.list()) {
    if (!file.isRegular() || !file.hasExtension("lua")) {
      continue;
    }

    Stream is(0);
    if (!file.read(&is)) {
      continue;
    }

    if (luaL_loadbufferx(l_, is.begin(), is.available(), file, "t") != 0 ||
        lua_pcall(l_, 0, LUA_MULTRET, 0) != 0)
    {
      OZ_ERROR("oz::Lua: %s", lua_tostring(l_, -1));
    }
  }
}

void Lua::init(const char* libs)
{
  destroy();

  libs = String::index(libs, 'A') >= 0 ? "ctiosmdp" : libs;

  l_ = luaL_newstate();
  if (l_ == nullptr) {
    OZ_ERROR("oz::Lua: Failed to create Lua state");
  }

  luaL_requiref(l_, "", luaopen_base, true);

  if (String::index(libs, 'c') >= 0) {
#ifndef LUA_JITLIBNAME
    luaL_requiref(l_, LUA_COLIBNAME, luaopen_coroutine, true);
#endif
  }
  if (String::index(libs, 't') >= 0) {
    luaL_requiref(l_, LUA_TABLIBNAME, luaopen_table, true);
  }
  if (String::index(libs, 'i') >= 0) {
    luaL_requiref(l_, LUA_IOLIBNAME, luaopen_io, true);
  }
  if (String::index(libs, 'o') >= 0) {
    luaL_requiref(l_, LUA_OSLIBNAME, luaopen_os, true);
  }
  if (String::index(libs, 's') >= 0) {
    luaL_requiref(l_, LUA_STRLIBNAME, luaopen_string, true);
  }
  if (String::index(libs, 'm') >= 0) {
    luaL_requiref(l_, LUA_MATHLIBNAME, luaopen_math, true);
    luaL_dostring(l_, String::format("math.random(%u)", randomSeed));
  }
  if (String::index(libs, 'd') >= 0) {
    luaL_requiref(l_, LUA_DBLIBNAME, luaopen_debug, true);
  }
  if (String::index(libs, 'p') >= 0) {
    luaL_requiref(l_, LUA_LOADLIBNAME, luaopen_package, true);
  }

  lua_settop(l_, 0);
}

void Lua::destroy()
{
  if (l_ != nullptr) {
    OZ_ASSERT(lua_gettop(l_) == 0);

    lua_close(l_);
    l_ = nullptr;
  }
}

}
