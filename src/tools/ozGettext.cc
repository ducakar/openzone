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
 * @file tools/ozGettext.cc
 *
 * Extract class, weapon, BSP and entity names from game data and generate POT template files for
 * translations.
 */

#include "oz/oz.hh"

#include <cstdlib>

using namespace oz;

enum LuaSyntaxState
{
  NORMAL,
  STRING1,
  STRING2,
  LINE_COMMENT,
  MULTILINE_COMMENT
};

HashString<String> titles;
HashString<String> messages;

static void printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage: %s <data_dir>\n"
    "\n"
    "<data_dir>  Package directory that contains source game data. The output POT\n"
    "            files will be written into <data_dir>/lingua.",
    invocationName );
}

static void readBSP( File* file )
{
  if( !config.load( *file ) ) {
    throw Exception( "Failed to load '%s'", file->path().cstr() );
  }

  const char* title = config.get( "title", "" );

  if( String::isEmpty( title ) ) {
    titles.include( file->baseName(), file->path() );
  }
  else {
    soft_assert( !file->baseName().equals( title ) );

    titles.include( title, file->path() );
  }

  char keyBuffer[] = "model  .title";

  for( int i = 0; i < 100; ++i ) {
    keyBuffer[5] = char( '0' + i / 10 );
    keyBuffer[6] = char( '0' + i % 10 );

    const char* modelTitle = config.get( keyBuffer, "" );

    if( !String::isEmpty( modelTitle ) ) {
      titles.include( modelTitle, file->path() );
    }
  }

  config.clear();
}

static void readClass( File* file )
{
  if( !config.load( *file ) ) {
    throw Exception( "Failed to read '%s'", file->path().cstr() );
  }

  const char* title    = config.get( "title", "" );
  const char* weapon00 = config.get( "weapon00.title", "" );
  const char* weapon01 = config.get( "weapon01.title", "" );
  const char* weapon02 = config.get( "weapon02.title", "" );
  const char* weapon03 = config.get( "weapon03.title", "" );

  if( String::isEmpty( title ) ) {
    titles.include( file->baseName(), file->path() );
  }
  else {
    soft_assert( !file->baseName().equals( title ) );

    titles.include( title, file->path() );
  }

  if( !String::isEmpty( weapon00 ) ) {
    titles.include( weapon00, file->path() );
  }
  if( !String::isEmpty( weapon01 ) ) {
    titles.include( weapon01, file->path() );
  }
  if( !String::isEmpty( weapon02 ) ) {
    titles.include( weapon02, file->path() );
  }
  if( !String::isEmpty( weapon03 ) ) {
    titles.include( weapon03, file->path() );
  }

  config.clear();
}

static void readLua( File* file )
{
  if( !file->map() ) {
    throw Exception( "Failed to read '%s'", file->path().cstr() );
  }

  InputStream is = file->inputStream();

  LuaSyntaxState state          = NORMAL;
  int            lineNum        = 1;
  int            gettextLineNum = 1;
  char           last[4]        = { '\0', '\0', '\0', '\0' };
  char           charString[2]  = { '\0', '\0' };
  String         lastString     = "";
  bool           restartString  = true;
  bool           inGettext      = false;

  while( is.isAvailable() ) {
    last[3] = last[2];
    last[2] = last[1];
    last[1] = last[0];
    last[0] = is.readChar();

    switch( state ) {
      case NORMAL: {
        if( last[0] == '(' ) {
          if( is.length() >= 10 && aEquals( is.getPos() - 10, "ozGettext", 9 ) ) {
            inGettext = true;
            gettextLineNum = lineNum;
          }
        }
        else if( last[0] == ')' ) {
          if( inGettext ) {
            inGettext = false;

            String locationInfo = String::str( "%s:%d", file->path().cstr(), gettextLineNum );
            messages.include( lastString, locationInfo );
          }
        }
        else if( last[1] == '.' && last[0] == '.' ) {
          restartString = false;
        }
        else if( last[0] == '"' || last[0] == '\'' ) {
          state = last[0] == '"' ? STRING1 : STRING2;

          if( restartString ) {
            lastString = "";
          }
        }
        else if( last[1] == '-' && last[0] == '-' ) {
          state = LINE_COMMENT;
        }
        else if( last[3] == '-' && last[2] == '-' && last[1] == '[' && last[0] == '[' ) {
          state = MULTILINE_COMMENT;
        }
        break;
      }
      case STRING1:
      case STRING2: {
        if( last[1] == '\\' ) {
          if( last[0] == 'n' ) {
            charString[0] = '\n';
          }
          else if( last[0] == 't' ) {
            charString[0] = '\t';
          }
          else {
            charString[0] = last[0];
          }

          lastString += charString;
        }
        else if( ( state == STRING1 && last[0] == '"' ) ||
                 ( state == STRING2 && last[0] == '\'' ) )
        {
          state = NORMAL;
          restartString = true;
        }
        else if( last[0] != '\\' ) {
          charString[0] = last[0];
          lastString += charString;
        }
        break;
      }
      case LINE_COMMENT: {
        if( last[0] == '\n' ) {
          state = NORMAL;
        }
        break;
      }
      case MULTILINE_COMMENT: {
        if( last[3] == ']' && last[2] == ']' && last[1] == '-' && last[0] == '-' ) {
          state = NORMAL;
        }
        break;
      }
    }

    if( last[0] == '\n' ) {
      ++lineNum;
    }
  }
}

