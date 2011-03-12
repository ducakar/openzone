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
#include "client/Sky.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"
#include "client/OBJ.hpp"
#include "client/MD2.hpp"
#include "client/Render.hpp"

#include <cerrno>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL_main.h>

using namespace oz;
using oz::uint;

bool Alloc::isLocked = true;

static void prebuildTextures( const char* srcDir, const char* destDir,
                              bool wrap, int magFilter, int minFilter )
{
  oz::log.println( "Prebuilding textures in '%s' {", srcDir );
  oz::log.indent();

  String dirName = srcDir;
  Directory dir( srcDir );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + dirName + "'" );
  }

  dirName = dirName + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "png" ) && !ent.hasExtension( "jpg" )  ) {
      continue;
    }

    String name = ent.baseName();
    String srcPath = dirName + ent;
    String destPath = String( destDir ) + "/" + name + ".ozcTex";

    oz::log.println( "Prebuilding texture '%s' {", srcPath.cstr() );
    oz::log.indent();

    int nMipmaps;
    uint id = client::context.loadRawTexture( srcPath, &nMipmaps, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    OutputStream os = buffer.outputStream();

    oz::log.println( "Compiling into '%s'", destPath.cstr() );
    client::context.writeTexture( id, nMipmaps, &os );

    if( !buffer.write( destPath, os.length() ) ) {
      throw Exception( "Texture writing failed" );
    }

    oz::log.unindent();
    oz::log.println( "}" );
  }

  oz::log.unindent();
  oz::log.println( "}" );
}

static void prebuildModels( const char* path )
{
  oz::log.println( "Prebuilding models in '%s' {", path );
  oz::log.indent();

  String dirName = path;
  Directory dir( path );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + dirName + "'" );
  }

  dirName = dirName + "/";

  foreach( ent, dir.citer() ) {
    struct stat statInfo;

    String path = dirName + ent;
    String objPath = path + "/data.obj";
    String md2Path = path + "/tris.md2";

    if( stat( objPath, &statInfo ) == 0 ) {
      client::OBJ::prebuild( path );
    }
    else if( stat( md2Path, &statInfo ) == 0 ) {
      client::MD2::prebuild( path );
    }
  }

  oz::log.unindent();
  oz::log.println( "}" );
}

static void prebuildBSPs( const char* path )
{
  oz::log.println( "Prebuilding BSPs in '%s' {", path );
  oz::log.indent();

  String dirName = path;
  Directory dir( path );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + dirName + "'" );
  }

  dirName = dirName + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "rc" ) ) {
      continue;
    }

    String name = ent.baseName();

    QBSP::prebuild( name );
    client::BSP::prebuild( name );
  }

  oz::log.unindent();
  oz::log.println( "}" );
}

static void prebuildTerras( const char* path )
{
  oz::log.println( "Prebuilding Terras in '%s' {", path );
  oz::log.indent();

  String dirName = path;
  Directory dir( path );

  if( !dir.isOpened() ) {
    throw Exception( "Cannot open directory '" + dirName + "'" );
  }

  dirName = dirName + "/";

  foreach( ent, dir.citer() ) {
    if( !ent.hasExtension( "rc" ) ) {
      continue;
    }

    String name = ent.baseName();

    orbis.terra.prebuild( name );
    client::terra.prebuild( name );
  }

  oz::log.unindent();
  oz::log.println( "}" );
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
    if( argc != 2 ) {
      oz::log.println( "Usage: %s data_directory", program_invocation_short_name );
      oz::log.println();
      return -1;
    }

    oz::log.printlnETD( OZ_APPLICATION_NAME " Prebuild started at" );

    SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
    SDL_SetVideoMode( 400, 40, 32, SDL_OPENGL );
    SDL_WM_SetCaption( "OpenZone :: Prebuilding data ...", null );

    oz::log.print( "Setting working directory to data directory '%s' ...", argv[1] );
    if( chdir( argv[1] ) != 0 ) {
      oz::log.printEnd( " Failed" );
      return -1;
    }
    oz::log.printEnd( " OK" );

    long startTime = SDL_GetTicks();

    client::render.init();
    buffer.alloc( 10 * 1024 * 1024 );
    matrix.init();

    client::ui::Mouse::prebuild();

    prebuildTextures( "ui", "ui", true, GL_LINEAR, GL_LINEAR );
    prebuildTextures( "textures/oz", "bsp/tex", true, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR );

    client::Sky::prebuild( "sky" );

    prebuildTerras( "terra" );

    prebuildBSPs( "maps" );

    prebuildModels( "mdl" );

    long endTime = SDL_GetTicks();

    oz::log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

    SDL_Quit();
  }
  catch( const Exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printException( e );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
      fprintf( stderr, "  in %s\n\n", e.function );
      fprintf( stderr, "  at %s:%d\n\n", e.file, e.line );
    }

    exitCode = -1;
  }
  catch( const std::exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s", e.what() );
    oz::log.println();

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n\n", e.what() );
    }

    exitCode = -1;
  }

  client::compiler.free();
  matrix.free();
  buffer.dealloc();
  client::render.free();
  config.clear();

  Alloc::printStatistics();
  oz::log.printlnETD( OZ_APPLICATION_NAME " Prebuild finished at" );

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return exitCode;
}
