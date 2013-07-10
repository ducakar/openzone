/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2013 Davorin Učakar
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
 */

/**
 * @file tests/noise.cc
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>

#include <SDL.h>
#include <noise/noise.h>

using namespace oz;
using namespace noise;

static const int width  = 513;
static const int height = 513;

static uint data[width][height];

static void populate()
{
  module::Perlin mod;
  mod.SetFrequency( 8 );

  for( int x = 0; x < width; ++x ) {
    for( int y = 0; y < height; ++y ) {
      double v = mod.GetValue( 0.01 * x, 0.01 * y, 1 );
      uint value = 120 + uint( v * 50.0 );

      hard_assert( 0 <= value && value <= 255 );

      data[x][y] = 0xff000000 | value | ( value << 8 ) | ( value << 16 );
    }
  }
}

int main()
{
  System::init();
  SDL_Init( SDL_INIT_VIDEO );
  Window::create( "test", width, height );

  uint t0 = Time::clock();
  populate();
  Log() << "populate time: " << ( Time::clock() - t0 ) << " ms";

  t0 = Time::clock();

  Buffer b0( reinterpret_cast<char*>( data ), int( sizeof( data ) ) );
  Buffer b1 = b0.deflate( 1 );
  Buffer b2 = b1.inflate();
  Buffer b3 = b2.deflate( 1 );

  Log() << "zlib time: " << ( Time::clock() - t0 ) << " ms";

  Log() << "b0.length() = " << b0.length();
  Log() << "b1.length() = " << b1.length();
  Log() << "b2.length() = " << b2.length();
  Log() << "b3.length() = " << b3.length();

  Log() << "b0 == b2: " << ( mCompare( b0.begin(), b2.begin(), size_t( b0.length() ) ) == 0 );
  Log() << "b1 == b3: " << ( mCompare( b1.begin(), b3.begin(), size_t( b1.length() ) ) == 0 );

  bool      isAlive = true;
  SDL_Event event;

  uint texId;
  glGenTextures( 1, &texId );
  glBindTexture( GL_TEXTURE_2D, texId );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );

  glEnable( GL_TEXTURE_2D );

  while( isAlive ) {
    SDL_PollEvent( &event );
    isAlive &= event.type != SDL_KEYUP;

    glClearColor( 0.2f, 0.2f, 0.2f, 0.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glBegin( GL_QUADS );
      glTexCoord2d( 0, 1 ); glVertex2d( -1, -1 );
      glTexCoord2d( 1, 1 ); glVertex2d( +1, -1 );
      glTexCoord2d( 1, 0 ); glVertex2d( +1, +1 );
      glTexCoord2d( 0, 0 ); glVertex2d( -1, +1 );
    glEnd();

    Window::swapBuffers();
    Time::sleep( 20 );
  }

  Window::destroy();
  SDL_Quit();
  return 0;
}
