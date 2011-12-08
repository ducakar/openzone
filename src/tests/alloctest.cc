/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
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
 * Davorin Učakar
 * <davorin.ucakar@gmail.com>
 */

/**
 * @file tests/alloctest.cc
 */

#include "stable.hh"

#include "matrix/Library.hh"
#include "matrix/Synapse.hh"
#include "matrix/Matrix.hh"

#include <SDL/SDL_main.h>
#include <unistd.h>

using namespace oz;

bool Alloc::isLocked = true;

int main( int, char** )
{
  Alloc::isLocked = false;

  SDL_Init( 0 );

  chdir( "share/openzone" );

  matrix::library.init();
  matrix::matrix.init();
  matrix::synapse.load();
  matrix::orbis.load();

  for( int i = 0; i < 1000000; ++i ) {
    float x = -matrix::Orbis::DIM + Math::rand() * 2.0f * matrix::Orbis::DIM;
    float y = -matrix::Orbis::DIM + Math::rand() * 2.0f * matrix::Orbis::DIM;
    float z = -matrix::Orbis::DIM + Math::rand() * 2.0f * matrix::Orbis::DIM;

    matrix::synapse.addObject( "Tree2", Point3( x, y, z ), matrix::NORTH );
  }

  matrix::synapse.unload();
  matrix::orbis.unload();
  matrix::matrix.free();
  matrix::library.init();

  SDL_Quit();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}
