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

#include "stable.hh"

#include "client/SMM.hh"

#include "client/Context.hh"
#include "client/OpenGL.hh"

namespace oz
{
namespace client
{

void SMM::load()
{
  const String& name = library.models[id].name;
  const String& path = library.models[id].path;

  log.verboseMode = true;
  log.print( "Loading SMM model '%s' ...", name.cstr() );

  PhysFile file( path );
  if( !file.map() ) {
    throw Exception( "Cannot mmap model file" );
  }
  InputStream is = file.inputStream();

  shaderId = library.shaderIndex( is.readString() );
  mesh.load( &is, GL_STATIC_DRAW );

  file.unmap();

  log.printEnd( " OK" );
  log.verboseMode = false;

  isLoaded = true;
}

SMM::SMM( int id_ ) :
  id( id_ ), isLoaded( false )
{}

SMM::~SMM()
{
  const String& name = library.models[id].name;

  log.verboseMode = true;
  log.print( "Unloading SMM model '%s' ...", name.cstr() );

  mesh.unload();

  log.printEnd( " OK" );
  log.verboseMode = false;

  OZ_GL_CHECK_ERROR();
}

void SMM::draw( int mask ) const
{
  shader.use( shaderId );
  tf.apply();

  mesh.draw( mask );
}

}
}
