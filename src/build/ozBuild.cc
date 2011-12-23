/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file build/ozBuild.cc
 *
 * Data builder.
 */

#include "stable.hh"

#include "BuildInfo.hh"

#include "matrix/Library.hh"

#include "client/Render.hh"
#include "client/Module.hh"
#include "client/OpenGL.hh"

#include "build/Context.hh"
#include "build/Compiler.hh"
#include "build/Mouse.hh"
#include "build/Caelum.hh"
#include "build/Terra.hh"
#include "build/BSP.hh"
#include "build/OBJ.hh"
#include "build/MD2.hh"
#include "build/MD3.hh"

#include "build/modules/GalileoModule.hh"

#include <unistd.h>

#include <SDL/SDL_main.h>
#include <physfs.h>
#include <IL/il.h>

bool oz::Alloc::isLocked = true;

namespace oz
{
namespace build
{

static const char* const CREATE_DIRS[] = {
  "bsp",
  "caelum",
  "class",
  "glsl",
  "lua",
  "lua/matrix",
  "lua/mission",
  "lua/nirvana",
  "mdl",
  "music",
  "name",
  "snd",
  "terra",
  "ui",
  "ui/cur",
  "ui/font",
  "ui/icon"
};

static void printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "ozBuild [OPTIONS] <prefix>" );
  log.println();
  log.println( "<prefix>" );
  log.println( "\tSets data directory to <prefix>/share/openzone." );
  log.println();
  log.println( "-v" );
  log.println( "\tMore verbose log output." );
  log.println();
  log.println( "-u" );
  log.println( "\tBuild UI." );
  log.println();
  log.println( "-t" );
  log.println( "\tBuild terrae (terrains)." );
  log.println();
  log.println( "-c" );
  log.println( "\tBuild caela (skies)." );
  log.println();
  log.println( "-b" );
  log.println( "\tCompile maps into BSPs and build BPSs with referenced textures." );
  log.println();
  log.println( "-m" );
  log.println( "\tBuild models." );
  log.println();
  log.println( "-o" );
  log.println( "\tBuild modules." );
  log.println();
  log.println( "-l" );
  log.println( "\tCheck syntax of Lua scripts." );
  log.println();
  log.println( "-A" );
  log.println( "\tBuild everything." );
  log.println();
  log.println( "-C" );
  log.println( "\tUse S3 texture compression" );
  log.println();
  log.unindent();
}

static void createDirs()
{
  log.println( "Creating directory structure {" );
  log.indent();

  for( int i = 0; i < aLength( CREATE_DIRS ); ++i ) {
    log.print( "%-11s ...", CREATE_DIRS[i] );

    File dir( CREATE_DIRS[i] );
    if( dir.getType() == File::DIRECTORY ) {
      log.printEnd( " OK, exists" );
      continue;
    }

    if( !File::mkdir( CREATE_DIRS[i] ) ) {
      throw Exception( "Failed to create directories" );
    }

    log.printEnd( " OK, created" );
  }

  log.unindent();
  log.println( "}" );
}

static void buildTextures( const char* srcDir, const char* destDir,
                           bool wrap, int magFilter, int minFilter )
{
  log.println( "Building textures in '%s' {", srcDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  File dir( sSrcDir );
  DArray<File> dirList = dir.ls();

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "png" ) && !file->hasExtension( "jpg" ) ) {
      continue;
    }

    String srcPath = file->path();
    String destPath = sDestDir + file->baseName() + ".ozcTex";

    log.println( "Building texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = Context::loadRawTexture( srcPath, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    BufferStream os;

    log.println( "Compiling into '%s'", destPath.cstr() );
    Context::writeTexture( id, &os );

    glDeleteTextures( 1, &id );

    if( !File( destPath ).write( &os ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  log.unindent();
  log.println( "}" );
}

static void buildTerrae()
{
  log.println( "Building Terrae {" );
  log.indent();

  String srcDir = "terra";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    Terra::build( file->baseName() );
  }

  log.unindent();
  log.println( "}" );
}

static void buildCaela()
{
  log.println( "Building Caela {" );
  log.indent();

  String srcDir = "caelum";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    String name = file->baseName();

    Caelum::build( name );
  }

  log.unindent();
  log.println( "}" );
}

