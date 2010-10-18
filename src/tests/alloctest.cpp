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
#ifdef OZ_MSVC
# include <direct.h>
#else
# include <unistd.h>
#endif

using namespace oz;

int main( int, char** )
{
  SDL_Init( 0 );

  chdir( "data" );

  matrix.init();
  orbis.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;
    float y = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;
    float z = -Orbis::DIM + Math::frand() * 2.0f * Orbis::DIM;

    synapse.addObject( "Tree2", Vec3( x, y, z ) );
  }

  orbis.unload();
  matrix.free();

  SDL_Quit();
  return 0;
}
