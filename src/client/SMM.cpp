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
 * Davorin Učakar <davorin.ucakar@gmail.com>
 */

/**
 * @file client/SMM.cpp
 *
 * Single mesh model.
 *
 * Common model format that all simple models are compiled to.
 */

#include "stable.hpp"

#include "client/SMM.hpp"

#include "client/Context.hpp"
#include "client/OpenGL.hpp"

namespace oz
{
namespace client
{

void SMM::load()
{
  const String& name = library.models[id].name;
  const String& path = library.models[id].path;

  log.print( "Loading SMM model '%s' ...", name.cstr() );

  File file( path );
  if( !file.map() ) {
    throw Exception( "Cannot mmap model file" );
  }
  InputStream is = file.inputStream();

  shaderId = library.shaderIndex( is.readString() );
  mesh.load( &is, GL_STATIC_DRAW );

  file.unmap();

  isLoaded = true;

  log.printEnd( " OK" );
}

SMM::SMM( int id_ ) : id( id_ ), isLoaded( false )
{}

SMM::~SMM()
{
  const String& name = library.models[id].name;

  log.print( "Unloading SMM model '%s' ...", name.cstr() );

  mesh.unload();

  log.printEnd( " OK" );

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