static void compileBSPs()
{
  log.println( "Compiling BSPs {" );
  log.indent();

  String dirName = "data/maps";
  File dir( dirName );
  DArray<File> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "map" ) ) {
      continue;
    }

    String baseName = file->baseName();
    const char* dot = String::findLast( baseName, '.' );

    if( baseName.equals( "autosave" ) ||
        ( dot != null && String::equals( dot + 1, "autosave" ) ) )
    {
      continue;
    }

    String cmdLine = String::str( "q3map2 -fs_basepath . -fs_game data %s", file->path().cstr() );

    log.println( "%s", cmdLine.cstr() );
    log.println();
    log.println( "========== q3map2 OUTPUT BEGIN %s ==========", baseName.cstr() );
    log.println();
    if( system( cmdLine ) != 0 ) {
      throw Exception( "BSP map compilation failed" );
    }
    log.println();
    log.println( "========== q3map2 OUTPUT END %s ==========", baseName.cstr() );
    log.println();
  }

  log.unindent();
  log.println( "}" );
}

static void buildBSPs()
{
  log.println( "Building BSPs {" );
  log.indent();

  String srcDir = "data/maps";
  String destDir = "bsp";
  File dir( srcDir );
  DArray<File> dirList = dir.ls();

  srcDir = srcDir + "/";
  destDir = destDir + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "rc" ) ) {
      continue;
    }

    BSP::build( file->baseName() );
  }

  log.unindent();
  log.println( "}" );
}

