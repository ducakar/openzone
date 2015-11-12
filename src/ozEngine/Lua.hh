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
 * @file ozEngine/Lua.hh
 *
 * Wrapper for %Lua interpreter.
 */

#pragma once

#include "common.hh"

// Forward declaration to prevent pollution from Lua headers.
struct lua_State;

namespace oz
{

/**
 * %Lua VM wrapper/base class.
 *
 * It provides common functionality for %Lua VMs, like (de)serialisation of variables to (from)
 * binary streams or JSON files, registering scripting API function and constants etc.
 */
class Lua
{
public:

  /**
   * %Lua types.
   */
  enum Type
  {
    NIL,           ///< Nil.
    BOOLEAN,       ///< Boolean.
    LIGHTUSERDATA, ///< Light user data.
    NUMBER,        ///< Number.
    STRING,        ///< %String.
    TABLE,         ///< Table.
    FUNCTION,      ///< Function.
    USERDATA,      ///< User data.
    THREAD         ///< %Thread / coroutine.
  };

  /**
   * Lua C API.
   */
  typedef int Function(lua_State*);

  /**
   * Wrapper class for reading stack values, e.g. values returnes by a function.
   */
  class Result
  {
    friend class Lua;

  private:

    lua_State* l; ///< %Lua state.

  private:

    /**
     * Create stack
     */
    explicit Result(lua_State* l);

    /**
     * Read a bool value at a given (1-based) stack index.
     */
    void read(int index, bool& value) const;

    /**
     * Read an int value at a given (1-based) stack index.
     */
    void read(int index, int& value) const;

    /**
     * Read a float value at a given (1-based) stack index.
     */
    void read(int index, float& value) const;

    /**
     * Read a String value at a given (1-based) stack index.
     */
    void read(int index, String& value) const;

  public:

    /**
     * Clear stack if a valid instance (i.e. has not been moved).
     */
    ~Result();

    /**
     * Move constructor.
     */
    Result(Result&& r);

    /**
     * Move operator.
     */
    Result& operator = (Result&& r);

    /**
     * Return the first value on the stack as a bool.
     */
    operator bool () const
    {
      return toBool(1);
    }

    /**
     * Return the first value on the stack as an int.
     */
    operator int () const
    {
      return toInt(1);
    }

    /**
     * Return the first value on the stack as a float.
     */
    operator float () const
    {
      return toFloat(1);
    }

    /**
     * Return the first value on the stack as a String.
     */
    operator String () const
    {
      return toString(1);
    }

