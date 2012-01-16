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

#include "build/Lingua.hh"
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
#include <FreeImage.h>

namespace oz
{
namespace build
{

static void printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "ozBuild [OPTIONS] <data_src_dir> <data_out_dir>" );
  log.println();
  log.println( "<data_src_dir>" );
  log.println( "\tUses <data_src_dir> as game data source directory." );
  log.println();
  log.println( "<data_out_dir>" );
  log.println( "\tWhere to write built game data (data that doesn't need building are copied)." );
  log.println( "\tThis directory must differ from <data_src_dir>." );
  log.println();
  log.println( "-v" );
  log.println( "\tMore verbose log output." );
  log.println();
  log.println( "-l" );
  log.println( "\tBuild translations." );
  log.println();
  log.println( "-u" );
  log.println( "\tBuild UI." );
  log.println();
  log.println( "-g" );
  log.println( "\tCopy shaders." );
  log.println();
  log.println( "-c" );
  log.println( "\tBuild caela (skies)." );
  log.println();
  log.println( "-t" );
  log.println( "\tBuild terrae (terrains)." );
  log.println();
  log.println( "-b" );
  log.println( "\tCompile maps into BSPs and build BPSs with referenced textures." );
  log.println();
  log.println( "-m" );
  log.println( "\tBuild models." );
  log.println();
  log.println( "-s" );
  log.println( "\tCopy sounds (only used ones)." );
  log.println();
  log.println( "-a" );
  log.println( "\tCopy object class definitions." );
  log.println();
  log.println( "-f" );
  log.println( "\tCopy fragment pool definitions." );
  log.println();
  log.println( "-n" );
  log.println( "\tCopy name lists." );
  log.println();
  log.println( "-x" );
  log.println( "\tCheck and copy Lua scripts." );
  log.println();
  log.println( "-o" );
  log.println( "\tBuild modules." );
  log.println();
  log.println( "-r" );
  log.println( "\tCopy music tracks." );
  log.println();
  log.println( "-A" );
  log.println( "\tBuild everything." );
  log.println();
  log.println( "-C" );
  log.println( "\tUse S3 texture compression" );
  log.println();
  log.unindent();
}

static void copyFiles( const char* srcDir, const char* destDir, const char* ext, bool recurse )
{
  log.println( "Copying *.%s from '%s' to '%s' {", ext, srcDir, destDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PhysFile dir( sSrcDir );
  DArray<PhysFile> dirList = dir.ls();

  if( !dirList.isEmpty() ) {
    File::mkdir( destDir );
  }

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( file->getType() == PhysFile::DIRECTORY ) {
      if( recurse ) {
        copyFiles( srcDir + ( "/" + file->name() ), destDir + ( "/" + file->name() ), ext, true );
      }
    }
    else if( file->hasExtension( ext ) || fileName.beginsWith( "README" ) ||
      fileName.beginsWith( "COPYING" ) )
    {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( String::str( "%s/%s", destDir, fileName.cstr() ) );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
      continue;
    }
  }

  log.unindent();
  log.println( "}" );
}

