/*
 *  Config.hpp
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#pragma once

/**
 * @file Config.hpp
 */

#include "HashString.hpp"

namespace oz
{

/**
 * Container for application variables.
 *
 * Variables are key-value pairs. They can be loaded from or saved to a .rc file.
 */
class Config
{
  private:

    /// Hashtable size.
    static const int SIZE = 256;

    /// Size of buffer used when loading from file (maximum key/value length).
    static const int BUFFER_SIZE = 1024;

    /// Column for value alignment when writing .rc configuration files.
    static const int ALIGNMENT = 32;

    /**
     * Configuration entry.
     */
    struct Elem
    {
      const char* key;   ///< Key.
      const char* value; ///< Value.

      /**
       * True iff <code>String::compare( key, e.key ) < 0</code>.
       */
      bool operator < ( const Elem& e ) const
      {
        return String::compare( key, e.key ) < 0;
      }
    };

    HashString<String, SIZE>            vars;     ///< %List of variables.
#ifndef NDEBUG
    mutable HashString<nullptr_t, SIZE> usedVars; ///< %List of accessed variables.
#endif

    /**
     * Load configuration from a .rc file (does not override existing variables).
     */
    bool loadConf( const char* file );

    /**
     * Save configuration into a .rc file (entries are sorted by their keys).
     */
    bool saveConf( const char* file );

  public:

    /**
     * Destructor.
     */
    ~Config();

    /**
     * Add a variable, override if already exists.
     */
    void add( const char* key, const char* value );

    /**
     * Add a variable only if it does not exist.
     */
    void include( const char* key, const char* value );

    /**
     * Remove a variable.
     */
    void exclude( const char* key );

    /**
     * True iff variable exists in configuration.
     */
    bool contains( const char* key ) const;

    /**
     * Get variable value or <tt>defVal</tt> if the key does not exist.
     */
    bool get( const char* key, bool defVal ) const;

    /**
     * Get variable value or <tt>defVal</tt> if the key does not exist.
     */
    int get( const char* key, int defVal ) const;

    /**
     * Get variable value or <tt>defVal</tt> if the key does not exist.
     */
    float get( const char* key, float defVal ) const;

    /**
     * Get variable value or <tt>defVal</tt> if the key does not exist.
     */
    const char* get( const char* key, const char* defVal ) const;

    /**
     * Get variable value or set and return <tt>defVal</tt> if the key does not exist.
     */
    bool getSet( const char* key, bool defVal );

    /**
     * Get variable value or set and return <tt>defVal</tt> if the key does not exist.
     */
    int getSet( const char* key, int defVal );

    /**
     * Get variable value or set and return <tt>defVal</tt> if the key does not exist.
     */
    float getSet( const char* key, float defVal );

    /**
     * Get variable value or set and return <tt>defVal</tt> if the key does not exist.
     */
    const char* getSet( const char* key, const char* defVal );

    /**
     * Load variables from a .rc file.
     *
     * Lines in the file must have the following format:
     * <pre>
     *   name "value"
     * </pre>
     * Everything else is ignored.
     *
     * @return True on success.
     */
    bool load( const char* file );

    /**
     * Write variables to a .rc file.
     *
     * The .rc file will have lines with the following format
     * <pre>
     *   name "value"
     * </pre>
     *
     * @return True on success.
     */
    bool save( const char* file );

    /**
     * Clear variables.
     */
    void clear();

    /**
     * Return variables in a formatted string.
     *
     * It's formatted like
     * <pre>
     *   key1 = "value1"
     *   key2 = "value2"
     * </pre>
     *
     * @param indentString prepended at the beginning of each line.
     */
    String toString( const String& indentString = "" );

};

/// It is usually very convenient to have a globally defined Config instance.
extern Config config;

}
