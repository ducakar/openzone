/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2011 Davorin Učakar
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
 * @file client/MD3.cc
 */

#include "stable.hh"

#include "client/MD3.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

MD3::MD3( int id_ ) : id( id_ ), isLoaded( false )
{}

MD3::~MD3()
{
  OZ_GL_CHECK_ERROR();
}

void MD3::load()
{}

void MD3::drawFrame( int ) const
{}

}
}
