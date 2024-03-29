/*
 * ozCore - OpenZone Core Library.
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
 * @file ozCore/Json.hh
 *
 * `Json` class.
 */

#pragma once

#include "Map.hh"
#include "Mat4.hh"
#include "File.hh"
#include "EnumMap.hh"

namespace oz
{

/**
 * DOM class for reading JSON configuration files.
 *
 * This implementation strictly follows JSON standard with the following exceptions:
 * - `inf` and `-inf` (case-sensitive) represent positive and negative infinity respectively,
 * - `nan` (case-sensitive) represents not-a-number and
 * - C++-style comments are supported.
 */
class Json
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
    int         indentSpaces;    ///< Number of spaces used for indentation.
    int         alignmentColumn; ///< Value alignment column.
    const char* numberFormat;    ///< `printf()`-like number format.
  };

  /// Default format (2 space indent, alignment on 32nd column, 9 significant digits, "\\n" EOL).
  static const Format DEFAULT_FORMAT;

  /**
   * Key-value type used in object initialiser lists.
   */
  using Pair = Map<const char*, Json>::Pair;

  /**
   * %Range for JSON arrays with constant access to elements.
   */
  using ArrayCRange = List<const Json>::CRange;

  /**
   * %Range for JSON arrays with non-constant access to elements.
   */
  using ArrayRange = List<Json>::Range;

  /**
   * %Range for JSON objects with constant access to elements.
   */
  using ObjectCRange = Map<String, Json>::CRange;

  /**
   * %Range for JSON objects with non-constant access to elements.
   */
  using ObjectRange = Map<String, Json>::Range;

private:

  /**
   * Internal representation of a JSON array.
   */
  using Array = List<Json>;

  /**
   * Internal representation of a JSON object.
   */
  using Object = Map<String, Json>;

  class Parser;
  class Formatter;

private:

  union
  {
    bool       boolean_;               ///< Boolean value storage.
    double     number_;                ///< Number value storage.
    String*    string_;                ///< Pointer to string value storage.
    Array*     array_;                 ///< Pointer to array value storage.
    Object*    object_;                ///< Pointer to object value storage.
  };
  String       comment_;               ///< Comment.
  Type         type_        = NIL;     ///< Value type, `Json::Type`.
  mutable bool wasAccessed_ = false;   ///< For warnings about unused variables.

private:

  /**
   * Internal constructor for linear algebra types.
   */
  OZ_INTERNAL
  explicit Json(const float* vector, int count, const char* comment);

  /**
   * Helper function for `get()` for reading vectors, quaternions, matrices etc.
   */
  OZ_INTERNAL
  bool getVector(float* vector, int count) const;

