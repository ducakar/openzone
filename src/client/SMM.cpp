/*
 *  SMM.cpp
 *
 *  Single mesh model
 *  Common model format that all simple models are compiled to.
 *
 *  Copyright (C) 2002-2011, Davorin Učakar <davorin.ucakar@gmail.com>
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/SMM.hpp"

#include "client/Context.hpp"

namespace oz
{
namespace client
{

  void SMM::load()
  {
    String modelPath = "mdl/" + name + ".ozcSMM";

    log.println( "Loading SMM model '%s' {", modelPath.cstr() );
    log.indent();

    Buffer buffer;
    if( !buffer.read( modelPath ) ) {
      throw Exception( "Cannot read model file" );
    }
    InputStream is = buffer.inputStream();

    mesh.load( &is, GL_STATIC_DRAW );

    hard_assert( !is.isAvailable() );

    isLoaded = true;

    log.unindent();
    log.println( "}" );
  }

  SMM::SMM( const char* name_ ) : name( name_ ), isLoaded( false )
  {}

  SMM::~SMM()
  {
    log.print( "Unloading SMM model '%s' ...", name.cstr() );

    mesh.unload();

    log.printEnd( " OK" );

    hard_assert( glGetError() == GL_NO_ERROR );
  }

  void SMM::draw() const
  {
    mesh.draw( Mesh::SOLID_BIT | Mesh::ALPHA_BIT );
  }

}
}
