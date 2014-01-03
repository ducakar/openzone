/*
 * ozCore - OpenZone Core Library.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

#include "HashMap.hh"
#include "File.hh"

namespace oz
{

/**
 * DOM class for reading JSON configuration files.
 *
 * This implementation strictly follows JSON standard with the following exceptions:
 * - `inf` and `-inf` (case-sensitive) represent positive and negative infinity respectively,
 * - `nan` and `-nan` (case-sensitive) represent not-a-number and
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

private:

  struct Data;
  struct BooleanData;
  struct NumberData;
  struct StringData;
  struct ArrayData;
  struct ObjectData;
  struct Parser;
  struct Formatter;

public:

  /**
   * %Iterator for %JSON arrays with constant access to elements.
   */
  typedef oz::ArrayIterator<const JSON> ArrayCIterator;

  /**
   * %Iterator for %JSON arrays with non-constant access to elements.
   */
  typedef oz::ArrayIterator<JSON> ArrayIterator;

  /**
   * %Iterator for %JSON objects with constant access to elements.
   */
  typedef HashMap<String, JSON>::CIterator ObjectCIterator;

  /**
   * %Iterator for %JSON objects with non-constant access to elements.
   */
  typedef HashMap<String, JSON>::Iterator ObjectIterator;

public:

  /// Default format (2 space indent, alignment on 32nd column, 9 significant digits, "\\n" EOL).
  static const Format DEFAULT_FORMAT;

private:

  /// A null value instance, required by `operator []`.
  static const JSON NIL_VALUE;

  Data*        data;        ///< Pointer to internal data structure.
  Type         valueType;   ///< Value type, `JSON::Type`.
  mutable bool wasAccessed; ///< For warnings about unused variables.

private:

  /**
   * Internal constructor.
   */
  explicit JSON( Data* data, Type valueType );

