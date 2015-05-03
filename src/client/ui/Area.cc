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
 * @file client/ui/Area.cc
 */

#include <client/ui/Area.hh>

#include <client/ui/Mouse.hh>

namespace oz
{
namespace client
{
namespace ui
{

Area::Area(int width_, int height_) :
  flags(ENABLED_BIT | VISIBLE_BIT), parent(nullptr),
  x(0), y(0), width(width_), height(height_), defaultX(0), defaultY(0)
{}

Area::~Area()
{
  children.free();
}

void Area::updateChildren()
{
  for (Area& child : children) {
    if ((child.flags & UPDATE_BIT) && child.isEnabled()) {
      child.onUpdate();
    }

    child.updateChildren();
  }
}

bool Area::passMouseEvents()
{
  for (Area& child : children) {
    if (child.flags & GRAB_BIT) {
      child.onMouseEvent();
      return true;
    }
  }

  for (auto i = children.iterator(); i.isValid();) {
    Area* child = i;
    ++i;

    if (child->x <= mouse.x && mouse.x < child->x + child->width &&
        child->y <= mouse.y && mouse.y < child->y + child->height)
    {
      // If event is passed to a child, we won't handle it on parent. Of course we assume
      // children do not overlap, so event can only be passed to one of them.
      if (child->isVisible() && child->onMouseEvent()) {
        return true;
      }
    }
  }

  return false;
}

bool Area::passKeyEvents()
{
  for (auto i = children.iterator(); i.isValid();) {
    Area* child = i;
    ++i;

    if (child->isVisible()) {
      child->onKeyEvent();
    }
  }

  return true;
}

void Area::drawChildren()
{
  // Render in opposite order; last added child (the first one in the list) should be rendered last.
  for (Area* child = children.last(); child != nullptr; child = child->prev[0]) {
    if (child->isVisible()) {
      child->onDraw();
    }
  }
}

void Area::onVisibilityChange(bool)
{}

void Area::onRealign()
{}

void Area::onUpdate()
{}

bool Area::onMouseEvent()
{
  return passMouseEvents();
}

bool Area::onKeyEvent()
{
  return passKeyEvents();
}

void Area::onDraw()
{
  drawChildren();
}

void Area::enable(bool doEnable)
{
  bool isEnabled = flags & ENABLED_BIT;

  if (doEnable == isEnabled) {
    return;
  }

  if (doEnable) {
    flags |= ENABLED_BIT;
  }
  else {
    flags &= ~ENABLED_BIT;
  }

  if (flags & VISIBLE_BIT) {
    for (Area& child : children) {
      if (child.isVisible()) {
        child.onVisibilityChange(doEnable);
      }
    }

    onVisibilityChange(doEnable);
  }
}

void Area::show(bool doShow)
{
  bool isVisible = flags & VISIBLE_BIT;

  if (doShow == isVisible) {
    return;
  }

  if (doShow) {
    flags |= VISIBLE_BIT;
  }
  else {
    flags &= ~VISIBLE_BIT;
  }

  if (flags & ENABLED_BIT) {
    for (Area& child : children) {
      if (child.isVisible()) {
        child.onVisibilityChange(doShow);
      }
    }

    onVisibilityChange(doShow);
  }
}

Pos2 Area::align(int localX, int localY, int width, int height) const
{
  return {
    localX == CENTRE ? x + (this->width - width) / 2 :
    localX < 0 ? x + this->width - width + localX : x + localX,
    localY == CENTRE ? y + (this->height - height) / 2 :
    localY < 0 ? y + this->height - height + localY : y + localY
  };
}

void Area::realign()
{
  if (parent != nullptr) {
    Pos2 pos = parent->align(defaultX, defaultY, width, height);

    x = pos.x;
    y = pos.y;
  }

  onRealign();

  for (Area& child : children) {
    child.realign();
  }
}

void Area::move(int moveX, int moveY)
{
  if (parent == nullptr) {
    return;
  }

  moveX = clamp(moveX, parent->x - x, parent->x + parent->width  - x - width);
  moveY = clamp(moveY, parent->y - y, parent->y + parent->height - y - height);

  x += moveX;
  y += moveY;

  for (Area& child : children) {
    child.move(moveX, moveY);
  }
}

void Area::add(Area* area, int localX, int localY)
{
  area->width  = clamp(area->width,  1, width);
  area->height = clamp(area->height, 1, height);

  area->defaultX = localX;
  area->defaultY = localY;

  area->parent = this;
  area->realign();

  children.pushFirst(area);
}

void Area::remove(Area* area)
{
  hard_assert(children.has(area));

  children.erase(area);
  delete area;
}

void Area::raise()
{
  if (parent != nullptr && parent->children.first() != this) {
    hard_assert(parent->children.has(this));

    parent->children.erase(this);
    parent->children.pushFirst(this);
  }
}

void Area::sink()
{
  if (parent != nullptr && parent->children.last() != this) {
    hard_assert(parent->children.has(this));

    parent->children.erase(this);
    parent->children.pushLast(this);
  }
}

}
}
}
