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
 * @file oz/Config.hh
 *
 * Config class.
 */

#pragma once

#include "HashString.hh"
#include "PFile.hh"

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
      explicit Value( const char* text_, bool isUsed_ = false ) :
        text( text_ ), isUsed( isUsed_ )
      {}
    };

    HashString<Value> vars;     ///< %List of variables.
    String            filePath; ///< Must be remembered to warn about unused variables.

    /**
     * Load configuration from a .rc file (does not override existing variables).
     */
    void loadConf( InputStream* istream );

    /**
     * Save configuration into a .rc file (entries are sorted by their keys).
     */
    void saveConf( BufferStream* bstream, const char* lineEnd );

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
     * @code
     *   name "value"
     * @endcode
     * Everything else is ignored.
     *
     * @return True on success.
     */
    bool load( File& file );

    /**
     * Load variables from a .rc file.
     *
     * Lines in the file must have the following format:
     * @code
     *   name "value"
     * @endcode
     * Everything else is ignored.
     *
     * @return True on success.
     */
    bool load( PFile& file );

    /**
     * Write variables to a .rc file.
     *
     * The .rc file will have lines with the following format
     * @code
     *   name "value"
     * @endcode
     *
     * @return True on success.
     */
    bool save( File& file, const char* lineEnd = "\n" );

    /**
     * Write variables to a .rc file.
     *
     * The .rc file will have lines with the following format
     * @code
     *   name "value"
     * @endcode
     *
     * @return True on success.
     */
    bool save( PFile& file, const char* lineEnd = "\n" );

    /**
     * Remove unused variables.
     *
     * Variables are tagged used once it is accessed via <tt>get()</tt> or <tt>getSet()</tt>.
     * <tt>contains()</tt> does not make difference.
     */
    void removeUnused();

    /**
     * Clear variables.
     *
     * @param issueWarnings issue warnings about unused variables.
     */
    void clear( bool issueWarnings = false );

    /**
     * Return variables in a formatted string.
     *
     * It's formatted like
     * @code
     *   key1 = "value1"
     *   key2 = "value2"
     * @endcode
     *
     * @param indentString prepended at the beginning of each line.
     */
    String toString( const char* indentString = "" );

};

/**
 * It is usually very convenient to have a globally defined Config instance.
 *
 * @ingroup oz
 */
extern Config config;

}
