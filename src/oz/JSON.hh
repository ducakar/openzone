/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 * @file oz/JSON.hh
 *
 * JSON class.
 */

#pragma once

#include "Vector.hh"
#include "HashString.hh"
#include "PFile.hh"

namespace oz
{

/**
 * DOM class for reading JSON configuration files.
 *
 * @ingroup oz
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
     * Thrown by <tt>Value</tt> class when an invalid value type requested.
     *
     * E.g. if a value is an integer an one calls <tt>Value::asString()</tt> function on it.
     */
    class TypeException : public std::exception
    {
      public:

        /**
         * Exception description ("oz::JSON::TypeException").
         */
        const char* what() const noexcept override;

    };

    /**
     * Thrown by read function on parse errors.
     */
    class ParseException : public std::exception
    {
      public:

        /**
         * Exception description ("oz::JSON::ParseException").
         */
        const char* what() const noexcept override;

    };

    /**
     * JSON value.
     */
    class Value
    {
      friend class JSON;

      private:

        /// Internal base struct for value data representation.
        struct Data;

        /// Internal struct for boolean representation.
        struct BooleanData;

        /// Internal struct for number representation.
        struct NumberData;

        /// Internal struct for string representation.
        struct StringData;

        /// Internal struct for array representation.
        struct ArrayData;

        /// Internal struct for Object representation.
        struct ObjectData;

        static const Value nil; ///< A null value, required by <tt>operator []</tt> functions.

        Data* data;             ///< Pointer to internal data struct.
        Type  valueType;        ///< Value type, <tt>JSON::Type</tt>.

      private:

        /**
         * Internal function for parsing.
         */
        static Value parseValue( InputStream* is );

        /**
         * Internal function for parsing string.
         */
        static String parseString( InputStream* is );

        /**
         * Internal function for parsing array.
         */
        static Value parseArray( InputStream* is );

        /**
         * Internal function to parsing object.
         */
        static Value parseObject( InputStream* is );

        /**
         * Internal constructor.
         */
        Value( Data* data, Type valueType );

      public:

        /**
         * Create a null instance.
         */
        Value();

        /**
         * Destructor.
         */
        ~Value();

        /**
         * Move constructor.
         */
        Value( Value&& v );

        /**
         * Move operator.
         */
        Value& operator = ( Value&& v );

        /**
         * Type of value.
         */
        Type type() const
        {
          return valueType;
        }

        /**
         * Number of entries if value is an object or array, -1 otherwise.
         */
        int length() const;

        /**
         * Return boolean value.
         *
         * If value is not a boolean <tt>TypeException</tt> is thrown.
         */
        bool asBool() const;

        /**
         * Return number value cast to integer.
         *
         * If value is not a number <tt>TypeException</tt> is thrown.
         */
        int asInt() const;

        /**
         * Return number value.
         *
         * If value is not a number <tt>TypeException</tt> is thrown.
         */
        float asFloat() const;

        /**
         * Return string value.
         *
         * If value is not a string <tt>TypeException</tt> is thrown.
         */
        const String asString() const;

        /**
         * Return boolean value or <tt>defaultValue</tt> if null.
         *
         * If value is not either a boolean or a null <tt>TypeException</tt> is thrown.
         */
        bool get( bool defaultValue ) const;

        /**
         * Return integer value or <tt>defaultValue</tt> if null.
         *
         * If value is not either a number or a null <tt>TypeException</tt> is thrown.
         */
        int get( int defaultValue ) const;

        /**
         * Return number value or <tt>defaultValue</tt> if null.
         *
         * If value is not either a number or a null <tt>TypeException</tt> is thrown.
         */
        float get( float defaultValue ) const;

        /**
         * Return string value or <tt>defaultValue</tt> if null.
         *
         * If value is not either a string or a null <tt>TypeException</tt> is thrown.
         */
        const String& get( const String& defaultValue ) const;

        /**
         * Return string value or <tt>defaultValue</tt> if null.
         *
         * If value is not either a string or a null <tt>TypeException</tt> is thrown.
         */
        const char* get( const char* defaultValue ) const;

        /**
         * Returns value at position <tt>i</tt> in an array.
         *
         * If value is not an array a <tt>TypeException</tt> is thrown and if index is out of
         * bounds, a null value is returned.
         */
        const Value& operator [] ( int i ) const;

        /**
         * Returns value for <tt>key</tt> in an object
         *
         * If value is not an object a <tt>TypeException</tt> is thrown and if key does not exist,
         * a null value is returned.
         */
        const Value& operator [] ( const char* key ) const;

        /**
         * Format value as a string.
         *
         * This effectively creates a JSON output.
         *
         * Note that this function adds double-quotes around a string value.
         */
        String toString() const;

    };

  public:

    /**
     * JSON document root value.
     */
    Value root;

    /**
     * Load a JSON file.
     *
     * @return true iff file is successfully read and parsed.
     */
    bool load( File file );

    /**
     * Load a JSON file.
     *
     * @return true iff file is successfully read and parsed.
     */
    bool load( PFile file );

    /**
     * Clear root value and release resources.
     */
    void clear();

};

}
