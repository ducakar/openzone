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

#include <stable.hh>
#include <client/ui/Area.hh>

#include <client/Camera.hh>
#include <client/Shape.hh>
#include <client/OpenGL.hh>

#include <SDL_ttf.h>

namespace oz
{
namespace client
{
namespace ui
{

Area::Area( int width_, int height_ ) :
  flags( 0 ), parent( nullptr ), x( 0 ), y( 0 ), width( width_ ), height( height_ ),
  defaultX( 0 ), defaultY( 0 )
{}

Area::~Area()
{
  children.free();
}

void Area::reposition()
{
  if( parent != nullptr ) {
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
  if( parent == nullptr ) {
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

void Area::updateChildren()
{
  foreach( child, children.iter() ) {
    if( ( child->flags & ( UPDATE_BIT | DISABLED_BIT ) ) == UPDATE_BIT ) {
      child->onUpdate();
    }

    child->updateChildren();
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
      if( !( child->flags & ( IGNORE_BIT | DISABLED_BIT ) ) && child->onMouseEvent() ) {
        return true;
      }
    }
  }

  return false;
}

void Area::drawChildren()
{
  // Render in opposite order; last added child (the first one in the list) should be rendered last.
  for( Area* child = children.last(); child != nullptr; child = child->prev[0] ) {
    if( !( child->flags & ( HIDDEN_BIT | DISABLED_BIT ) ) ) {
      child->onDraw();
    }
  }
}

void Area::onVisibilityChange( bool )
{}

void Area::onReposition()
{}

void Area::onUpdate()
{}

bool Area::onMouseEvent()
{
  return false;
}

void Area::onDraw()
{}

void Area::show( bool doShow )
{
  if( flags & DISABLED_BIT ) {
    return;
  }

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

void Area::enable( bool doEnable )
{
  if( doEnable ) {
    flags &= ~DISABLED_BIT;
  }
  else {
    flags |= DISABLED_BIT;
  }

  bool doShow = doEnable && !( flags & HIDDEN_BIT );

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
}

void Area::remove( Area* area )
{
  hard_assert( children.has( area ) );

  children.erase( area );
  delete area;
}

void Area::raise()
{
  if( parent != nullptr && parent->children.first() != this ) {
    parent->children.erase( this );
    parent->children.pushFirst( this );
  }
}

void Area::sink()
{
  if( parent != nullptr && parent->children.last() != this ) {
    parent->children.erase( this );
    parent->children.pushLast( this );
  }
}

}
}
}
