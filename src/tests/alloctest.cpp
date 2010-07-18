/*
 *  alloctest.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Matrix.hpp"

#include <SDL_main.h>
#include <unistd.h>

OZ_IMPORT()

int main( int, char** )
{
  SDL_Init( 0 );

  chdir( "data" );

  log.init( "", false, "  " );
  matrix.init();
  world.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;
    float y = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;
    float z = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;

    switch( Math::randn( 3 ) ) {
      default: {
        synapse.addObject( "Tree2", Vec3( x, y, z ) );
        break;
      }
    }
  }

  world.unload();
  matrix.free();

  SDL_Quit();
  return 0;
}
