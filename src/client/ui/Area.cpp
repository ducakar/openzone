/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/Shape.hpp"

#include "client/ui/Area.hpp"

#include "client/Camera.hpp"

#include <SDL/SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

  const SDL_Colour Area::SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

  Vector<Area*> Area::updateAreas;

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

  void Area::setFont( Font::Type type )
  {
    currentFont = font.fonts[type];
    textHeight  = Font::INFOS[type].height;
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

  void Area::print( int x, int y, int align, const char* s, ... )
  {
    hard_assert( s != null );

    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    if( buffer[0] == '\0' ) {
      return;
    }

    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, SDL_COLOUR_WHITE );

    textWidth = text->w;

    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    if( align & ALIGN_RIGHT ) {
      x -= text->w;
    }
    else if( align & ALIGN_HCENTRE ) {
      x -= text->w / 2;
    }
    if( align & ALIGN_TOP ) {
      y -= text->h;
    }
    else if( align & ALIGN_VCENTRE ) {
      y -= text->h / 2;
    }

    glBindTexture( GL_TEXTURE_2D, font.textTexId );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, text->w, text->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                  text->pixels);

    glUniform1i( param.oz_IsTextureEnabled, true );

    glUniform4f( param.oz_Colour, 0.0f, 0.0f, 0.0f, 1.0f );
    shape.fill( x + 1, y - 1, text->w, text->h );
    glUniform4f( param.oz_Colour, 1.0f, 1.0f, 1.0f, 1.0f );
    shape.fill( x, y, text->w, text->h );

    glUniform1i( param.oz_IsTextureEnabled, false );

    SDL_FreeSurface( text );
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