static void buildBSPTextures()
{
  log.println( "Building BSP textures {" );
  log.indent();

  for( int i = 0; i < library.textures.length(); ++i ) {
    if( !BSP::usedTextures.get( i ) ) {
      continue;
    }

    String srcPath = library.textures[i].path;
    String destPath = "bsp/" + library.textures[i].name + ".ozcTex";

    log.println( "Building texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = Context::loadRawTexture( srcPath );

    hard_assert( id != 0 );

    BufferStream os;

    log.println( "Compiling into '%s'", destPath.cstr() );
    Context::writeTexture( id, &os );

    int slash = destPath.lastIndex( '/' );
    hard_assert( slash != -1 );
    String dir = destPath.substring( 0, slash );

    File::mkdir( dir );

    if( !File( destPath ).write( &os ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  BSP::usedTextures.dealloc();

  log.unindent();
  log.println( "}" );
}

static void buildModels()
{
  log.println( "Building models {" );
  log.indent();

  String dirName = "mdl";
  File dir( dirName );
  DArray<File> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    String name = file->name();
    String path = file->path();

    if( File( path + "/data.obj" ).getType() != File::MISSING ) {
      OBJ::build( path );
    }
    else if( File( path + "/tris.md2" ).getType() != File::MISSING ) {
      MD2::build( path );
    }
    else if( File( path + "/.md3" ).getType() != File::MISSING ) {
      MD3::build( path );
    }
  }

  log.unindent();
  log.println( "}" );
}

static void buildModules()
{
  log.println( "Building Modules {" );
  log.indent();

  GalileoModule::build();

  log.unindent();
  log.println( "}" );
}

static void checkLua( const char* path )
{
  log.println( "Checking Lua scripts '%s' {", path );
  log.indent();

  String srcDir = String::str( "%s/", path );
  File dir( path );
  DArray<File> dirList = dir.ls();

  String sources;

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    sources = sources + " " + file->path();
  }

  log.println( "luac -p%s", sources.cstr() );
  if( system( "luac -p" + sources ) != 0 ) {
    throw Exception( "Lua syntax check failed" );
  }

  log.unindent();
  log.println( "}" );
}

static void shutdown()
{
  compiler.free();
  client::render.free( true );
  library.free();
  config.clear( true );

  SDL_Quit();

  Alloc::printStatistics();

  log.print( OZ_APPLICATION_TITLE " Build finished on " );
  log.printTime();
  log.printEnd();
}

int main( int argc, char** argv )
{
  bool doUI      = false;
  bool doTerrae  = false;
  bool doCaela   = false;
  bool doBSPs    = false;
  bool doModels  = false;
  bool doModules = false;
  bool doLua     = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vutcbmolCA" ) ) != -1 ) {
    switch( opt ) {
      case 'v': {
        log.isVerbose = true;
        break;
      }
      case 'u': {
        doUI = true;
        break;
      }
      case 't': {
        doTerrae = true;
        break;
      }
      case 'c': {
        doCaela = true;
        break;
      }
      case 'b': {
        doBSPs = true;
        break;
      }
      case 'm': {
        doModels = true;
        break;
      }
      case 'o': {
        doModules = true;
        break;
      }
      case 'l': {
        doLua = true;
        break;
      }
      case 'C': {
        Context::useS3TC = true;
        break;
      }
      case 'A': {
        doUI      = true;
        doTerrae  = true;
        doCaela   = true;
        doBSPs    = true;
        doModels  = true;
        doModules = true;
        doLua     = true;
        break;
      }
      default: {
        log.println();
        printUsage();
        return EXIT_FAILURE;
      }
    }
  }

  if( optind != argc - 1 ) {
    printUsage();
    return EXIT_FAILURE;
  }

  String dataDir = argv[optind];

  log.print( OZ_APPLICATION_TITLE " Build started on " );
  log.printTime();
  log.printEnd();

  log.println( "Build details {" );
  log.indent();

  log.println( "Date:            %s", BuildInfo::TIME );
  log.println( "Host system:     %s", BuildInfo::HOST_SYSTEM );
  log.println( "Target system:   %s", BuildInfo::TARGET_SYSTEM );
  log.println( "Build type:      %s", BuildInfo::BUILD_TYPE );
  log.println( "Compiler:        %s", BuildInfo::COMPILER );
  log.println( "Compiler flags:  %s", BuildInfo::CXX_FLAGS );
  log.println( "Linker flags:    %s", BuildInfo::EXE_LINKER_FLAGS );

  log.unindent();
  log.println( "}" );

  SDL_Init( SDL_INIT_VIDEO );
  PHYSFS_init( null );
  ilInit();

  if( !File::chdir( dataDir ) ) {
    log.println( "Failed to set working directory '%s'", dataDir.cstr() );
    return EXIT_FAILURE;
  }

  PHYSFS_mount( ".", null, 1 );

  uint startTime = SDL_GetTicks();

  library.buildInit();

  config.add( "screen.width", "400" );
  config.add( "screen.height", "40" );
  config.add( "screen.bpp", "32" );
  config.add( "screen.full", "false" );
  client::render.init( true );
  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Building data ...", null );

  if( !client::shader.hasS3TC && Context::useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  createDirs();

  if( doUI ) {
    bool useS3TC = Context::useS3TC;
    Context::useS3TC = false;

    Mouse::build();
    buildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );

    Context::useS3TC = useS3TC;
  }

  if( doTerrae ) {
    buildTerrae();
  }

  if( doCaela ) {
    buildCaela();
  }

  if( doBSPs ) {
    compileBSPs();
    buildBSPs();
    buildBSPTextures();
  }

  if( doModels ) {
    buildModels();
  }

  if( doModules ) {
    buildModules();
  }

  if( doLua ) {
    checkLua( "lua/matrix" );
    checkLua( "lua/nirvana" );
    checkLua( "lua/mission" );
  }

  uint endTime = SDL_GetTicks();

  ilShutDown();
  PHYSFS_deinit();
  SDL_Quit();

  log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  return EXIT_SUCCESS;
}

}
}

int main( int argc, char** argv )
{
  oz::System::init();
  oz::Alloc::isLocked = false;

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright © 2002-2011 Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = oz::build::main( argc, argv );
    oz::build::shutdown();
  }
  catch( const std::exception& e ) {
    oz::log.printException( e );

    oz::System::bell();
    oz::System::abort( false );
  }

//   oz::Alloc::isLocked = true;
//   oz::Alloc::printLeaks();
  return exitCode;
}
