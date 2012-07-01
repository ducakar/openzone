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
 * @file client/ui/Area.cc
 */

#include "stable.hh"

#include "client/ui/Area.hh"

#include "client/Camera.hh"
#include "client/Shape.hh"
#include "client/OpenGL.hh"

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

Vector<Area*> Area::updateAreas;

Area::Area( int width_, int height_ ) :
  flags( 0 ), parent( null ), x( 0 ), y( 0 ), width( width_ ), height( height_ ),
  defaultX( 0 ), defaultY( 0 )
{}

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

void Area::reposition()
{
  if( parent != null ) {
    x = defaultX == CENTRE ? parent->x + ( parent->width - width ) / 2 :
        defaultX < 0 ? parent->x + parent->width - width + defaultX : parent->x + defaultX;

    y = defaultY == CENTRE ? parent->y + ( parent->height - height ) / 2 :
        defaultY < 0 ? parent->y + parent->height - height + defaultY : parent->y + defaultY;
  }

  onReposition();

  foreach( child, children.iter() ) {
    child->reposition();
  }
}

void Area::move( int moveX, int moveY )
{
  if( parent == null ) {
    return;
  }

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

void Area::onVisibilityChange( bool )
{}

void Area::onReposition()
{}

void Area::onUpdate()
{
  hard_assert( false );
}

bool Area::onMouseEvent()
{
  return false;
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

  foreach( child, children.iter() ) {
    child->onVisibilityChange( doShow );
  }
  onVisibilityChange( doShow );
}

void Area::add( Area* area, int localX, int localY )
{
  area->width  = clamp( area->width,  1, width  );
  area->height = clamp( area->height, 1, height );

  area->defaultX = localX;
  area->defaultY = localY;

  area->parent = this;
  area->reposition();

  children.pushFirst( area );

  if( area->flags & UPDATE_BIT ) {
    updateAreas.add( area );
  }
}

void Area::remove( Area* area )
{
  hard_assert( children.has( area ) );

  if( area->flags & UPDATE_BIT ) {
    updateAreas.exclude( area );
  }

  children.remove( area );
  delete area;
}

void Area::raise()
{
  if( parent != null && parent->children.first() != this ) {
    parent->children.remove( this );
    parent->children.pushFirst( this );
  }
}

void Area::sink()
{
  if( parent != null && parent->children.last() != this ) {
    parent->children.remove( this );
    parent->children.pushLast( this );
  }
}

}
}
}