public:

  /**
   * Create null value.
   */
  Json() = default;

  /**
   * Create an instance of a given type with a default value.
   *
   * Default value is false for a boolean, 0.0 for a number, "" for a string or an empty container
   * for an array or an object.
   */
  Json(Type type, const char* comment = "");

  /**
   * Create null value.
   */
  Json(nullptr_t, const char* comment = "");

  /**
   * Create a boolean value.
   */
  Json(bool value, const char* comment = "");

  /**
   * Create a number value for an integer.
   */
  Json(int value, const char* comment = "");

  /**
   * Create a number value for a float.
   */
  Json(float value, const char* comment = "");

  /**
   * Create a number value for a double.
   */
  Json(double value, const char* comment = "");

  /**
   * Create a string value for a given string.
   */
  Json(const String& value, const char* comment = "");

  /**
   * Create a string value for a given string.
   */
  Json(const char* value, const char* comment = "");

  /**
   * Create an array of 3 numbers representing `Vec3` components.
   */
  Json(const Vec3& v, const char* comment = "");

  /**
   * Create an array of 4 numbers representing `Point` components.
   */
  Json(const Point& p, const char* comment = "");

  /**
   * Create an array of 4 numbers representing `Plane` components.
   */
  Json(const Plane& p, const char* comment = "");

  /**
   * Create an array of 4 numbers representing `Vec4` components.
   */
  Json(const Vec4& v, const char* comment = "");

  /**
   * Create an array of 4 numbers representing `Quat` components.
   */
  Json(const Quat& q, const char* comment = "");

  /**
   * Create an array of 9 numbers representing `Mat3` components.
   */
  Json(const Mat3& m, const char* comment = "");

  /**
   * Create an array of 16 numbers representing `Mat4` components.
   */
  Json(const Mat4& m, const char* comment = "");

  /**
   * Create an array from initialiser list of JSON values.
   *
   * This constructor might be ambiguous when elements appear in pairs. Adding `Json` before the
   * first array element solves the issue:
   * @code
   * Json array = {Json {0, 1}, {1, 2}, {2, 0}};
   * @endcode
   */
  Json(InitialiserList<Json> il, const char* comment = "");

  /**
   * Create an object from initialiser list of string-JSON pairs.
   *
   * For disambiguation from array initialiser one should add `Json::Pair` before the first element:
   * @code
   * Json object = {Json::Pair {"key1", 1}, {"key2", 2}, {"key3", 3}};
   * @endcode
   */
  Json(InitialiserList<Pair> il, const char* comment = "");

  /**
   * Destructor.
   */
  ~Json();

  /**
   * Copy constructor.
   */
  Json(const Json& other);

  /**
   * Move constructor.
   */
  Json(Json&& other) noexcept;

  /**
   * Copy operator.
   */
  Json& operator=(const Json& other);

  /**
   * Move operator.
   */
  Json& operator=(Json&& other) noexcept;

  /**
   * Equality.
   */
  bool operator==(const Json& other) const;

  /**
   * JSON array range with constant access.
   *
   * An empty range is returned if the JSON element is not an array.
   */
  ArrayCRange arrayCRange() const noexcept;

  /**
   * JSON array range with non-constant access.
   *
   * An empty range is returned if the JSON element is not an array.
   */
  ArrayRange arrayRange() noexcept;

  /**
   * JSON object range with constant access.
   *
   * An empty range is returned if the JSON element is not an object.
   */
  ObjectCRange objectCRange() const noexcept;

  /**
   * JSON object range with non-constant access.
   *
   * An empty range is returned if the JSON element is not an object.
   */
  ObjectRange objectRange() noexcept;

  /**
   * Type of value.
   */
  OZ_ALWAYS_INLINE
  Type type() const noexcept
  {
    return type_;
  }

  /**
   * True iff null.
   */
  OZ_ALWAYS_INLINE
  bool isNull() const noexcept
  {
    return type_ == NIL;
  }

  /**
   * Number of entries if an array or an object, -1 otherwise.
   */
  int size() const noexcept;

  /**
   * True iff `length() <= 0`.
   */
  bool isEmpty() const noexcept
  {
    return size() <= 0;
  }

  /**
   * Returns value at position `i` in an array.
   *
   * If the index is out of bounds or the value not an array, a null value is returned.
   */
  const Json& operator[](int i) const;

  /**
   * Returns value for `key` in an object.
   *
   * If the key does not exist or the value not an object, a null value is returned.
   */
  const Json& operator[](const char* key) const;

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
  Number get(Number defaultValue) const
  {
    return Number(get(double(defaultValue)));
  }

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  String get(const String& defaultValue) const;

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  String get(String&& defaultValue) const;

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  File get(const File& defaultValue) const
  {
    return static_cast<File&&>(get(static_cast<const String&>(defaultValue)));
  }

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  File get(File&& defaultValue) const
  {
    return static_cast<File&&>(get(static_cast<String&&>(defaultValue)));
  }

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  const char* get(const char* defaultValue) const;

  /**
   * If a string return it, `defaultValue` otherwise.
   */
  Vec3 get(const Vec3& defaultValue) const;

  /**
   * If an array of at least 3 numbers, return it as `Point`, `defaultValue` otherwise.
   */
  Point get(const Point& defaultValue) const;

  /**
   * If an array of at least 4 numbers, return it as `Plane`, `defaultValue` otherwise.
   */
  Plane get(const Plane& defaultValue) const;

  /**
   * If an array of at least 4 numbers, return it as `Vec4`, `defaultValue` otherwise.
   */
  Vec4 get(const Vec4& defaultValue) const;

  /**
   * If an array of at least 4 numbers, return it as `Quat`, `defaultValue` otherwise.
   */
  Quat get(const Quat& defaultValue) const;

  /**
   * If an array of at least 9 numbers, return it as `Mat3`, `defaultValue` otherwise.
   */
  Mat3 get(const Mat3& defaultValue) const;

  /**
   * If an array of at least 16 numbers, return it as `Mat4`, `defaultValue` otherwise.
   */
  Mat4 get(const Mat4& defaultValue) const;

  /**
   * Write array values to a given array.
   *
   * If the JSON array does not contain enough elements or is not an array at all the redundant
   * elements in the output array are left intact. When an array element is of an invalid type
   * `defaultValue` is used.
   *
   * @return number of elements actually read.
   */
  template <typename Type>
  int getArray(Type* array, int count, const Type& defaultValue) const
  {
    ArrayCRange::Begin iter = arrayCRange().begin();
    count = min<int>(count, size());

    for (int i = 0; i < count; ++i, ++iter) {
      array[i] = iter->get(defaultValue);
    }
    return count;
  }

  /**
   * If a string convert it to the respective enum according to `enumMap`, `defaultValue` otherwise.
   */
  template <class Enum>
  Enum get(Enum defaultValue, const EnumMap<Enum>& enumMap) const
  {
    return enumMap[get(enumMap[int(defaultValue)])];
  }

  /**
   * Get comment assigned to this value.
   */
  OZ_ALWAYS_INLINE
  const String& comment() const
  {
    return comment_;
  }

  /**
   * Append a value to array (copy).
   *
   * If the current value is not an array, `System::error()` is invoked.
   */
  Json& add(const Json& value);

  /**
   * Append a value to array (move).
   *
   * If the current value is not an array, `System::error()` is invoked.
   */
  Json& add(Json&& value);

  /**
   * Add a value with a given key to the object (copy) overwriting an existing entry with that key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  Json& add(const char* key, const Json& value);

  /**
   * Add a value with a given key to the object (move) overwriting an existing entry with that key.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  Json& add(const char* key, Json&& value);

  /**
   * Add a value with a given key to the object (copy) if the key does not exist in the object.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  Json& include(const char* key, const Json& value);

  /**
   * Add a value with a given key to the object (move) if the key does not exist in the object.
   *
   * If the current value is not an object, `System::error()` is invoked.
   */
  Json& include(const char* key, Json&& value);

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
   * Clear existing value and read new contents from a JSON file.
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
