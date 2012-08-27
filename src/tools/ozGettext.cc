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

namespace oz
{

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
    "            files will be written into <data_dir>/lingua.\n\n",
    invocationName );
}

static void readBSP( File* file )
{
  JSON config;

  if( !config.load( file ) ) {
    OZ_ERROR( "Failed to load '%s'", file->path().cstr() );
  }

  const char* title = config["title"].get( "" );

  if( String::isEmpty( title ) ) {
    titles.include( file->baseName(), file->path() );
  }
  else {
    soft_assert( !file->baseName().equals( title ) );

    titles.include( title, file->path() );
  }

  const JSON& models = config["models"];
  int nModels = models.length();

  if( !models.isNull() && models.type() != JSON::ARRAY ) {
    OZ_ERROR( "'models' entry in '%s' is not an array", file->path().cstr() );
  }

  for( int i = 0; i < nModels; ++i ) {
    const char* modelTitle = models[i]["title"].get( "" );

    if( !String::isEmpty( modelTitle ) ) {
      titles.include( modelTitle, file->path() );
    }
  }

  config.clear();
}

static void readClass( File* file )
{
  JSON config;

  if( !config.load( file ) ) {
    OZ_ERROR( "Failed to read '%s'", file->path().cstr() );
  }

  const char* title = config["title"].get( "" );

  if( String::isEmpty( title ) ) {
    titles.include( file->baseName(), file->path() );
  }
  else {
    soft_assert( !file->baseName().equals( title ) );

    titles.include( title, file->path() );
  }

  const JSON& weaponsConfig = config["weapons"];

  for( int i = 0; i < weaponsConfig.length(); ++i ) {
    const char* weaponTitle = weaponsConfig[i]["title"].get( "" );

    if( !String::isEmpty( weaponTitle ) ) {
      titles.include( weaponTitle, file->path() );
    }
  }

  config.clear();
}

