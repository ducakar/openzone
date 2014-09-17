/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/JSON.hh
 *
 * `JSON` class.
 */

#pragma once

#include "Map.hh"
#include "File.hh"

namespace oz
{

/**
 * DOM class for reading JSON configuration files.
 *
 * This implementation strictly follows JSON standard with the following exceptions:
 * - `inf` and `-inf` (case-sensitive) represent positive and negative infinity respectively,
 * - `nan` (case-sensitive) represents not-a-number and
 * - C++-style comments are allowed.
 */
class JSON
{
public:

  /**
   * Value types.
   */
  enum Type
  {
    NIL,
    BOOLEAN,
    NUMBER,
    STRING,
    ARRAY,
    OBJECT
  };

  /**
   * Formatter parameters.
   */
  struct Format
  {
    int         indentSpaces;      ///< Number of spaces used for indentitation.
    int         alignmentColumn;   ///< Value alignment column.
    int         significantDigits; ///< Significant digits for formatting numbers.
    const char* lineEnd;           ///< EOL character sequence.
  };

  /// Default format (2 space indent, alignment on 32nd column, 9 significant digits, "\\n" EOL).
  static const Format DEFAULT_FORMAT;

  /**
   * Key-value type used in object initialiser lists.
   */
  typedef Map<const char*, JSON>::Pair Pair;

  /**
   * %Iterator for %JSON arrays with constant access to elements.
   */
  typedef List<const JSON>::CIterator ArrayCIterator;

  /**
   * %Iterator for %JSON arrays with non-constant access to elements.
   */
  typedef List<JSON>::Iterator ArrayIterator;

  /**
   * %Iterator for %JSON objects with constant access to elements.
   */
  typedef Map<String, JSON>::CIterator ObjectCIterator;

  /**
   * %Iterator for %JSON objects with non-constant access to elements.
   */
  typedef Map<String, JSON>::Iterator ObjectIterator;

private:

  struct Data;
  struct StringData;
  struct ArrayData;
  struct ObjectData;
  struct Parser;
  struct Formatter;

private:

  union
  {
    bool       boolean;     ///< Boolean value storage.
    double     number;      ///< Number value storage.
    Data*      data;        ///< Pointer to other, complex, value storage.
  };
  Type         valueType;   ///< Value type, `JSON::Type`.
  mutable bool wasAccessed; ///< For warnings about unused variables.

private:

  /**
   * Internal constructor.
   */
  explicit JSON(Data* data, Type valueType);

  /**
   * Helper function for `get()` for reading vectors, quaternions, matrices etc.
   */
  bool getVector(float* vector, int count) const;

public:

  /**
   * Create an instance of a given type with a default value.
   *
   * Default value is false for a boolean, 0.0 for a number, "" for a string or an empty container
   * for an array or an object.
   */
  JSON(Type type = NIL);

  /**
   * Create null value.
   */
  JSON(nullptr_t);

  /**
   * Create a boolean value.
   */
  JSON(bool value);

  /**
   * Create a number value for an integer.
   */
  JSON(int value);

  /**
   * Create a number value for a float.
   */
  JSON(float value);

  /**
   * Create a number value for a double.
   */
  JSON(double value);

  /**
   * Create a string value for a given string.
   */
  JSON(const String& value);

  /**
   * Create a string value for a given string.
   */
  JSON(const char* value);

  /**
   * Create an array of 3 numbers representing `Vec3` components.
   */
  JSON(const Vec3& v);

  /**
   * Create an array of 4 numbers representing `Vec4` components.
   */
  JSON(const Vec4& v);

  /**
   * Create an array of 4 numbers representing `Point` components.
   */
  JSON(const Point& p);

  /**
   * Create an array of 4 numbers representing `Plane` components.
   */
  JSON(const Plane& p);

  /**
   * Create an array of 4 numbers representing `Quat` components.
   */
  JSON(const Quat& q);

  /**
   * Create an array of 9 numbers representing `Mat3` components.
   */
  JSON(const Mat3& m);

  /**
   * Create an array of 16 numbers representing `Mat4` components.
   */
  JSON(const Mat4& m);

  /**
   * Create an array from initialiser list of JSON values.
   *
   * This constructor might be ambiguous when elements appear in pairs. Adding `JSON` before the
   * first array element solves the issue:
   * @code
   * JSON array = { JSON { 0, 1 }, { 1, 2 }, { 2, 0 } };
   * @endcode
   */
  JSON(InitialiserList<JSON> l);

  /**
   * Create an object from initialiser list of string-JSON pairs.
   *
   * For disambiguation from array initialiser one should add `JSON::Pair` before the first element:
   * @code
   * JSON object = { JSON::Pair { "key1", 1 }, { "key2", 2 }, { "key3", 3 } };
   * @endcode
   */
  JSON(InitialiserList<Pair> l);

  /**
   * Load from a file.
   */
  explicit JSON(const File& file);

  /**
   * Destructor.
   */
  ~JSON();

  /**
   * Copy constructor.
   */
  JSON(const JSON& j);

  /**
   * Move constructor.
   */
  JSON(JSON&& j);

  /**
   * Copy operator.
   */
  JSON& operator = (const JSON& j);

  /**
   * Move operator.
   */
  JSON& operator = (JSON&& j);

  /**
   * Equality.
   */
  bool operator == (const JSON& j) const;

  /**
   * Inequality.
   */
  bool operator != (const JSON& j) const;

  /**
   * %JSON array iterator with constant access.
   *
   * An invalid iterator is returned if the %JSON element is not an array.
   */
  ArrayCIterator arrayCIter() const;

  /**
   * %JSON array iterator with non-constant access.
   *
   * An invalid iterator is returned if the %JSON element is not an array.
   */
  ArrayIterator arrayIter();

