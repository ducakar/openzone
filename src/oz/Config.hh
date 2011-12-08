/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 * Copyright (C) 2002-2011  Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file oz/Config.hh
 */

#pragma once

#include "HashString.hh"

namespace oz
{

/**
 * Container for application variables.
 *
 * Variables are key-value pairs. They can be loaded from or saved to a .rc file.
 *
 * @ingroup oz
 */
class Config
{
  private:

    /// Hashtable size.
    static const int SIZE = 256;

    /// Size of buffer used when loading from file (maximum key/value length).
    static const int LINE_BUFFER_SIZE = 1024;

    /// Column for value alignment when writing .rc configuration files.
    static const int ALIGNMENT = 32;

    /**
     * Entry value.
     */
    struct Value
    {
      String       text;   ///< Entry value.
      mutable bool isUsed; ///< To track whether the entry has been used.

      /**
       * Constructor.
       */
      explicit Value( const char* text_, bool isUsed_ = false ) : text( text_ ), isUsed( isUsed_ )
      {}
    };

    HashString<Value, SIZE> vars;                   ///< %List of variables.
    String                  filePath;               ///< Needed to warn about unused variables.
    char                    line[LINE_BUFFER_SIZE]; ///< Internal buffer used during file parsing.

    /**
     * Load configuration from a .rc file (does not override existing variables).
     */
    bool loadConf( const char* path );

    /**
     * Save configuration into a .rc file (entries are sorted by their keys).
     */
    bool saveConf( const char* path );

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
    bool load( const char* path );

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
    bool save( const char* path );

    /**
     * Clear variables.
     *
     * @param issueWarnings issue warnings about unused variables, works only if <tt>NDEBUG</tt> is
     * not defined.
     */
    void clear( bool issueWarnings = false );

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

/**
 * It is usually very convenient to have a globally defined Config instance.
 *
 * @ingroup oz
 */
extern Config config;

}