static void readLua( File* file )
{
  if( !file->map() ) {
    OZ_ERROR( "Failed to read '%s'", file->path().cstr() );
  }

  InputStream is = file->inputStream();

  LuaSyntaxState state          = NORMAL;
  int            lineNum        = 1;
  int            gettextLineNum = 1;
  char           last[4]        = { '\0', '\0', '\0', '\0' };
  List<char>     lastString;
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

            lastString.add( '\0' );

            String locationInfo = String::str( "%s:%d", file->path().cstr(), gettextLineNum );
            String message      = &lastString[0];

            messages.include( message, locationInfo );
          }
        }
        else if( last[1] == '.' && last[0] == '.' ) {
          restartString = false;
        }
        else if( last[0] == '"' || last[0] == '\'' ) {
          state = last[0] == '"' ? STRING1 : STRING2;

          if( restartString ) {
            lastString.clear();
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
            lastString.add( '\n' );
          }
          else if( last[0] == 't' ) {
            lastString.add( '\t' );
          }
          else {
            lastString.add( last[0] );
          }
        }
        else if( ( state == STRING1 && last[0] == '"' ) ||
                 ( state == STRING2 && last[0] == '\'' ) )
        {
          state = NORMAL;
          restartString = true;
        }
        else if( last[0] != '\\' ) {
          lastString.add( last[0] );
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

static void readCredits( File* file )
{
  if( !file->map() ) {
    OZ_ERROR( "Failed to map '%s'", file->path().cstr() );
  }

  String contents;
  InputStream is = file->inputStream();

  while( is.isAvailable() ) {
    contents += is.readLine() + "\n";
  }

  titles.include( contents, file->path() );
}

static void readSequence( File* file )
{
  JSON sequence;
  sequence.load( file );

  int nSteps = sequence.length();
  for( int i = 0; i < nSteps; ++i ) {
    const char* title = sequence[i]["title"].get( "" );

    if( !String::isEmpty( title ) ) {
      String locationInfo = String::str( "%s:step #%d", file->path().cstr(), i + 1 );

      messages.include( title, locationInfo );
    }
  }
}

static void readDescription( File* file )
{
  JSON descriptionConfig;
  descriptionConfig.load( file );

  const char* title       = descriptionConfig["title"].get( "" );
  const char* description = descriptionConfig["description"].get( "" );

  if( !String::isEmpty( title ) ) {
    messages.include( title, file->path() );
  }
  if( !String::isEmpty( description ) ) {
    messages.include( description, file->path() );
  }
}

static void writePOT( const HashString<String>* hs, const char* filePath )
{
  BufferStream bs;
  String s;

  bool isFirst = true;
  foreach( i, hs->citer() ) {
    if( !isFirst ) {
      bs.writeChar( '\n' );
    }
    isFirst = false;

    bs.writeLine( "#: " + i->value );

    // Escape backslashes and quotes.
    s = i->key;
    for( int j = 0; j < s.length(); ++j ) {
      if( s[j] == '\\' || s[j] == '"' ) {
        s = s.substring( 0, j ) + "\\" + s.substring( j );
        ++j;
      }
    }

    // If multi-line, put each line into a new line in .pot file and escape newlines.
    if( s.index( '\n' ) < 0 ) {
      bs.writeLine( String::str( "msgid \"%s\"", s.cstr() ) );
    }
    else {
      DArray<String> stringLines = s.split( '\n' );

      bs.writeLine( "msgid \"\"" );

      foreach( l, stringLines.citer() ) {
        if( &*l == &stringLines.last() && l->isEmpty() ) {
          break;
        }

        if( l == &stringLines.last() ) {
          s = String::str( "\"%s\"", l->cstr() );
        }
        else {
          s = String::str( "\"%s\\n\"", l->cstr() );
        }

        bs.writeLine( s );
      }
    }

    bs.writeLine( "msgstr \"\"" );
  }

  File outFile( filePath );

  if( !outFile.write( bs.begin(), bs.length() ) ) {
    OZ_ERROR( "Failed to write '%s'", outFile.path().cstr() );
  }
}

static int main( int argc, char** argv )
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
    OZ_ERROR( "Package directory cannot be root ('/')" );
  }

  String pkgName = pkgDir.substring( pkgDir.lastIndex( '/' ) + 1 );

  BufferStream bs;

  File bspDir( pkgDir + "/baseq3/maps" );
  DArray<File> files = bspDir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    readBSP( file );
  }

  File classDir( pkgDir + "/class" );
  files = classDir.ls();

  foreach( file, files.iter() ) {
    if( !file->hasExtension( "json" ) ) {
      continue;
    }

    readClass( file );
  }

  File creditsFile( pkgDir + "/credits/" + pkgName + ".txt" );
  readCredits( &creditsFile );

  if( !titles.isEmpty() ) {
    String mainPOT = String::str( "%s/lingua/%s.pot", pkgDir.cstr(), pkgName.cstr() );
    Log::print( "%s ...", mainPOT.cstr() );

    File::mkdir( pkgDir + "/lingua" );

    writePOT( &titles, mainPOT );

    titles.clear();
    titles.dealloc();

    Log::printEnd( " OK" );
  }

  File missionsDir( pkgDir + "/mission" );
  DArray<File> missions = missionsDir.ls();

  foreach( mission, missions.iter() ) {
    mission->stat();

    if( mission->type() != File::DIRECTORY ) {
      continue;
    }

    files = mission->ls();

    foreach( file, files.iter() ) {
      if( file->hasExtension( "lua" ) ) {
        readLua( file );
      }
      else if( file->hasExtension( "json" ) ) {
        if( file->path().endsWith( ".sequence.json" ) ) {
          readSequence( file );
        }
        else {
          readDescription( file );
        }
      }
    }

    if( !messages.isEmpty() ) {
      String missionPOT = mission->path() + "/lingua/messages.pot";
      Log::print( "%s ...", missionPOT.cstr() );

      File::mkdir( mission->path() + "/lingua" );
      writePOT( &messages, mission->path() + "/lingua/messages.pot" );

      messages.clear();
      messages.dealloc();

      Log::printEnd( " OK" );
    }
  }

  return EXIT_SUCCESS;
}

}

int main( int argc, char** argv )
{
  return oz::main( argc, argv );
}
