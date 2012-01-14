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

void Lingua::buildCatalogue( const char* srcDir, const char* lang, const char* name )
{
  log.print( "%s/%s ...", lang, name );

  File srcFile( String::str( "%s/lingua/%s/%s.po", srcDir, lang, name ) );
  File outFile( String::str( "lingua/%s/%s.ozCat", lang, name ) );

  FILE* fs = fopen( srcFile.path(), "r" );
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
            throw Exception( "%s:%d: invalid escape sequence '\\%c'",
                             srcFile.path().cstr(), lineNum, line[i] );
          }
        }
        strcpy( line + i, line + i + 1 );
      }
    }

    if( line[0] == '\n' || line[0] == '#' ) {
      continue;
    }

    if( String::beginsWith( line, "msgid" ) ) {
      mode = 1;

      if( !lastOriginal.isEmpty() ) {
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

  if( !lastOriginal.isEmpty() ) {
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

  outFile.write( &ostream );

  log.printEnd( " OK" );
}

void Lingua::build()
{
  log.println( "Building localisations {" );
  log.indent();

  PhysFile linguaDir( "lingua" );
  DArray<PhysFile> linguae = linguaDir.ls();

  foreach( catDir, linguae.iter() ) {
    if( catDir->getType() != PhysFile::DIRECTORY ) {
      continue;
    }

    String langCode = catDir->baseName();
    DArray<PhysFile> files = catDir->ls();

    foreach( file, files.citer() ) {
      if( !file->hasExtension( "po" ) ) {
        continue;
      }

      buildCatalogue( file->mountPoint(), langCode, file->baseName() );
    }
  }

  log.unindent();
  log.println( "}" );
}

}
}
