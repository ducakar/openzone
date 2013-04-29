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
 * @file client/ui/Label.cc
 */

#include <stable.hh>
#include <client/ui/Label.hh>

#include <client/Shader.hh>
#include <client/Shape.hh>
#include <client/ui/Area.hh>
#include <client/ui/Style.hh>
#include <ozEngine/GL.hh>

namespace oz
{
namespace client
{
namespace ui
{

Label::Label() :
  x( 0 ), y( 0 ), align( Area::ALIGN_NONE ), font( Font::MONO ), offsetX( 0 ), offsetY( 0 ),
  width( 0 ), height( 0 ), activeTexId( 0 ), hasChanged( false )
{
  glGenTextures( 2, texIds );

  glBindTexture( GL_TEXTURE_2D, texIds[0] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, texIds[1] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
}

Label::Label( int x_, int y_, int align_, Font::Type font_, const char* s, ... ) :
  offsetX( 0 ), offsetY( 0 ), activeTexId( 0 ), hasChanged( false )
{
  glGenTextures( 2, texIds );

  glBindTexture( GL_TEXTURE_2D, texIds[0] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, texIds[1] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );

  va_list ap;
  va_start( ap, s );
  vset( x_, y_, align_, font_, s, ap );
  va_end( ap );
}

Label::~Label()
{
  glDeleteTextures( 2, texIds );
}

Label::Label( Label&& l ) :
  x( l.x ), y( l.y ), align( l.align ), font( l.font ), offsetX( l.offsetX ), offsetY( l.offsetY ),
  width( l.width ), height( l.height ), newWidth( l.newWidth ), newHeight( l.newHeight ),
  activeTexId( l.activeTexId ), hasChanged( l.hasChanged )
{
  texIds[0] = l.texIds[0];
  texIds[1] = l.texIds[1];

  l.x           = 0;
  l.y           = 0;
  l.align       = Area::ALIGN_NONE;
  l.font        = Font::MONO;
  l.offsetX     = 0;
  l.offsetY     = 0;
  l.width       = 0;
  l.height      = 0;
  l.activeTexId = 0;
  l.hasChanged  = false;

  glGenTextures( 2, l.texIds );

  glBindTexture( GL_TEXTURE_2D, l.texIds[0] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, l.texIds[1] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

  glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
}

Label& Label::operator = ( Label&& l )
{
  if( &l == this ) {
    return *this;
  }

  x           = l.x;
  y           = l.y;
  align       = l.align;
  font        = l.font;
  offsetX     = l.offsetX;
  offsetY     = l.offsetY;
  width       = l.width;
  height      = l.height;
  activeTexId = l.activeTexId;
  hasChanged  = l.hasChanged;

  swap( texIds[0], l.texIds[0] );
  swap( texIds[1], l.texIds[1] );

  l.x           = 0;
  l.y           = 0;
  l.align       = Area::ALIGN_NONE;
  l.font        = Font::MONO;
  l.offsetX     = 0;
  l.offsetY     = 0;
  l.width       = 0;
  l.height      = 0;
  l.activeTexId = 0;
  l.hasChanged  = false;

  return *this;
}

void Label::vset( int x_, int y_, int align_, Font::Type font_, const char* s, va_list ap )
{
  hard_assert( s != nullptr );

  x          = x_;
  y          = y_;
  align      = align_;
  font       = font_;
  hasChanged = true;

  char buffer[1024];
  vsnprintf( buffer, 1024, s, ap );
  buffer[1023] = '\0';

  if( buffer[0] == '\0' || ( buffer[0] == ' ' && buffer[1] == '\0' ) ) {
    newWidth  = 0;
    newHeight = 0;
  }
  else {
    uint texId = activeTexId == texIds[0] ? texIds[1] : texIds[0];

    style.fonts[font].draw( buffer, texId, &newWidth, &newHeight );
  }
}

void Label::set( int x_, int y_, int align_, Font::Type font_, const char* s, ... )
{
  va_list ap;
  va_start( ap, s );
  vset( x_, y_, align_, font_, s, ap );
  va_end( ap );
}

void Label::set( int x_, int y_, const char* s, ... )
{
  va_list ap;
  va_start( ap, s );
  vset( x_, y_, align, font, s, ap );
  va_end( ap );
}

void Label::set( int x_, int y_ )
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

void Label::set( const char* s, ... )
{
  va_list ap;
  va_start( ap, s );
  vset( x, y, align, font, s, ap );
  va_end( ap );
}

void Label::draw( const Area* area, bool allowChanged )
{
  if( ( allowChanged || !hasChanged ) && activeTexId != 0 ) {
    glBindTexture( GL_TEXTURE_2D, activeTexId );

    int posX = area->x + ( x < 0 ? area->width  + offsetX : offsetX );
    int posY = area->y + ( y < 0 ? area->height + offsetY : offsetY );

    shape.colour( style.colours.textBackground );
    shape.fillInv( posX + 1, posY - 1, width, height );
    shape.colour( style.colours.text );
    shape.fillInv( posX, posY, width, height );

    glBindTexture( GL_TEXTURE_2D, shader.defaultTexture );
  }

  if( hasChanged ) {
    hasChanged  = false;
    activeTexId = newWidth == 0 ? 0 : activeTexId == texIds[0] ? texIds[1] : texIds[0];

    offsetX = x;
    offsetY = y;
    width   = newWidth;
    height  = newHeight;

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
}

}
}
}
