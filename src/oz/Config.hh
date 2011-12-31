/*
 * liboz - OpenZone core library.
 *
 * Copyright © 2002-2012 Davorin Učakar
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/**
 * @file oz/Config.hh
 */

#pragma once

#include "HashString.hh"
#include "File.hh"
#include "PhysFile.hh"

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
      explicit Value( const char* text_, bool isUsed_ = false ) : text( text_ ), isUsed( isUsed_ )
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
     * <pre>
     *   name "value"
     * </pre>
     * Everything else is ignored.
     *
     * @return True on success.
     */
    bool load( File& file );

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
    bool load( PhysFile& file );

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
    bool save( File& file, const char* lineEnd = "\n" );

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
