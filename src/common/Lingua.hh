/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2019 Davorin Učakar
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

#include <ozCore/ozCore.hh>

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
public:

  /**
   * Detect language code from the environment, optionally using language mappings.
   *
   * This detects the two- or three-letter language code (before the "_" in locale name) from
   * environment variables, e.g. "sl" for "sl_SI.UTF-8". Additionally it respects mappings specified
   * in `*.json` files inside `lingua/`. On Windows and Linux those codes don't match so the
   * mappings are used to fix this, e.g. "Slovenian_cp1250" -> "Slovenian" -> (mapping) -> "sl".
   */
  static String detectLanguage(const char* fallback);

  /**
   * Obtain translation from the loaded catalogue.
   */
  const char* get(const char* message) const;

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
