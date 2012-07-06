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
 * @file client/ui/LoadingArea.cc
 */

#include "stable.hh"

#include "client/ui/LoadingArea.hh"

#include "client/Shape.hh"
#include "client/Camera.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{
namespace ui
{

LoadingArea::LoadingArea() :
  Area( camera.width, camera.height ),
  status( width / 2, height / 5, ALIGN_CENTRE, Font::LARGE, " " )
{
  flags = PINNED_BIT;
}

LoadingArea::~LoadingArea()
{}

void LoadingArea::onReposition()
{
  width  = camera.width;
  height = camera.height;

  status.set( width / 2, height / 5 );
}

void LoadingArea::onDraw()
{
  shape.colour( 0.1f, 0.1f, 0.1f, 1.0f );
  shape.fill( 0, 0, width, height );

  status.draw( this, false );
}

}
}
}