static void buildTextures( const char* srcDir, const char* destDir,
                           bool wrap, int magFilter, int minFilter )
{
  log.println( "Building textures in '%s' to '%s' {", srcDir, destDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  PhysFile dir( sSrcDir );
  DArray<PhysFile> dirList = dir.ls();

  if( !dirList.isEmpty() ) {
    File::mkdir( destDir );
  }

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( String::str( "%s/%s", destDir, fileName.cstr() ) );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
      continue;
    }

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

static void buildCaela()
{
  log.println( "Building Caela {" );
  log.indent();

  String srcDir = "caelum";
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

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

static void buildTerrae()
{
  log.println( "Building Terrae {" );
  log.indent();

  String srcDir = "terra";
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

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

static void compileBSPs()
{
  log.println( "Compiling BSPs {" );
  log.indent();

  String dirName = "data/maps";
  PhysFile dir( dirName );
  DArray<PhysFile> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "map" ) ) {
      continue;
    }

    String baseName = file->baseName();
    const char* dot = String::findLast( baseName, '.' );

    if( baseName.equals( "autosave" ) || ( dot != null && String::equals( dot + 1, "autosave" ) ) )
    {
      continue;
    }

    String cmdLine = String::str( "q3map2 -fs_basepath %s -fs_game data %s",
                                  file->mountPoint().cstr(), file->realPath().cstr() );

    log.println( "%s", cmdLine.cstr() );
    log.println();
    log.println( "========== q3map2 OUTPUT BEGIN %s ==========", baseName.cstr() );
    log.println();

    fflush( stdout );
    fflush( stderr );

    if( system( cmdLine ) != 0 ) {
      throw Exception( "BSP map compilation failed" );
    }

    fflush( stdout );
    fflush( stderr );

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
  PhysFile dir( srcDir );
  DArray<PhysFile> dirList = dir.ls();

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

static void buildBSPTextures( const char* mountPoint )
{
  log.println( "Building BSP textures {" );
  log.indent();

  for( int i = 0; i < library.textures.length(); ++i ) {
    if( !BSP::usedTextures.get( i ) ) {
      continue;
    }

    String srcPath = String::str( "%s/%s", mountPoint, library.textures[i].path.cstr() );
    String destPath = String::str( "bsp/%s.ozcTex", library.textures[i].name.cstr() );

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
  PhysFile dir( dirName );
  DArray<PhysFile> dirList = dir.ls();

  dirName = dirName + "/";

  foreach( file, dirList.citer() ) {
    String path = file->path();

    if( PhysFile( path + "/data.obj" ).getType() != PhysFile::MISSING ) {
      OBJ::build( path );
    }
    else if( PhysFile( path + "/tris.md2" ).getType() != PhysFile::MISSING ) {
      MD2::build( path );
    }
    else if( PhysFile( path + "/.md3" ).getType() != PhysFile::MISSING ) {
      MD3::build( path );
    }
  }

  log.unindent();
  log.println( "}" );
}

static void buildSounds()
{
  log.println( "Copying used sounds {" );
  log.indent();

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
  PhysFile dir( path );
  DArray<PhysFile> dirList = dir.ls();

  String sources;

  foreach( file, dirList.citer() ) {
    if( !file->hasExtension( "lua" ) ) {
      continue;
    }

    String cmdLine = "luac -p " + file->realPath();

    log.println( "%s", cmdLine.cstr() );
    if( system( cmdLine ) != 0 ) {
      throw Exception( "Lua syntax check failed" );
    }
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
  bool doCat     = false;
  bool doUI      = false;
  bool doShaders = false;
  bool doCaela   = false;
  bool doTerrae  = false;
  bool doBSPs    = false;
  bool doModels  = false;
  bool doSounds  = false;
  bool doClasses = false;
  bool doFrags   = false;
  bool doNames   = false;
  bool doLua     = false;
  bool doModules = false;
  bool doMusic   = false;

  optind = 1;
  int opt;
  while( ( opt = getopt( argc, argv, "vlugtcbmsafnxorCA" ) ) != -1 ) {
    switch( opt ) {
      case 'v': {
        log.isVerbose = true;
        break;
      }
      case 'l': {
        doCat = true;
        break;
      }
      case 'u': {
        doUI = true;
        break;
      }
      case 'g': {
        doShaders = true;
        break;
      }
      case 'c': {
        doCaela = true;
        break;
      }
      case 't': {
        doTerrae = true;
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
      case 's': {
        doSounds = true;
        break;
      }
      case 'a': {
        doClasses = true;
        break;
      }
      case 'f': {
        doFrags = true;
        break;
      }
      case 'n': {
        doNames = true;
        break;
      }
      case 'x': {
        doLua = true;
        break;
      }
      case 'o': {
        doModules = true;
        break;
      }
      case 'r': {
        doMusic = true;
        break;
      }
      case 'C': {
        Context::useS3TC = true;
        break;
      }
      case 'A': {
        doCat     = true;
        doUI      = true;
        doShaders = true;
        doCaela   = true;
        doTerrae  = true;
        doBSPs    = true;
        doModels  = true;
        doSounds  = true;
        doNames   = true;
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

  if( optind != argc - 2 ) {
    printUsage();
    return EXIT_FAILURE;
  }

  String dataDir = argv[optind];
  String outDir = argv[optind + 1];

  if( dataDir[0] != '/' ) {
    dataDir = File::cwd() + "/" + dataDir;
  }
  if( outDir[0] != '/' ) {
    outDir = File::cwd() + "/" + outDir;
  }

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
  if( !PhysFile::init() ) {
    throw Exception( "PhysicsFS initialisation failed" );
  }
  FreeImage_Initialise();

  File::mkdir( outDir );

  log.println( "Chdir to output directory '%s'", outDir.cstr() );
  if( !File::chdir( outDir ) ) {
    log.println( "Failed to set working directory '%s'", outDir.cstr() );
    return EXIT_FAILURE;
  }

  log.println( "Adding source directory '%s' to search path", dataDir.cstr() );
  if( !PhysFile::mount( dataDir, null, true ) ) {
    log.println( "Failed to add directory '%s' to search path", dataDir.cstr() );
    return EXIT_FAILURE;
  }

  uint startTime = Time::clock();

  // FIXME
//   library.init();

  config.add( "screen.width", "400" );
  config.add( "screen.height", "40" );
  config.add( "screen.full", "false" );
  client::render.init( true );
  SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Building data ...", null );

  if( !client::shader.hasS3TC && Context::useS3TC ) {
    throw Exception( "S3 texture compression enabled but not supported" );
  }

  // copy package README
  DArray<PhysFile> dirList = PhysFile( "/" ).ls();

  foreach( file, dirList.iter() ) {
    String fileName = file->name();

    if( fileName.beginsWith( "README" ) || fileName.beginsWith( "COPYING" ) ) {
      log.print( "Copying '%s' ...", fileName.cstr() );

      if( !file->map() ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      InputStream is = file->inputStream();
      File destFile( fileName );

      if( !destFile.write( &is ) ) {
        throw Exception( "Failed to copy '%s'", file->realPath().cstr() );
      }

      file->unmap();

      log.printEnd( " OK" );
    }
  }

  dirList.dealloc();

  if( doCat ) {
    lingua.build();
  }
  if( doUI ) {
    bool useS3TC = Context::useS3TC;
    Context::useS3TC = false;

    if( PhysFile( "ui/cur" ).getType() != PhysFile::MISSING ) {
      Mouse::build();
    }

    copyFiles( "ui", "ui", "", false );
    copyFiles( "ui/font", "ui/font", "ttf", false );
    buildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );
    buildTextures( "ui/galileo", "ui/galileo", true, GL_LINEAR, GL_LINEAR );

    Context::useS3TC = useS3TC;
  }
  if( doShaders ) {
    copyFiles( "glsl", "glsl", "glsl", false );
    copyFiles( "glsl", "glsl", "vert", false );
    copyFiles( "glsl", "glsl", "frag", false );
  }
  if( doCaela ) {
    buildCaela();
  }
  if( doTerrae ) {
    buildTerrae();
  }
  if( doBSPs ) {
    compileBSPs();
    buildBSPs();
    buildBSPTextures( dataDir );
  }
  if( doModels ) {
    buildModels();
  }
  if( doSounds ) {
    buildSounds();
  }
  if( doClasses ) {
    copyFiles( "class", "class", "rc", false );
  }
  if( doFrags ) {
    copyFiles( "frag", "frag", "rc", false );
  }
  if( doNames ) {
    copyFiles( "name", "name", "txt", false );
  }
  if( doLua ) {
    checkLua( "lua/matrix" );
    checkLua( "lua/nirvana" );
    checkLua( "lua/mission" );

    copyFiles( "lua", "lua", "lua", true );
  }
  if( doModules ) {
    buildModules();
  }
  if( doMusic ) {
    copyFiles( "music", "music", "oga", true );
  }

  uint endTime = Time::clock();

  FreeImage_DeInitialise();
  PhysFile::free();
  SDL_Quit();

  log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  return EXIT_SUCCESS;
}

}
}

int main( int argc, char** argv )
{
  oz::System::init();

  int exitCode = EXIT_FAILURE;

  printf( "OpenZone  Copyright © 2002-2012 Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    exitCode = oz::build::main( argc, argv );
    oz::build::shutdown();
  }
  catch( const std::exception& e ) {
    oz::log.verboseMode = false;
    oz::log.printException( e );

    oz::System::bell();
    oz::System::abort();
  }

//   oz::Alloc::printLeaks();
  return exitCode;
}
