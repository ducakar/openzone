/*
 * ozEngine - OpenZone Engine Library.
 *
 * Copyright © 2002-2019 Davorin Učakar
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
  using Function = int(lua_State*);

  /**
   * Wrapper class for reading stack values, e.g. values returned by a function.
   */
  class Result
  {
    friend class Lua;

  private:

    lua_State* l_ = nullptr; ///< %Lua state.

  private:

    /**
     * Create stack
     */
    explicit Result(lua_State* l);

    /**
     * Read a bool value at a given (1-based) stack index.
     */
    void read(int index, bool* value) const;

    /**
     * Read an int value at a given (1-based) stack index.
     */
    void read(int index, int* value) const;

    /**
     * Read a float value at a given (1-based) stack index.
     */
    void read(int index, float* value) const;

    /**
     * Read a string value at a given (1-based) stack index.
     */
    void read(int index, String* value) const;

    /**
     * Read a C function value at a given (1-based) stack index.
     */
    void read(int index, Function** value) const;

    /**
     * Read a (light) user data pointer at a given (1-based) stack index.
     */
    void read(int index, void** value) const;

    /**
     * Read a (light) user data pointer at a given (1-based) stack index and cast it to the desired
     * type.
     */
    template <typename Type>
    void read(int index, Type** value) const
    {
      void* data = nullptr;
      read(index, &data);
      *value = static_cast<Type*>(data);
    }

  public:

    /**
     * Clear stack if a valid instance (i.e. has not been moved).
     */
    ~Result();

    /**
     * No copying.
     */
    Result(const Result&) = delete;

    /**
     * No moving.
     */
    Result(Result&& other) = delete;

    /**
     * No copying.
     */
    Result& operator=(const Result&) = delete;

    /**
     * No moving.
     */
    Result& operator=(Result&& other) = delete;

    /**
     * Return the stack value at a given (1-based) index as a bool.
     */
    bool toBool(int index = 1) const
    {
      bool value = false;
      read(index, &value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as an integer.
     */
    int toInt(int index = 1) const
    {
      int value = 0;
      read(index, &value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a float.
     */
    float toFloat(int index = 1) const
    {
      float value = 0.0f;
      read(index, &value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a string.
     */
    String toString(int index = 1) const
    {
      String value;
      read(index, &value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a C function.
     */
    Function* toFunction(int index = 1) const
    {
      Function* value = nullptr;
      read(index, &value);
      return value;
    }

    /**
     * Return the stack value at a given (1-based) index as a (light) user data pointer.
     */
    template <typename Type>
    Type* toPointer(int index = 1) const
    {
      Type* value = nullptr;
      read(index, &value);
      return value;
    }

    /**
     * Read stack values starting with (1-based) index N into given variables.
     */
    template <int N = 1, typename Head, typename ...Tail>
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

    lua_State*   l_;      ///< %Lua state.
    const Field* parent_; ///< Parent field / global variable.
    const char*  name_;   ///< %String key.
    int          index_;  ///< Integer key.

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
     * NOOP for `pushValue()` variadic template.
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
    template <typename Value, typename ...Tail>
    void pushValue(const Value& value, const Tail&... tail) const
    {
      pushValue(value);
      pushValue(tail...);
    }

    /**
     * Helper for assigning light user data.
     */
    void assign(void* data) const;

    /**
     * Call a function with given number of arguments.
     *
     * Call is MULTRET so the stack contains all the returned values after the function finishes.
     */
    Result call(int nArgs) const;

  public:

    /**
     * Destruction, no-op.
     */
    ~Field() = default;

    OZ_NO_COPY(Field)
    OZ_NO_MOVE(Field)

    /**
     * Assign first value of a function's result.
     */
    Field& operator=(const Result& other);

    /**
     * Assign an empty value of a given type to the field.
     */
    Field& operator=(Type type);

    /**
     * Assign boolean value to the field.
     */
    Field& operator=(bool value);

    /**
     * Assign an integer value to the field.
     */
    Field& operator=(int value);

    /**
     * Assign a number value to the field.
     */
    Field& operator=(float value);

    /**
     * Assign a string value to the field.
     */
    Field& operator=(const char* value);

    /**
     * Assign a C function to the field.
     */
    Field& operator=(Function* func);

    /**
     * Assign light user data pointer to the field.
     */
    template <class Data>
    Field& operator=(Data* data)
    {
      assign(data);
      return *this;
    }

    /**
     * Access a field of the current field (should be a table).
     */
    Field operator[](const char* name) const;

    /**
     * Access a field of the current field (should be a table).
     */
    Field operator[](int index) const;

    /**
     * Call the current field (must be a function).
     */
    template <typename ...Params>
    Result operator()(const Params&... params) const
    {
      push();
      pushValue(params...);
      return call(sizeof...(params));
    }

    /**
     * Get value of the current field as a boolean.
     */
    operator bool() const
    {
      return toBool();
    }

    /**
     * Get value of the current field as an integer.
     */
    operator int() const
    {
      return toInt();
    }

    /**
     * Get value of the current field as a float.
     */
    operator float() const
    {
      return toFloat();
    }

    /**
     * Get value of the current field as a string.
     */
    operator String() const
    {
      return toString();
    }

    /**
     * Get value of the current field as a C function.
     */
    operator Function*() const
    {
      return toFunction();
    }

    /**
     * Get value of the current field as a pointer to (light) user data.
     */
    template <typename Type>
    operator Type*() const
    {
      return static_cast<Type*>(toPointer());
    }

    /**
     * Determine type of the current field.
     */
    Type type() const;

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
     * Get value of the current field as a C function.
     */
    Function* toFunction() const;

    /**
     * Get value of the current field as a pointer to (light) user data.
     */
    void* toPointer() const;

    /**
     * Set metatable or remove it if null.
     */
    void setMetatable(const char* name);

  };

public:

  lua_State* l_ = nullptr; ///< %Lua state escriptor.

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

  OZ_NO_COPY(Lua)
  OZ_GENERIC_MOVE(Lua)

  /**
   * Execute a chunk of code.
   */
  Result operator()(const char* code) const;

  /**
   * Access a global variable.
   */
  Field operator[](const char* name) const;

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
