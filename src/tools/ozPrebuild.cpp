/*
 *  ozPrebuildData.cpp
 *
 *  Prebuild data
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

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

using namespace oz;

bool Alloc::isLocked = true;

static bool forceRebuild = false;

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
    if( !forceRebuild && stat( destPath, &destInfo ) == 0 && destInfo.st_mtime > srcInfo.st_mtime )
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
      if( !forceRebuild && stat( dirName + name + ".ozcSMM", &destInfo ) == 0 &&
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
      if( !forceRebuild && ( stat( dirName + name + ".ozcSMM", &destInfo ) == 0 ||
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
    if( !forceRebuild && stat( destPath0, &destInfo0 ) == 0 && stat( destPath1, &destInfo1 ) == 0 &&
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
    if( !forceRebuild && stat( destPath0, &destInfo0 ) == 0 && stat( destPath1, &destInfo1 ) == 0 &&
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

static void prebuildLua( const char* path )
{
  log.println( "Prebuilding Lua scripts '%s' {", path );
  log.indent();

  bool doRebuild = forceRebuild;

  String srcDir = path;
  String destFile = srcDir + ".luac";
  Directory dir( path );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + srcDir + "'" );
  }

  srcDir = srcDir + "/";

  struct stat destInfo;
  if( stat( destFile, &destInfo ) != 0 ) {
    doRebuild = true;
  }

  String sources;

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "lua" ) ) {
      continue;
    }

    String srcPath = srcDir + ent;

    struct stat srcInfo;
    if( stat( srcPath, &srcInfo ) != 0 ) {
      throw Exception( "Lua .lua stat error" );
    }
    if( destInfo.st_mtime <= srcInfo.st_mtime )
    {
      doRebuild = true;
    }

    sources = sources + " " + srcPath;
  }

  if( doRebuild ) {
#ifdef NDEBUG
    log.println( "luac -s -o %s%s", destFile.cstr(), sources.cstr() );
    system( "luac -s -o " + destFile + sources );
#else
    log.println( "luac -o %s%s", destFile.cstr(), sources.cstr() );
    system( "luac -o " + destFile + sources );
#endif
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
    if( argc == 2 && String::equals( argv[1], "--help" ) ) {
      log.println( "Usage: %s [options] [data_directory]", program_invocation_short_name );
      log.println( "-f" );
      log.println( "  Force rebuild of all resources" );
      log.println();
      return -1;
    }
    else if( argc >= 2 && String::equals( argv[1], "-f" ) ) {
      forceRebuild = true;
    }

    log.printlnETD( OZ_APPLICATION_TITLE " Prebuild started at" );

    String dataDir = OZ_INSTALL_PREFIX "/share/" OZ_APPLICATION_NAME;
    if( argc >= 2 && !String::equals( argv[argc - 1], "-f" ) ) {
      dataDir = argv[argc - 1];
    }

    log.print( "Setting working directory to data directory '%s' ...", dataDir.cstr() );
    if( chdir( dataDir ) != 0 ) {
      log.printEnd( " Failed" );
      return -1;
    }
    log.printEnd( " OK" );

    SDL_Init( SDL_INIT_VIDEO );

    long startTime = SDL_GetTicks();

    translator.prebuildInit();

    config.add( "screen.width", "400" );
    config.add( "screen.height", "40" );
    config.add( "screen.bpp", "32" );
    config.add( "screen.full", "false" );
    client::render.init();
    SDL_WM_SetCaption( OZ_APPLICATION_TITLE " :: Prebuilding data ...", null );

    buffer.alloc( 10 * 1024 * 1024 );

    client::ui::Mouse::prebuild();

    prebuildTextures( "ui/icon", "ui/icon", true, GL_LINEAR, GL_LINEAR );
    prebuildTextures( "data/textures/oz", "bsp/tex", true,
                      client::Context::DEFAULT_MAG_FILTER, client::Context::DEFAULT_MIN_FILTER );

    client::Caelum::prebuild( "caelum" );
    prebuildTerras();

    prebuildBSPs();
    prebuildModels();

    prebuildLua( "lua/matrix" );
    prebuildLua( "lua/nirvana" );

    long endTime = SDL_GetTicks();

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

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return exitCode;
}
