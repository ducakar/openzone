/*
 *  GalileoModule.cpp
 *
 *  [description]
 *
 *  Copyright (C) 2002-2011  Davorin Učakar
 *  This software is covered by GNU GPLv3. See COPYING file for details.
 */

#include "stable.hpp"

#include "client/modules/GalileoModule.hpp"

#include "client/Context.hpp"

#include "client/ui/UI.hpp"

#include "client/OpenGL.hpp"

#include "luamacros.hpp"

namespace oz
{
namespace client
{

  GalileoModule galileoModule;

#ifndef OZ_TOOLS

  GalileoModule::GalileoModule() : galileoFrame( null )
  {}

  void GalileoModule::read( InputStream* istream )
  {
    int nMarkers = istream->readInt();
    for( int i = 0; i < nMarkers; ++i ) {
      markers.add( istream->readPoint3() );
    }
  }

  void GalileoModule::write( OutputStream* ostream ) const
  {
    ostream->writeInt( markers.length() );
    for( int i = 0; i < markers.length(); ++i ) {
      ostream->writePoint3( markers[i] );
    }
  }

  void GalileoModule::load()
  {
    galileoFrame = new ui::GalileoFrame();
    ui::ui.root->add( galileoFrame );
  }

  void GalileoModule::unload()
  {
    if( galileoFrame != null ) {
      ui::ui.root->remove( galileoFrame );
      galileoFrame = null;
    }

    markers.clear();
    markers.dealloc();
  }

  void GalileoModule::init()
  {
    OZ_LUA_FUNC( ozGalileoAddMarker );
  }

  int GalileoModule::ozGalileoAddMarker( lua_State* l )
  {
    ARG( 2 );

    galileoModule.markers.add( Point3( tofloat( 1 ), tofloat( 2 ), 0.0f ) );
    return 0;
  }

#else

  void GalileoModule::prebuild()
  {
    Config terraConfig;
    Buffer buffer( 1024 * 1024 );

    for( int i = 0; i < library.terras.length(); ++i ) {
      const String& name = library.terras[i].name;

      terraConfig.load( "terra/" + name + ".rc" );

      String srcTextureFile = String( "terra/" ) + terraConfig.get( "mapTexture", "" );
      String destTextureFile = "ui/galileo/" + name + ".ozcTex";

      OutputStream ostream = buffer.outputStream();

      uint id = context.loadRawTexture( srcTextureFile, true, GL_LINEAR, GL_LINEAR );
      context.writeTexture( id, &ostream );

      terraConfig.clear();

      buffer.write( destTextureFile, ostream.length() );
    }
  }

#endif

}
}
