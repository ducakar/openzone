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

#include <client/Context.hpp>
#include "client/Compiler.hpp"
#include "client/Terra.hpp"
#include "client/BSP.hpp"
#include "client/OBJ.hpp"
#include "client/MD2.hpp"

#include <sys/stat.h>
#include <SDL_main.h>

#include <csignal>

using namespace oz;
using oz::uint;

bool Alloc::isLocked = true;

static void prebuildTextures( const char* path, bool wrap, int magFilter, int minFilter )
{
  log.println( "Prebuilding textures in '%s' {", path );
  log.indent();

  String dirName = path;
  Directory dir( path );

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
    String destPath = dirName + name + ".ozcTex";

    log.println( "Prebuilding texture '%s' {", srcPath.cstr() );
    log.indent();

    uint id = client::context.loadRawTexture( srcPath, wrap, magFilter, minFilter );

    hard_assert( id != 0 );

    int nMipmaps, size;
    client::context.getTextureSize( id, &nMipmaps, &size );

    Buffer buffer( size );
    OutputStream os = buffer.outputStream();

    log.println( "Compiling into '%s'", destPath.cstr() );
    client::context.writeTexture( id, nMipmaps, &os );

    hard_assert( !os.isAvailable() );
    if( !buffer.write( destPath ) ) {
      throw Exception( "Texture writing failed" );
    }

    log.unindent();
    log.println( "}" );
  }

  log.unindent();
  log.println( "}" );
}

static void prebuildModels( const char* path )
{
  log.println( "Prebuilding models in '%s' {", path );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

static void prebuildBSPs( const char* path )
{
  log.println( "Prebuilding BSPs in '%s' {", path );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

static void prebuildTerras( const char* path )
{
  log.println( "Prebuilding Terras in '%s' {", path );
  log.indent();

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

  log.unindent();
  log.println( "}" );
}

int main( int, char** )
{
  System::catchSignals();

  Alloc::isLocked = false;
  onleave( []() {
    Alloc::isLocked = true;
    Alloc::printLeaks();
  } );

  try {
    SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
    SDL_SetVideoMode( 200, 100, 32, SDL_OPENGL );
    SDL_WM_SetCaption( "OpenZone :: Prebuilding data ...", null );

    long startTime = SDL_GetTicks();

    matrix.init();

    prebuildTextures( "ui", true, GL_LINEAR, GL_LINEAR );
    prebuildTextures( "textures/oz", true, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR );

    prebuildTerras( "terra" );

    prebuildBSPs( "maps" );

    prebuildModels( "mdl" );

    client::compiler.free();
    matrix.free();

    long endTime = SDL_GetTicks();

    log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

    SDL_Quit();
  }
  catch( const Exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.printException( e );

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
      fprintf( stderr, "  in %s\n\n", e.function );
      fprintf( stderr, "  at %s:%d\n", e.file, e.line );
    }
  }
  catch( const std::exception& e ) {
    oz::log.resetIndent();
    oz::log.println();
    oz::log.println( "EXCEPTION: %s", e.what() );

    if( oz::log.isFile() ) {
      fprintf( stderr, "\nEXCEPTION: %s\n", e.what() );
    }
  }

  Alloc::printStatistics();
  return 0;
}