static void writePOT( const HashString<String>* hs, const char* filePath  )
{
  BufferStream bs;
  String s;

  bool isFirst = true;
  foreach( i, hs->citer() ) {
    if( !isFirst ) {
      bs.writeChar( '\n' );
    }
    isFirst = false;

    s = "#: " + i.value();

    bs.writeChars( s, s.length() );
    bs.writeChar( '\n' );

    if( i.key().index( '\n' ) < 0 ) {
      s = String::str( "msgid \"%s\"", i.key().cstr() );

      bs.writeChars( s, s.length() );
      bs.writeChar( '\n' );
    }
    else {
      DArray<String> stringLines = i.key().split( '\n' );

      s = "msgid \"\"";

      bs.writeChars( s, s.length() );
      bs.writeChar( '\n' );

      foreach( l, stringLines.citer() ) {
        if( l == &stringLines.last() ) {
          s = String::str( "\"%s\"", l->cstr() );
        }
        else {
          s = String::str( "\"%s\\n\"", l->cstr() );
        }

        bs.writeChars( s, s.length() );
        bs.writeChar( '\n' );
      }
    }

    s = "msgstr \"\"";

    bs.writeChars( s, s.length() );
    bs.writeChar( '\n' );
  }

  File outFile( filePath );

  if( !outFile.write( bs.begin(), bs.length() ) ) {
    throw Exception( "Failed to write '%s'", outFile.path().cstr() );
  }
}

int main( int argc, char** argv )
{
  System::init();

  String invocationName = File( argv[0] ).baseName();

  if( argc != 2 ) {
    printUsage( invocationName );
    return EXIT_FAILURE;
  }

#ifdef _WIN32
  String pkgDir = String::replace( argv[1], '\\', '/' );
#else
  String pkgDir = argv[1];
#endif

  while( !pkgDir.isEmpty() && pkgDir.last() == '/' ) {
    pkgDir = pkgDir.substring( 0, pkgDir.length() - 1 );
  }
  if( pkgDir.isEmpty() ) {
    throw Exception( "Package directory cannot be root ('/')" );
  }

  String pkgName = pkgDir.substring( pkgDir.lastIndex( '/' ) + 1 );

  File::mkdir( pkgDir + "/lingua" );

  BufferStream bs;

  File bspDir( pkgDir + "/baseq3/maps" );
  DArray<File> files = bspDir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    readBSP( file );
  }

  File classDir( pkgDir + "/class" );
  files = classDir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    readClass( file );
  }

  String mainPOT = String::str( "%s/lingua/%s.pot.new", pkgDir.cstr(), pkgName.cstr() );
  writePOT( &titles, mainPOT );

  titles.clear();
  titles.dealloc();

  File missionDir( pkgDir + "/lua/mission" );
  files = missionDir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    readLua( file );

    String missionPOT = String::str( "%s/lingua/%s.pot.new", pkgDir.cstr(),
                                     file->baseName().cstr() );
    writePOT( &messages, missionPOT );

    messages.clear();
    messages.dealloc();
  }

  return EXIT_SUCCESS;
}
