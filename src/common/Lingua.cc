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
 * @file common/Lingua.cc
 */

#include <common/Lingua.hh>

#include <cstdlib>

namespace oz
{

static String language;

String Lingua::detectLanguage(const char* fallback)
{
  String lang = Gettext::systemLanguage();

  if (lang.isEmpty() || lang == "C") {
    lang = fallback;
  }

  int underscore = lang.index('_');
  if (underscore >= 2) {
    lang = lang.substring(0, underscore);
  }

  for (const File& file : File("@lingua").list("json")) {
    Json langMap(file);

    if (!langMap.isNull()) {
      for (const auto& langAlias : langMap.objectCIter()) {
        if (lang == langAlias.key) {
          lang = langAlias.value.get(lang);
          break;
        }
      }
    }
  }

  return lang;
}

bool Lingua::initMission(const char* mission)
{
  clear();

  File file = String::format("@mission/%s/lingua/%s.mo", mission, language.c());
  return catalogue.import(file);
}

void Lingua::clear()
{
  catalogue.clear();
}

bool Lingua::init(const char* language_)
{
  language = language_;
  catalogue.clear();

  File dir = "@lingua/" + language;
  if (dir.stat().type == File::MISSING) {
    return false;
  }

  for (const File& file : dir.list()) {
    if (!file.hasExtension("mo")) {
      continue;
    }

    catalogue.import(file);
  }
  return true;
}

void Lingua::destroy()
{
  catalogue.clear();
  language = "";
}

Lingua lingua;

}
