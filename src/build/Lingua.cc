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

#include "stable.hh"

#include "build/Lingua.hh"

namespace oz
{
namespace build
{

Lingua lingua;

void Lingua::buildCatalogue( const char* directory, const char* catalogue )
{
  Log::print( "%s/%s ...", directory, catalogue );

  File::mkdir( String::str( "%s", directory ) );

  PFile srcFile( String::str( "%s/%s.po", directory, catalogue ) );
  File outFile( String::str( "%s/%s.ozCat", directory, catalogue ) );

  if( !srcFile.map() ) {
    throw Exception( "Cannot map catalogue source file '%s'", srcFile.path().cstr() );
  }

  InputStream is = srcFile.inputStream();

  List<String> messages;

  String lastOriginal;
  String lastTranslation;

  int mode = 0; // 1 for original, 2 for translation
  int lineNum = 0;

  while( is.isAvailable() ) {
    ++lineNum;

    String line = is.readLine();

    // replace "\n" with the real newline
    for( int i = 1; i < line.length(); ++i ) {
      if( line[i] == '\0' ) {
        break;
      }
      if( line[i - 1] == '\\' ) {
        switch( line[i] ) {
          case '\\': {
            line = line.substring( 0, i - 1 ) + line.substring( i );
            break;
          }
          case 'n': {
            line = line.substring( 0, i - 1 ) + "\n" + line.substring( i + 1 );
            break;
          }
          default: {
            throw Exception( "%s:%d: invalid escape sequence '\\%c'",
                             srcFile.path().cstr(), lineNum, line[i] );
          }
        }
      }
    }

    if( line.isEmpty() || line[0] == '\n' || line[0] == '#' ) {
      continue;
    }

    if( String::beginsWith( line, "msgid" ) ) {
      mode = 1;

      if( !lastOriginal.isEmpty() && !lastOriginal.equals( lastTranslation ) ) {
        messages.add( lastOriginal );
        messages.add( lastTranslation );
      }

      char* begin = strchr( line, '"' );
      char* end = strrchr( line, '"' );

      if( begin == null || end == null || begin >= end ) {
        throw Exception( "%s:%d: syntax error", srcFile.path().cstr(), lineNum );
      }

      ++begin;
      *end = '\0';

      lastOriginal = begin;
      lastTranslation = "";
    }

    if( String::beginsWith( line, "msgstr" ) ) {
      mode = 2;

      char* begin = strchr( line, '"' );
      char* end = strrchr( line, '"' );

      if( begin == null || end == null || begin >= end ) {
        throw Exception( "%s:%d: syntax error", srcFile.path().cstr(), lineNum );
      }

      ++begin;
      *end = '\0';

      lastTranslation = begin;
    }

    if( line[0] == '"' ) {
      char* begin = strchr( line, '"' );
      char* end = strrchr( line, '"' );

      if( begin == null || end == null || begin >= end ) {
        throw Exception( "%s:%d: syntax error", srcFile.path().cstr(), lineNum );
      }

      ++begin;
      *end = '\0';

      if( mode == 1 ) {
        lastOriginal = lastOriginal + begin;
      }
      else if( mode == 2 ) {
        lastTranslation = lastTranslation + begin;
      }
      else {
        throw Exception( "%s:%d: loose string", srcFile.path().cstr(), lineNum );
      }
    }
  }

  srcFile.unmap();

  if( !lastOriginal.isEmpty() && !lastOriginal.equals( lastTranslation ) ) {
    messages.add( lastOriginal );
    messages.add( lastTranslation );
  }

  BufferStream ostream;

  ostream.writeInt( messages.length() / 2 );

  for( int i = 0; i < messages.length(); i += 2 ) {
    ostream.writeInt( messages[i].hash() );
    ostream.writeString( messages[i] );
    ostream.writeString( messages[i + 1] );
  }

  if( !outFile.write( ostream.begin(), ostream.length() ) ) {
    throw Exception( "Catalogue write failed" );
  }

  Log::printEnd( " OK" );
}

void Lingua::build()
{
  Log::println( "Building package localisations {" );
  Log::indent();

  PFile linguaDir( "lingua" );
  DArray<PFile> languages = linguaDir.ls();

  if( !languages.isEmpty() ) {
    File::mkdir( linguaDir.path() );
  }

  foreach( langDir, languages.iter() ) {
    langDir->stat();

    if( langDir->type() != File::DIRECTORY ) {
      continue;
    }

    DArray<PFile> catalogues = langDir->ls();

    if( !catalogues.isEmpty() ) {
      File::mkdir( langDir->path() );
    }

    foreach( catalogue, catalogues.citer() ) {
      if( !catalogue->hasExtension( "po" ) ) {
        continue;
      }

      buildCatalogue( langDir->path(), catalogue->baseName() );
    }
  }

  PFile missionsDir( "mission" );
  DArray<PFile> missions = missionsDir.ls();

  foreach( mission, missions.citer() ) {
    linguaDir.setPath( mission->path() + "/lingua" );
    languages = linguaDir.ls();

    foreach( catalogue, languages.citer() ) {
      if( !catalogue->hasExtension( "po" ) ) {
        continue;
      }

      File::mkdir( "mission" );
      File::mkdir( mission->path() );
      File::mkdir( linguaDir.path() );

      buildCatalogue( linguaDir.path(), catalogue->baseName() );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

void Lingua::buildMissions()
{
  Log::println( "Building mission localisations {" );
  Log::indent();

  PFile missionsDir( "mission" );
  DArray<PFile> missions = missionsDir.ls();

  foreach( mission, missions.citer() ) {
    PFile linguaDir( mission->path() + "/lingua" );
    DArray<PFile> languages = linguaDir.ls();

    foreach( catalogue, languages.citer() ) {
      if( !catalogue->hasExtension( "po" ) ) {
        continue;
      }

      File::mkdir( "mission" );
      File::mkdir( mission->path() );
      File::mkdir( linguaDir.path() );

      buildCatalogue( linguaDir.path(), catalogue->baseName() );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

}
}
