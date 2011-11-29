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
 * @file tools/ozSineSample.cpp
 *
 * Generate S16LE sine sample, used for <tt>src/oz/bellSample.inc</tt>.
 */

#include <stable.hpp>

#include <cstdio>
#include <cstdlib>

#include <SDL/SDL_main.h>

static const int   RATE      = 44100;
static const int   SAMPLES   = 11025;
static const int   COLS      = 12;
static const float AMPLITUDE = 16384.0f;
static const float FREQUENCY = 800.0f;

using namespace oz;

int main( int, char** )
{
  short samples[SAMPLES];

  for( int i = 0; i < SAMPLES; ++i ) {
    samples[i] = short( AMPLITUDE * Math::sin( float( i ) / RATE * FREQUENCY * Math::TAU ) + 0.5f );

    printf( "%6d", samples[i] );

    if( i != SAMPLES - 1 ) {
      printf( "," );

      if( i % COLS == COLS - 1 ) {
        printf( "\n" );
      }
      else {
        printf( " " );
      }
    }
  }
  printf( "\n" );

  return EXIT_SUCCESS;
}
