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

static void prebuildTexture( const char* srcPath, const char* destPath, bool wrap,
                             int magFilter, int minFilter )
{
  uint id = client::context.loadRawTexture( srcPath, wrap, magFilter, minFilter );

  hard_assert( id != 0 );

  int nMipmaps, size;
  client::context.getTextureSize( id, &nMipmaps, &size );

  Buffer buffer( size );
  OutputStream os = buffer.outputStream();

  client::context.writeTexture( id, nMipmaps, &os );

  hard_assert( !os.isAvailable() );
  if( !buffer.write( destPath ) ) {
    throw Exception( "Texture writing failed" );
  }
  log.println( "Prebuilt texture '%s' -> '%s'", srcPath, destPath );
}

int main( int, char** )
{
  System::catchSignals();

  Alloc::isLocked = false;
  onleave( []() {
    Alloc::isLocked = true;
    Alloc::printLeaks();
  } );

  SDL_Init( SDL_INIT_NOPARACHUTE | SDL_INIT_VIDEO );
  SDL_SetVideoMode( 100, 100, 32, SDL_OPENGL );

  long startTime = SDL_GetTicks();

  translator.init();
  matrix.init();

  foreach( terra, translator.terras.citer() ) {
    try {
      orbis.terra.prebuild( terra->name );
      client::terra.prebuild();
    }
    catch( const Exception& ) {
    }
  }

  foreach( bsp, translator.bsps.citer() ) {
    try {
      BSP::prebuild( bsp->name );
      client::BSP::prebuild( bsp->name );
    }
    catch( const Exception& e ) {
      log.resetIndent();
      log.println();
      log.printException( e );
      exit( -1 );
    }
  }

  foreach( model, translator.models.citer() ) {
    struct stat statInfo;

    if( stat( model->path + "/data.obj", &statInfo ) == 0 ) {
      client::OBJ::prebuild( model->name );
    }
    else if( stat( model->path + "/tris.md2", &statInfo ) == 0 ) {
      client::MD2::prebuild( model->name );
    }
  }

  foreach( texture, translator.textures.citer() ) {
    prebuildTexture( texture->path, "bsp/tex/" + texture->name + ".ozTex",
                     true, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR );
  }

  Directory uiDir( "ui" );
  foreach( ent, uiDir.citer() ) {
    if( ent.hasExtension( "png" ) ) {
      String name = &*ent;
      String baseName = name.substring( 0, name.lastIndex( '.' ) );

      prebuildTexture( "ui/" + name,
                       "ui/" + baseName + ".ozTex", true, GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR );
    }
  }

  client::compiler.free();
  matrix.free();
  translator.free();

  long endTime = SDL_GetTicks();

  log.println( "Build time: %.2f s", float( endTime - startTime ) / 1000.0f );

  SDL_Quit();

  Alloc::printStatistics();
  return 0;
}
