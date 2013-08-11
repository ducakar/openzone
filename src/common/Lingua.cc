/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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

String Lingua::detectLanguage( const char* language_ )
{
  String lang = language_;

  if( !lang.isEmpty() ) {
    if( File( "@lingua/" + lang ).type() == File::MISSING ) {
      lang = "";
    }
    return lang;
  }

  lang = getenv( "LANGUAGE" );

  if( !lang.isEmpty() && File( "@lingua/" + lang ).type() != File::MISSING ) {
    return lang;
  }

  lang = getenv( "LC_MESSAGES" );

  int underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( File( "@lingua/" + lang ).type() != File::MISSING ) {
      return lang;
    }
  }

  lang = getenv( "LANG" );

  underscore = lang.index( '_' );
  if( underscore >= 2 ) {
    lang = lang.substring( 0, underscore );

    if( File( "@lingua/" + lang ).type() != File::MISSING ) {
      return lang;
    }
  }

  lang = "";
  return lang;
}

bool Lingua::initMission( const char* mission )
{
  clear();

  File file = String::str( "@mission/%s/lingua/%s.mo", mission, language.cstr() );
  return catalogue.import( file );
}

void Lingua::clear()
{
  catalogue.clear();
}

bool Lingua::init( const char* language_ )
{
  language = language_;
  catalogue.clear();

  File dir = "@lingua/" + language;
  if( dir.type() == File::MISSING ) {
    return false;
  }

  DArray<File> files = dir.ls();

  foreach( file, files.citer() ) {
    if( !file->hasExtension( "mo" ) ) {
      continue;
    }

    catalogue.import( *file );
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