public:

  /**
   * Create an instance of a given type with a default value.
   *
   * Default value is false for a boolean, 0.0 for a number, "" for a string or an empty container
   * for an array or an object.
   */
  explicit JSON( Type type = NIL );

  /**
   * Create a boolean value.
   */
  explicit JSON( bool value );

  /**
   * Create a number value for an integer.
   */
  explicit JSON( int value );

  /**
   * Create a number value for a float.
   */
  explicit JSON( float value );

  /**
   * Create a number value for a double.
   */
  explicit JSON( double value );

  /**
   * Create a string value for a given string.
   */
  explicit JSON( const String& value );

  /**
   * Create a string value for a given string.
   */
  explicit JSON( const char* value );

  /**
   * Create an array of 3 numbers representing `Vec3` components.
   */
  explicit JSON( const Vec3& v );

  /**
   * Create an array of 4 numbers representing `Vec4` components.
   */
  explicit JSON( const Vec4& v );

  /**
   * Create an array of 4 numbers representing `Point` components.
   */
  explicit JSON( const Point& p );

  /**
   * Create an array of 4 numbers representing `Plane` components.
   */
  explicit JSON( const Plane& p );

  /**
   * Create an array of 4 numbers representing `Quat` components.
   */
  explicit JSON( const Quat& q );

  /**
   * Create an array of 9 numbers representing `Mat33` components.
   */
  explicit JSON( const Mat33& m );

  /**
   * Create an array of 16 numbers representing `Mat44` components.
   */
  explicit JSON( const Mat44& m );

  /**
   * Load from a file.
   */
  explicit JSON( const File& file );

  /**
   * Destructor.
   */
  ~JSON();

  /**
   * Copy constructor.
   */
  JSON( const JSON& v );

  /**
   * Move constructor.
   */
  JSON( JSON&& v );

  /**
   * Copy operator.
   */
  JSON& operator = ( const JSON& v );

  /**
   * Move operator.
   */
  JSON& operator = ( JSON&& v );

  /**
   * Type of value.
   */
  Type type() const
  {
    return valueType;
  }

  /**
   * True iff null value.
   */
  bool isNull() const
  {
    return valueType == NIL;
  }

  /**
   * Number of entries if value is an array or an object, -1 otherwise.
   */
  int length() const;

  /**
   * If an array or an object, true iff empty, always true otherwise.
   */
  int isEmpty() const;

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
   * Returns value at position `i` in an array.
   *
   * If the index is out of bounds or the value is null, a null value is returned.
   * If the value is not either an array or a null, `System::error()` is invoked.
   */
  const JSON& operator [] ( int i ) const;

  /**
   * Returns value for `key` in an object.
   *
   * If the key does not exist or the value is null, a null value is returned.
   * If value is not either an object or a null, `System::error()` is invoked.
   */
  const JSON& operator [] ( const char* key ) const;

  /**
   * True iff value is an object and contains a given key.
   *
   * If value is not either null or an object, `System::error()` is invoked.
   */
  bool contains( const char* key ) const;

  /**
   * Get boolean value.
   *
   * If value is not a boolean, `System::error()` is invoked.
   */
  bool asBool() const;

  /**
   * Get number value cast to integer.
   *
   * If value is not a number, `System::error()` is invoked.
   */
  int asInt() const;

  /**
   * Get number value.
   *
   * If value is not a number, `System::error()` is invoked.
   */
  float asFloat() const;

  /**
   * Get number value.
   *
   * If value is not a number, `System::error()` is invoked.
   */
  double asDouble() const;

  /**
   * Get string value.
   *
   * If value is not a string, `System::error()` is invoked.
   */
  const String& asString() const;

  /**
   * Get a number array as a `Vec3`.
   *
   * If value is not an array of 3 numbers, `System::error()` is invoked.
   */
  Vec3 asVec3() const;

  /**
   * Get a number array as a `Vec4`.
   *
   * If value is not an array of 4 numbers, `System::error()` is invoked.
   */
  Vec4 asVec4() const;

  /**
   * Get a number array as a `Point`.
   *
   * If value is not an array of 3 numbers, `System::error()` is invoked.
   */
  Point asPoint() const;

  /**
   * Get a number array as a `Plane`.
   *
   * If value is not an array of 4 numbers, `System::error()` is invoked.
   */
  Plane asPlane() const;

  /**
   * Get a number array as a `Quat` .
   *
   * If value is not an array of 4 numbers, `System::error()` is invoked.
   */
  Quat asQuat() const;

  /**
   * Get a number array as a `Mat33`.
   *
   * If value is not an array of 9 numbers, `System::error()` is invoked.
   */
  Mat33 asMat33() const;

  /**
   * Get a number array as a `Mat44`.
   *
   * If value is not an array of 16 numbers, `System::error()` is invoked.
   */
  Mat44 asMat44() const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are booleans,
   * `System::error()` is invoked.
   */
  void asArray( bool* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are numbers,
   * `System::error()` is invoked.
   */
  void asArray( int* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are numbers,
   * `System::error()` is invoked.
   */
  void asArray( float* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are numbers,
   * `System::error()` is invoked.
   */
  void asArray( double* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are strings,
   * `System::error()` is invoked.
   */
  void asArray( String* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 3 numbers, `System::error()` is invoked.
   */
  void asArray( Vec3* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 4 numbers, `System::error()` is invoked.
   */
  void asArray( Vec4* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 3 numbers, `System::error()` is invoked.
   */
  void asArray( Point* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 4 numbers, `System::error()` is invoked.
   */
  void asArray( Plane* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 4 numbers, `System::error()` is invoked.
   */
  void asArray( Quat* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 9 numbers, `System::error()` is invoked.
   */
  void asArray( Mat33* array, int count ) const;

  /**
   * Write array values into a given array.
   *
   * If value is not an array of the specified length or if not all array elements are arrays of
   * 16 numbers, `System::error()` is invoked.
   */
  void asArray( Mat44* array, int count ) const;

  /**
   * Return boolean value or `defaultValue` if null.
   *
   * If value is not either null or a boolean, `System::error()` is invoked.
   */
  bool get( bool defaultValue ) const;

  /**
   * Return integer value or `defaultValue` if null.
   *
   * If value is not either null or a number, `System::error()` is invoked.
   */
  int get( int defaultValue ) const;

  /**
   * Return number value or `defaultValue` if null.
   *
   * If value is not either null or a number, `System::error()` is invoked.
   */
  float get( float defaultValue ) const;

  /**
   * Return number value or `defaultValue` if null.
   *
   * If value is not either null or a number, `System::error()` is invoked.
   */
  double get( double defaultValue ) const;

  /**
   * Return string value or `defaultValue` if null.
   *
   * If value is not either null or a string, `System::error()` is invoked.
   */
  const String& get( const String& defaultValue ) const;

  /**
   * Return string value or `defaultValue` if null.
   *
   * If value is not either null or a string, `System::error()` is invoked.
   */
  const char* get( const char* defaultValue ) const;

  /**
   * Return number array as a `Vec3` or `defaultValue` if null.
   *
   * If value is not either null or an array of 3 numbers, `System::error()` is invoked.
   */
  Vec3 get( const Vec3& defaultValue ) const;

  /**
   * Return number array as a `Vec4` or `defaultValue` if null.
   *
   * If value is not either null or an array of 4 numbers, `System::error()` is invoked.
   */
  Vec4 get( const Vec4& defaultValue ) const;

  /**
   * Return number array as a `Point` or `defaultValue` if null.
   *
   * If value is not either null or an array of 3 numbers, `System::error()` is invoked.
   */
  Point get( const Point& defaultValue ) const;

  /**
   * Return number array as a `Plane` or `defaultValue` if null.
   *
   * If value is not either null or an array of 4 numbers, `System::error()` is invoked.
   */
  Plane get( const Plane& defaultValue ) const;

  /**
   * Return number array as a `Quat` or `defaultValue` if null.
   *
   * If value is not either null or an array of 4 numbers, `System::error()` is invoked.
   */
  Quat get( const Quat& defaultValue ) const;

  /**
   * Return number array as a `Mat33` or `defaultValue` if null.
   *
   * If value is not either null or an array of 9 numbers, `System::error()` is invoked.
   */
  Mat33 get( const Mat33& defaultValue ) const;

  /**
   * Return number array as a `Mat44` or `defaultValue` if null.
   *
   * If value is not either null or an array of 16 numbers, `System::error()` is invoked.
   */
  Mat44 get( const Mat44& defaultValue ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with boolean elements,
   * `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( bool* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with number elements,
   * `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( int* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with number elements,
   * `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( float* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with number elements,
   * `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( double* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with string elements,
   * `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( String* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 3 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Vec3* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 4 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Vec4* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 3 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Point* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 4 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Plane* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 4 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Quat* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 9 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Mat33* array, int count ) const;

  /**
   * Write array values into a given array (it is left unchanged if %JSON element is null).
   *
   * If value is not either null or an array of the specified length with arrays of 16 numbers as
   * elements, `System::error()` is invoked.
   *
   * @return true if destination array has been filled, false if %JSON element is null.
   */
  bool get( Mat44* array, int count ) const;

  /**
   * Append a value to array (copy).
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const JSON& json );

  /**
   * Append a value to array (move).
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( JSON&& json );

  /**
   * Append a default value of a given type to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( Type type = NIL )
  {
    return add( JSON( type ) );
  }

  /**
   * Append a boolean value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( bool value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a number value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( int value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a number value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( float value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a number value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( double value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a string value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const String& value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a string value to array.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* value )
  {
    return add( JSON( value ) );
  }

  /**
   * Append a `Vec3` to array as a new array of 3 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Vec3& v )
  {
    return add( JSON( v ) );
  }

  /**
   * Append a `Vec4` to array as a new array of 4 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Vec4& v )
  {
    return add( JSON( v ) );
  }

  /**
   * Append a `Point` to array as a new array of 3 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Point& p )
  {
    return add( JSON( p ) );
  }

  /**
   * Append a `Plane` to array as a new array of 4 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Plane& p )
  {
    return add( JSON( p ) );
  }

  /**
   * Append a `Quat` to array as a new array of 4 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Quat& q )
  {
    return add( JSON( q ) );
  }

  /**
   * Append a `Mat33` to array as a new array of 9 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Mat33& m )
  {
    return add( JSON( m ) );
  }

  /**
   * Append a `Mat44` to array as a new array of 16 numbers.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const Mat44& m )
  {
    return add( JSON( m ) );
  }

  /**
   * Add a value with a given key to the object (copy), overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, const JSON& value );

  /**
   * Add a value with a given key to the object (move), overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, JSON&& value );

  /**
   * Add a default value of a given type with a given key to the object (copy), overwriting any
   * existing entry with that key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, Type type )
  {
    return add( key, JSON( type ) );
  }

  /**
   * Add a boolean value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, bool value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, int value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, float value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, double value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a string value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, const String& value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a string value with a given key to the object, overwriting any existing entry with that
   * key.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& add( const char* key, const char* value )
  {
    return add( key, JSON( value ) );
  }

  /**
   * Add a `Vec3` with a given key to the object as a new array of 3 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Vec3& v )
  {
    return add( key, JSON( v ) );
  }

  /**
   * Add a `Vec4` with a given key to the object as a new array of 4 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Vec4& v )
  {
    return add( key, JSON( v ) );
  }

  /**
   * Add a `Point` with a given key to the object as a new array of 3 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Point& p )
  {
    return add( key, JSON( p ) );
  }

  /**
   * Add a `Plane` with a given key to the object as a new array of 4 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Plane& p )
  {
    return add( key, JSON( p ) );
  }

  /**
   * Add a `Quat` with a given key to the object as a new array of 4 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Quat& q )
  {
    return add( key, JSON( q ) );
  }

  /**
   * Add a `Mat33` with a given key to the object as a new array of 9 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Mat33& m )
  {
    return add( key, JSON( m ) );
  }

  /**
   * Add a `Mat44` with a given key to the object as a new array of 16 numbers, overwriting any
   * existing entry with that key.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& add( const char* key, const Mat44& m )
  {
    return add( key, JSON( m ) );
  }

  /**
   * Add a value with a given key to the object (copy), if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, const JSON& json );

  /**
   * Add a value with a given key to the object (move), if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, JSON&& json );

  /**
   * Add a default value of a given type and with a given key to the object, if the key does not
   * exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, Type type )
  {
    return include( key, JSON( type ) );
  }

  /**
   * Add a boolean value with a given key to the object, if the key does not exist in the
   * object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, bool value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, int value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, float value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a number value with a given key to the object, if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, double value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a string value with a given key to the object, if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, const String& value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a string value with a given key to the object, if the key does not exist in the object.
   *
   * If current value is not an object, `System::error()` is invoked.
   */
  JSON& include( const char* key, const char* value )
  {
    return include( key, JSON( value ) );
  }

  /**
   * Add a `Vec3` with a given key to the object as a new array of 3 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Vec3& v )
  {
    return include( key, JSON( v ) );
  }

  /**
   * Add a `Vec4` with a given key to the object as a new array of 4 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Vec4& v )
  {
    return include( key, JSON( v ) );
  }

  /**
   * Add a `Point` with a given key to the object as a new array of 3 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Point& p )
  {
    return include( key, JSON( p ) );
  }

  /**
   * Add a `Plane` with a given key to the object as a new array of 4 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Plane& p )
  {
    return include( key, JSON( p ) );
  }

  /**
   * Add a `Quat` with a given key to the object as a new array of 4 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Quat& q )
  {
    return include( key, JSON( q ) );
  }

  /**
   * Add a `Mat33` with a given key to the object as a new array of 9 numbers, if the key does not
   * exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Mat33& m )
  {
    return include( key, JSON( m ) );
  }

  /**
   * Add a `Mat44` with a given key to the object as a new array of 16 numbers, if the key does
   * not exist in the object.
   *
   * If current value is not an array, `System::error()` is invoked.
   */
  JSON& include( const char* key, const Mat44& m )
  {
    return include( key, JSON( m ) );
  }

  /**
   * Remove element at a given index from an array.
   *
   * If current value is not an array, `System::error()` is invoked.
   *
   * @return false iff an invalid index was given.
   */
  bool erase( int index );

  /**
   * Remove element with a given key from an object.
   *
   * If current value is not an object, `System::error()` is invoked.
   *
   * @return true iff key was found (and removed).
   */
  bool exclude( const char* key );

  /**
   * Recursively clear node and its children.
   *
   * @param warnUnused warn about unused variables.
   * @return True iff `warnUnused` is true and there are unused variables.
   */
  bool clear( bool warnUnused = false );

  /**
   * String representation of a value.
   *
   * This function does not format the string or sort object entries.
   */
  String toString() const;

  /**
   * Formatted String representation of a value.
   *
   * This function returns string written by `write()` method.
   */
  String toFormattedString( const Format& format = DEFAULT_FORMAT ) const;

  /**
   * Clear existing value and read new contents from a %JSON file.
   *
   * If file open fails, existing value is kept intact.
   *
   * @return true iff file is successfully read and parsed.
   */
  bool load( const File& file );

  /**
   * Write to a file.
   */
  bool save( const File& file, const Format& format = DEFAULT_FORMAT ) const;

};

}
