/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2012 Davorin Učakar
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

#include <stable.hh>
#include <client/ui/Font.hh>

#include <client/Shader.hh>
#include <client/OpenGL.hh>

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

static const SDL_Colour SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

static SDL_Surface* textSurface;
static uint         texId;

int Font::size( const char* s ) const
{
  int width;
  TTF_SizeUTF8( static_cast<TTF_Font*>( handle ), s, &width, nullptr );
  return width;
}

void Font::draw( const char* s, uint texId_, int* width, int* height ) const
{
  textSurface = TTF_RenderUTF8_Blended( static_cast<TTF_Font*>( handle ), s, SDL_COLOUR_WHITE );
  texId       = texId_;

  OZ_MAIN_CALL( const_cast<Font*>( this ), {
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, textSurface->w, textSurface->h, 0, GL_RGBA,
                  GL_UNSIGNED_BYTE, textSurface->pixels );
    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  } )

  *width  = textSurface->w;
  *height = textSurface->h;

  SDL_FreeSurface( textSurface );
}

void Font::init( const char* name, int height_ )
{
  height = height_;
  file.setPath( String::str( "ui/font/%s.ttf", name ) );

  if( !file.map() ) {
    OZ_ERROR( "Failed to read font file '%s'", file.path().cstr() );
  }

  InputStream istream = file.inputStream();

  handle = TTF_OpenFontRW( SDL_RWFromConstMem( istream.begin(), istream.capacity() ), true, height );
  if( handle == nullptr ) {
    OZ_ERROR( "%s", TTF_GetError() );
  }
}

void Font::destroy()
{
  if( handle != nullptr ) {
    TTF_CloseFont( static_cast<TTF_Font*>( handle ) );
  }
  file.unmap();
}

}
}
}
