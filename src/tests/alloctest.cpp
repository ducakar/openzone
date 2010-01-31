/*
 *  alloctest.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2010, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU General Public License v3. See COPYING for details.
 */

#include "precompiled.h"

#include "matrix/Matrix.h"

using namespace oz;

int main()
{
  SDL_Init( 0 );

  log.init( "", false, "  " );
  matrix.init();
  world.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -World::DIM + Math::frand() * 2.0f * World::DIM;
    float y = -World::DIM + Math::frand() * 2.0f * World::DIM;
    float z = -World::DIM + Math::frand() * 2.0f * World::DIM;

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
