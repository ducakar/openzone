/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2014 Davorin Učakar
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
 * @file client/ui/Label.cc
 */

#include <client/ui/Label.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/ui/Area.hh>
#include <client/ui/Style.hh>

#include <cstdio>

namespace oz
{
namespace client
{
namespace ui
{

static const int EMPTY_HASH = hash( "" );

Label::Label() :
  x( 0 ), y( 0 ), align( Area::ALIGN_NONE ), font( Font::MONO ), offsetX( 0 ), offsetY( 0 ),
  width( 0 ), height( 0 ), lastHash( EMPTY_HASH ), texId( 0 )
{}

Label::Label( int x, int y, int align_, Font::Type font_, const char* s, ... ) :
  align( align_ ), font( font_ ), offsetX( 0 ), offsetY( 0 ), lastHash( EMPTY_HASH ), texId( 0 )
{
  va_list ap;
  va_start( ap, s );
  vset( x, y, s, ap );
  va_end( ap );
}

Label::~Label()
{
  clear();
}

Label::Label( Label&& l ) :
  x( l.x ), y( l.y ), align( l.align ), font( l.font ), offsetX( l.offsetX ), offsetY( l.offsetY ),
  width( l.width ), height( l.height ), lastHash( l.lastHash ), texId( l.texId )
{
  l.x        = 0;
  l.y        = 0;
  l.align    = Area::ALIGN_NONE;
  l.font     = Font::MONO;
  l.offsetX  = 0;
  l.offsetY  = 0;
  l.width    = 0;
  l.height   = 0;
  l.lastHash = EMPTY_HASH;
  l.texId    = 0;
}

Label& Label::operator = ( Label&& l )
{
  if( &l == this ) {
    return *this;
  }

  clear();

  x        = l.x;
  y        = l.y;
  align    = l.align;
  font     = l.font;
  offsetX  = l.offsetX;
  offsetY  = l.offsetY;
  width    = l.width;
  height   = l.height;
  lastHash = l.lastHash;
  texId    = l.texId;

  l.x        = 0;
  l.y        = 0;
  l.align    = Area::ALIGN_NONE;
  l.font     = Font::MONO;
  l.offsetX  = 0;
  l.offsetY  = 0;
  l.width    = 0;
  l.height   = 0;
  l.lastHash = EMPTY_HASH;
  l.texId    = 0;

  return *this;
}

void Label::vset( int x, int y, const char* s, va_list ap )
{
  hard_assert( s != nullptr );

  char buffer[1024];
  vsnprintf( buffer, 1024, s, ap );
  buffer[1023] = '\0';

  if( buffer[0] == '\0' || ( buffer[0] == ' ' && buffer[1] == '\0' ) ) {
    clear();
  }
  else {
    int newHash = hash( buffer );

    if( newHash != lastHash ) {
      MainCall() << [&]()
      {
        if( texId == 0 ) {
          glGenTextures( 1, &texId );
        }

        glBindTexture( GL_TEXTURE_2D, texId );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

        width = -1;
        style.fonts[font].upload( buffer, &width, &height );

        glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
      };
    }
  }

  setPosition( x, y );
}

void Label::set( int x, int y, const char* s, ... )
{
  va_list ap;
  va_start( ap, s );
  vset( x, y, s, ap );
  va_end( ap );
}

void Label::setPosition( int x_, int y_ )
{
  x       = x_;
  y       = y_;
  offsetX = x_;
  offsetY = y_;

  if( align & Area::ALIGN_RIGHT ) {
    offsetX -= width;
  }
  else if( align & Area::ALIGN_HCENTRE ) {
    offsetX -= width / 2;
  }
  if( align & Area::ALIGN_TOP ) {
    offsetY -= height;
  }
  else if( align & Area::ALIGN_VCENTRE ) {
    offsetY -= height / 2;
  }
}

void Label::setText( const char* s, ... )
{
  va_list ap;
  va_start( ap, s );
  vset( x, y, s, ap );
  va_end( ap );
}

void Label::draw( const Area* area )
{
  if( texId == 0 ) {
    return;
  }

  int posX = area->x + ( x < 0 ? area->width  + offsetX : offsetX );
  int posY = area->y + ( y < 0 ? area->height + offsetY : offsetY );

  glBindTexture( GL_TEXTURE_2D, texId );

  shape.colour( style.colours.textBackground );
  shape.fill( posX + 1, posY - 1, width, height );
  shape.colour( style.colours.text );
  shape.fill( posX, posY, width, height );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
}

void Label::clear()
{
  if( texId != 0 ) {
    MainCall() << [&]()
    {
      glDeleteTextures( 1, &texId );
    };

    offsetX  = x;
    offsetY  = y;
    width    = 0;
    height   = 0;
    lastHash = EMPTY_HASH;
    texId    = 0;
  }
}

}
}
}
