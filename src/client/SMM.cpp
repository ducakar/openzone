/*
 *  SMM.cpp
 *
 *  Single mesh model
 *  Common model format that all simple models are compiled to.
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
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

    Buffer buffer;
    if( !buffer.read( path ) ) {
      throw Exception( "Cannot read model file" );
    }
    InputStream is = buffer.inputStream();

    shaderId = library.shaderIndex( is.readString() );
    mesh.load( &is, GL_STATIC_DRAW );

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

  void SMM::draw() const
  {
    shader.use( shaderId );
    tf.apply();

    mesh.draw( Mesh::SOLID_BIT | Mesh::ALPHA_BIT );
  }

}
}
