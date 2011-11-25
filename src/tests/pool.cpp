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
 * @file tests/pool.cpp
 */

#include "stable.hpp"

#include <SDL/SDL_main.h>

using namespace oz;

bool Alloc::isLocked = true;

class A
{
  public:

    int value;
    A*  next[1];

    static Pool<A, 2> pool;

  OZ_STATIC_POOL_ALLOC( pool )

};

Pool<A, 2> A::pool;

int main( int, char** )
{
  Alloc::isLocked = false;

  const int max = 10000;

  A* array[max];
  for( int i = 0; i < max; ++i ) {
    array[i] = new A();
    array[i]->value = 10;
  }
  for( int i = 0; i < max; ++i ) {
    delete array[i];
  }

  A::pool.free();

  Alloc::isLocked = true;
  Alloc::printLeaks();
  return 0;
}
