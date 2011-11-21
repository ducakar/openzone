/*
 * OpenZone - Simple Cross-Platform FPS/RTS Game Engine
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
 * @file client/ui/Area.cpp
 */

#include "stable.hpp"

#include "client/ui/Area.hpp"

#include "client/Shape.hpp"
#include "client/OpenGL.hpp"

#include <SDL/SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

Vector<Area*> Area::updateAreas;

int Area::uiWidth;
int Area::uiHeight;
int Area::uiCentreX;
int Area::uiCentreY;

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
  x = x < 0 ? Area::uiWidth  - width  + x : x;
  y = y < 0 ? Area::uiHeight - height + y : y;
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
  x = x < 0 ? this->x + this->width  + x : this->x + x;
  y = y < 0 ? this->y + this->height + y : this->y + y;

  shape.rect( x, y, width, height );
}

void Area::realign( int newX, int newY )
{
  newX = newX < 0 ? parent->width  - width  + newX : newX;
  newY = newY < 0 ? parent->height - height + newY : newY;

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

void Area::onVisibilityChange()
{}

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

  onVisibilityChange();
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
