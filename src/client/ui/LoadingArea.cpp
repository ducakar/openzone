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
 * @file client/ui/LoadingArea.cpp
 */

#include "stable.hpp"

#include "client/ui/LoadingArea.hpp"

#include "client/Camera.hpp"
#include "client/Shader.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{
namespace ui
{

LoadingArea::LoadingArea() : Area( camera.width, camera.height ),
    status( camera.width / 2, camera.height / 5, ALIGN_CENTRE, Font::LARGE, " " )
{
  flags = PINNED_BIT;
}

LoadingArea::~LoadingArea()
{}

void LoadingArea::onDraw()
{
  glUniform4f( param.oz_Colour, 0.1f, 0.1f, 0.1f, 1.0f );
  fill( 0, 0, camera.width, camera.height );

  status.draw( this );
}

}
}
}
