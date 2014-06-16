/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 *
 * Lingua class.
 */

#pragma once

#include <common/common.hh>

/**
 * @def OZ_GETTEXT
 * Wrapper macro for `lingua.get()`.
 *
 * This macro is needed so that standard `xgettext` command can be used to extract strings for
 * translation from source. Unfortunately `xgettext` is not capable of recognising C++ constructs
 * like `oz::lingua.get()`, that's why we need this macro.
 */
#define OZ_GETTEXT(s) \
  oz::lingua.get(s)

namespace oz
{

/**
 * Translation catalogue database.
 */
class Lingua
{
private:

  Gettext catalogue; ///< Gettext catalogue.

public:

  /**
   * Check given language or, if `nullptr` / empty string, try to detect it.
   *
   * If the given `language` is a non-empty string, check if translations exist for that language,
   * i.e. check for VFS directory `lingua/\<language\>/`. If translations exist `language` is
   * returned, otherwise an empty string.
   *
   * If `language` is `nullptr` or en empty string, it tries to derive language from environment
   * variables (currently this only test Linux-specific variables LC_ALL, LC_MESSAGES and LANG in
   * that order). The first derived language code for which translations exist is returned.
   * If none is valid, an empty string is returned.
   */
  static String detectLanguage(const char* language);

  /**
   * Obtain translation from the loaded catalogue.
   */
  const char* get(const char* message) const
  {
    return catalogue.get(message);
  }

  /**
   * Initialise per-mission Lingua instance.
   *
   * This function loads catalogue from `mission/\<mission\>/lingua/\<language\>.ozCat` that
   * contains translations of strings that appear inside mission scripts.
   */
  bool initMission(const char* mission);

  /**
   * Clear per-mission Lingua instance.
   */
  void clear();

  /**
   * Initialise global Lingua instance.
   *
   * The global instance must be initialised first so that `language` member is set properly. This
   * function loads all catalogues from `lingua/\<language\>/` directory.
   *
   * The global Lingua instance contains translations for strings that appear in the engine (UI) and
   * titles/descriptions of objects and structures from game data.
   */
  bool init(const char* language);

  /**
   * Destroy global instance.
   */
  void destroy();

};

/**
 * Global instance, used for non-mission-specific translations.
 */
extern Lingua lingua;

}
