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
 * @file client/ui/Font.cc
 */

#include <client/ui/Font.hh>

#include <client/Shader.hh>

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

static const SDL_Colour SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

int Font::size( const char* s ) const
{
  int width;
  TTF_SizeUTF8( handle, s, &width, nullptr );
  return width;
}

void Font::draw( const char* s, uint texId, int* width, int* height ) const
{
  SDL_Surface* textSurface = TTF_RenderUTF8_Blended( handle, s, SDL_COLOUR_WHITE );

  MainCall() << [&]()
  {
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, textSurface->pixels );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  };

  *width  = textSurface->w;
  *height = textSurface->h;

  SDL_FreeSurface( textSurface );
}

void Font::init( const char* name, int height_ )
{
  height = height_;

  File file( String::str( "@ui/font/%s.ttf", name ) );

  buffer = file.read();
  if( buffer.isEmpty() ) {
    OZ_ERROR( "Failed to read font file '%s'", file.path().cstr() );
  }

  InputStream istream = buffer.inputStream();

  handle = TTF_OpenFontRW( SDL_RWFromConstMem( istream.begin(), istream.capacity() ), true, height );
  if( handle == nullptr ) {
    OZ_ERROR( "%s", TTF_GetError() );
  }
}

void Font::destroy()
{
  if( handle != nullptr ) {
    TTF_CloseFont( handle );
  }

  buffer.deallocate();
}

}
}
}