    /**
     * Return the stack value at a given (1-based) index as a bool.
     */
    bool toBool(int index = 1) const
    {
      bool value;
      read(index, value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as an int.
     */
    int toInt(int index = 1) const
    {
      int value;
      read(index, value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a float.
     */
    float toFloat(int index = 1) const
    {
      float value;
      read(index, value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a String.
     */
    String toString(int index = 1) const
    {
      String value;
      read(index, value);
      return value;
    }

    /**
     * Read stack values starting with (1-based) index N into given variables.
     */
    template <int N = 1, typename Head, typename... Tail>
    void to(Head& value, Tail&... tail) const
    {
      read(N, value);
      to<N + 1, Tail...>(tail...);
    }

    /**
     * Read stack values at (1-based) index N into a given variable.
     */
    template <int N = 1, typename Head>
    void to(Head& value) const
    {
      read(N, value);
    }

  };

  /**
   * Table key or global variable accessor.
   *
   * This class is used to read or modify table fields or global variables in a %Lua state.
   */
  class Field
  {
    friend class Lua;

  private:

    lua_State*   l;      ///< %Lua state.
    const Field* parent; ///< Parent field / global variable.
    const char*  name;   ///< %String key..
    int          index;  ///< Integer key.

  private:

    /**
     * Create accessor for string key.
     */
    explicit Field(lua_State* l, const Field* parent, const char* name);

    /**
     * Create accessor for integer key.
     */
    explicit Field(lua_State* l, const Field* parent, int index);

    /**
     * Recursively push all parent field values and finally this field value onto the stack.
     */
    void push() const;

    /**
     * NOP.
     */
    void pushValue() const
    {}

    /**
     * Push nil onto the stack.
     */
    void pushValue(nullptr_t) const;

    /**
     * Push boolean onto the stack.
     */
    void pushValue(bool value) const;

    /**
     * Push integer onto the stack.
     */
    void pushValue(int value) const;

    /**
     * Push number onto the stack.
     */
    void pushValue(float value) const;

    /**
     * Push string value onto the stack.
     */
    void pushValue(const char* value) const;

    /**
     * Push values onto the stack.
     */
    template <typename Value, typename... Tail>
    void pushValue(Value value, Tail... tail) const
    {
      pushValue(value);
      pushValue(static_cast<Tail&&>(tail)...);
    }

    /**
     * Call a function with given number of arguments.
     *
     * Call is MULTRET so the stack contains all the returned values after the function finishes.
     */
    Result call(int nArgs) const;

  public:

    /**
     * Move constructor.
     */
    Field(Field&& s);

    /**
     * Move operator.
     */
    Field& operator = (Field&& s);

    /**
     * Asssing nil to the field.
     */
    Field& operator = (nullptr_t);

    /**
     * Assign boolean value to the field.
     */
    Field& operator = (bool value);

    /**
     * Assign an integer value to the field.
     */
    Field& operator = (int value);

    /**
     * Assign a number value to the field.
     */
    Field& operator = (float value);

    /**
     * Assign a string value to the field.
     */
    Field& operator = (const char* value);

    /**
     * Assing a function to the field.
     */
    Field& operator = (Function* func);

    /**
     * Access a field of the current field (should be a table).
     */
    Field operator [] (const char* name) const;

    /**
     * Access a field of the current field (should be a table).
     */
    Field operator [] (int index) const;

    /**
     * Call the current field (must be a function).
     */
    template <typename... Params>
    Result operator () (Params... params) const
    {
      push();
      pushValue(static_cast<Params&&>(params)...);
      return call(sizeof...(params));
    }

    /**
     * Get value of the current field as a boolean.
     */
    operator bool () const
    {
      return toBool();
    }

    /**
     * Get value of the current field as an integer.
     */
    operator int () const
    {
      return toInt();
    }

    /**
     * Get value of the current field as a float.
     */
    operator float () const
    {
      return toFloat();
    }

    /**
     * Get value of the current field as a string.
     */
    operator String () const
    {
      return toString();
    }

    /**
     * Get value of the current field as a boolean.
     */
    bool toBool() const;

    /**
     * Get value of the current field as a integer.
     */
    int toInt() const;

    /**
     * Get value of the current field as a float.
     */
    float toFloat() const;

    /**
     * Get value of the current field as a string.
     */
    String toString() const;

    /**
     * Determine type of the current field.
     */
    Type type() const;

  };

protected:

  lua_State* l = nullptr; ///< %Lua state escriptor.

public:

  static int randomSeed; ///< Random seed for Lua environments.

public:

  /**
   * Create an invalid instance.
   */
  Lua() = default;

  /**
   * Create a new %Lua state loading only given libraries.
   *
   * @sa oz::Lua::init()
   */
  explicit Lua(const char* libs);

  /**
   * Destroy state.
   */
  ~Lua();

  /**
   * Move constructor.
   */
  Lua(Lua&& s);

  /**
   * Move operator.
   */
  Lua& operator = (Lua&& s);

  /**
   * Execute a chunk of code.
   */
  Result operator () (const char* code) const;

  /**
   * Access a global variable.
   */
  Field operator [] (const char* name) const;

  /**
   * Read serialised Lua value and push it on the stack (recursively for tables).
   */
  static void readValue(lua_State* l, Stream* is);

  /**
   * Read Lua value from a JSON value and push it on the stack (recursively for tables).
   */
  static void readValue(lua_State* l, const Json& json);

  /**
   * Serialise Lua value at the top of the stack (recursively for tables).
   */
  static void writeValue(lua_State* l, Stream* os);

  /**
   * Return Lua value at the top of the stack (recursively for tables) as a JSON value.
   */
  static Json writeValue(lua_State* l);

  /**
   * Load and execute a script.
   */
  Result load(const File& file);

  /**
   * Load and execute all `*.lua` files in a directory.
   */
  void loadDir(const File& dir) const;

  /**
   * (Re)create a new %Lua state loading only given libraries.
   *
   * Characters of `libs` flag which default libraries should be loaded:
   *
   * - 'c' coroutine
   * - 't' table
   * - 'i' io
   * - 'o' os
   * - 's' string
   * - 'm' math
   * - 'd' debug
   * - 'p' package
   * - 'A' all libraries
   */
  void init(const char* libs);

  /**
   * Destroy %Lua state.
   */
  void destroy();

};

}
