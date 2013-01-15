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
 * JSON class.
 */

#pragma once

#include "HashMap.hh"
#include "BufferStream.hh"
#include "PFile.hh"

namespace oz
{

/**
 * DOM class for reading JSON configuration files.
 *
 * This implementation strictly follows JSON standard with the following exceptions:
 * @li `inf` and `-inf` (case-sensitive) represent positive and negative infinity respectively,
 * @li `nan` and `-nan` (case-sensitive) represent not-a-number.
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
     * %Iterator for %JSON objects with constant access.
     */
    class CIterator : public IteratorBase<const HashMap<String, JSON>::Elem>
    {
      private:

        /**
         * HashMap iterator used internally.
         */
        HashMap<String, JSON>::CIterator objectIter;

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        explicit CIterator() :
          IteratorBase<const HashMap<String, JSON>::Elem>( nullptr )
        {}

        /**
         * Create iterator for the given %JSON object's data.
         */
        explicit CIterator( const ObjectData* data );

        /**
         * Advance to the next element.
         */
        CIterator& operator ++ ();

    };

    /**
     * %Iterator for %JSON objects with non-constant access.
     */
    class Iterator : public IteratorBase<HashMap<String, JSON>::Elem>
    {
      private:

        /**
         * HashMap iterator used internally.
         */
        HashMap<String, JSON>::Iterator objectIter;

      public:

        /**
         * Default constructor, creates an invalid iterator.
         */
        OZ_ALWAYS_INLINE
        explicit Iterator() :
          IteratorBase<HashMap<String, JSON>::Elem>( nullptr )
        {}

        /**
         * Create iterator for the given %JSON object's data.
         */
        explicit Iterator( ObjectData* data );

        /**
         * Advance to the next element.
         */
        Iterator& operator ++ ();

    };

  private:

    /// A null value instance, required by `operator []`.
    static const JSON NIL_VALUE;

    Data*        data;        ///< Pointer to internal data structure.
    Type         valueType;   ///< Value type, `JSON::Type`.
    mutable bool wasAccessed; ///< For warnings about unused variables.

    /**
     * Internal constructor.
     */
    explicit JSON( Data* data, Type valueType );

  public:

    /**
     * Create a null instance.
     */
    explicit JSON();

    /**
     * Create from a file contents.
     */
    explicit JSON( const char* path );

    /**
     * Create from a file contents.
     */
    explicit JSON( File* file );

    /**
     * Create from a file contents.
     */
    explicit JSON( PFile* file );

    /**
     * Destructor.
     */
    ~JSON();

    /**
     * Move constructor.
     */
    JSON( JSON&& v );

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
     * True iff value is an object and contains the given key.
     *
     * If value is not either null or an object, `System::error()` is invoked.
     */
    bool contains( const char* key );

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
     * %JSON object iterator with constant access.
     *
     * An invalid iterator is returned if the %JSON element is not an object.
     */
    CIterator objectCIter() const;

