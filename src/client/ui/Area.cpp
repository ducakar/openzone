/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/ui/Area.hpp"

#include "client/Shape.hpp"
#include "client/Camera.hpp"

#include "client/OpenGL.hpp"

#include <SDL/SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

  const SDL_Colour Area::SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

  Vector<Area*> Area::updateAreas;

  void Area::Label::vset( int x_, int y_, int align_, Font::Type font_, const char* s, va_list ap )
  {
    hard_assert( s != null );

    x     = x_;
    y     = y_;
    align = align_;
    font  = font_;

    char buffer[1024];
    vsnprintf( buffer, 1024, s, ap );
    buffer[1023] = '\0';

    if( buffer[0] == '\0' ) {
      offsetX = 0;
      offsetY = 0;
      width   = 0;
      height  = 0;

      activeTexId = 0;
      return;
    }

    SDL_Surface* text = TTF_RenderUTF8_Blended( ui::font.fonts[font], buffer, SDL_COLOUR_WHITE );

    glBindTexture( GL_TEXTURE_2D, texId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, text->w, text->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  text->pixels );
    glBindTexture( GL_TEXTURE_2D, 0 );

    SDL_FreeSurface( text );

    activeTexId = texId;

    offsetX = x;
    offsetY = y;
    width   = text->w;
    height  = text->h;

    if( align & ALIGN_RIGHT ) {
      offsetX -= width;
    }
    else if( align & ALIGN_HCENTRE ) {
      offsetX -= width / 2;
    }
    if( align & ALIGN_TOP ) {
      offsetY -= height;
    }
    else if( align & ALIGN_VCENTRE ) {
      offsetY -= height / 2;
    }
  }

  Area::Label::Label() : x( 0 ), y( 0 ),align( ALIGN_NONE ), font( Font::MONO ),
      offsetX( 0 ), offsetY( 0 ), width( 0 ), height( 0 ), activeTexId( 0 )
  {
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  }

  Area::Label::~Label()
  {
    glDeleteTextures( 1, &texId );
  }

  Area::Label::Label( int x, int y, int align, Font::Type font, const char* s, ... )
  {
    glGenTextures( 1, &texId );
    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Area::Label::set( int x, int y, int align, Font::Type font, const char* s, ... )
  {
    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Area::Label::setText( const char* s, ... )
  {
    va_list ap;
    va_start( ap, s );
    vset( x, y, align, font, s, ap );
    va_end( ap );
  }

  void Area::Label::draw( const Area* area ) const
  {
    glBindTexture( GL_TEXTURE_2D, width == 0 ? 0 : texId );

    int posX = area->x + ( x < 0 ? area->width  + offsetX : offsetX );
    int posY = area->y + ( y < 0 ? area->height + offsetY : offsetY );

    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    shape.fill( posX + 1, posY - 1, width, height );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( posX, posY, width, height );

    glBindTexture( GL_TEXTURE_2D, 0 );
  }

  Area::Area( int width_, int height_ ) :
      flags( 0 ), currentFont( font.fonts[Font::SANS] ), parent( null ), x( 0 ), y( 0 ),
      width( width_ ), height( height_ ), textWidth( 0 ),
      textHeight( Font::INFOS[Font::SANS].height )
  {}

  Area::Area( int x_, int y_, int width_, int height_ ) :
      flags( 0 ), currentFont( font.fonts[Font::SANS] ), parent( null ), x( x_ ), y( y_ ),
      width( width_ ), height( height_ ), textWidth( 0 ),
      textHeight( Font::INFOS[Font::SANS].height )
  {
    x = x < 0 ? camera.width  - width  + x : x;
    y = y < 0 ? camera.height - height + y : y;
  }

  Area::~Area()
  {
    children.free();
  }

  void Area::fill( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    shape.fill( x, y, width, height );
  }

  void Area::rect( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width + x  : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    shape.rect( x, y, width, height );
  }

  void Area::realign( int newX, int newY )
  {
    int dx = newX - x;
    int dy = newY - y;

    x = newX;
    y = newY;

    foreach( child, children.iter() ) {
      child->x += dx;
      child->y += dy;
    }
  }

  void Area::move( int moveX, int moveY )
  {
    moveX = clamp( moveX, parent->x - x, parent->x + parent->width  - x - width  );
    moveY = clamp( moveY, parent->y - y, parent->y + parent->height - y - height );

    x += moveX;
    y += moveY;

    foreach( child, children.iter() ) {
      child->move( moveX, moveY );
    }
  }

  bool Area::passMouseEvents()
  {
    foreach( child, children.iter() ) {
      if( child->flags & GRAB_BIT ) {
        child->onMouseEvent();
        return true;
      }
    }
    foreach( child, children.iter() ) {
      if( child->x <= mouse.x && mouse.x < child->x + child->width &&
          child->y <= mouse.y && mouse.y < child->y + child->height )
      {
        // If event is passed to a child, we won't handle it on parent. Of course we assume
        // children do not overlap, so event can only be passed to one of them.
        if( !( child->flags & IGNORE_BIT ) && child->onMouseEvent() ) {
          return true;
        }
      }
    }
    return false;
  }

  void Area::drawChildren()
  {
    // render in opposite order; last added child (the first one in the list) should be rendered
    // last
    for( Area* child = children.last(); child != null; child = child->prev[0] ) {
      if( !( child->flags & HIDDEN_BIT ) ) {
        child->onDraw();
      }
    }
  }

  void Area::update()
  {
    for( int i = 0; i < updateAreas.length(); ++i ) {
      if( updateAreas[i]->flags & UPDATE_BIT ) {
        updateAreas[i]->onUpdate();
      }
    }
  }

  bool Area::onMouseEvent()
  {
    return false;
  }

  void Area::onUpdate()
  {
    hard_assert( false );
  }

  void Area::onDraw()
  {}

  void Area::show( bool doShow )
  {
    if( doShow ) {
      flags &= ~( IGNORE_BIT | HIDDEN_BIT );
    }
    else {
      flags |= IGNORE_BIT | HIDDEN_BIT;
    }
  }

  void Area::add( Area* area, int relativeX, int relativeY )
  {
    area->width  = clamp( area->width,  1, width  );
    area->height = clamp( area->height, 1, height );

    relativeX = relativeX < 0 ? width  + relativeX : relativeX;
    relativeY = relativeY < 0 ? height + relativeY : relativeY;

    relativeX = clamp( relativeX, 0, width  - area->width  );
    relativeY = clamp( relativeY, 0, height - area->height );

    area->realign( x + relativeX, y + relativeY );
    area->parent = this;

    children.pushFirst( area );

    if( area->flags & UPDATE_BIT ) {
      updateAreas.add( area );
    }
  }

  void Area::add( Area* area )
  {
    add( area, area->x, area->y );
  }

  void Area::remove( Area* area )
  {
    hard_assert( children.contains( area ) );

    if( area->flags & UPDATE_BIT ) {
      updateAreas.exclude( area );
    }

    children.remove( area );
    delete area;
  }

  void Area::focus( Area* area )
  {
    hard_assert( children.contains( area ) );

    if( children.first() != area ) {
      children.remove( area );
      children.pushFirst( area );
    }
  }

}
}
}
