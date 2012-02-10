/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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
 */

/**
 * @file common/Lingua.hh
 */

#pragma once

#include "oz/oz.hh"

/**
 * @def OZ_GETTEXT
 *
 * Wrapper macro for <tt>lingua.get()</tt>.
 *
 * This macro is needed so that standard <tt>xgettext</tt> command can be used to extract strings
 * for translation from source. Sadly <tt>xgettext</tt> is not capable of recognising C++ constructs
 * like <tt>oz::lingua.get()</tt>, that's why we need this macro.
 *
 * @ingroup common
 */
#define OZ_GETTEXT( s ) \
  oz::lingua.get( s )

namespace oz
{

/**
 * Translation catalogue database.
 *
 * @ingroup common
 */
class Lingua
{
  private:

    /**
     * Internal structure for message entry in a hashtable.
     */
    struct Message
    {
      String   original;    ///< Original message that appears in the source code or resource files.
      String   translation; ///< Translated message from the catalogue.
      Message* next;        ///< Next entry in the hashtable linked list.

      OZ_PLACEMENT_POOL_ALLOC( Message, 256 );
    };

    static String locale;    ///< Language code (should match subdirectory in lingua).

    Message**     messages;  ///< Message hashtable.
    int           nMessages; ///< Size of hastable.
    Pool<Message> msgPool;   ///< Memory pool for messages.

  public:

    /**
     * Default constructor, creates uninitialised instance.
     */
    Lingua();

    /**
     * Obtain translation from the loaded catalogue.
     */
    const char* get( const char* message ) const;

    /**
     * Initialise a secondary Lingua instance with given catalogue.
     *
     * This function is intended to be used with secondary instances of Lingua. The global Lingua
     * instance must be initialised first with <tt>init()</tt>.
     *
     * Secondary instances of Lingua are used for translations of strings that appear inside mission
     * scripts.
     *
     * This function loads catalogue from <tt>lingua/\<locale\>/domain/\<domain\>.ozCat</tt>.
     */
    bool initDomain( const char* domain );

    /**
     * Initialise global Lingua instance.
     *
     * The global instance must be initialised first so that <tt>locale</tt> member is set properly.
     * This function loads all catalogues from <tt>lingua/\<locale\>/main</tt> directory.
     *
     * The global Lingua instance contains translations for strings that appear in the engine (UI)
     * and titles and descriptions of objects and structures from game data.
     */
    bool init( const char* locale );

    /**
     * Free allocated resources.
     */
    void free();

};

/**
 * Global instance, used for non-mission-specific translations.
 *
 * @ingroup common
 */
extern Lingua lingua;

}
