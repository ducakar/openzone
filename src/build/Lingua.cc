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

void Lingua::buildCatalogue( const char* lang, const char* category, const char* name )
{
  Log::print( "%s/%s ...", lang, name );

  File::mkdir( "lingua" );
  File::mkdir( String::str( "lingua/%s", lang ) );
  File::mkdir( String::str( "lingua/%s/%s", lang, category ) );

  PFile srcFile( String::str( "lingua/%s/%s/%s.po", lang, category, name ) );
  File outFile( String::str( "lingua/%s/%s/%s.ozCat", lang, category, name ) );

  String realSrcPath = srcFile.realDir() + "/" + srcFile.path();

  FILE* fs = fopen( realSrcPath, "r" );
  if( fs == null ) {
    throw Exception( "Cannot read catalogue source '%s'", srcFile.path().cstr() );
  }

  Vector<String> messages;

  String lastOriginal;
  String lastTranslation;

  char line[1024];
  int  lineNum = 0;

  int mode = 0; // 1 for original, 2 for translation

  while( fgets( line, 1024, fs ) != null ) {
    ++lineNum;

    // replace "\n" with the real newline
    for( int i = 1; i < 1024; ++i ) {
      if( line[i] == '\0' ) {
        break;
      }
      if( line[i - 1] == '\\' ) {
        switch( line[i] ) {
          case '\\': {
            line[i - 1] = '\\';
            break;
          }
          case 'n': {
            line[i - 1] = '\n';
            break;
          }
          default: {
            fclose( fs );
            throw Exception( "%s:%d: invalid escape sequence '\\%c'",
                             srcFile.path().cstr(), lineNum, line[i] );
          }
        }
        memmove( line + i, line + i + 1, size_t( 1024 - i - 1 ) );
      }
    }

    if( line[0] == '\n' || line[0] == '#' ) {
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
        fclose( fs );
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
        fclose( fs );
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
        fclose( fs );
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
        fclose( fs );
        throw Exception( "%s:%d: loose string", srcFile.path().cstr(), lineNum );
      }
    }
  }

  if( !lastOriginal.isEmpty() && !lastOriginal.equals( lastTranslation ) ) {
    messages.add( lastOriginal );
    messages.add( lastTranslation );
  }

  fclose( fs );

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
  Log::println( "Building localisations {" );
  Log::indent();

  PFile linguaDir( "lingua" );
  DArray<PFile> languages = linguaDir.ls();

  foreach( langDir, languages.iter() ) {
    langDir->stat();

    if( langDir->type() != File::DIRECTORY ) {
      continue;
    }

    String langCode = langDir->baseName();

    PFile linguaMainDir( langDir->path() + "/main" );
    DArray<PFile> mainCats = linguaMainDir.ls();

    foreach( file, mainCats.citer() ) {
      if( !file->hasExtension( "po" ) ) {
        continue;
      }

      buildCatalogue( langCode, "main", file->baseName() );
    }

    PFile linguaDomainDir( langDir->path() + "/domain" );
    DArray<PFile> domainCats = linguaDomainDir.ls();

    foreach( file, domainCats.citer() ) {
      if( !file->hasExtension( "po" ) ) {
        continue;
      }

      buildCatalogue( langCode, "domain", file->baseName() );
    }
  }

  Log::unindent();
  Log::println( "}" );
}

}
}
