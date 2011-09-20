/*
 *  ozPrebuildData.cpp
 *
 *  Prebuild data
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "Build.hpp"

#include "matrix/Translator.hpp"
#include "matrix/Orbis.hpp"
#include "matrix/QBSP.hpp"
#include "matrix/Matrix.hpp"

#include "client/Context.hpp"
#include "client/Compiler.hpp"
#include "client/Caelum.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"
#include "client/OBJ.hpp"
#include "client/MD2.hpp"
#include "client/Render.hpp"

#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL/SDL_main.h>

#ifdef OZ_MINGW
# define program_invocation_short_name "ozPrebuild"
#endif

using namespace oz;

static const char* const CREATE_DIRS[] = {
  "bsp",
  "bsp/tex",
  "caelum",
  "class",
  "glsl",
  "lua",
  "lua/matrix",
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

bool Alloc::isLocked = true;

static bool doForceRebuild = false;

static void printUsage()
{
  log.println( "Usage:" );
  log.indent();
  log.println( "%s [OPTIONS] [DATA_DIR]", program_invocation_short_name );
  log.println();
  log.println( "--force" );
  log.println( "-f" );
  log.println( "\tForce rebuild of all resources" );
  log.println();
  log.unindent();
}

static void createDirs()
{
  log.println( "Creating directory structure {" );
  log.indent();

  for( int i = 0; i < aLength( CREATE_DIRS ); ++i ) {
    log.print( "%-11s ...", CREATE_DIRS[i] );

    struct stat dirStat;
    if( stat( CREATE_DIRS[i], &dirStat ) == 0 ) {
      log.printEnd( " OK, exists" );
      continue;
    }

#ifdef OZ_MINGW
    if( mkdir( CREATE_DIRS[i] ) != 0 ) {
#else
    if( mkdir( CREATE_DIRS[i], S_IRUSR | S_IWUSR | S_IXUSR ) != 0 ) {
#endif
      log.printEnd( " Failed" );
      throw Exception( "Failed to create directories" );
    }

    log.printEnd( " OK, created" );
  }

  log.unindent();
  log.println( "}" );
}

static void prebuildTextures( const char* srcDir, const char* destDir,
                              bool wrap, int magFilter, int minFilter )
{
  log.println( "Prebuilding textures in '%s' {", srcDir );
  log.indent();

  String sSrcDir = srcDir;
  String sDestDir = destDir;
  Directory dir( sSrcDir );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + sSrcDir + "'" );
  }

  sSrcDir  = sSrcDir + "/";
  sDestDir = sDestDir + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "png" ) && !ent.hasExtension( "jpg" ) ) {
      continue;
    }

    String srcPath = sSrcDir + ent;
    String destPath = sDestDir + ent.baseName() + ".ozcTex";

    struct stat srcInfo;
    struct stat destInfo;

    if( stat( srcPath, &srcInfo ) != 0 ) {
      throw Exception( "Source texture '" + srcPath + "' stat error" );
    }
    if( !doForceRebuild && stat( destPath, &destInfo ) == 0 &&
        destInfo.st_mtime > srcInfo.st_mtime )
    {
      continue;
    }

    log.println( "Prebuilding texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = client::context.loadRawTexture( srcPath, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    OutputStream os = buffer.outputStream();

    log.println( "Compiling into '%s'", destPath.cstr() );
    client::context.writeTexture( id, &os );

    if( !buffer.write( destPath, os.length() ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  log.unindent();
  log.println( "}" );
}

static void prebuildModels()
{
  log.println( "Prebuilding models {" );
  log.indent();

  String dirName = "mdl";
  Directory dir( dirName );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + dirName + "'" );
  }

  dirName = dirName + "/";

  foreach( ent, dir.citer() ) {
    struct stat srcInfo0;
    struct stat srcInfo1;
    struct stat configInfo;
    struct stat destInfo;

    String name = static_cast<const char*>( ent );
    String path = dirName + name;

    if( stat( path + "/data.obj", &srcInfo0 ) == 0 ) {
      if( stat( path + "/data.mtl", &srcInfo1 ) != 0 ||
          stat( path + "/config.rc", &configInfo ) != 0 )
      {
        throw Exception( "OBJ model '" + name + "' source files missing" );
      }
      if( !doForceRebuild && stat( dirName + name + ".ozcSMM", &destInfo ) == 0 &&
          configInfo.st_mtime < destInfo.st_mtime &&
          srcInfo0.st_mtime < destInfo.st_mtime && srcInfo1.st_mtime < destInfo.st_mtime )
      {
        continue;
      }

      client::OBJ::prebuild( path );
    }
    else if( stat( path + "/tris.md2", &srcInfo0 ) == 0 ) {
      if( stat( path + "/skin.jpg", &srcInfo1 ) != 0 ||
          stat( path + "/config.rc", &configInfo ) != 0 )
      {
        throw Exception( "MD2 model '" + name + "' source files missing" );
      }
      if( !doForceRebuild && ( stat( dirName + name + ".ozcSMM", &destInfo ) == 0 ||
          stat( dirName + name + ".ozcMD2", &destInfo ) == 0 ) &&
          configInfo.st_mtime < destInfo.st_mtime &&
          srcInfo0.st_mtime < destInfo.st_mtime && srcInfo1.st_mtime < destInfo.st_mtime )
      {
        continue;
      }

      client::MD2::prebuild( path );
    }
  }

  log.unindent();
  log.println( "}" );
}

static void prebuildBSPs()
{
  log.println( "Prebuilding BSPs {" );
  log.indent();

  String srcDir = "data/maps";
  String destDir = "bsp";
  Directory dir( srcDir );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + srcDir + "'" );
  }

  srcDir = srcDir + "/";
  destDir = destDir + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "rc" ) ) {
      continue;
    }

    String srcPath0 = srcDir + ent;
    String srcPath1 = srcDir + ent.baseName() + ".bsp";
    String destPath0 = destDir + ent.baseName() + ".ozBSP";
    String destPath1 = destDir + ent.baseName() + ".ozcBSP";

    struct stat srcInfo0;
    struct stat srcInfo1;
    struct stat destInfo0;
    struct stat destInfo1;

    if( stat( srcPath0, &srcInfo0 ) != 0 || stat( srcPath1, &srcInfo1 ) != 0 ) {
      throw Exception( "Source BSP stat error" );
    }
    if( !doForceRebuild && stat( destPath0, &destInfo0 ) == 0 && stat( destPath1, &destInfo1 ) == 0 &&
        destInfo0.st_mtime > srcInfo0.st_mtime && destInfo0.st_mtime > srcInfo1.st_mtime &&
        destInfo1.st_mtime >= destInfo0.st_mtime )
    {
      continue;
    }

    String name = ent.baseName();

    QBSP::prebuild( name );
    client::BSP::prebuild( name );
  }

  log.unindent();
  log.println( "}" );
}

static void prebuildTerras()
{
  log.println( "Prebuilding Terra {" );
  log.indent();

  String srcDir = "terra";
  Directory dir( srcDir );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + srcDir + "'" );
  }

  srcDir = srcDir + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "rc" ) ) {
      continue;
    }

    String srcPath = srcDir + ent;
    String destPath0 = srcDir + ent.baseName() + ".ozTerra";
    String destPath1 = srcDir + ent.baseName() + ".ozcTerra";

    struct stat srcInfo;
    struct stat destInfo0;
    struct stat destInfo1;

    if( stat( srcPath, &srcInfo ) != 0 ) {
      throw Exception( "Terra .rc stat error" );
    }
    if( !doForceRebuild && stat( destPath0, &destInfo0 ) == 0 && stat( destPath1, &destInfo1 ) == 0 &&
        destInfo0.st_mtime > srcInfo.st_mtime && destInfo1.st_mtime >= destInfo0.st_mtime )
    {
      continue;
    }

    String name = ent.baseName();

    orbis.terra.prebuild( name );
    client::terra.prebuild( name );
  }

  log.unindent();
  log.println( "}" );
}

static void checkLua( const char* path )
{
  log.println( "Checking Lua scripts '%s' {", path );
  log.indent();

  String srcDir = path + String( "/" );
  Directory dir( path );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + srcDir + "'" );
  }

  String sources;

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "lua" ) ) {
      continue;
    }

    sources = sources + " " + srcDir + ent;
  }

  log.println( "luac -p%s", sources.cstr() );
  if( system( "luac -p" + sources ) != 0 ) {
    throw Exception( "Lua syntax check failed" );
  }

  log.unindent();
  log.println( "}" );
}

int main( int argc, char** argv )
{
  System::catchSignals();

  Alloc::isLocked = false;

  int exitCode = 0;

  printf( "OpenZone  Copyright (C) 2002-2011  Davorin Učakar\n"
      "This program comes with ABSOLUTELY NO WARRANTY.\n"
      "This is free software, and you are welcome to redistribute it\n"
      "under certain conditions; See COPYING file for details.\n\n" );

  try {
    for( int i = 1; i < argc; ++i ) {
      if( String::equals( argv[i], "--help" ) ) {
        printUsage();
        return -1;
      }
      else if( String::equals( argv[i], "--force" ) || String::equals( argv[i], "-f" ) ) {
        doForceRebuild = true;
      }
      else if( argv[i][0] != '-' && !config.contains( "data.dir" ) ) {
        config.add( "data.dir", argv[i] );
      }
      else {
        log.println( "Invalid command-line option '%s'", argv[i] );
        log.println();
        printUsage();
        return -1;
      }
    }

    log.printlnETD( OZ_APPLICATION_TITLE " Prebuild started at" );

    log.println( "Build details {" );
    log.indent();

    log.println( "Date:            %s", Build::TIME );
    log.println( "Host system:     %s", Build::HOST_SYSTEM );
    log.println( "Target system:   %s", Build::TARGET_SYSTEM );
    log.println( "Build type:      %s", Build::BUILD_TYPE );
    log.println( "Compiler:        %s", Build::COMPILER );
    log.println( "Compiler flags:  %s", Build::CXX_FLAGS );
    log.println( "Linker flags:    %s", Build::EXE_LINKER_FLAGS );

    log.unindent();
    log.println( "}" );

    String dataDir = config.get( "data.dir", OZ_INSTALL_PREFIX "/share/" OZ_APPLICATION_NAME );

    log.print( "Setting working directory to data directory '%s' ...", dataDir.cstr() );
    if( chdir( dataDir ) != 0 ) {
      log.printEnd( " Failed" );
      return -1;
    }
    log.printEnd( " OK" );

    SDL_Init( SDL_INIT_VIDEO );

    uint startTime = SDL_GetTicks();

    translator.prebuildInit();

    config.add( "screen.width", "400" );
    config.add( "screen.height", "40" );
    config.add( "screen.bpp", "32" );
    config.add( "screen.full", "false" );
    client::render.init();
    SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Prebuilding data ...", null );

    buffer.alloc( 10 * 1024 * 1024 );

    createDirs();

    client::ui::Mouse::prebuild();

    prebuildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );
    prebuildTextures( "data/textures/oz", "bsp/tex", true,
                      client::Context::DEFAULT_MAG_FILTER, client::Context::DEFAULT_MIN_FILTER );

    client::Caelum::prebuild( "caelum" );
    prebuildTerras();

    prebuildBSPs();
    prebuildModels();

    checkLua( "lua/matrix" );
    checkLua( "lua/nirvana" );

    uint endTime = SDL_GetTicks();

    log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

    SDL_Quit();
  }
  catch( const Exception& e ) {
    log.resetIndent();
    log.println();
    log.printException( e );
    log.println();

    if( log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
      fprintf( stderr, "  in %s\n\n", e.function );
      fprintf( stderr, "  at %s:%d\n\n", e.file, e.line );
    }

    exitCode = -1;
  }
  catch( const std::exception& e ) {
    log.resetIndent();
    log.println();
    log.println( "EXCEPTION: %s", e.what() );
    log.println();

    if( log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n\n", e.what() );
    }

    exitCode = -1;
  }

  client::compiler.free();
  buffer.dealloc();
  client::render.free();
  translator.free();
  config.clear();

  Alloc::printStatistics();
  log.printlnETD( OZ_APPLICATION_TITLE " Prebuild finished at" );

//   Alloc::isLocked = true;
//   Alloc::printLeaks();
  return exitCode;
}