  /**
   * %JSON object iterator with constant access.
   *
   * An invalid iterator is returned if the %JSON element is not an object.
   */
  ObjectCIterator objectCIter() const;

  /**
   * %JSON object iterator with non-constant access.
   *
   * An invalid iterator is returned if the %JSON element is not an object.
   */
  ObjectIterator objectIter();

  /**
   * Type of value.
   */
  OZ_ALWAYS_INLINE
  Type type() const
  {
    return valueType;
  }

  /**
   * True iff null.
   */
  OZ_ALWAYS_INLINE
  bool isNull() const
  {
    return valueType == NIL;
  }

  /**
   * Number of entries if an array or an object, -1 otherwise.
   */
  int length() const;

  /**
   * True iff `length() <= 0`.
   */
  int isEmpty() const
  {
    return length() <= 0;
  }

  /**
   * Returns value at position `i` in an array.
   *
   * If the index is out of bounds or the value not an array null, a null value is returned.
   */
  const JSON& operator [] (int i) const;

  /**
   * Returns value for `key` in an object.
   *
   * If the key does not exist or the value not an object, a null value is returned.
   */
  const JSON& operator [] (const char* key) const;

  /**
   * True iff value is an object and contains a given key.
   *
   * If value is not either null or an object, `System::error()` is invoked.
   */
  bool contains(const char* key) const;

  /**
   * If a boolean return it, `defaultValue` otherwise.
   */
  bool get(bool defaultValue) const;

  /**
   * If a number return it, `defaultValue` otherwise.
   */
  double get(double defaultValue) const;

  /**
   * If a number return it cast to the specified numeric type, `defaultValue` otherwise.
   */
  template <typename Number>
  Number get(Number defaultValue = Number()) const
  {
    return Number(get(double(defaultValue)));
  }

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  const String& get(const String& defaultValue = String::EMPTY) const;

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  const char* get(const char* defaultValue = "") const;

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  Vec3 get(const Vec3& defaultValue = Vec3::ZERO) const;

  /**
   * If an array of at least 4 numbers, return it as `Vec4`, `defaultValue` otherwise.
   */
  Vec4 get(const Vec4& defaultValue = Vec4::ZERO) const;

  /**
   * If an array of at least 3 numbers, return it as `Point`, `defaultValue` otherwise.
   */
  Point get(const Point& defaultValue = Point::ORIGIN) const;

  /**
   * If an array of at least 4 numbers, return it as `Plane`, `defaultValue` otherwise.
   */
  Plane get(const Plane& defaultValue = Plane::ZERO) const;

  /**
   * If an array of at least 4 numbers, return it as `Quat`, `defaultValue` otherwise.
   */
  Quat get(const Quat& defaultValue = Quat::ID) const;

  /**
   * If an array of at least 9 numbers, return it as `Mat3`, `defaultValue` otherwise.
   */
  Mat3 get(const Mat3& defaultValue = Mat3::ID) const;

  /**
   * If an array of at least 4 numbers, return it as `Mat4`, `defaultValue` otherwise.
   */
  Mat4 get(const Mat4& defaultValue = Mat4::ID) const;

  /**
   * Write array values to a given array.
   *
   * If the %JSON array does not contain enough elements or is not an array at all the redundant
   * elements in the output array are left intact. When an array element is of an invalid type
   * `defaultValue` is used.
   *
   * @return number of elements actually read.
   */
  template <typename Type>
  int getArray(Type* array, int count, const Type& defaultValue) const
  {
    ArrayCIterator iter = arrayCIter();
    count = min<int>(count, length());

    for (int i = 0; i < count; ++i, ++iter) {
      array[i] = iter->get(defaultValue);
    }
    return count;
  }

  /**
   * Append a value to array (copy).
   *
   * If the current value is not an array, `System::error()` is invoked.
   */
  JSON& add(const JSON& json);

  /**
   * Append a value to array (move).
   *
   * If the current value is not an array, `System::error()` is invoked.
   */
  JSON& add(JSON&& json);

  /**
   * Add a value with a given key to the object (copy) overwriting an existing entry with that key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add(const char* key, const JSON& value);

  /**
   * Add a value with a given key to the object (move) overwriting an existing entry with that key.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  JSON& add(const char* key, JSON&& value);

  /**
   * Add a value with a given key to the object (copy) if the key does not exist in the object.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  JSON& include(const char* key, const JSON& json);

  /**
   * Add a value with a given key to the object (move) if the key does not exist in the object.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  JSON& include(const char* key, JSON&& json);

  /**
   * Remove element at a given index from an array.
   *
   * If the current value is not an array, `System::error()` is invoked.
   *
   * @return false iff an invalid index was given.
   */
  bool erase(int index);

  /**
   * Remove element with a given key from an object.
   *
   * If the current value is not an object, `System::error()` is invoked.
   *
   * @return true iff key was found (and removed).
   */
  bool exclude(const char* key);

  /**
   * Recursively clear node and its children.
   *
   * @param warnUnused warn about unused variables.
   * @return True iff `warnUnused` is true and there are unused variables.
   */
  bool clear(bool warnUnused = false);

  /**
   * String representation of a value.
   *
   * This function does not format the string or sort object entries.
   */
  String toString() const;

  /**
   * Formatted String representation of a value.
   *
   * This function returns string written by `write()` function.
   */
  String toFormattedString(const Format& format = DEFAULT_FORMAT) const;

  /**
   * Clear existing value and read new contents from a %JSON file.
   *
   * If file open fails, existing value is kept intact.
   *
   * @return true iff file is successfully read and parsed.
   */
  bool load(const File& file);

  /**
   * Write to a file.
   */
  bool save(const File& file, const Format& format = DEFAULT_FORMAT) const;

};

}
