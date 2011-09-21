/*
 *  alloctest.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "matrix/Translator.hpp"
#include "matrix/Synapse.hpp"
#include "matrix/Matrix.hpp"

#include <SDL/SDL_main.h>
#include <unistd.h>

using namespace oz;

bool Alloc::isLocked = true;

int main( int, char** )
{
  Alloc::isLocked = false;

  SDL_Init( 0 );

  chdir( "data" );

  translator.init();
  matrix.init();
  synapse.load();
  orbis.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;
    float y = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;
    float z = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;

    synapse.addObject( "Tree2", Point3( x, y, z ) );
  }

  synapse.unload();
  orbis.unload();
  matrix.free();
  translator.init();

  SDL_Quit();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}
