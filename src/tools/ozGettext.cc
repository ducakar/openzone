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
 * @file tools/ozGettext.cc
 *
 * Extract class, weapon, BSP and entity names from game data and generate POT template files for
 * translations.
 */

#include <ozCore/ozCore.hh>

#include <cstdlib>
#include <cstdio>

using namespace oz;

enum LuaSyntaxState
{
  NORMAL,
  STRING1,
  STRING2,
  LINE_COMMENT,
  MULTILINE_COMMENT
};

static HashMap<String, String> messages;

static void printUsage( const char* invocationName )
{
  Log::printRaw(
    "Usage: %s <data_dir>\n"
    "\n"
    "<data_dir>  Package directory that contains source game data. The output POT\n"
    "            files will be written into <data_dir>/lingua.\n\n",
    invocationName );
}

static void readLuaChunk( const char* begin, int size, const char* path )
{
  InputStream is( begin, begin + size );

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
          if( is.tell() >= 10 && aEquals( is.pos() - 10, 9, "ozGettext" ) ) {
            inGettext = true;
            gettextLineNum = lineNum;
          }
        }
        else if( last[0] == ')' ) {
          if( inGettext ) {
            inGettext = false;

            lastString.add( '\0' );

            String locationInfo = String::str( "%s:%d", path, gettextLineNum );
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

static void readLua( const File& file )
{
  if( file.type() != File::REGULAR ) {
    OZ_ERROR( "Failed to read '%s'", file.path().cstr() );
  }

  InputStream buffer = file.inputStream();
  readLuaChunk( buffer.begin(), buffer.available(), file.path() );
}

static void readBSP( const File& file )
{
  JSON config;

  if( !config.load( file ) ) {
    OZ_ERROR( "Failed to load '%s'", file.path().cstr() );
  }

  const char* title       = config["title"].get( "" );
  const char* description = config["description"].get( "" );

  if( String::isEmpty( title ) ) {
    messages.include( file.baseName(), file.path() );
  }
  else {
    messages.include( title, file.path() );
  }
  if( !String::isEmpty( description ) ) {
    messages.include( description, file.path() );
  }

  const JSON& entities = config["entities"];
  int nEntities = entities.length();

  if( !entities.isNull() && entities.type() != JSON::ARRAY ) {
    OZ_ERROR( "'entities' entry in '%s' is not an array", file.path().cstr() );
  }

  for( int i = 0; i < nEntities; ++i ) {
    const char* entityTitle = entities[i]["title"].get( "" );

    if( !String::isEmpty( entityTitle ) ) {
      messages.include( entityTitle, file.path() );
    }
  }

  config.clear();
}

static void readClass( const File& file )
{
  JSON config;

  if( !config.load( file ) ) {
    OZ_ERROR( "Failed to read '%s'", file.path().cstr() );
  }

  const char* title       = config["title"].get( "" );
  const char* description = config["description"].get( "" );

  if( String::isEmpty( title ) ) {
    messages.include( file.baseName(), file.path() );
  }
  else {
    messages.include( title, file.path() );
  }
  if( !String::isEmpty( description ) ) {
    messages.include( description, file.path() );
  }

  const JSON& weaponsConfig = config["weapons"];

  for( int i = 0; i < weaponsConfig.length(); ++i ) {
    const char* weaponTitle = weaponsConfig[i]["title"].get( "" );

    if( !String::isEmpty( weaponTitle ) ) {
      messages.include( weaponTitle, file.path() );
    }
  }
}

static void readNirvana( const File& dir )
{
  File techFile = dir.path() + "/techGraph.json";
  JSON techConfig;

  if( techConfig.load( techFile ) ) {
    for( const JSON& node : techConfig.arrayCIter() ) {
      const char* technology  = node["technology"].get( "" );
      const char* title       = node["title"].get( technology );
      const char* description = node["description"].get( "" );

      if( !String::isEmpty( technology ) ) {
        messages.include( title, techFile.path() );
      }
      if( !String::isEmpty( description ) ) {
        messages.include( description, techFile.path() );
      }
    }
  }

  File mindsDir = dir.path() + "/mind";

  for( const File& file : mindsDir.ls() ) {
    if( !file.hasExtension( "json" ) ) {
      continue;
    }

    JSON json = file;

    for( const JSON& state : json.arrayCIter() ) {
      const String& onEnter  = state["onEnter"].get( String::EMPTY );
      const String& onUpdate = state["onUpdate"].get( String::EMPTY );

      if( !onEnter.isEmpty() ) {
        readLuaChunk( onEnter.cstr(), onEnter.length(), file.path() );
      }
      if( !onUpdate.isEmpty() ) {
        readLuaChunk( onUpdate.cstr(), onUpdate.length(), file.path() );
      }

      for( const JSON& link : state["links"].arrayCIter() ) {
        const String& condition = link["if"].get( String::EMPTY );

        if( !condition.isEmpty() ) {
          readLuaChunk( condition.cstr(), condition.length(), file.path() );
        }
      }
    }
  }
}

static void readCredits( const File& file )
{
  if( file.type() != File::REGULAR ) {
    OZ_ERROR( "Failed to read '%s'", file.path().cstr() );
  }

  Buffer buffer = file.read();
  InputStream is = buffer.inputStream();
  String contents;

  while( is.isAvailable() ) {
    contents += is.readLine() + "\n";
  }

  messages.include( contents, file.path() );
}

static void readSequence( const File& file )
{
  JSON sequence( file );

  int nSteps = sequence.length();
  for( int i = 0; i < nSteps; ++i ) {
    const char* title = sequence[i]["title"].get( "" );

    if( !String::isEmpty( title ) ) {
      String locationInfo = String::str( "%s:step #%d", file.path().cstr(), i + 1 );

      messages.include( title, locationInfo );
    }
  }
}

static void readDescription( const File& file )
{
  JSON descriptionConfig( file );

  const char* title       = descriptionConfig["title"].get( "" );
  const char* description = descriptionConfig["description"].get( "" );

  if( !String::isEmpty( title ) ) {
    messages.include( title, file.path() );
  }
  if( !String::isEmpty( description ) ) {
    messages.include( description, file.path() );
  }
}

static void writePOT( const HashMap<String, String>* hs, const char* filePath )
{
  OutputStream os( 0 );
  String s;

  bool isFirst = true;
  for( const auto& i : *hs ) {
    if( !isFirst ) {
      os.writeChar( '\n' );
    }
    isFirst = false;

    os.writeLine( "#: " + i.value );

    // Escape backslashes and quotes.
    s = i.key;
    for( int j = 0; j < s.length(); ++j ) {
      if( s[j] == '\\' || s[j] == '"' ) {
        s = s.substring( 0, j ) + "\\" + s.substring( j );
        ++j;
      }
    }

    // If multi-line, put each line into a new line in .pot file and escape newlines.
    if( s.index( '\n' ) < 0 ) {
      os.writeLine( String::str( "msgid \"%s\"", s.cstr() ) );
    }
    else {
      DArray<String> stringLines = s.split( '\n' );

      os.writeLine( "msgid \"\"" );

      for( const String& l : stringLines ) {
        if( &l == &stringLines.last() && l.isEmpty() ) {
          break;
        }

        if( &l == &stringLines.last() ) {
          s = String::str( "\"%s\"", l.cstr() );
        }
        else {
          s = String::str( "\"%s\\n\"", l.cstr() );
        }

        os.writeLine( s );
      }
    }

    os.writeLine( "msgstr \"\"" );
  }

  File outFile = filePath;

  if( !outFile.write( os.begin(), os.tell() ) ) {
    OZ_ERROR( "Failed to write '%s'", outFile.path().cstr() );
  }
}

int main( int argc, char** argv )
{
  System::init();

  String invocationName = String::fileBaseName( argv[0] );

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

  File luaCommonDir  = pkgDir + "/lua/common";
  File luaMatrixDir  = pkgDir + "/lua/matrix";
  File luaNirvanaDir = pkgDir + "/lua/nirvana";
  File luaClientDir  = pkgDir + "/lua/client";

  DArray<File> luaCommonFiles  = luaCommonDir.ls();
  DArray<File> luaMatrixFiles  = luaMatrixDir.ls();
  DArray<File> luaNirvanaFiles = luaNirvanaDir.ls();
  DArray<File> luaClientFiles  = luaClientDir.ls();

  List<File> luaFiles;
  luaFiles.addAll( luaCommonFiles.begin(), luaCommonFiles.length() );
  luaFiles.addAll( luaMatrixFiles.begin(), luaMatrixFiles.length() );
  luaFiles.addAll( luaNirvanaFiles.begin(), luaNirvanaFiles.length() );
  luaFiles.addAll( luaClientFiles.begin(), luaClientFiles.length() );

  for( const File& file : luaFiles ) {
    if( !file.hasExtension( "lua" ) ) {
      continue;
    }

    readLua( file );
  }

  File bspDir = pkgDir + "/baseq3/maps";

  for( const File& file : bspDir.ls() ) {
    if( !file.hasExtension( "json" ) ) {
      continue;
    }

    readBSP( file );
  }

  File classDir = pkgDir + "/class";

  for( const File& file : classDir.ls() ) {
    if( !file.hasExtension( "json" ) ) {
      continue;
    }

    readClass( file );
  }

  File nirvanaDir = pkgDir + "/nirvana";
  readNirvana( nirvanaDir );

  File creditsFile = pkgDir + "/credits/" + pkgName + ".txt";
  readCredits( creditsFile );

  if( !messages.isEmpty() ) {
    String mainPOT = String::str( "%s/lingua/%s.pot", pkgDir.cstr(), pkgName.cstr() );
    Log::print( "%s ...", mainPOT.cstr() );

    File::mkdir( pkgDir + "/lingua" );
    writePOT( &messages, mainPOT );
    Log::printEnd( " OK" );
  }

  messages.clear();

  File missionsDir = pkgDir + "/mission";

  for( const File& mission : missionsDir.ls() ) {
    if( mission.type() != File::DIRECTORY ) {
      continue;
    }

    for( const File& file : mission.ls() ) {
      if( file.hasExtension( "lua" ) ) {
        readLua( file );
      }
      else if( file.hasExtension( "json" ) ) {
        if( file.path().endsWith( ".sequence.json" ) ) {
          readSequence( file );
        }
        else {
          readDescription( file );
        }
      }
    }

    if( !messages.isEmpty() ) {
      String missionPOT = mission.path() + "/lingua/messages.pot";
      Log::print( "%s ...", missionPOT.cstr() );

      File::mkdir( mission.path() + "/lingua" );
      writePOT( &messages, mission.path() + "/lingua/messages.pot" );
      Log::printEnd( " OK" );
    }
  }

  messages.clear();
  messages.deallocate();

  return EXIT_SUCCESS;
}
