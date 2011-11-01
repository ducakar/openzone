/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
 * Copyright (C) 2002-2011  Davorin Učakar
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
 *
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file tests/alloctest.cpp
 */

#include "stable.hpp"

#include "matrix/Library.hpp"
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

  chdir( "share/openzone" );

  library.init();
  matrix.init();
  synapse.load();
  orbis.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;
    float y = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;
    float z = -Orbis::DIM + Math::rand() * 2.0f * Orbis::DIM;

    synapse.addObject( "Tree2", Point3( x, y, z ), NORTH );
  }

  synapse.unload();
  orbis.unload();
  matrix.free();
  library.init();

  SDL_Quit();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}
