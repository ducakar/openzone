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
 * @file client/SMM.cc
 *
 * Single mesh model.
 *
 * Common model format that all simple models are compiled to.
 */

#include <stable.hh>
#include <client/SMM.hh>

#include <client/Context.hh>
#include <client/OpenGL.hh>

namespace oz
{
namespace client
{

SMM::SMM( int id_ ) :
  id( id_ ), isPreloaded( false ), isLoaded( false )
{}

void SMM::preload()
{
  file.setPath( liber.models[id].path );

  if( !file.map() ) {
    OZ_ERROR( "SMM file '%s' mmap failed", file.path().cstr() );
  }

  isPreloaded = true;
}

void SMM::load()
{
  InputStream is = file.inputStream();

  mesh.load( &is, GL_STATIC_DRAW );

  hard_assert( !is.isAvailable() );

  file.setPath( "" );

  isLoaded = true;
}

}
}
