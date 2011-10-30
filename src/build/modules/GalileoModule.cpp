/*
 *  GalileoModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "build/modules/GalileoModule.hpp"

#include "matrix/Library.hpp"

#include "client/OpenGL.hpp"

#include "build/Context.hpp"

namespace oz
{
namespace build
{

void GalileoModule::build()
{
  Config terraConfig;
  Buffer buffer( 2 * 1024 * 1024 );

  for( int i = 0; i < library.terras.length(); ++i ) {
    const String& name = library.terras[i].name;

    terraConfig.load( "terra/" + name + ".rc" );

    // disable warnings
    terraConfig.get( "step", 0.0f );
    terraConfig.get( "bias", 0.0f );
    terraConfig.get( "waterTexture", "" );
    terraConfig.get( "detailTexture", "" );

    String srcTextureFile = String( "terra/" ) + terraConfig.get( "mapTexture", "" );
    String destTextureFile = "ui/galileo/" + name + ".ozcTex";

    OutputStream ostream = buffer.outputStream();

    uint id = Context::loadRawTexture( srcTextureFile, true, GL_LINEAR, GL_LINEAR );
    Context::writeTexture( id, &ostream );

    terraConfig.clear();

    buffer.write( destTextureFile, ostream.length() );
  }
}

}
}