    /**
     * %JSON object iterator with non-constant access.
     *
     * An invalid iterator is returned if the %JSON element is not an object.
     */
    Iterator objectIter();

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
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are booleans,
     * `System::error()` is invoked.
     */
    void asArray( bool* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are numbers,
     * `System::error()` is invoked.
     */
    void asArray( int* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are numbers,
     * `System::error()` is invoked.
     */
    void asArray( float* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are strings,
     * `System::error()` is invoked.
     */
    void asArray( String* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 3 numbers, `System::error()` is invoked.
     */
    void asArray( Vec3* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 4 numbers, `System::error()` is invoked.
     */
    void asArray( Vec4* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 3 numbers, `System::error()` is invoked.
     */
    void asArray( Point* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 4 numbers, `System::error()` is invoked.
     */
    void asArray( Plane* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 4 numbers, `System::error()` is invoked.
     */
    void asArray( Quat* array, int count ) const;

    /**
     * Write array values into the given array.
     *
     * If value is not an array of the specified length or if not all array elements are arrays of
     * 9 numbers, `System::error()` is invoked.
     */
    void asArray( Mat33* array, int count ) const;

    /**
     * Write array values into the given array.
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
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with boolean elements,
     * `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( bool* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with number elements,
     * `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( int* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with number elements,
     * `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( float* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with string elements,
     * `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( String* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 3 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Vec3* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 4 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Vec4* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 3 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Point* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 4 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Plane* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 4 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Quat* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 9 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Mat33* array, int count ) const;

    /**
     * Write array values into the given array (it is left unchanged if %JSON element is null).
     *
     * If value is not either null or an array of the specified length with arrays of 16 numbers as
     * elements, `System::error()` is invoked.
     *
     * @return true if destination array has been filled, false if %JSON element is null.
     */
    bool get( Mat44* array, int count ) const;

    /**
     * Clear existing value and set to null.
     */
    void setNull();

    /**
     * Clear existing value and set to a boolean.
     */
    void set( bool value );

    /**
     * Clear existing value and set to a number.
     */
    void set( int value );

    /**
     * Clear existing value and set to a number.
     */
    void set( float value );

    /**
     * Clear existing value and set to a string.
     */
    void set( const String& value );

    /**
     * Clear existing value and set to a string.
     */
    void set( const char* value );

    /**
     * Clear existing value and set to an array of 3 numbers representing `Vec3` components.
     */
    void set( const Vec3& v );

    /**
     * Clear existing value and set to an array of 4 numbers representing `Vec4` components.
     */
    void set( const Vec4& v );

    /**
     * Clear existing value and set to an array of 3 numbers representing `Point` components.
     */
    void set( const Point& p );

    /**
     * Clear existing value and set to an array of 4 numbers representing `Plane` components.
     */
    void set( const Plane& p );

    /**
     * Clear existing value and set to an array of 4 numbers representing `Quat` components.
     */
    void set( const Quat& q );

    /**
     * Clear existing value and set to an array of 9 numbers representing `Mat33` components.
     */
    void set( const Mat33& m );

    /**
     * Clear existing value and set to an array of 16 numbers representing `Mat44` components.
     */
    void set( const Mat44& m );

    /**
     * Clear existing value and set to an empty array.
     */
    void setArray();

    /**
     * Clear existing value and set to an empty object.
     */
    void setObject();

    /**
     * Add a null value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& addNull();

    /**
     * Add a boolean value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( bool value );

    /**
     * Add a number value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( int value );

    /**
     * Add a number value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( float value );

    /**
     * Add a string value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const String& value );

    /**
     * Add a string value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* value );

    /**
     * Add a `Vec3` to array as a new array of 3 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Vec3& v );

    /**
     * Add a `Vec4` to array as a new array of 4 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Vec4& v );

    /**
     * Add a `Point` to array as a new array of 3 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Point& p );

    /**
     * Add a `Plane` to array as a new array of 4 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Plane& p );

    /**
     * Add a `Quat` to array as a new array of 4 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Quat& q );

    /**
     * Add a `Mat33` to array as a new array of 9 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Mat33& m );

    /**
     * Add a `Mat44` to array as a new array of 16 numbers.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const Mat44& m );

    /**
     * Add an empty array value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& addArray();

    /**
     * Add an empty object value to array.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& addObject();

    /**
     * Add a null value with the given key to the object, overwriting any existing entry with that
     * key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& addNull( const char* key );

    /**
     * Add a boolean value with the given key to the object, overwriting any existing entry with
     * that key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& add( const char* key, bool value );

    /**
     * Add a number value with the given key to the object, overwriting any existing entry with that
     * key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& add( const char* key, int value );

    /**
     * Add a number value with the given key to the object, overwriting any existing entry with that
     * key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& add( const char* key, float value );

    /**
     * Add a string value with the given key to the object, overwriting any existing entry with that
     * key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& add( const char* key, const String& value );

    /**
     * Add a string value with the given key to the object, overwriting any existing entry with that
     * key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& add( const char* key, const char* value );

    /**
     * Add a `Vec3` with the given key to the object as a new array of 3 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Vec3& v );

    /**
     * Add a `Vec4` with the given key to the object as a new array of 4 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Vec4& v );

    /**
     * Add a `Point` with the given key to the object as a new array of 3 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Point& p );

    /**
     * Add a `Plane` with the given key to the object as a new array of 4 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Plane& p );

    /**
     * Add a `Quat` with the given key to the object as a new array of 4 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Quat& q );

    /**
     * Add a `Mat44` with the given key to the object as a new array of 16 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Mat44& m );

    /**
     * Add a `Mat33` with the given key to the object as a new array of 9 numbers, overwriting any
     * existing entry with that key.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& add( const char* key, const Mat33& m );

    /**
     * Add an empty array value with the given key to the object, overwriting any existing entry
     * with that key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& addArray( const char* key );

    /**
     * Add an empty object value with the given key to the object, overwriting any existing entry
     * with that key.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& addObject( const char* key );

    /**
     * Add a null value with the given key to the object, if the key does not exist in the object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& includeNull( const char* key );

    /**
     * Add a boolean value with the given key to the object, if the key does not exist in the
     * object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& include( const char* key, bool value );

    /**
     * Add a number value with the given key to the object, if the key does not exist in the object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& include( const char* key, int value );

    /**
     * Add a number value with the given key to the object, if the key does not exist in the object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& include( const char* key, float value );

    /**
     * Add a string value with the given key to the object, if the key does not exist in the object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& include( const char* key, const String& value );

    /**
     * Add a string value with the given key to the object, if the key does not exist in the object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& include( const char* key, const char* value );

    /**
     * Add a `Vec3` with the given key to the object as a new array of 3 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Vec3& v );

    /**
     * Add a `Vec4` with the given key to the object as a new array of 4 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Vec4& v );

    /**
     * Add a `Point` with the given key to the object as a new array of 3 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Point& p );

    /**
     * Add a `Plane` with the given key to the object as a new array of 4 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Plane& p );

    /**
     * Add a `Quat` with the given key to the object as a new array of 4 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Quat& q );

    /**
     * Add a `Mat33` with the given key to the object as a new array of 9 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Mat33& m );

    /**
     * Add a `Mat44` with the given key to the object as a new array of 16 numbers, if the key does
     * not exist in the object.
     *
     * If current value is not an array, `System::error()` is invoked.
     */
    JSON& include( const char* key, const Mat44& m );

    /**
     * Add an empty array value with the given key to the object, if the key does not exist in the
     * object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& includeArray( const char* key );

    /**
     * Add an empty object value with the given key to the object, if the key does not exist in the
     * object.
     *
     * If current value is not an object, `System::error()` is invoked.
     */
    JSON& includeObject( const char* key );

    /**
     * Remove element at the given index from an array.
     *
     * If current value is not an array, `System::error()` is invoked.
     *
     * @return false iff an invalid index was given.
     */
    bool erase( int index );

    /**
     * Remove element with the given key from an object.
     *
     * If current value is not an object, `System::error()` is invoked.
     *
     * @return true iff key was found (and removed).
     */
    bool exclude( const char* key );

    /**
     * Recursively clear node and its children.
     *
     * @param unusedWarnings warn about unused variables.
     */
    void clear( bool unusedWarnings = false );

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
    String toFormattedString(const char* lineEnd = "\n" ) const;

    /**
     * Clear existing value and read new contents from a stream.
     *
     * @param istream input stream.
     * @param path optional file path, used for error messages.
     */
    void read( InputStream* istream, const char* path = "InputStream" );

    /**
     * Write formatted %JSON to a stream.
     */
    void write( BufferStream* ostream, const char* lineEnd = "\n" ) const;

    /**
     * Clear existing value and read new contents from a %JSON file (via `File` class).
     *
     * If file open fails, existing value is kept intact.
     *
     * @return true iff file is successfully read and parsed.
     */
    bool load( const char* path );

    /**
     * Clear existing value and read new contents from a %JSON file.
     *
     * If file open fails, existing value is kept intact.
     *
     * @return true iff file is successfully read and parsed.
     */
    bool load( File* file );

    /**
     * Clear existing value and read new contents from a %JSON file.
     *
     * If file open fails, existing value is kept intact.
     *
     * @return true iff file is successfully read and parsed.
     */
    bool load( PFile* file );

    /**
     * Write to a file (via `File` class).
     */
    bool save( const char* path, const char* lineEnd = "\n" ) const;

    /**
     * Write to a file.
     */
    bool save( File* file, const char* lineEnd = "\n" ) const;

    /**
     * Write to a file.
     */
    bool save( PFile* file, const char* lineEnd = "\n" ) const;

};

}
