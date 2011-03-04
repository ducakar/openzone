/*
 *  Frame.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "ui/Area.hpp"

#include <SDL_ttf.h>
#include <GL/gl.h>

namespace oz
{
namespace client
{
namespace ui
{

  const SDL_Color Area::SDL_COLOUR_WHITE = { 0xff, 0xff, 0xff, 0xff };

  Vector<Area*> Area::updateAreas;

  Area::Area( int width_, int height_ ) :
      parent( null ), x( 0 ), y( 0 ), width( width_ ), height( height_ ), flags( 0 ),
      currentFont( font.sansFont ), fontColour( (SDL_Colour) { 0xff, 0xff, 0xff, 0x00 } ),
      textWidth( 0 ), textHeight( font.sansHeight )
  {}

  Area::Area( int x_, int y_, int width_, int height_ ) :
      parent( null ), x( x_ ), y( y_ ), width( width_ ), height( height_ ), flags( 0 ),
      currentFont( font.sansFont ), fontColour( (SDL_Colour) { 0xff, 0xff, 0xff, 0x00 } ),
      textWidth( 0 ), textHeight( font.sansHeight )
  {}

  Area::~Area()
  {
    children.free();
  }

  void Area::setFont( Font::Type type )
  {
    if( type == Font::MONO ) {
      currentFont = font.monoFont;
      textHeight  = font.monoHeight;
    }
    else if( type == Font::SANS ) {
      currentFont = font.sansFont;
      textHeight  = font.sansHeight;
    }
    else {
      currentFont = font.titleFont;
      textHeight  = font.titleHeight;
    }
  }

  void Area::setFontColour( ubyte r, ubyte g, ubyte b )
  {
    fontColour.r = r;
    fontColour.g = g;
    fontColour.b = b;
  }

  void Area::fill( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glRecti( x, y, x + width, y + height );
  }

  void Area::rect( int x, int y, int width, int height ) const
  {
    x = x < 0 ? this->x + this->width + x  : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glBegin( GL_LINE_LOOP );
      glVertex2f( float( x         ) + 0.5f, float( y          ) + 0.5f );
      glVertex2f( float( x + width ) - 0.5f, float( y          ) + 0.5f );
      glVertex2f( float( x + width ) - 0.5f, float( y + height ) - 0.5f );
      glVertex2f( float( x         ) + 0.5f, float( y + height ) - 0.5f );
    glEnd();
  }

  void Area::print( int x, int y, const char* s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColour );

    // flip
    uint* pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; ++i ) {
      for( int j = 0; j < text->w; ++j ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    x = x < 0 ? this->x + this->width  + x : this->x + x;
    y = y < 0 ? this->y + this->height + y : this->y + y;

    glRasterPos2i( x, y );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    textWidth = text->w;
    SDL_FreeSurface( text );
  }

  void Area::printCentred( int baseX, int baseY, const char* s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColour );

    // flip
    uint* pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; ++i ) {
      for( int j = 0; j < text->w; ++j ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    baseX = baseX < 0 ? this->x + this->width  + baseX : this->x + baseX;
    baseY = baseY < 0 ? this->y + this->height + baseY : this->y + baseY;

    glRasterPos2i( baseX - text->w / 2, baseY - text->h / 2 );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    textWidth = text->w;
    SDL_FreeSurface( text );
  }

  void Area::printBaseline( int x, int baseY, const char* s, ... )
  {
    char buffer[1024];
    va_list ap;

    va_start( ap, s );
    vsnprintf( buffer, 1024, s, ap );
    va_end( ap );
    buffer[1023] = '\0';

    SDL_Surface* text = TTF_RenderUTF8_Blended( currentFont, buffer, fontColour );

    // flip
    uint* pixels = reinterpret_cast<uint*>( text->pixels );
    for( int i = 0; i < text->h / 2; ++i ) {
      for( int j = 0; j < text->w; ++j ) {
        swap( pixels[i * text->w + j], pixels[( text->h - i - 1 ) * text->w + j] );
      }
    }

    x     =     x < 0 ? this->x + this->width  + x     : this->x + x;
    baseY = baseY < 0 ? this->y + this->height + baseY : this->y + baseY;

    glRasterPos2i( x, baseY - text->h / 2 );
    glDrawPixels( text->w, text->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    textWidth = text->w;
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
    moveX = bound( moveX, parent->x - x, parent->x + parent->width  - x - width  );
    moveY = bound( moveY, parent->y - y, parent->y + parent->height - y - height );

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
        hard_assert( updateAreas[i]->flags & UPDATE_FUNC_BIT );

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

      if( flags & UPDATE_FUNC_BIT ) {
        flags |= UPDATE_BIT;
      }
    }
    else {
      flags |= IGNORE_BIT | HIDDEN_BIT;
      flags &= ~UPDATE_BIT;
    }
  }

  void Area::add( Area* area, int relativeX, int relativeY )
  {
    area->width  = bound( area->width,  1, width  );
    area->height = bound( area->height, 1, height );

    relativeX = relativeX < 0 ? width  + relativeX : relativeX;
    relativeY = relativeY < 0 ? height + relativeY : relativeY;

    relativeX = bound( relativeX, 0, width  - area->width  );
    relativeY = bound( relativeY, 0, height - area->height );

    area->realign( x + relativeX, y + relativeY );
    area->parent = this;

    children.pushFirst( area );

    if( area->flags & UPDATE_FUNC_BIT ) {
      updateAreas.add( area );
    }
  }

  void Area::add( Area* area )
  {
    add( area, area->x, area->y );
  }

  void Area::remove( Area* area )
  {
    if( area->flags & UPDATE_FUNC_BIT ) {
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
