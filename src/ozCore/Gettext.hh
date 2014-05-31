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
 * @file ozCore/Gettext.hh
 *
 * `Gettext` class.
 */

#pragma once

#include "File.hh"

namespace oz
{

/**
 * Translation catalogue database.
 *
 * It reads messages from GNU gettext binary (*.mo) catalogues and allows merging of multiple
 * catalogues into one.
 */
class Gettext
{
private:

  struct Message;

  Message** table;       ///< Message hashtable.
  Message*  messages;    ///< Messages.
  char*     strings;     ///< Strings buffer.
  int       nBuckets;    ///< Number of hashtable buckets.
  int       nMessages;   ///< Size of hashtable.
  int       stringsSize; ///< Size of `strings` array.

public:

  /**
   * Detect system language from `LANG` and `LC_*` variables.
   *
   * This function reads locale name from the first of `LC_ALL`, `LC_MESSAGES` or `LANG` environment
   * variables that has non-empty value. If all those variables are empty or nonexistent `fallback`
   * is returned.
   */
  static const char* systemLanguage( const char* fallback = "C" );

  /**
   * Default constructor, creates an empty instance.
   */
  Gettext();

  /**
   * Create a new instance and import translations from a given file.
   */
  explicit Gettext( const File& file );

  /**
   * Destructor.
   */
  ~Gettext();

  /**
   * Move constructor.
   */
  Gettext( Gettext&& gt );

  /**
   * Move operator.
   */
  Gettext& operator = ( Gettext&& gt );

  /**
   * Number of messages it contains.
   */
  OZ_ALWAYS_INLINE
  int length() const
  {
    return nMessages;
  }

  /**
   * True iff it contains no messages.
   */
  OZ_ALWAYS_INLINE
  bool isEmpty() const
  {
    return nMessages == 0;
  }

  /**
   * Check whether the catalogue contains a given translation.
   */
  bool contains( const char* message ) const;

  /**
   * Obtain translation from the catalogue.
   *
   * If translation for a given string does not exist that string itself is returned. An empty
   * string is always translated to an empty string -- use `catalogueDescriptions()` to get
   * catalogue descriptions.
   */
  const char* get( const char* message ) const;

  /**
   * Obtain descriptions of all .mo catalogues that have been merged into this catalogue.
   */
  List<const char*> catalogueDescriptions() const;

  /**
   * Import messages from GNU gettext binary .mo file.
   *
   * Existing messages are kept in the catalogue. There is no checking for duplicates, so both
   * copies will be kept in the final catalogue. However, the later added copy should always
   * override the earlier one in the hashtable making the old one inaccessible.
   *
   * Empty strings (storage for catalogue descriptions) are never included in the hashtable, use
   * `catalogueDescriptions()` to obtain those.
   */
  bool import( const File& file );

  /**
   * Clear catalogue.
   */
  void clear();

};

}
